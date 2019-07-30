//---------------------------------------------------------------------------
#ifndef UnitSyncH
#define UnitSyncH
//---------------------------------------------------------------------------
struct MSyncData;
struct MSyncPHeader;
struct MSyncPHello;
struct MSyncPData;
struct MSyncPConf;
struct MSyncPWOL;

class MSyncState;
class MSyncStates;
class MSync;
//---------------------------------------------------------------------------
#include <winsock2.h>
#include "UnitWinAPI.h"
#include "UnitSLList.h"
#include "UnitComputers.h"
#include "UnitStates.h"
#include "UnitAuth.h"
//---------------------------------------------------------------------------
#define SYNC_Version        0x31    // Версия сетевого интерфейса
#define SYNC_Port           7005    // Номер порта клиента
#define SYNC_WaitInterval   3000    // Интервал между сеансами посылки данных (в мсек.)
#define SYNC_SendInterval   256     // Задержка между посылками пакетов в сеансе (в мсек.)
#define SYNC_SendRetryes    3       // Количество посылок одного и того же пакета в сеансе
#define SYNC_MinSize        sizeof(MSyncPHello)
#define SYNC_MaxSize        sizeof(MSyncPData)
//---------------------------------------------------------------------------
#define MAC_AddrLength      6
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
#pragma pack(push, 1)

struct MSyncData
{
    __int64 SystemTime;         // Системное время, используемое при всех расчетах
    char Number;                // Номер компьютера, с которым ассоциировано состояние
    unsigned State;             // Состояние компьютера (режимы работы)
    __int64 StartWorkTime;      // Время запуска в работу (абсолютное время)
    int SizeWorkTime;           // На какое время запущен (в минутах)
    __int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
    int SizeFineTime;           // Время ожидания в режиме штрафа (в минутах)
    __int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
    unsigned Programs;          // Групы программ, разрешенных для запуска
    unsigned Commands;          // Дополнительные команды
};

struct MSyncPHeader
{
    unsigned char Version;      // Версия сетевого интерфейса
    unsigned char Type;         // Тип пакета
    unsigned int Seed;          // Сеансовый ID (функция от random сервера и клиента)
};

struct MSyncPHello
{
    MSyncPHeader Header;        // Заголовок
    unsigned char MAC[MAC_Size];// MAC пакета
};

struct MSyncPData
{
    MSyncPHeader Header;        // Заголовок
    MSyncData Data;             // Данные для синхронизации с клиентом
    unsigned char MAC[MAC_Size];// MAC пакета
};

struct MSyncPConf
{
    MSyncPHeader Header;        // Заголовок
    unsigned char MAC[MAC_Size];// MAC пакета
};

struct MSyncPWOL
{
    unsigned int ZeroFirst;                     // Нули в начале пакета
    unsigned char Sync[MAC_AddrLength];         // Последовательность для синхронизации
    unsigned char Magic[16][MAC_AddrLength];    // 16 раз MAC-адрес
};

#pragma pack(pop)
//---------------------------------------------------------------------------
class MSyncState:public MSLListItem
{
private:
    void Copy(const MListItem *SrcItem_) {}
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

    MState *State;                      // Состояние компьютера, с которым связана запись
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

public:
    // Связать MSyncState с MState и IP-адресом компьютера
    void Associate(MState *State_, u_long IP_);
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

    MSyncState();
    ~MSyncState();
};
//---------------------------------------------------------------------------
class MSyncStates:public MSLList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MSyncState; }
    void item_del(MListItem *Item_) const { delete (MSyncState*)Item_; }

public:
    // Создать объекты синхронизации для компьютеров и связать с их MState
    void Associate(MStates *States_, MComputers *Computers_);
    // Пакетная обработка тех же операций для MSyncState
    bool Start() const;
    bool Stop() const;
    // Найти объект по IP-адресу
    MSyncState *Search(u_long IP_) const;

    MSyncStates() {}
    ~MSyncStates() { Clear(); }
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

    unsigned PCount;                            // Общий счетчик отправленных пакетов (индикация)
    mutable MWAPI::CRITICAL_SECTION CS_PCount;  // и объект синхронизации доступа потоков к нему

    union MPacket
    {
        MSyncPHello Hello;
        MSyncPConf Conf;
    } Packet;                   // Буфер для принимаемых пакетов

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    static bool PollData(SOCKET Socket_);
    static bool UpdateMAC(MSyncStates *States_);
    void ThreadExecute();
    void sPCount(unsigned Value_);      // thread safe

public:
    bool NetInit(unsigned Code_, MAuth *MAC_);  // Инициализация WinSocket
    bool NetFree();                             // Освобождение WinSocket
    bool Associate(MStates *States_, MComputers *Computers_);
    bool Start();                               // Создание сокетов и запуск потока синхронизации
    void Stop();                                // Остановка потока синхронизации и закрытие сокетов

    void SetARPFile(char *File_, unsigned Code_, bool AutoSave_);
    bool SaveARP() const { return SyncStates.Save(); }
    bool LoadARP() { return SyncStates.Load(); }
    DWORD gLastErr() const { return SyncStates.gLastErr(); }

    // Индикация процесса снхронизации
    unsigned gPCountMax() const;        // thread safe
    unsigned gPCount() const;           // thread safe

    MSync();
    ~MSync();
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
    
public:
    bool NetInit(unsigned Code_, MAuth *MAC_);  // Инициализация WinSocket
    bool NetFree();                             // Освобождение WinSocket
    void Associate(MStateCl *State_);
    bool Start();
    void Stop();

    MSyncCl();
    ~MSyncCl();
};
//---------------------------------------------------------------------------
#endif

