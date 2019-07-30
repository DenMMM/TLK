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
#define SYNC_Version        0x31    // ������ �������� ����������
#define SYNC_Port           7005    // ����� ����� �������
#define SYNC_WaitInterval   3000    // �������� ����� �������� ������� ������ (� ����.)
#define SYNC_SendInterval   200     // �������� ����� ��������� ������� � ������ (� ����.)
#define SYNC_SendRetryes    3       // ���������� ������� ������ � ���� �� ������ � ������
//---------------------------------------------------------------------------
#define MAC_AddrLength      6
//---------------------------------------------------------------------------
// ����� �������� �������������
#define mspNone             0       //
#define mspWait             1       // �������� ���������� ������ � ����� ������
#define mspSend1            2       // �������� ������
#define mspMagic            3       // �������� "MagicPacket"
//---------------------------------------------------------------------------
// ���� ������� ��� ������ ������� � ������� (MPacketHeader.Type)
#define mptSyncData         1       // ����� � ������� ��� �������
#define mptDataConfirm      2       // ������������� � ��������� �������� ������
//---------------------------------------------------------------------------
struct MPacketHeader
{
    unsigned char Version;      // ������ �������� ����������
    unsigned int SNumber;       // ����� ������
    unsigned char Type;         // ��� ������
};

struct MSyncData
{
    __int64 SystemTime;         // ��������� �����, ������������ ��� ���� ��������
    int Number;                 // ����� ����������, � ������� ������������� ���������
    unsigned State;             // ��������� ���������� (������ ������)
    __int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
    int SizeWorkTime;           // �� ����� ����� ������� ��������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;           // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� ������� ��� ����������
};

struct MPacketBasic
{
    MPacketHeader Header;       // ���������
};

struct MPacketData
{
    MPacketHeader Header;       // ���������
    MSyncData Data;             // ������ ��� ������������� � ��������
};

struct MPacketWOL
{
    unsigned int ZeroFirst;                     // ���� � ������ ������
    unsigned char Sync[MAC_AddrLength];         // ������������������ ��� �������������
    unsigned char Magic[16][MAC_AddrLength];    // 16 ��� MAC-�����
};
//---------------------------------------------------------------------------
class MSyncState:public MSLListItem
{
private:
    MState *State;                      // ��������� ����������, � ������� ������� ������
    u_long IP;                          // IP-����� ����������-�������
    sockaddr_in Address;                // ����� � ������� socket
    bool KnownMAC;                      // ��������� ������� ���������� MAC-������
    unsigned char MAC[MAC_AddrLength];  // MAC-����� ������� ����� ����������
    int Process;                        // ��������� �������� �������������
    int SendCount;                      // ������� ��������� �� ����� �������
    DWORD LastSendTime;                 // ����� ��������� �������� ������
    unsigned int SNumber;               // ����� ������

    union MPacket
    {
        MPacketData PacketData;
        MPacketWOL PacketWOL;
    } Packet;                           // �����, ������������ �������
    int PacketSize;                     // ������ ������������� ������

    // ��������� "����������" ������� �������
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

    // ��������� �������� ������������
    unsigned GetCounter();

    // ������� ��������� ����������/�������� ������
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
    // ��������������� �������
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
    bool Init;                  // ���� �� ��������� ������������� WinSocket
    bool AutoSaveARP;           // ������������� ��������� ARP-��� ��� ���������� MAC-�������
    HANDLE Thread;              // ���������� ������, ��������������� ��������/�����
    DWORD ThreadID;             // ������������� ������
    SOCKET Socket;              // ����� ��� ���������� �������������
    SOCKET SocketBC;            // ����� ��� �������� "Magic Packet"
    MPacketBasic Packet;        // ����� ��� ����������� �� �������� ������� 
    unsigned NetCode;           // ��� ���������� ��� �������� ������
    MStates *States;            // C�������� ����������� ��� �������������
    MSyncStates SyncStates;     // ��������� �������� �������������
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

    bool NetInit(unsigned Code_);       // ������������� WinSocket
    bool NetFree();                     // ������������ WinSocket
    bool Associate(MStates *States_, MComputers *Computers_);
    bool Start();                       // �������� ������� � ������ ������ �������������
    void Stop();                        // ��������� ������ ������������� � �������� �������

    // ��������� �������� ������������
    unsigned GetCounterMax();
    unsigned GetCounter();

    MSync();
    ~MSync();
};
//---------------------------------------------------------------------------
class MSyncCl
{
private:
    bool Init;                  // ���� �� ��������� ������������� WinSocket
    HANDLE Thread;              // ���������� ������, ��������������� �����/��������
    DWORD ThreadID;             // ������������� ������
    SOCKET Socket;              // ����� ��� ������/�������� ������
    DWORD LastSendTime;         // ����� ��������� �������� ������
    unsigned NetCode;           // ��� ���������� ��� �������� ������
    unsigned SNumber;           // ����� ������ ������ � ��������
    unsigned Process;           // ��������� �������� �������������
    unsigned SendCount;         // ������� ��������� �� ����� �������
    MStateCl *State;            // ���������, � ������� ������������ ������� ���������
    MSyncData SyncData;         // ����� ������ ���������

    union MPacket
    {
        MPacketData Data;
//        MPacketBasic Basic;
    } RcvPacket;               // ����� ��� ����������� �� ������� �������
    MPacketBasic SndPacket;    // �����, ������������ �������
    sockaddr_in SndAddr;       // ����� �������, �������� ������������ �����

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    static bool PollData(SOCKET Socket_);
    void ThreadExecute();
    bool Recv(sockaddr_in *From_, char *Packet_, int PacketSize_);
    bool Send(SOCKET Socket_);
public:
    bool NetInit(unsigned Code_);       // ������������� WinSocket
    bool NetFree();                     // ������������ WinSocket
    void Associate(MStateCl *State_);
    bool Start();
    void Stop();

    MSyncCl();
    ~MSyncCl();
};
//---------------------------------------------------------------------------
#endif

