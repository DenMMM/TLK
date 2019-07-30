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
#define SYNC_Version        0x31    // ������ �������� ����������
#define SYNC_Port           7005    // ����� ����� �������
#define SYNC_WaitInterval   3000    // �������� ����� �������� ������� ������ (� ����.)
#define SYNC_SendInterval   256     // �������� ����� ��������� ������� � ������ (� ����.)
#define SYNC_SendRetryes    3       // ���������� ������� ������ � ���� �� ������ � ������
#define SYNC_MinSize        sizeof(MSyncPHello)
#define SYNC_MaxSize        sizeof(MSyncPData)
//---------------------------------------------------------------------------
#define MAC_AddrLength      6
//---------------------------------------------------------------------------
// ����� �������� �������������
#define mspNone             0       //
#define mspWait             1       // �������� ����� ������/������
#define mspHello            2       // �������� ����� ����� ���������� ID - seed
#define mspSyncData         3       // �������� ������ (������)
#define mspSyncConf         4       // �������� ������ (������)
#define mspMagic            5       // �������� WOL magic-������ (������)
//---------------------------------------------------------------------------
// ���� ������� ��� ������ ������� � ������� (MPacketHeader.Type)
#define mptHello            1       // hello �� ������� ������� � ��������
#define mptSyncData         2       // ����� � ������� ��� ������� (������)
#define mptSyncConf         3       // ������������� � ��������� ������ (������)
//---------------------------------------------------------------------------
/*
 ������:mspWait       <>      ������:mspWait
 ===================================================
    attempt:
            MState::NetSyncNewData();
            MState::NetSyncData(MSyncData);

 ������:mspHello      =>      ������:mspWait
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

 ������:mspHello      <=      ������:mspHello
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

 ������:mspSyncData   =>      ������:mspHello
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

 ������:mspSyncData   <=      ������:mspSyncConf
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

 ������:mspWait       ==      ������:mspWait
 ===================================================
                            attempt:
                                    MStateCl::NewSyncData(MSyncPData.Data);

*/
//---------------------------------------------------------------------------
#pragma pack(push, 1)

struct MSyncData
{
    __int64 SystemTime;         // ��������� �����, ������������ ��� ���� ��������
    char Number;                // ����� ����������, � ������� ������������� ���������
    unsigned State;             // ��������� ���������� (������ ������)
    __int64 StartWorkTime;      // ����� ������� � ������ (���������� �����)
    int SizeWorkTime;           // �� ����� ����� ������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;           // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� �������
};

struct MSyncPHeader
{
    unsigned char Version;      // ������ �������� ����������
    unsigned char Type;         // ��� ������
    unsigned int Seed;          // ��������� ID (������� �� random ������� � �������)
};

struct MSyncPHello
{
    MSyncPHeader Header;        // ���������
    unsigned char MAC[MAC_Size];// MAC ������
};

struct MSyncPData
{
    MSyncPHeader Header;        // ���������
    MSyncData Data;             // ������ ��� ������������� � ��������
    unsigned char MAC[MAC_Size];// MAC ������
};

struct MSyncPConf
{
    MSyncPHeader Header;        // ���������
    unsigned char MAC[MAC_Size];// MAC ������
};

struct MSyncPWOL
{
    unsigned int ZeroFirst;                     // ���� � ������ ������
    unsigned char Sync[MAC_AddrLength];         // ������������������ ��� �������������
    unsigned char Magic[16][MAC_AddrLength];    // 16 ��� MAC-�����
};

#pragma pack(pop)
//---------------------------------------------------------------------------
class MSyncState:public MSLListItem
{
private:
    void Copy(const MListItem *SrcItem_) {}
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

    MState *State;                      // ��������� ����������, � ������� ������� ������
    u_long IP;                          // IP-����� ����������-�������
    sockaddr_in Address;                // ����� � ������� socket
    bool KnownMAC;                      // ��������� ������� ���������� MAC-������
    unsigned char MAC[MAC_AddrLength];  // MAC-����� ������� ����� ����������
    unsigned Process;                   // ��������� �������� �������������
    unsigned Seed;                      // ID ������
    unsigned SendCount;                 // ������� ��������� �� ����� �������
    DWORD LastSendTime;                 // ����� ��������� �������� ������

    MSyncData SyncData;                 // ������ ����������� � MState, �� ��� �� ������������
    union MPacket
    {
        MSyncPHello Hello;
        MSyncPData Data;
        MSyncPWOL WOL;
    } Packet;                           // �����, ������������ �������
    int PacketSize;                     // ������ ������������� ������

public:
    // ������� MSyncState � MState � IP-������� ����������
    void Associate(MState *State_, u_long IP_);
    // ������������� � ��������/������
    bool Start();
    // ��������� ��������, ��������� ����� �� ��������� MState
    bool Stop();
    // ��������� �������� �������� ������, ���� ���������
    bool Send(SOCKET Socket_, SOCKET SocketBC_, unsigned Code_, MAuth *MAC_);
    // ���������� �������� � ���������������� IP �����
    bool Recv(char *Packet_, int PacketSize_, unsigned Code_, MAuth *MAC_);
    // �������� �������� MAC ��� ����� IP
    bool UpdateMAC(unsigned char *MAC_);
    // ������ IP, � ������� ������������ ��������
    u_long gIP() const { return IP; }
    // ������ ������� ������� ��������� (��� �������� ��������� ��������)
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
    // ������� ������� ������������� ��� ����������� � ������� � �� MState
    void Associate(MStates *States_, MComputers *Computers_);
    // �������� ��������� ��� �� �������� ��� MSyncState
    bool Start() const;
    bool Stop() const;
    // ����� ������ �� IP-������
    MSyncState *Search(u_long IP_) const;

    MSyncStates() {}
    ~MSyncStates() { Clear(); }
};
//---------------------------------------------------------------------------
class MSync
{
private:
    bool Init;                  // ���� �� ��������� ������������� WinSocket
    bool AutoSaveARP;           // ��������� ARP-��� ������������� ��� ���������� MAC-�������
    HANDLE Thread;              // ���������� ������, ��������������� ��������/�����
    DWORD ThreadID;             // ������������� ������
    SOCKET Socket;              // ����� ��� ���������� �������������
    SOCKET SocketBC;            // ����� ��� �������� "Magic Packet"
    unsigned NetCode;           // ��� ���������� ��� �������� ������
    MAuth *NetMAC;              // �������������� ������� ��������
    MStates *States;            // C�������� ����������� ��� �������������
    MSyncStates SyncStates;     // ������� �������� �������������

    unsigned PCount;                            // ����� ������� ������������ ������� (���������)
    mutable MWAPI::CRITICAL_SECTION CS_PCount;  // � ������ ������������� ������� ������� � ����

    union MPacket
    {
        MSyncPHello Hello;
        MSyncPConf Conf;
    } Packet;                   // ����� ��� ����������� �������

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    static bool PollData(SOCKET Socket_);
    static bool UpdateMAC(MSyncStates *States_);
    void ThreadExecute();
    void sPCount(unsigned Value_);      // thread safe

public:
    bool NetInit(unsigned Code_, MAuth *MAC_);  // ������������� WinSocket
    bool NetFree();                             // ������������ WinSocket
    bool Associate(MStates *States_, MComputers *Computers_);
    bool Start();                               // �������� ������� � ������ ������ �������������
    void Stop();                                // ��������� ������ ������������� � �������� �������

    void SetARPFile(char *File_, unsigned Code_, bool AutoSave_);
    bool SaveARP() const { return SyncStates.Save(); }
    bool LoadARP() { return SyncStates.Load(); }
    DWORD gLastErr() const { return SyncStates.gLastErr(); }

    // ��������� �������� ������������
    unsigned gPCountMax() const;        // thread safe
    unsigned gPCount() const;           // thread safe

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
    unsigned Process;           // ��������� �������� �������������
    unsigned SendCount;         // ������� ��������� �� ����� �������
    unsigned Seed;              // ID ������
    unsigned NetCode;           // ��� ���������� ��� �������� ������
    MAuth *NetMAC;              // �������������� ������� ��������
    MStateCl *State;            // ���������, � ������� ������������ ������� ���������
    MSyncData SyncData;         // ����� ������ ���������

    union MRcvPacket
    {
        MSyncPHello Hello;
        MSyncPData Data;
    } RcvPacket;                // ����� ��� ����������� �� ������� �������

    union MSndPacket
    {
        MSyncPHello Hello;
        MSyncPConf Conf;
    } SndPacket;                // ����� ��� ������������ ������� �������
    int SndPacketSize;          // ������ ������������� ������
    sockaddr_in SndAddr;        // ����� �������

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    static bool PollData(SOCKET Socket_);
    void ThreadExecute();
    bool Recv(sockaddr_in *From_, char *Packet_, int PacketSize_);
    bool Send(SOCKET Socket_);
    
public:
    bool NetInit(unsigned Code_, MAuth *MAC_);  // ������������� WinSocket
    bool NetFree();                             // ������������ WinSocket
    void Associate(MStateCl *State_);
    bool Start();
    void Stop();

    MSyncCl();
    ~MSyncCl();
};
//---------------------------------------------------------------------------
#endif

