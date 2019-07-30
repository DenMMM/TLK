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
#define SYNC_Version        0x30    // ������ �������� ���������� �������������
#define SYNC_SyncPort       7005    // ����� ����� ��� �������� ������ �������
#define SYNC_WaitInterval   3000    // �������� ����� �������� ������� ������ (� ����.)
#define SYNC_SendInterval   200     // �������� ����� ��������� ������� � ������ (� ����.)
#define SYNC_SendRetryes    5       // ���������� ������� ������ � ���� �� ������ � ������
/*
    ������ �������� ���� 10����/���.
������� ������ ������� (200����)                    0.000160 ���.
�������� ������ �� �������                          0.000640 ���.
������ ���� �������� ������� (�� 100 �����������)   0.016000 ���.

    ��������� �������� ���� (%10 �� 10����/���. - 1����/���.)
������� ������ ������� (200����)                    0.001600 ���.
�������� ������ �� �������                          0.006400 ���.
������ ���� �������� ������� (�� 100 �����������)   0.160000 ���.
*/
//---------------------------------------------------------------------------
#define MAC_AddrLength 6
//---------------------------------------------------------------------------
// Main Sync Process
#define mspNone     0       //
#define mspWait     1       // �������� ���������� ������ � ����� ������
#define mspSend1    2       // �������� ������
#define mspMagic    3       // �������� "MagicPacket"
//---------------------------------------------------------------------------
// Main Packet Type
#define mptSyncData     1   // ����� � ������� ��� �������
#define mptDataConfirm  2   // ������������� � ��������� �������� ������
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
    MState *State;                      // ��������� ����������, � ������� ������� ������
    u_long IP;                          // IP-����� ����������
    sockaddr_in Address;
    bool KnownMAC;                      // ��������� ������� ���������� MAC-������
    unsigned char MAC[MAC_AddrLength];  // MAC-����� ������� ����� ����������
    int Process;                        // ��������� �������� �������������
    int SendCount;                      // ������� ��������� �� ����� �������
    DWORD LastSendTime;                 // ����� ��������� �������� ������
    unsigned int SNumber;               // ����� ������

    union MPacket
    {
        MPacketData PacketData;
        MPacketPwrOn PacketPwrOn;
    } Packet;                           // �����, ������������ �������

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
    bool Init;
    bool AutoSaveARP;           // ������������� ��������� ARP-��� ��� ���������� MAC-�������
    HANDLE Thread;              // ���������� ������, ��������������� ��������/�����
    DWORD ThreadID;             // ������������� ������
    MSyncStates SyncStates;     // ��������� �������� �������������
    MStates *States;            //
    SOCKET Socket;              // ����� ��� ���������� �������������
    SOCKET SocketBC;            // ����� ��� �������� "Magic Packet"
    MPacketBasic Packet;        // ����� ��� ������ ������� �� ��������
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

    // ��������� �������� ������������
    unsigned GetCounterMax();
    unsigned GetCounter();

    MSync();
    ~MSync();
};
//---------------------------------------------------------------------------
#endif

