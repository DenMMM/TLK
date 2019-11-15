//---------------------------------------------------------------------------
#ifndef UnitSyncH
#define UnitSyncH
//---------------------------------------------------------------------------
#include <atomic>
#include <chrono>
#include <winsock2.h>

#include "UnitWinAPI.h"
#include "UnitSLList.h"
#include "UnitComputers.h"
#include "UnitStates.h"
#include "UnitSyncMsgs.h"
#include "UnitRandCounter.h"
//---------------------------------------------------------------------------
class MSyncStatesItem;
class MSyncStates;
class MSync;
class MSyncCl;
//---------------------------------------------------------------------------
#define SYNC_Version        0x31    // Версия сетевого интерфейса
#define SYNC_Port           7005    // Номер порта клиента
#define SYNC_WaitInterval   3000    // Интервал между сеансами посылки данных (в мсек.)
#define SYNC_SendInterval   256     // Задержка между посылками пакетов в сеансе (в мсек.)
#define SYNC_SendRetryes    3       // Количество посылок одного и того же пакета в сеансе
#define SYNC_MinSize        sizeof(MSyncPHello)
#define SYNC_MaxSize        sizeof(MSyncPData)
//---------------------------------------------------------------------------
// Этапы процесса синхронизации
#define mspNone             0       //
#define mspWait             1       // Ожидание новых данных/сеанса
#define mspHello            2       // Отправка своей части сеансового ID - seed
#define mspSyncData         3       // Отправка данных (сервер)
#define mspSyncConf         4       // Отправка данных (клиент)
#define mspMagic            5       // Отправка WOL magic-пакета (сервер)
//---------------------------------------------------------------------------
// Типы пакетов при обмене клиента и сервера (MPacketHeader.Type)
#define mptHello            1       // hello от сервера клиенту и наоборот
#define mptSyncData         2       // Пакет с данными для клиента (сервер)
#define mptSyncConf         3       // Подтверждение о получении данных (клиент)
//---------------------------------------------------------------------------
/*
 сервер:mspWait       <>      клиент:mspWait
 ===================================================
    attempt:
            MState::NetSyncNewData();
            MState::NetSyncData(MSyncData);

 сервер:mspHello      =>      клиент:mspWait
 ===================================================
 MSyncPHello.Header.Version=SYNC_Version;
 MSyncPHello.Header.Type=mptHello;
 MSyncPHello.Header.Seed=random1;
 MSyncPHello.MAC=hmac(MSyncPHello.Header,MAuth);
 encode(MSyncPHello,ENC_Net1);
                            check:
                                    Header.Version,
                                    Header.Type,
                                    MAC

 сервер:mspHello      <=      клиент:mspHello
 ===================================================
 MSyncPHello.Header.Version=SYNC_Version;
 MSyncPHello.Header.Type=mptHello;
 MSyncPHello.Header.Seed=random2;
 MSyncPHello.MAC=hmac(MSyncPHello.Header,MAuth);
 encode(MSyncPHello,ENC_Net1);
    check:
            Header.Version,
            Header.Type,
            MAC

 сервер:mspSyncData   =>      клиент:mspHello
 ===================================================
 MSyncPData.Header.Version=SYNC_Version;
 MSyncPData.Header.Type=mptSyncData;
 MSyncPData.Header.Seed=mix(random1,random2);
 MSyncPData.Data...
 MSyncPData.MAC=mac(MSyncPData.Header+MSyncPData.Data,MAuth);
 encode(MSyncPData,ENC_Net1);
                            check:
                                    Header.Version,
                                    Header.Type,
                                    Header.Seed, (anti-replay)
                                    MAC

 сервер:mspSyncData   <=      клиент:mspSyncConf
 ===================================================
 MSyncPConf.Header.Version=SYNC_Version;
 MSyncPConf.Header.Type=mptSynConf;
 MSyncPConf.Header.Seed=mix(random1,random2);
 MSyncPData.MAC=mac(MSyncPConf.Header,MAuth);
 encode(MSyncPData,ENC_Net1);
    check:
            Header.Version,
            Header.Type,
            Header.Seed, (anti-replay)
			MAC
	attempt:
			MState::NetSyncExecuted(true);

 сервер:mspWait       ==      клиент:mspWait
 ===================================================
							attempt:
									MStateCl::NewSyncData(MSyncPData.Data);

*/
//---------------------------------------------------------------------------
class MSyncStatesItem:
	public MSLListItemSimple <
		MSLListItem <MSyncStates, MSyncStatesItem>,
		MSyncStatesItem>
{
private:
	MStatesItem *State;					// Состояние компьютера, с которым связана запись
	u_long IP;                          // IP-адрес компьютера-клиента
	sockaddr_in Address;                // Адрес в формате socket
	bool KnownMAC;                      // Индикатор наличия известного MAC-адреса
	unsigned char MAC[MAC_AddrLength];  // MAC-адрес сетевой платы компьютера
	unsigned Process;                   // Состояние процесса синхронизации
	unsigned Seed;                      // ID сеанса
	unsigned SendCount;                 // Счетчик посланных за сеанс пакетов
	DWORD LastSendTime;                 // Время последней отправки пакета

	MSyncData SyncData;                 // Данные запрошенные у MState, но еще не отправленные
	union MPacket
	{
		MSyncPHello Hello;
		MSyncPData Data;
		MSyncPWOL WOL;
	} Packet;                           // Пакет, отправляемый клиенту
	int PacketSize;                     // Размер отправляемого пакета

	// Генерация Seed со стороны сервера
	static MRandCounter SeedRand;
	static MWAPI::CRITICAL_SECTION CS_Seed;

	unsigned NextSeed()
	{
		MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Seed);  /// Для MSync излишне
		return SeedRand++;
	}

public:
	// Функции механизма сохранения/загрузки данных
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

	// Связать MSyncState с MState и IP-адресом компьютера
	void Associate(MStatesItem *State_, u_long IP_);
	// Подготовиться к отправке/приему
	bool Start();
	// Завершить операции, проверить нужно ли сохранить MState
	bool Stop();
	// Выполнить операцию отправки пакета, если требуется
	bool Send(SOCKET Socket_, SOCKET SocketBC_, unsigned Code_, MAuth *MAC_);
	// Обработать принятый с ассоциированного IP пакет
	bool Recv(char *Packet_, int PacketSize_, unsigned Code_, MAuth *MAC_);
	// Пытаемся обновить MAC для этого IP
	bool UpdateMAC(unsigned char *MAC_);
	// Узнать IP, с которым обмениваемся пакетами
	u_long gIP() const { return IP; }
	// Узнать сколько пакетов отправили (для условной индикации процесса)
	unsigned gPCount() const { return Process==mspWait? 0: SendCount+1; }

	MSyncStatesItem():
		State(nullptr),
		IP(INADDR_NONE),
		KnownMAC(false),
		Process(mspNone),
		PacketSize(0),
		SendCount(0),
		LastSendTime(0),
		Seed(0)
	{
	}
};

class MSyncStates:
	public MSLListSimple <
		MSLList <MSyncStates, MSyncStatesItem>,
		MSyncStatesItem>
{
public:
	// Создать объекты синхронизации для компьютеров и связать с их MState
	void Associate(MStates *States_, MComputers *Computers_);
	// Пакетная обработка тех же операций для MSyncState
	bool Start() const;
	bool Stop() const;
	// Найти объект по IP-адресу
	const_iterator Search(u_long IP_) const;
	iterator Search(u_long IP_)
	{
		const auto *const_this=this;
		return const_cast_iter(const_this->Search(IP_));
	}
};
//---------------------------------------------------------------------------
class MSync
{
private:
    bool Init;                  // Была ли выполнена инициализация WinSocket
    bool AutoSaveARP;           // Сохранять ARP-кэш автоматически при обновлении MAC-адресов
    HANDLE Thread;              // Дескриптор потока, осуществляющего отправку/прием
    DWORD ThreadID;             // Идентификатор потока
    SOCKET Socket;              // Сокет для выполнения синхронизации
    SOCKET SocketBC;            // Сокет для отправки "Magic Packet"
    unsigned NetCode;           // Код шифрования для сетевого обмена
    MAuth *NetMAC;              // Аутентификатор сетевых операций
    MStates *States;            // Cостояния компьютеров для синхронизации
    MSyncStates SyncStates;     // Объекты процесса синхронизации

	std::atomic_uint PCount;	// Счетчик отправленных пакетов (индикация)

    union MPacket
    {
        MSyncPHello Hello;
        MSyncPConf Conf;
    } Packet;                   // Буфер для принимаемых пакетов

    static DWORD WINAPI ThreadFunc(LPVOID Data);
	static bool PollData(SOCKET Socket_);
    static bool UpdateMAC(MSyncStates *States_);
    void ThreadExecute();

	void sPCount(unsigned Value_) noexcept { PCount.store(Value_); }

public:
    bool NetInit(unsigned Code_, MAuth *MAC_);  // Инициализация WinSocket
    bool NetFree();                             // Освобождение WinSocket
    bool Associate(MStates *States_, MComputers *Computers_);
    bool Start();                               // Создание сокетов и запуск потока синхронизации
    void Stop();                                // Остановка потока синхронизации и закрытие сокетов

    void SetARPFile(wchar_t *File_, unsigned Code_, bool AutoSave_);
    bool SaveARP() const { return SyncStates.Save(); }
    bool LoadARP() { return SyncStates.Load(); }
	DWORD gLastErr() const noexcept { return SyncStates.gLastErr(); }

	// Индикация процесса снхронизации
	unsigned gPCountMax() const noexcept { return SyncStates.gCount()*SYNC_SendRetryes; }
	unsigned gPCount() const noexcept { return PCount.load(); }

	MSync():
		Init(false),
		AutoSaveARP(false),
		Thread(nullptr),
		ThreadID(0),
		NetCode(0),
		NetMAC(nullptr),
		States(nullptr),
		Socket(INVALID_SOCKET),
		SocketBC(INVALID_SOCKET)
	{
	}

	MSync(const MSync&) = delete;
	MSync(MSync&&) = delete;
	MSync& operator=(const MSync&) = delete;
	MSync& operator=(MSync&&) = delete;

	~MSync()
	{
///		Stop();
	}
};
//---------------------------------------------------------------------------
class MSyncCl
{
private:
	bool Init;                  // Была ли выполнена инициализация WinSocket
	HANDLE Thread;              // Дескриптор потока, осуществляющего прием/отправку
	DWORD ThreadID;             // Идентификатор потока
	SOCKET Socket;              // Сокет для приема/отправки данных
	DWORD LastSendTime;         // Время последней отправки пакета
	unsigned Process;           // Состояние процесса синхронизации
	unsigned SendCount;         // Счетчик посланных за сеанс пакетов
	unsigned Seed;              // ID сеанса
	unsigned NetCode;           // Код шифрования для сетевого обмена
	MAuth *NetMAC;              // Аутентификатор сетевых операций
	MStateCl *State;            // Состояние, с которым ассоциирован сетевой интерфейс
	MSyncData SyncData;         // Новые данные состояния

	union MRcvPacket
	{
		MSyncPHello Hello;
		MSyncPData Data;
	} RcvPacket;                // Буфер для принимаемых от сервера пакетов

	union MSndPacket
	{
		MSyncPHello Hello;
		MSyncPConf Conf;
	} SndPacket;                // Буфер для отправляемых серверу пакетов
	int SndPacketSize;          // Размер отправляемого пакета
	sockaddr_in SndAddr;        // Адрес сервера

	static DWORD WINAPI ThreadFunc(LPVOID Data);
	static bool PollData(SOCKET Socket_);
	void ThreadExecute();
	bool Recv(sockaddr_in *From_, char *Packet_, int PacketSize_);
	bool Send(SOCKET Socket_);

	// Генерация Seed со стороны клиента
	MRandCounter SeedRand;
	unsigned NextSeed() { return SeedRand++; }

public:
	bool NetInit(unsigned Code_, MAuth *MAC_);  // Инициализация WinSocket
	bool NetFree();                             // Освобождение WinSocket
	void Associate(MStateCl *State_);
	bool Start();
	void Stop();

	MSyncCl():
		Init(false),
		Thread(nullptr),
		ThreadID(0),
		Socket(INVALID_SOCKET),
		NetMAC(nullptr),
		State(nullptr),
		Process(mspNone),
		SendCount(0),
		LastSendTime(0),
		NetCode(0),
		SeedRand(
			std::chrono::system_clock::
			now().time_since_epoch().count())
	{
	}

	MSyncCl(const MSyncCl&) = delete;
	MSyncCl(MSyncCl&&) = delete;
	MSyncCl& operator=(const MSyncCl&) = delete;
	MSyncCl& operator=(MSyncCl&&) = delete;

	~MSyncCl()
	{
///		Stop();
	}
};
//---------------------------------------------------------------------------
#endif

