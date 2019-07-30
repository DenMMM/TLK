//---------------------------------------------------------------------------
#ifndef UnitSyncH
#define UnitSyncH
//---------------------------------------------------------------------------
struct MSyncData;
struct MPacketHeader;
struct MPacketBasic;
struct MPacketData;
struct MPacketPwrOn;

class MSyncState;
class MSyncStates;
class MSync;
//---------------------------------------------------------------------------
#include <winsock2.h>
#include "UnitSLList.h"
#include "UnitComputers.h"
#include "UnitStates.h"
//---------------------------------------------------------------------------
#define SYNC_Version        0x31    // Версия сетевого интерфейса
#define SYNC_Port           7005    // Номер порта клиента
#define SYNC_WaitInterval   3000    // Интервал между сеансами посылки данных (в мсек.)
#define SYNC_SendInterval   200     // Задержка между посылками пакетов в сеансе (в мсек.)
#define SYNC_SendRetryes    3       // Количество посылок одного и того же пакета в сеансе
//---------------------------------------------------------------------------
#define MAC_AddrLength      6
//---------------------------------------------------------------------------
// Этапы процесса синхронизации
#define mspNone             0       //
#define mspWait             1       // Ожидание следующего сеанса и новых данных
#define mspSend1            2       // Отправка данных
#define mspMagic            3       // Отправка "MagicPacket"
//---------------------------------------------------------------------------
// Типы пакетов при обмене клиента и сервера (MPacketHeader.Type)
#define mptSyncData         1       // Пакет с данными для клиента
#define mptDataConfirm      2       // Подтверждение о получении клиентом данных
//---------------------------------------------------------------------------
struct MPacketHeader
{
    unsigned char Version;      // Версия сетевого интерфейса
    unsigned int SNumber;       // Номер сеанса
    unsigned char Type;         // Тип пакета
};

struct MSyncData
{
    __int64 SystemTime;         // Системное время, используемое при всех расчетах
    int Number;                 // Номер компьютера, с которым ассоциировано состояние
    unsigned State;             // Состояние компьютера (режимы работы)
    __int64 StartWorkTime;      // Время запуска компьютера в работу (абсолютное время)
    int SizeWorkTime;           // На какое время запущен компьютер (в минутах)
    __int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
    int SizeFineTime;           // Время ожидания в режиме штрафа (в минутах)
    __int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
    unsigned Programs;          // Групы программ, разрешенных для запуска
    unsigned Commands;          // Дополнительные команды для компьютера
};

struct MPacketBasic
{
    MPacketHeader Header;       // Заголовок
};

struct MPacketData
{
    MPacketHeader Header;       // Заголовок
    MSyncData Data;             // Данные для синхронизации с клиентом
};

struct MPacketWOL
{
    unsigned int ZeroFirst;                     // Нули в начале пакета
    unsigned char Sync[MAC_AddrLength];         // Последовательность для синхронизации
    unsigned char Magic[16][MAC_AddrLength];    // 16 раз MAC-адрес
};
//---------------------------------------------------------------------------
class MSyncState:public MSLListItem
{
private:
    MState *State;                      // Состояние компьютера, с которым связана запись
    u_long IP;                          // IP-адрес компьютера-клиента
    sockaddr_in Address;                // Адрес в формате socket
    bool KnownMAC;                      // Индикатор наличия известного MAC-адреса
    unsigned char MAC[MAC_AddrLength];  // MAC-адрес сетевой платы компьютера
    int Process;                        // Состояние процесса синхронизации
    int SendCount;                      // Счетчик посланных за сеанс пакетов
    DWORD LastSendTime;                 // Время последней отправки пакета
    unsigned int SNumber;               // Номер сеанса

    union MPacket
    {
        MPacketData PacketData;
        MPacketWOL PacketWOL;
    } Packet;                           // Пакет, отправляемый клиенту
    int PacketSize;                     // Размер отправляемого пакета

    // Генератор "уникальных" номеров сеансов
    static unsigned int NextSNumber();

public:
    bool Copy(MListItem *SrcItem_) { return false; }

    void Associate(MState *State_, u_long IP_);
    u_long GetIP();
    bool UpdateMAC(unsigned char *MAC_);
    bool Start();
    bool Stop();
    bool Send(SOCKET Socket_, SOCKET SocketBC_, unsigned Code_);
    bool Recv(char *Packet_, int PacketSize_, unsigned Code_);

    // Индикация процесса снхронизации
    unsigned GetCounter();

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MSyncState();
    ~MSyncState();
};
//---------------------------------------------------------------------------
class MSyncStates:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MSyncState; }
    void operator_delete(MListItem *DelItem_) { delete (MSyncState*)DelItem_; }
public:
    bool Copy(MList *SrcList_) { return false; }
public:
    // Вспомогательные функции
    MSyncState *Search(u_long IP_);
    bool Associate(MStates *States_, MComputers *Computers_);
    bool Start();
    bool Stop();

    MSyncStates() { constructor(); }
    ~MSyncStates() { destructor(); }
};
//---------------------------------------------------------------------------
class MSync
{
private:
    bool Init;                  // Была ли выполнена инициализация WinSocket
    bool AutoSaveARP;           // Автоматически сохранять ARP-кэш при обновлении MAC-адресов
    HANDLE Thread;              // Дескриптор потока, осуществляющего отправку/прием
    DWORD ThreadID;             // Идентификатор потока
    SOCKET Socket;              // Сокет для выполнения синхронизации
    SOCKET SocketBC;            // Сокет для отправки "Magic Packet"
    MPacketBasic Packet;        // Буфер для принимаемых от клиентов пакетов 
    unsigned NetCode;           // Код шифрования для сетевого обмена
    MStates *States;            // Cостояния компьютеров для синхронизации
    MSyncStates SyncStates;     // Состояния процесса синхронизации
    CRITICAL_SECTION CS_PCounter;
    unsigned Counter;

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    static bool PollData(SOCKET Socket_);
    static bool UpdateMAC(MSyncStates *States_);
    void ThreadExecute();

    void PCounterSet(unsigned Value_);
public:
    bool SetARPFile(char *File_, unsigned Code_, bool AutoSave_);
    bool SaveARP();
    bool LoadARP();

    bool NetInit(unsigned Code_);       // Инициализация WinSocket
    bool NetFree();                     // Освобождение WinSocket
    bool Associate(MStates *States_, MComputers *Computers_);
    bool Start();                       // Открытие сокетов и запуск потока синхронизации
    void Stop();                        // Остановка потока синхронизации и закрытие сокетов

    // Индикация процесса снхронизации
    unsigned GetCounterMax();
    unsigned GetCounter();

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
    unsigned NetCode;           // Код шифрования для сетевого обмена
    unsigned SNumber;           // Номер сеанса обмена с сервером
    unsigned Process;           // Состояние процесса синхронизации
    unsigned SendCount;         // Счетчик посланных за сеанс пакетов
    MStateCl *State;            // Состояние, с которым ассоциирован сетевой интерфейс
    MSyncData SyncData;         // Новые данные состояния

    union MPacket
    {
        MPacketData Data;
//        MPacketBasic Basic;
    } RcvPacket;               // Буфер для принимаемых от сервера пакетов
    MPacketBasic SndPacket;    // Пакет, отправляемый серверу
    sockaddr_in SndAddr;       // Адрес сервера, которому отправляется пакет

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    static bool PollData(SOCKET Socket_);
    void ThreadExecute();
    bool Recv(sockaddr_in *From_, char *Packet_, int PacketSize_);
    bool Send(SOCKET Socket_);
public:
    bool NetInit(unsigned Code_);       // Инициализация WinSocket
    bool NetFree();                     // Освобождение WinSocket
    void Associate(MStateCl *State_);
    bool Start();
    void Stop();

    MSyncCl();
    ~MSyncCl();
};
//---------------------------------------------------------------------------
#endif

