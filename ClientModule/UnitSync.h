//---------------------------------------------------------------------------
#ifndef UnitSyncH
#define UnitSyncH
//---------------------------------------------------------------------------
#include <winsock2.h>
//---------------------------------------------------------------------------
#include "UnitState.h"
//---------------------------------------------------------------------------
#define SYNC_Version        0x30    // Версия сетевого интерфейса синхронизации
#define SYNC_SyncPort       7005    // Номер порта для приема данных
#define SYNC_SendInterval   100     // Задержка между посылками пакетов в сеансе (в мсек.)
#define SYNC_SendRetryes    5       // Количество посылок пакета за один сеанс
//---------------------------------------------------------------------------
// Main Sync Process
#define mspNone     0       //
#define mspWait     1       // Ожидание следующего сеанса и новых данных
#define mspSend1    2       // Отправка подтверждения о получении данных
//---------------------------------------------------------------------------
// Main Packet Type
#define mptSyncData     1       // Пакет с данными для клиента
#define mptDataConfirm  2       // Подтверждение о получении клиентом данных
//---------------------------------------------------------------------------
struct MPacketHeader;
struct MPacketBasic;
struct MPacketData;
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
//---------------------------------------------------------------------------
class MSync
{
private:
    bool Init;
    HANDLE Thread;              // Дескриптор потока, осуществляющего отправку/прием
    DWORD ThreadID;             // Идентификатор потока
    MState *State;              // Состояние, с которым ассоциирован сетевой интерфейс
    SOCKET Socket;              // Сокет для приема/отправки данных

    sockaddr_in Address;        // Адрес сервера, с которым идет обмен данными
    unsigned int SNumber;       // Номер сеанса
    int Process;                // Состояние процесса синхронизации
    int SendCount;              // Счетчик посланных за сеанс пакетов
    DWORD LastSendTime;         // Время последней отправки пакета
    MSyncData SyncData;         // Новые данные состояния

    union MPacket
    {
        MPacketData Data;
        MPacketBasic Basic;
    } RecvPacket;               // Пакет, принимаемый от сервера
    MPacketBasic SendPacket;    // Пакет, отправляемый серверу

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    static bool PollData(SOCKET Socket_);
    void ThreadExecute();
    bool Recv(sockaddr_in *From_, char *Packet_, int PacketSize_);
    bool Send(SOCKET Socket_);
public:
    bool NetInit();
    bool NetFree();
    void Associate(MState *State_);
    bool Start();
    void Stop();

    MSync();
    ~MSync();
};
//---------------------------------------------------------------------------
#endif

