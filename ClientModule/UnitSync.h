//---------------------------------------------------------------------------
#ifndef UnitSyncH
#define UnitSyncH
//---------------------------------------------------------------------------
#include <winsock2.h>
//---------------------------------------------------------------------------
#include "UnitState.h"
//---------------------------------------------------------------------------
#define SYNC_Version        0x30    // ������ �������� ���������� �������������
#define SYNC_SyncPort       7005    // ����� ����� ��� ������ ������
#define SYNC_SendInterval   100     // �������� ����� ��������� ������� � ������ (� ����.)
#define SYNC_SendRetryes    5       // ���������� ������� ������ �� ���� �����
//---------------------------------------------------------------------------
// Main Sync Process
#define mspNone     0       //
#define mspWait     1       // �������� ���������� ������ � ����� ������
#define mspSend1    2       // �������� ������������� � ��������� ������
//---------------------------------------------------------------------------
// Main Packet Type
#define mptSyncData     1       // ����� � ������� ��� �������
#define mptDataConfirm  2       // ������������� � ��������� �������� ������
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
    unsigned char Version;      // ������ �������� ����������
    unsigned int SNumber;       // ����� ������
    unsigned char Type;         // ��� ������
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
    HANDLE Thread;              // ���������� ������, ��������������� ��������/�����
    DWORD ThreadID;             // ������������� ������
    MState *State;              // ���������, � ������� ������������ ������� ���������
    SOCKET Socket;              // ����� ��� ������/�������� ������

    sockaddr_in Address;        // ����� �������, � ������� ���� ����� �������
    unsigned int SNumber;       // ����� ������
    int Process;                // ��������� �������� �������������
    int SendCount;              // ������� ��������� �� ����� �������
    DWORD LastSendTime;         // ����� ��������� �������� ������
    MSyncData SyncData;         // ����� ������ ���������

    union MPacket
    {
        MPacketData Data;
        MPacketBasic Basic;
    } RecvPacket;               // �����, ����������� �� �������
    MPacketBasic SendPacket;    // �����, ������������ �������

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

