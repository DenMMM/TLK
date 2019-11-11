//---------------------------------------------------------------------------
#ifndef UnitSendH
#define UnitSendH
//---------------------------------------------------------------------------
#include <winsock2.h>
#include <vector>
#include <chrono>

#include "UnitSLList.h"
#include "UnitStates.h"
#include "UnitComputers.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
#include "UnitAuth.h"
#include "UnitRandCounter.h"
//---------------------------------------------------------------------------
struct MSendHello;
struct MSendRequest;

class MSend;
class MSendSrv;
class MSendCl;
//---------------------------------------------------------------------------
#define SEND_Version        0x31    // Версия сетевого интерфейса
#define SEND_Port           7005    // Номер порта клиента
// Ограничения на размер данных в процессе обмена (MSendRequest.Size)
#define SEND_MinData        (sizeof(unsigned)+MAC_Size)
#define SEND_MaxData        (MAX_SLFileSize+MAC_Size)   // Размер упакованного объекта+MAC
//---------------------------------------------------------------------------
// Что сервер хочет отправить клиенту/скачать с него (MSendRequest.Type)
#define mstSendGames        1       // Список игр для клиента
#define mstSendConfig       2       // Настройки для клиента
#define mstGetGames         3       // Запрос списка игр
#define mstGetConfig        4       // Запрос настроек клиента
#define mstAccept           5       // Подтверждение клиентом успешной доставки
//---------------------------------------------------------------------------
// Main Send State
#define mssNone             0
#define mssSendGames        1
#define mssSendConfig       2
#define mssGetGames         3
#define mssGetConfig        4
//---------------------------------------------------------------------------
// События, посылаемые MSendSrv оболочке для индикации процесса
#define mseConnecting       1   // Устанавливается соединение
#define mseSending          2   // Отправка данных
#define mseReceiving        3   // Прием данных
#define mseDisconnecting    4   // Разъединение
#define mseExecute          5   // Сеанс завершился успешно
#define mseNotConnect       6   // Не удалось соединиться с клиентом
#define mseProtError        7   // Ошибка протокола
#define mseFreeParam        8   // Можно удалить поданные для групповой обработки данные
//---------------------------------------------------------------------------
/*
 сервер               =>      клиент
 ===================================================
 MSendHello.Version=SEND_Version
 MSendHello.Seed=random1;
 MSendHello.MAC=hmac(MSendHello-MSendHello.MAC,MAuth);
 encode(MSendHello,ENC_Net1);
                             check:
                                    Version,
                                    MAC

 сервер               <=      клиент
 ===================================================
 MSendHello.Version=SEND_Version
 MSendHello.Seed=random2;
 MSendHello.MAC=hmac(MSendHello-MSendHello.MAC,MAuth);
 encode(MSendHello,ENC_Net1);
    check:
            Version,
            MAC

            
 [mssSendGames/mssSendConfig]

 сервер               =>      клиент
 ===================================================
 MSendRequest.Type=mstSendGames/mstSendConfig;
 MSendRequest.Seed=mix(random1,random2);
 MSendRequest.Size=MSLList::GetAllDataSize()+sizeof(Seed)+MAC_Size;
 MSendRequest.MAC=hmac(MSendRequest-MSendRequest.MAC,MAuth);
 encode(MSendRequest,ENC_Net1);
                             check:
                                    Type,
                                    Seed,
                                    Size,
                                    MAC
                             attempt:
                                    new char[Size]

 сервер               =>      клиент
 ===================================================
 Seed=mix(random1,random2);
 Data[]=MSLList::SetAllData();
 MAC=hmac(Seed+Data,MAuth);
 encode(Seed+Data+MAC,ENC_Net1);
                             check:
                                    Seed,
                                    MAC
                             attempt:
                                    MSLList::GetAllData();
                                    MStateCl::New...();

 сервер               <=      клиент
 ===================================================
 MSendRequest.Type=mstAccept;
 MSendRequest.Seed=mix(random1,random2);
 MSendRequest.Size=0;
 MSendRequest.MAC=hmac(MSendRequest-MSendRequest.MAC,MAuth);
 encode(MSendRequest,ENC_Net1);
    check:
            Type,
            Seed,
            Size,
            MAC
    attempt:
            Event(*Computer,mseExecute);


 [mssGetGames/mssGetConfig]

 сервер               =>      клиент
 ===================================================
 MSendRequest.Type=mstGetGames/mstGetConfig;
 MSendRequest.Seed=mix(random1,random2);
 MSendRequest.Size=0;
 MSendRequest.MAC=hmac(MSendRequest-MSendRequest.MAC,MAuth);
                             check:
                                    Type,
                                    Seed,
                                    Size,
                                    MAC

 сервер               <=      клиент
 ===================================================
 MSendRequest.Type=mstAccept;
 MSendRequest.Seed=mix(random1,random2);
 MSendRequest.Size=MSLList::GetAllDataSize()+sizeof(Seed)+MAC_Size;
 MSendRequest.MAC=hmac(MSendRequest-MSendRequest.MAC,MAuth);
 encode(MSendRequest,ENC_Net1);
    check:
            Type,
            Seed,
            Size,
            MAC
    attempt:
            new char[Size]

 сервер               <=      клиент
 ===================================================
 Seed=mix(random1,random2);
 Data[]=MSLList::SetAllData();
 MAC=hmac(Seed+Data,MAuth);
 encode(Seed+Data+MAC,ENC_Net1);
    check:
            Seed,
            MAC
    attempt:
            MSLList::GetAllData();

*/
//---------------------------------------------------------------------------
#pragma pack(push, 1)

struct MSendHello
{
    unsigned char Version;      // Версия сетевого интерфейса
    unsigned int Seed;          // random сервера/клиента
    unsigned char MAC[MAC_Size];// MAC пакета
};

struct MSendRequest
{
    unsigned char Type;         // Тип запроса
    unsigned int Seed;          // Сеансовый ID (функция от random сервера и клиента)
    unsigned int Size;          // Размер последующих данных для приема
    unsigned char MAC[MAC_Size];// MAC пакета
};

#pragma pack(pop)
//---------------------------------------------------------------------------
class MSend
{
private:
    SOCKET lSocket;             // Сокет для ожидания соединений
    SOCKET rSocket;             // Сокет для соединения с клиентом
    HANDLE Thread;              // Дескриптор потока, осуществляющего отправку/прием
    DWORD ThreadID;             // ID потока
    unsigned NetCode;           // Ключ шифрования данных
    MAuth *NetMAC;              // Объект для вычисления и проверки MAC
    bool Init;                  // Флаг выполненной инициализации WinSock,NetCode,NetMAC

    union MPacket
    {
        MSendHello Hello;
        MSendRequest Request;
    } Packet;                   // Буфер для отправки/приема запросов

protected:
    bool Break;                 // Флаг прерывания сетевых операций

    // Операции с сокетами
    bool NetInit(unsigned Code_, MAuth *MAC_);  // Инициализация WinSocket
    bool NetFree();                             // Освобождение WinSocket
    bool Create(bool Srv_);
    bool Listen();      // Ожидать входящее соединения
    bool Accept();      // Принять входящее соединение
    bool Connect(const wchar_t *IP_, unsigned Time_);    // Создать исходящее соединение
    bool Snd(char *Data_, unsigned Size_, unsigned Time_);
    bool Rcv(char *Data_, unsigned Size_, unsigned Time_);
    bool Disconnect(unsigned Time_);            // Закрыть исходящее соединение
    void lClose();          // Закрыть слушающий сокет
    void rClose();          // Закрыть сокет удаленного соединения

    // Примитивы протокола
    bool SndHello(unsigned Seed_);
    bool RcvHello(unsigned *Seed_);
    bool SndRequest(unsigned char Type_, unsigned Seed_, unsigned Size_);
	bool RcvRequest(unsigned char *Type_, unsigned Seed_, unsigned *Size_);

	template <typename obj_type>
	bool SndObject(obj_type *Obj_, unsigned Type_, unsigned Seed_);

	template <typename obj_type>
	bool RcvObject(obj_type *Obj_, unsigned Size_, unsigned Seed_);

    // Операции с потоком отправки/приема
    static DWORD WINAPI ThreadF(LPVOID Data);
    virtual void ThreadP()=0;

    bool Start();
    void Stop();

public:
	MSend():
		lSocket(INVALID_SOCKET),
		rSocket(INVALID_SOCKET),
		Thread(nullptr),
		ThreadID(0),
		Break(false),
		NetCode(0),
		NetMAC(nullptr),
		Init(false)
	{
	}

	MSend(const MSend&) = delete;
	MSend(MSend&&) noexcept = delete;
	MSend& operator=(const MSend&) = delete;
	MSend& operator=(MSend&&) noexcept = delete;
	~MSend() = default;
};
//---------------------------------------------------------------------------
class MSendSrv: public MSend
{
private:
	HWND Window;                // Окно для обработки сообщений о процессе отправки
	UINT MinMsg;                //

	int Mode;                   			// Режим (что отправляем/запрашиваем)
	std::vector <MComputersItem*> *Comps;	// Массив со списком компьютеров для рассылки
	MComputersItem *Comp;					// Указатель на компьютер, для загрузки
	MGames *ObjGames;                   	// Объекты для
	MClOptions *ObjOptions;             	// отправки/приема

	void ThreadP();
	void ThreadSend();
	void ThreadGet();
	void Event(MComputersItem *Computer_, int Event_);

	// Генерация Seed со стороны сервера
	static MRandCounter SeedRand;
	static MWAPI::CRITICAL_SECTION CS_Seed;

	unsigned NextSeed()
	{
		MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Seed);  /// Для MSendSrv излишне
		return SeedRand++;
	}

public:
	bool NetInit(HWND Window_, UINT MinMsg_, unsigned Code_, MAuth *MAC_);
	bool NetFree();
	bool Send(std::vector <MComputersItem*> *Computers_, MGames *Games_, MClOptions *Options_);
	bool Get(MComputersItem *Computer_, MGames *Games_, MClOptions *Options_);
	void Stop();

	MSendSrv():
		Window(nullptr),
		Mode(mssNone),
		Comps(nullptr),
		Comp(nullptr),
		ObjGames(nullptr),
		ObjOptions(nullptr)
	{
	}

	MSendSrv(const MSendSrv&) = delete;
	MSendSrv(MSendSrv&&) noexcept = delete;
	MSendSrv& operator=(const MSendSrv&) = delete;
	MSendSrv& operator=(MSendSrv&&) noexcept = delete;
	~MSendSrv() = default;
};
//---------------------------------------------------------------------------
class MSendCl: public MSend
{
private:
    MStateCl *State;            // Кому сообщать об обновлении списка игр, настроек
    MClOptions Options;         // Буферные объекты
    MGames Games;               // для приема/отправки

    void ThreadP();

	// Генерация Seed со стороны клиента
	MRandCounter SeedRand;
	unsigned NextSeed() { return SeedRand++; }

public:
    bool NetInit(MStateCl *State_, unsigned Code_, MAuth *MAC_);
    bool NetFree();
    bool Start();
    void Stop();

	MSendCl():
		State(nullptr),
		SeedRand(
			std::chrono::system_clock::
			now().time_since_epoch().count())
	{
	}

	MSendCl(const MSendCl&) = delete;
	MSendCl(MSendCl&&) noexcept = delete;
	MSendCl& operator=(const MSendCl&) = delete;
	MSendCl& operator=(MSendCl&&) noexcept = delete;
	~MSendCl() = default;
};
//---------------------------------------------------------------------------
#endif

