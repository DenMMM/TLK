//---------------------------------------------------------------------------
#ifndef UnitSendH
#define UnitSendH
//---------------------------------------------------------------------------
#include <winsock2.h>
#include <vector>
#include <chrono>

#include "UnitSLList.h"
#include "UnitStates.h"
#include "UnitComputers.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
#include "UnitAuth.h"
#include "UnitRandCounter.h"
//---------------------------------------------------------------------------
struct MSendHello;
struct MSendRequest;

class MSend;
class MSendSrv;
class MSendCl;
//---------------------------------------------------------------------------
#define SEND_Version        0x31    // ������ �������� ����������
#define SEND_Port           7005    // ����� ����� �������
// ����������� �� ������ ������ � �������� ������ (MSendRequest.Size)
#define SEND_MinData        (sizeof(unsigned)+MAC_Size)
#define SEND_MaxData        (MAX_SLFileSize+MAC_Size)   // ������ ������������ �������+MAC
//---------------------------------------------------------------------------
// ��� ������ ����� ��������� �������/������� � ���� (MSendRequest.Type)
#define mstSendGames        1       // ������ ��� ��� �������
#define mstSendConfig       2       // ��������� ��� �������
#define mstGetGames         3       // ������ ������ ���
#define mstGetConfig        4       // ������ �������� �������
#define mstAccept           5       // ������������� �������� �������� ��������
//---------------------------------------------------------------------------
// Main Send State
#define mssNone             0
#define mssSendGames        1
#define mssSendConfig       2
#define mssGetGames         3
#define mssGetConfig        4
//---------------------------------------------------------------------------
// �������, ���������� MSendSrv �������� ��� ��������� ��������
#define mseConnecting       1   // ��������������� ����������
#define mseSending          2   // �������� ������
#define mseReceiving        3   // ����� ������
#define mseDisconnecting    4   // ������������
#define mseExecute          5   // ����� ���������� �������
#define mseNotConnect       6   // �� ������� ����������� � ��������
#define mseProtError        7   // ������ ���������
#define mseFreeParam        8   // ����� ������� �������� ��� ��������� ��������� ������
//---------------------------------------------------------------------------
/*
 ������               =>      ������
 ===================================================
 MSendHello.Version=SEND_Version
 MSendHello.Seed=random1;
 MSendHello.MAC=hmac(MSendHello-MSendHello.MAC,MAuth);
 encode(MSendHello,ENC_Net1);
                             check:
                                    Version,
                                    MAC

 ������               <=      ������
 ===================================================
 MSendHello.Version=SEND_Version
 MSendHello.Seed=random2;
 MSendHello.MAC=hmac(MSendHello-MSendHello.MAC,MAuth);
 encode(MSendHello,ENC_Net1);
    check:
            Version,
            MAC

            
 [mssSendGames/mssSendConfig]

 ������               =>      ������
 ===================================================
 MSendRequest.Type=mstSendGames/mstSendConfig;
 MSendRequest.Seed=mix(random1,random2);
 MSendRequest.Size=MSLList::GetAllDataSize()+sizeof(Seed)+MAC_Size;
 MSendRequest.MAC=hmac(MSendRequest-MSendRequest.MAC,MAuth);
 encode(MSendRequest,ENC_Net1);
                             check:
                                    Type,
                                    Seed,
                                    Size,
                                    MAC
                             attempt:
                                    new char[Size]

 ������               =>      ������
 ===================================================
 Seed=mix(random1,random2);
 Data[]=MSLList::SetAllData();
 MAC=hmac(Seed+Data,MAuth);
 encode(Seed+Data+MAC,ENC_Net1);
                             check:
                                    Seed,
                                    MAC
                             attempt:
                                    MSLList::GetAllData();
                                    MStateCl::New...();

 ������               <=      ������
 ===================================================
 MSendRequest.Type=mstAccept;
 MSendRequest.Seed=mix(random1,random2);
 MSendRequest.Size=0;
 MSendRequest.MAC=hmac(MSendRequest-MSendRequest.MAC,MAuth);
 encode(MSendRequest,ENC_Net1);
    check:
            Type,
            Seed,
            Size,
            MAC
    attempt:
            Event(*Computer,mseExecute);


 [mssGetGames/mssGetConfig]

 ������               =>      ������
 ===================================================
 MSendRequest.Type=mstGetGames/mstGetConfig;
 MSendRequest.Seed=mix(random1,random2);
 MSendRequest.Size=0;
 MSendRequest.MAC=hmac(MSendRequest-MSendRequest.MAC,MAuth);
                             check:
                                    Type,
                                    Seed,
                                    Size,
                                    MAC

 ������               <=      ������
 ===================================================
 MSendRequest.Type=mstAccept;
 MSendRequest.Seed=mix(random1,random2);
 MSendRequest.Size=MSLList::GetAllDataSize()+sizeof(Seed)+MAC_Size;
 MSendRequest.MAC=hmac(MSendRequest-MSendRequest.MAC,MAuth);
 encode(MSendRequest,ENC_Net1);
    check:
            Type,
            Seed,
            Size,
            MAC
    attempt:
            new char[Size]

 ������               <=      ������
 ===================================================
 Seed=mix(random1,random2);
 Data[]=MSLList::SetAllData();
 MAC=hmac(Seed+Data,MAuth);
 encode(Seed+Data+MAC,ENC_Net1);
    check:
            Seed,
            MAC
    attempt:
            MSLList::GetAllData();

*/
//---------------------------------------------------------------------------
#pragma pack(push, 1)

struct MSendHello
{
    unsigned char Version;      // ������ �������� ����������
    unsigned int Seed;          // random �������/�������
    unsigned char MAC[MAC_Size];// MAC ������
};

struct MSendRequest
{
    unsigned char Type;         // ��� �������
    unsigned int Seed;          // ��������� ID (������� �� random ������� � �������)
    unsigned int Size;          // ������ ����������� ������ ��� ������
    unsigned char MAC[MAC_Size];// MAC ������
};

#pragma pack(pop)
//---------------------------------------------------------------------------
class MSend
{
private:
    SOCKET lSocket;             // ����� ��� �������� ����������
    SOCKET rSocket;             // ����� ��� ���������� � ��������
    HANDLE Thread;              // ���������� ������, ��������������� ��������/�����
    DWORD ThreadID;             // ID ������
    unsigned NetCode;           // ���� ���������� ������
    MAuth *NetMAC;              // ������ ��� ���������� � �������� MAC
    bool Init;                  // ���� ����������� ������������� WinSock,NetCode,NetMAC

    union MPacket
    {
        MSendHello Hello;
        MSendRequest Request;
    } Packet;                   // ����� ��� ��������/������ ��������

protected:
    bool Break;                 // ���� ���������� ������� ��������

    // �������� � ��������
    bool NetInit(unsigned Code_, MAuth *MAC_);  // ������������� WinSocket
    bool NetFree();                             // ������������ WinSocket
    bool Create(bool Srv_);
    bool Listen();      // ������� �������� ����������
    bool Accept();      // ������� �������� ����������
    bool Connect(const wchar_t *IP_, unsigned Time_);    // ������� ��������� ����������
    bool Snd(char *Data_, unsigned Size_, unsigned Time_);
    bool Rcv(char *Data_, unsigned Size_, unsigned Time_);
    bool Disconnect(unsigned Time_);            // ������� ��������� ����������
    void lClose();          // ������� ��������� �����
    void rClose();          // ������� ����� ���������� ����������

    // ��������� ���������
    bool SndHello(unsigned Seed_);
    bool RcvHello(unsigned *Seed_);
    bool SndRequest(unsigned char Type_, unsigned Seed_, unsigned Size_);
	bool RcvRequest(unsigned char *Type_, unsigned Seed_, unsigned *Size_);

	template <typename obj_type>
	bool SndObject(obj_type *Obj_, unsigned Type_, unsigned Seed_);

	template <typename obj_type>
	bool RcvObject(obj_type *Obj_, unsigned Size_, unsigned Seed_);

    // �������� � ������� ��������/������
    static DWORD WINAPI ThreadF(LPVOID Data);
    virtual void ThreadP()=0;

    bool Start();
    void Stop();

public:
	MSend():
		lSocket(INVALID_SOCKET),
		rSocket(INVALID_SOCKET),
		Thread(nullptr),
		ThreadID(0),
		Break(false),
		NetCode(0),
		NetMAC(nullptr),
		Init(false)
	{
	}

	MSend(const MSend&) = delete;
	MSend(MSend&&) noexcept = delete;
	MSend& operator=(const MSend&) = delete;
	MSend& operator=(MSend&&) noexcept = delete;
	~MSend() = default;
};
//---------------------------------------------------------------------------
class MSendSrv: public MSend
{
private:
	HWND Window;                // ���� ��� ��������� ��������� � �������� ��������
	UINT MinMsg;                //

	int Mode;                   			// ����� (��� ����������/�����������)
	std::vector <MComputersItem*> *Comps;	// ������ �� ������� ����������� ��� ��������
	MComputersItem *Comp;					// ��������� �� ���������, ��� ��������
	MGames *ObjGames;                   	// ������� ���
	MClOptions *ObjOptions;             	// ��������/������

	void ThreadP();
	void ThreadSend();
	void ThreadGet();
	void Event(MComputersItem *Computer_, int Event_);

	// ��������� Seed �� ������� �������
	static MRandCounter SeedRand;
	static MWAPI::CRITICAL_SECTION CS_Seed;

	unsigned NextSeed()
	{
		MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Seed);  /// ��� MSendSrv �������
		return SeedRand++;
	}

public:
	bool NetInit(HWND Window_, UINT MinMsg_, unsigned Code_, MAuth *MAC_);
	bool NetFree();
	bool Send(std::vector <MComputersItem*> *Computers_, MGames *Games_, MClOptions *Options_);
	bool Get(MComputersItem *Computer_, MGames *Games_, MClOptions *Options_);
	void Stop();

	MSendSrv():
		Window(nullptr),
		Mode(mssNone),
		Comps(nullptr),
		Comp(nullptr),
		ObjGames(nullptr),
		ObjOptions(nullptr)
	{
	}

	MSendSrv(const MSendSrv&) = delete;
	MSendSrv(MSendSrv&&) noexcept = delete;
	MSendSrv& operator=(const MSendSrv&) = delete;
	MSendSrv& operator=(MSendSrv&&) noexcept = delete;
	~MSendSrv() = default;
};
//---------------------------------------------------------------------------
class MSendCl: public MSend
{
private:
    MStateCl *State;            // ���� �������� �� ���������� ������ ���, ��������
    MClOptions Options;         // �������� �������
    MGames Games;               // ��� ������/��������

    void ThreadP();

	// ��������� Seed �� ������� �������
	MRandCounter SeedRand;
	unsigned NextSeed() { return SeedRand++; }

public:
    bool NetInit(MStateCl *State_, unsigned Code_, MAuth *MAC_);
    bool NetFree();
    bool Start();
    void Stop();

	MSendCl():
		State(nullptr),
		SeedRand(
			std::chrono::system_clock::
			now().time_since_epoch().count())
	{
	}

	MSendCl(const MSendCl&) = delete;
	MSendCl(MSendCl&&) noexcept = delete;
	MSendCl& operator=(const MSendCl&) = delete;
	MSendCl& operator=(MSendCl&&) noexcept = delete;
	~MSendCl() = default;
};
//---------------------------------------------------------------------------
#endif

