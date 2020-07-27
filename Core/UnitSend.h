//---------------------------------------------------------------------------
#ifndef UnitSendH
#define UnitSendH
//---------------------------------------------------------------------------
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <winsock2.h>

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
#define SEND_MinData        (sizeof(std::uint32_t)+MAC_Size)
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
	std::uint8_t Version;		// ������ �������� ����������
	std::uint32_t Seed;			// random �������/�������
	unsigned char MAC[MAC_Size];// MAC ������
};

struct MSendRequest
{
	std::uint8_t Type;			// ��� �������
	std::uint32_t Seed;			// ��������� ID (������� �� random ������� � �������)
	std::uint32_t Size;			// ������ ����������� ������ ��� ������
	unsigned char MAC[MAC_Size];// MAC ������
};

#pragma pack(pop)
//---------------------------------------------------------------------------
class MSend
{
private:
    SOCKET lSocket;             // ����� ��� �������� ����������
    SOCKET rSocket;             // ����� ��� ���������� � ��������
	std::thread Thread;			// ���������� ������, ��������������� ��������/�����
	std::uint32_t NetCode;		// ���� ���������� ������
	MAuth *NetMAC;              // ������ ��� ���������� � �������� MAC
    bool Init;                  // ���� ����������� ������������� WinSock,NetCode,NetMAC

    union MPacket
    {
        MSendHello Hello;
        MSendRequest Request;
    } Packet;                   // ����� ��� ��������/������ ��������

protected:
	std::atomic_bool Break;		// ���� ���������� ������� ��������

    // �������� � ��������
	bool NetInit(std::uint32_t Code_, MAuth *MAC_);	// ������������� WinSocket
	bool NetFree();                             	// ������������ WinSocket
    bool Create(bool Srv_);
    bool Listen();      // ������� �������� ����������
    bool Accept();      // ������� �������� ����������
    bool Connect(const wchar_t *IP_, unsigned Time_);    // ������� ��������� ����������
	bool Snd(char *Data_, std::size_t Size_, unsigned Time_);
	bool Rcv(char *Data_, std::size_t Size_, unsigned Time_);
    bool Disconnect(unsigned Time_);            // ������� ��������� ����������
    void lClose();          // ������� ��������� �����
    void rClose();          // ������� ����� ���������� ����������

    // ��������� ���������
	bool SndHello(std::uint32_t Seed_);
	bool RcvHello(std::uint32_t *Seed_);
	bool SndRequest(std::uint8_t Type_, std::uint32_t Seed_, std::size_t Size_);
	bool RcvRequest(std::uint8_t *Type_, std::uint32_t Seed_, std::size_t *Size_);

	template <typename obj_type>
	bool SndObject(obj_type *Obj_, std::uint8_t Type_, std::uint32_t Seed_);

	template <typename obj_type>
	bool RcvObject(obj_type *Obj_, std::size_t Size_, std::uint32_t Seed_);

    // �������� � ������� ��������/������
    virtual void ThreadP()=0;

    bool Start();
    void Stop();

public:
	MSend():
		lSocket(INVALID_SOCKET),
		rSocket(INVALID_SOCKET),
//		Break(false),
		NetCode(0),
		NetMAC(nullptr),
		Init(false)
	{
	}

	MSend(const MSend&) = delete;
	MSend(MSend&&) = delete;
	MSend& operator=(const MSend&) = delete;
	MSend& operator=(MSend&&) = delete;
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
	static std::mutex mtxSeed;

	unsigned NextSeed()
	{
		std::lock_guard <std::mutex> lckObj(mtxSeed);	/// ��� MSendSrv �������
		return SeedRand++;
	}

public:
	bool NetInit(HWND Window_, UINT MinMsg_, std::uint32_t Code_, MAuth *MAC_);
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
	MSendSrv(MSendSrv&&) = delete;
	MSendSrv& operator=(const MSendSrv&) = delete;
	MSendSrv& operator=(MSendSrv&&) = delete;
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
	bool NetInit(MStateCl *State_, std::uint32_t Code_, MAuth *MAC_);
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
	MSendCl(MSendCl&&) = delete;
	MSendCl& operator=(const MSendCl&) = delete;
	MSendCl& operator=(MSendCl&&) = delete;
	~MSendCl() = default;
};
//---------------------------------------------------------------------------
#endif

