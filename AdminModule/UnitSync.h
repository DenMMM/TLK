//---------------------------------------------------------------------------
#ifndef UnitSyncH
#define UnitSyncH
//---------------------------------------------------------------------------
#include <winsock2.h>
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitStates.h"
#include "UnitComputers.h"
//---------------------------------------------------------------------------
#define SYNC_Version        0x30    // Версия сетевого интерфейса синхронизации
#define SYNC_SyncPort       7005    // Номер порта для отправки данных клиенту
#define SYNC_WaitInterval   3000    // Интервал между сеансами посылки данных (в мсек.)
#define SYNC_SendInterval   200     // Задержка между посылками пакетов в сеансе (в мсек.)
#define SYNC_SendRetryes    5       // Количество посылок одного и того же пакета в сеансе
/*
    Полная скорость сети 10Мбит/сек.
Оправка пакета клиенту (200байт)                    0.000160 сек.
Ожидание пакета от клиента                          0.000640 сек.
Полный цикл отправки пакетов (на 100 компьютеров)   0.016000 сек.

    Частичная скорость сети (%10 от 10Мбит/сек. - 1Мбит/сек.)
Оправка пакета клиенту (200байт)                    0.001600 сек.
Ожидание пакета от клиента                          0.006400 сек.
Полный цикл отправки пакетов (на 100 компьютеров)   0.160000 сек.
*/
//---------------------------------------------------------------------------
#define MAC_AddrLength 6
//---------------------------------------------------------------------------
// Main Sync Process
#define mspNone     0       //
#define mspWait     1       // Ожидание следующего сеанса и новых данных
#define mspSend1    2       // Отправка данных
#define mspMagic    3       // Отправка "MagicPacket"
//---------------------------------------------------------------------------
// Main Packet Type
#define mptSyncData     1   // Пакет с данными для клиента
#define mptDataConfirm  2   // Подтверждение о получении клиентом данных
//---------------------------------------------------------------------------
struct MPacketHeader;
struct MPacketBasic;
struct MPacketData;
struct MPacketPwrOn;
//---------------------------------------------------------------------------
class MSyncState;
class MSyncStates;
class MSync;
//---------------------------------------------------------------------------
struct MPacketHeader
{
    unsigned char Version;      // Версия сетевого интерфейса
    unsigned int SNumber;       // Номер сеанса
    unsigned char Type;         // Тип пакета
};

struct MPacketBasic
{
    MPacketHeader Header;
};

struct MPacketData
{
    MPacketHeader Header;
    MSyncData Data;
};

struct MPacketPwrOn
{
    unsigned int ZeroFirst;
    unsigned char Sync[MAC_AddrLength];
    unsigned char Magic[16][MAC_AddrLength];
};
//---------------------------------------------------------------------------
class MSyncState:public MSLListItem
{
public:
    bool Copy(MListItem *SrcItem_) { return false; }
private:
    MState *State;                      // Состояние компьютера, с которым связана запись
    u_long IP;                          // IP-адрес компьютера
    sockaddr_in Address;
    bool KnownMAC;                      // Индикатор наличия известного MAC-адреса
    unsigned char MAC[MAC_AddrLength];  // MAC-адрес сетевой платы компьютера
    int Process;                        // Состояние процесса синхронизации
    int SendCount;                      // Счетчик посланных за сеанс пакетов
    DWORD LastSendTime;                 // Время последней отправки пакета
    unsigned int SNumber;               // Номер сеанса

    union MPacket
    {
        MPacketData PacketData;
        MPacketPwrOn PacketPwrOn;
    } Packet;                           // Пакет, отправляемый клиенту

    int PacketSize;

    static unsigned int NextSNumber();

public:
    void Associate(MState *State_, u_long IP_);
    u_long GetIP();
    bool UpdateMAC(unsigned char *MAC_);
    bool Start();
    bool Stop();
    bool Send(SOCKET Socket_, SOCKET SocketBC_);
    bool Recv(char *Packet_, int PacketSize_);

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
    bool Init;
    bool AutoSaveARP;           // Автоматически сохранять ARP-кэш при обновлении MAC-адресов
    HANDLE Thread;              // Дескриптор потока, осуществляющего отправку/прием
    DWORD ThreadID;             // Идентификатор потока
    MSyncStates SyncStates;     // Состояния процесса синхронизации
    MStates *States;            //
    SOCKET Socket;              // Сокет для выполнения синхронизации
    SOCKET SocketBC;            // Сокет для отправки "Magic Packet"
    MPacketBasic Packet;        // Буфер для приема пакетов от клиентов
    CRITICAL_SECTION CS_PCounter;
    unsigned Counter;

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    static bool PollData(SOCKET Socket_);
    static bool UpdateMAC(MSyncStates *States_);
    void ThreadExecute();

    void PCounterSet(unsigned Value_);
public:
    bool SetARPFile(char *File_, unsigned Code_);
    void SetAutoSaveARP(bool AutoSave_);
    bool SaveARP();
    bool LoadARP();

    bool NetInit();
    bool NetFree();
    bool Associate(MStates *States_, MComputers *Computers_);
    bool Start();
    void Stop();

    // Индикация процесса снхронизации
    unsigned GetCounterMax();
    unsigned GetCounter();

    MSync();
    ~MSync();
};
//---------------------------------------------------------------------------
#endif

