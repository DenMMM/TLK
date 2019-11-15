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
#define SYNC_Version        0x31    // ������ �������� ����������
#define SYNC_Port           7005    // ����� ����� �������
#define SYNC_WaitInterval   3000    // �������� ����� �������� ������� ������ (� ����.)
#define SYNC_SendInterval   256     // �������� ����� ��������� ������� � ������ (� ����.)
#define SYNC_SendRetryes    3       // ���������� ������� ������ � ���� �� ������ � ������
#define SYNC_MinSize        sizeof(MSyncPHello)
#define SYNC_MaxSize        sizeof(MSyncPData)
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
class MSyncStatesItem:
	public MSLListItemSimple <
		MSLListItem <MSyncStates, MSyncStatesItem>,
		MSyncStatesItem>
{
private:
	MStatesItem *State;					// ��������� ����������, � ������� ������� ������
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

	// ��������� Seed �� ������� �������
	static MRandCounter SeedRand;
	static MWAPI::CRITICAL_SECTION CS_Seed;

	unsigned NextSeed()
	{
		MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Seed);  /// ��� MSync �������
		return SeedRand++;
	}

public:
	// ������� ��������� ����������/�������� ������
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

	// ������� MSyncState � MState � IP-������� ����������
	void Associate(MStatesItem *State_, u_long IP_);
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
	// ������� ������� ������������� ��� ����������� � ������� � �� MState
	void Associate(MStates *States_, MComputers *Computers_);
	// �������� ��������� ��� �� �������� ��� MSyncState
	bool Start() const;
	bool Stop() const;
	// ����� ������ �� IP-������
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

	std::atomic_uint PCount;	// ������� ������������ ������� (���������)

    union MPacket
    {
        MSyncPHello Hello;
        MSyncPConf Conf;
    } Packet;                   // ����� ��� ����������� �������

    static DWORD WINAPI ThreadFunc(LPVOID Data);
	static bool PollData(SOCKET Socket_);
    static bool UpdateMAC(MSyncStates *States_);
    void ThreadExecute();

	void sPCount(unsigned Value_) noexcept { PCount.store(Value_); }

public:
    bool NetInit(unsigned Code_, MAuth *MAC_);  // ������������� WinSocket
    bool NetFree();                             // ������������ WinSocket
    bool Associate(MStates *States_, MComputers *Computers_);
    bool Start();                               // �������� ������� � ������ ������ �������������
    void Stop();                                // ��������� ������ ������������� � �������� �������

    void SetARPFile(wchar_t *File_, unsigned Code_, bool AutoSave_);
    bool SaveARP() const { return SyncStates.Save(); }
    bool LoadARP() { return SyncStates.Load(); }
	DWORD gLastErr() const noexcept { return SyncStates.gLastErr(); }

	// ��������� �������� ������������
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

	// ��������� Seed �� ������� �������
	MRandCounter SeedRand;
	unsigned NextSeed() { return SeedRand++; }

public:
	bool NetInit(unsigned Code_, MAuth *MAC_);  // ������������� WinSocket
	bool NetFree();                             // ������������ WinSocket
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

