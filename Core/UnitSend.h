//---------------------------------------------------------------------------
#ifndef UnitSendH
#define UnitSendH
//---------------------------------------------------------------------------
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <winsock2.h>

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
#define SEND_Version        0x32    // Версия сетевого интерфейса
#define SEND_Port           7005    // Номер порта клиента
// Ограничения на размер данных в процессе обмена (MSendRequest.Size)
#define SEND_MinData        (sizeof(std::uint32_t)+MAC_Size)
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
	std::uint8_t Version;		// Версия сетевого интерфейса
	std::uint64_t SessId;		// random сервера/клиента
	unsigned char MAC[MAC_Size];// MAC пакета
};

struct MSendRequest
{
	std::uint8_t Type;			// Тип запроса
	std::uint64_t SessId;		// Сеансовый ID (функция от random сервера и клиента)
	std::uint32_t Size;			// Размер последующих данных для приема
	unsigned char MAC[MAC_Size];// MAC пакета
};

#pragma pack(pop)
//---------------------------------------------------------------------------
class MSend
{
private:
    SOCKET lSocket;             // Сокет для ожидания соединений
    SOCKET rSocket;             // Сокет для соединения с клиентом
	std::thread Thread;			// Дескриптор потока, осуществляющего отправку/прием
	std::uint32_t NetCode;		// Ключ шифрования данных
	MAuth *NetMAC;              // Объект для вычисления и проверки MAC
    bool Init;                  // Флаг выполненной инициализации WinSock,NetCode,NetMAC

    union MPacket
    {
        MSendHello Hello;
        MSendRequest Request;
    } Packet;                   // Буфер для отправки/приема запросов

protected:
	std::atomic_bool Break;		// Флаг прерывания сетевых операций

    // Операции с сокетами
	bool NetInit(std::uint32_t Code_, MAuth *MAC_);	// Инициализация WinSocket
	bool NetFree();                             	// Освобождение WinSocket
    bool Create(bool Srv_);
    bool Listen();      // Ожидать входящее соединения
    bool Accept();      // Принять входящее соединение
    bool Connect(const wchar_t *IP_, unsigned Time_);    // Создать исходящее соединение
	bool Snd(char *Data_, std::size_t Size_, unsigned Time_);
	bool Rcv(char *Data_, std::size_t Size_, unsigned Time_);
    bool Disconnect(unsigned Time_);            // Закрыть исходящее соединение
    void lClose();          // Закрыть слушающий сокет
    void rClose();          // Закрыть сокет удаленного соединения

    // Примитивы протокола
	bool SndHello(std::uint64_t SessId_);
	bool RcvHello(std::uint64_t *SessId_);
	bool SndRequest(std::uint8_t Type_, std::uint64_t SessId_, std::size_t Size_);
	bool RcvRequest(std::uint8_t *Type_, std::uint64_t SessId_, std::size_t *Size_);

	template <typename obj_type>
	bool SndObject(obj_type *Obj_, std::uint8_t Type_, std::uint64_t SessId_);

	template <typename obj_type>
	bool RcvObject(obj_type *Obj_, std::size_t Size_, std::uint64_t SessId_);

    // Операции с потоком отправки/приема
    virtual void ThreadP()=0;

    bool Start();
    void Stop();

public:
	MSend():
		lSocket(INVALID_SOCKET),
		rSocket(INVALID_SOCKET),
//		Break(false),
		NetCode(0),
		NetMAC(nullptr),
		Init(false)
	{
	}

	MSend(const MSend&) = delete;
	MSend(MSend&&) = delete;
	MSend& operator=(const MSend&) = delete;
	MSend& operator=(MSend&&) = delete;
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
	static MRandCounter SessIdRand;
	static std::mutex mtxSessId;

	auto NextSessId()
	{
		std::lock_guard <std::mutex> lckObj(mtxSessId);	/// Для MSendSrv излишне
		return SessIdRand++;
	}

public:
	bool NetInit(HWND Window_, UINT MinMsg_, std::uint32_t Code_, MAuth *MAC_);
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
	MSendSrv(MSendSrv&&) = delete;
	MSendSrv& operator=(const MSendSrv&) = delete;
	MSendSrv& operator=(MSendSrv&&) = delete;
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
	MRandCounter SessIdRand;
	auto NextSessId() { return SessIdRand++; }

public:
	bool NetInit(MStateCl *State_, std::uint32_t Code_, MAuth *MAC_);
    bool NetFree();
    bool Start();
    void Stop();

	MSendCl():
		State(nullptr),
		SessIdRand(
			[]() -> std::uint64_t {				/// rand ?
				std::array <std::uint64_t, 4> rnd_v;

				LARGE_INTEGER cntr;
				rnd_v[0]=
					::QueryPerformanceCounter(&cntr)?
					cntr.QuadPart:
					::GetTickCount();

				rnd_v[1]=std::chrono::system_clock::
					now().time_since_epoch().count();
				rnd_v[2]=CalcHwMacHash();		/// заменить на IP-адрес ?
				rnd_v[3]=0x4E4C432D444E4553;	// ASCII 'SEND-CLN'

				return fasthash64(&rnd_v, sizeof(rnd_v), 0);
			}()
        )
	{
	}

	MSendCl(const MSendCl&) = delete;
	MSendCl(MSendCl&&) = delete;
	MSendCl& operator=(const MSendCl&) = delete;
	MSendCl& operator=(MSendCl&&) = delete;
	~MSendCl() = default;
};
//---------------------------------------------------------------------------
#endif

