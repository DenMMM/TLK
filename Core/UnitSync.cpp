//---------------------------------------------------------------------------
#include <cstdlib>
#include <stdexcept>
#include <array>
#include <winsock2.h>
#include <ip2string.h>
//#include <Mstcpip.h>
#include <ntstatus.h>
#include <iphlpapi.h>

#ifdef _TEST_SYN
#include <iostream>
#endif

#pragma hdrstop

#include "UnitSync.h"
#include "UnitAuth.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MRandCounter
	MSyncStatesItem::SessIdRand(
		[]() -> std::uint64_t {				/// rand ?
			std::array <std::uint64_t, 4> rnd_v;

			LARGE_INTEGER cntr;
			rnd_v[0]=
				::QueryPerformanceCounter(&cntr)?
				cntr.QuadPart:
				::GetTickCount();

			rnd_v[1]=std::chrono::system_clock::
				now().time_since_epoch().count();
			rnd_v[2]=CalcHwMacHash();		/// �������� �� IP-����� ?
			rnd_v[3]=0x5652532D434E5953;	// ASCII 'SYNC-SRV'

			return fasthash64(&rnd_v, sizeof(rnd_v), 0);
		}()
	);

std::mutex
	MSyncStatesItem::mtxSessId;
//---------------------------------------------------------------------------
std::size_t MSyncStatesItem::GetDataSize() const
{
	return KnownMAC?
		sizeof(IP)+
        sizeof(MAC): 0;
}
//---------------------------------------------------------------------------
void *MSyncStatesItem::SetData(void *Data_) const
{
    if ( !KnownMAC ) return Data_;
    Data_=MemSet(Data_,IP);
    Data_=MemSetBin(Data_,MAC,sizeof(MAC));
    return Data_;
}
//---------------------------------------------------------------------------
const void *MSyncStatesItem::GetData(const void *Data_, const void *Limit_)
{
	if ( !(
		(Data_=MemGet(Data_,&IP,Limit_)) &&
		(Data_=MemGetBin(Data_,MAC,sizeof(MAC),Limit_))
		) ) return nullptr;

	KnownMAC=true;

	return Data_;
}
//---------------------------------------------------------------------------
void MSyncStatesItem::Associate(MStatesItem *State_, u_long IP_)
{
    State=State_;
    KnownMAC=(IP==IP_);
    IP=IP_;
	Process=mspNone;
}
//---------------------------------------------------------------------------
bool MSyncStatesItem::UpdateMAC(unsigned char *MAC_)
{
	bool NeedSave;
	// ��������� ����� �� �������� MAC-�����
	NeedSave=(!KnownMAC)||(memcmp(MAC,MAC_,MAC_AddrLength)!=0);
	// ��� ������������� ��������� MAC-�����
	if ( NeedSave ) { memcpy(MAC,MAC_,MAC_AddrLength); KnownMAC=true; }
	return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncStatesItem::Start()
{
	if ( (State==nullptr)||(IP==INADDR_NONE) ) return false;
	Process=mspWait;
	LastSendTime=::GetTickCount()-SYNC_WaitInterval;
	return true;
}
//---------------------------------------------------------------------------
bool MSyncStatesItem::Stop()
{
    bool NeedSave=false;
    switch(Process)
    {
        case mspMagic:
        case mspHello:
        case mspSyncData:
            NeedSave=State->NetEnd();
            break;
        default:
            break;
    }
    Process=mspNone;
    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncStatesItem::Send(SOCKET Socket_, SOCKET SocketBC_, std::uint32_t Code_, MAuth *MAC_)
{
    bool NeedSave=false;

    switch(Process)
    {
        case mspWait:
            if ( !State->NetBegin() ) break;
            if ( KnownMAC&&State->NetPwrOnNeed() )
            {
                // ������ ����������������� �����
                memset(&Address,0,sizeof(Address));
                Address.sin_family=AF_INET;
                Address.sin_port=::htons(0);
                Address.sin_addr.s_addr=INADDR_BROADCAST;
                // ��������� magic-�����
                Packet.WOL.ZeroFirst=0;
                memset(Packet.WOL.Sync,0xFF,MAC_AddrLength);
                for ( int i=0; i<16; i++ )
                    memcpy(Packet.WOL.Magic[i],MAC,MAC_AddrLength);
                // ������ ������ ������ � �������� ��� ��������
                PacketSize=sizeof(Packet.WOL);
                LastSendTime=::GetTickCount()-SYNC_SendInterval;
                SendCount=0; Process=mspMagic;
            } else if ( State->NetSyncNewData()||
                ((::GetTickCount()-LastSendTime)>=SYNC_WaitInterval) )
            {
                // ��������� ����� ���������� ��� ���� ������� � ������
                memset(&Address,0,sizeof(Address));
                Address.sin_family=AF_INET;
                Address.sin_port=::htons(SYNC_Port);
                Address.sin_addr.s_addr=IP;
				// ����������� ����� ������ ��� ������������� ����� ������ hello
				SyncData=State->NetSyncData();
				//
                SessId=NextSessId();
                // ��������� ��������� hello-������ �� ����� random
                Packet.Hello.Header.Version=SYNC_Version;
                Packet.Hello.Header.Type=mptHelloS;
				Packet.Hello.Header.SessId=SessId;
				// ��������� MAC
				MAC_->Calc(&Packet.Hello,
					sizeof(Packet.Hello)-sizeof(Packet.Hello.MAC),
					Packet.Hello.MAC,sizeof(Packet.Hello.MAC));
				// ������ ������ ������ ��� �������� � ������� ���
				PacketSize=sizeof(Packet.Hello);
                BasicEncode(&Packet,PacketSize,Code_);
                // �������� ��������, ������� �� �� ��������� �����
                LastSendTime=::GetTickCount()-SessId%SYNC_SendInterval;
                SendCount=0; Process=mspHello;
            } else NeedSave=State->NetEnd();
            break;
        case mspMagic:
        case mspHello:
        case mspSyncData:
            // ��������� �� ���� �� ��������� ��������� �����
            if ( (::GetTickCount()-LastSendTime)<SYNC_SendInterval ) break;
            // ��������� �� ���������� �� �������� ���������� �������
            if ( SendCount>=SYNC_SendRetryes )
            {
                // ��������� ������� �������� � ����������� (WOL-�������, SYNC-���)
                if ( Process==mspMagic ) State->NetPwrOnExecuted();
                else State->NetSyncExecuted(false);
                NeedSave=State->NetEnd();
                // ��������� � ����� �������� ����� ������
                Process=mspWait;
            } else
			{
                // ���������� �����
                ::sendto(Process==mspMagic?SocketBC_:Socket_,
                    (char*)&Packet,PacketSize,0,
					(sockaddr*)&Address,sizeof(Address));
#ifdef _TEST_SYN
				std::cout << "Srv [" << Process << "]=> Cl\r\n";
				::Beep(3000,100);
#endif
				// ��������� ����� �������� ������ � ����������� �������
                LastSendTime=::GetTickCount(); SendCount++;
			}
			break;
		default: break;
	}

	return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncStatesItem::Recv(char *Packet_, int PacketSize_, std::uint32_t Code_, MAuth *MAC_)
{
	MSyncPHeader *RecvHeader=(MSyncPHeader*)Packet_;
	bool NeedSave=false;

	if ( Process==mspNone ) goto error;
	// ��������� ���������� ������� ������
	if ( (PacketSize_<SYNC_MinSize)||
		(PacketSize_>SYNC_MaxSize) ) goto error;
	// �������������� ���
	BasicDecode(Packet_,PacketSize_,Code_);
	// ������� ������ � MAC
	if ( (RecvHeader->Version!=SYNC_Version)||
		(!MAC_->Check(Packet_,PacketSize_-MAC_Size,
		Packet_+PacketSize_-MAC_Size,MAC_Size)) ) goto error;

	// ������������ �����
	switch((int)RecvHeader->Type)
	{
		case mptHelloC:
			// ���� �� �� ���� ����� � ������ �� � ���� ������ ?
			if ( (Process!=mspHello)||
				(PacketSize_!=sizeof(MSyncPHello)) ) break;
			// ����� ���������� �� �������, �� �������� ��� mspHello
//            memset(&Address,0,sizeof(Address));
//            Address.sin_family=AF_INET;
//            Address.sin_port=::htons(SYNC_Port);
//            Address.sin_addr.s_addr=::inet_addr(IP);
			// ����� ������������� ��������� ID �� ������ random � �������
			{
				std::array <decltype(SessId),2> IdV;
				IdV[0]=SessId;
				IdV[1]=RecvHeader->SessId;
				SessId=fasthash64(&IdV,sizeof(IdV),Code_);  /// ��� �� ��������� 'Code_'
#ifdef _TEST_SYN
				std::cout << "SessId:\t\t" << SessId << "\r\n";
#endif
			}
			// ��������� ��������� ������
			Packet.Data.Header.Version=SYNC_Version;
			Packet.Data.Header.Type=mptSyncData;
			Packet.Data.Header.SessId=SessId;
			// �������� � ����� ����������� ��� �� ����� hello ������ ��� �������������
			Packet.Data.Data=SyncData;
			// ��������� MAC
			MAC_->Calc(&Packet.Data,
				sizeof(Packet.Data)-sizeof(Packet.Data.MAC),
				Packet.Data.MAC,sizeof(Packet.Data.MAC));
			// ������ ������ ������ ��� �������� � ������� ���
			PacketSize=sizeof(Packet.Data);
			BasicEncode(&Packet,PacketSize,Code_);
			//  �������� ��������
			LastSendTime=::GetTickCount()-SYNC_SendInterval;
			SendCount=0; Process=mspSyncData;
			break;
		case mptSyncConf:
			// ���� �� �� ���� �����, ������ �� � ���� ������ � seed ?
			if ( (Process!=mspSyncData)||
				(PacketSize_!=sizeof(MSyncPConf))||
				(RecvHeader->SessId!=SessId) ) break;
			// ��������� ������� �������� � ����������� �������
			State->NetSyncExecuted(true);
			NeedSave=State->NetEnd();
			// ��������� � ����� �������� ����� ������
			Process=mspWait;
			break;
		default: break;
	}

error:
    return NeedSave;
}
//---------------------------------------------------------------------------
void MSyncStates::Associate(MStates *States_, MComputers *Computers_)
{
	in_addr IP_inaddr;
	u_long IP;

	auto iLastSyncState=begin();
	auto iEnd=end();

	for ( auto &State: *States_ )
	{
		// ���� ���������, � ������� ������������� ���������
		auto iComputer=Computers_->Search(State.Associated());
		if ( iComputer==Computers_->end() ) continue;
		// ������������ IP-����� �� ������ � u_long
//		IP=::inet_addr(Computer->Address.c_str());
//		if ( IP==INADDR_NONE ) continue;
		const wchar_t *term;
		if ( ::RtlIpv4StringToAddressW(
			iComputer->Address.c_str(),
			TRUE,
			&term,               				/// nullptr allowed ?
			&IP_inaddr
			) != STATUS_SUCCESS ) continue;
		IP=IP_inaddr.S_un.S_addr;

		// ���� �� ����� ������ ��������� �������������
		// (����� ��������� �������� ��������� MAC-�����)
		// ��� ��������� �����
		auto iSyncState=Search(IP);
		if ( iSyncState==iEnd ) iSyncState=iterator(&Add(),this);   /// �������� ��� 'this'
		// ����������� ��������� ������������� � ���������� ���������� � ��� �������
		iSyncState->Associate(&State,IP);
		// ���� ��������� �� ��������� � ������, ��������� ��� � ������
		if ( iLastSyncState!=iEnd ) Swap(iLastSyncState,iSyncState);
		iLastSyncState=++iSyncState;
	}
	// ������� ���������� �� ������� �������
	while( iLastSyncState!=iEnd ) iLastSyncState=Del(iLastSyncState);
}
//---------------------------------------------------------------------------
bool MSyncStates::Start()
{
	for ( auto &SyncState: *this )
	{
		if ( !SyncState.Start() ) return false;
	}

	return true;
}
//---------------------------------------------------------------------------
bool MSyncStates::Stop()
{
	bool NeedSave=false;

	for ( auto &SyncState: *this )
	{
		NeedSave|=SyncState.Stop();
	}

	return NeedSave;
}
//---------------------------------------------------------------------------
MSyncStates::const_iterator MSyncStates::Search(u_long IP_) const
{
	auto iSyncState=begin();
	auto iEnd=end();

	while ( iSyncState!=iEnd )
	{
		if ( iSyncState->gIP()==IP_ ) break;
		++iSyncState;
	}

	return iSyncState;
}
//---------------------------------------------------------------------------
void MSync::SetARPFile(wchar_t *File_, std::uint32_t Code_, bool AutoSave_)
{
    AutoSaveARP=AutoSave_;
    SyncStates.SetDefaultFile(File_,Code_);
}
//---------------------------------------------------------------------------
bool MSync::NetInit(std::uint32_t Code_, MAuth *MAC_)
{
    WSADATA WSAData;
    NetCode=Code_;
    NetMAC=MAC_;
    Init=!::WSAStartup(MAKEWORD(1,1),&WSAData);
    return Init;
}
//---------------------------------------------------------------------------
bool MSync::NetFree()
{
	Init=::WSACleanup();
	return !Init;
}
//---------------------------------------------------------------------------
bool MSync::Associate(MStates *States_, MComputers *Computers_)
{
	States=nullptr;
	SyncStates.Associate(States_,Computers_);
	States=States_;
	// �������� ��� MAC-�������
	return AutoSaveARP? SyncStates.Save(): true;
}
//---------------------------------------------------------------------------
bool MSync::Start()
{
	sockaddr_in Address;
	BOOL SetValue;

	if (
		(!Init) ||
		(States==nullptr) ||
		Thread.joinable() ) return false;	/// throw ?

	Socket=SocketBC=INVALID_SOCKET;
	// ������� ����� ��� ��������/������ ���������
	if ( (Socket=::socket(AF_INET,SOCK_DGRAM,
		IPPROTO_IP))==INVALID_SOCKET ) goto error;
	// ������ ��������� ������ ������
	memset(&Address,0,sizeof(Address));
	Address.sin_family=AF_INET;
	Address.sin_port=::htons(0);            // ���� �������� �� !
	Address.sin_addr.s_addr=INADDR_ANY;
	// ������������ �����
	if ( ::bind(Socket,(sockaddr*)&Address,
		sizeof(Address))==SOCKET_ERROR ) goto error;

	// ������� ����� ��� �������� ����������������� �������
	if ( (SocketBC=::socket(AF_INET,SOCK_DGRAM,
		IPPROTO_IP))==INVALID_SOCKET ) goto error;
	// ������������� ��� ���� ����� broadcast
	SetValue=TRUE;
	if ( ::setsockopt(SocketBC,SOL_SOCKET,SO_BROADCAST,
		(char*)&SetValue,sizeof(SetValue))==SOCKET_ERROR ) goto error;
	// ������ ��������� ������ ������
	memset(&Address,0,sizeof(Address));
	Address.sin_family=AF_INET;
	Address.sin_port=::htons(0);
	Address.sin_addr.s_addr=INADDR_ANY;
	// ������������ �����
	if ( ::bind(SocketBC,(sockaddr*)&Address,
		sizeof(Address))==SOCKET_ERROR ) goto error;

	//
	if ( !SyncStates.Start() ) goto error;      /// try/catch ?

	IsStopping.store(false);
	try
	{
		std::thread TmpThread(
			[](MSync *Obj_)
			{
				// ��������� ����������
				Obj_->ThreadExecute();
				// ���������� ��������� - ������� �������
				Obj_->sPCount(0);
			}, this);
		Thread.swap(TmpThread);
	}
	catch (std::system_error &e)
	{
		goto error;
	}

	return true;

error:
	// ��������� ������
	if ( Socket!=INVALID_SOCKET ) { ::closesocket(Socket); Socket=INVALID_SOCKET; }
	if ( SocketBC!=INVALID_SOCKET ) { ::closesocket(SocketBC); SocketBC=INVALID_SOCKET; }
	return false;
}
//---------------------------------------------------------------------------
void MSync::Stop()
{
	// �������� ������ ������
	if ( Thread.joinable() )
	{
		IsStopping.store(true);
		Thread.join();
	}

	// ������� ������
	if ( Socket!=INVALID_SOCKET ) { ::closesocket(Socket); Socket=INVALID_SOCKET; }
	if ( SocketBC!=INVALID_SOCKET ) { ::closesocket(SocketBC); SocketBC=INVALID_SOCKET; }

	// ��� ������������� �������� ��������� �����������
	if ( SyncStates.Stop() ) States->Save();
}
//---------------------------------------------------------------------------
void MSync::ThreadExecute()
{
    MSG Message;
    sockaddr_in FromAddr;
    int RecvSize, AddrSize;
    bool NeedSave;
	DWORD StartCycleTime;
    unsigned Process;

    while(true)
    {
        StartCycleTime=::GetTickCount();

        // ��������� ���� ��������/������ ������
        NeedSave=false; Process=0;
		for ( auto &SyncState: SyncStates )
		{
			// ���������� ������� �������� �������
			while(PollData(Socket))
            {
                // ������ ������ �� ������
                memset(&FromAddr,0,sizeof(FromAddr)); AddrSize=sizeof(FromAddr);
                RecvSize=::recvfrom(Socket,(char*)&Packet,sizeof(Packet),
                    0,(sockaddr*)&FromAddr,&AddrSize);
                if ( (RecvSize==0)||(RecvSize==SOCKET_ERROR) ) continue;
				// ���� ��������� ������������� �� IP-������, � �������� ������ �����
				auto iRecvState=SyncStates.Search(FromAddr.sin_addr.s_addr);
				// �������� ��� ��� ��������� � �������� ������ ����� ��
				if ( iRecvState!=SyncStates.end() )
				{
					NeedSave|=iRecvState->Recv((char*)&Packet,RecvSize,NetCode,NetMAC);
					NeedSave|=SyncState.Send(Socket,SocketBC,NetCode,NetMAC);
				}
			}
			// ���� ���������� ������ "��-����������"
			NeedSave|=SyncState.Send(Socket,SocketBC,NetCode,NetMAC);
			Process+=SyncState.gPCount();
		}
		// ��������� ����� �� ��������� ����������� ��������� �����������
		if ( NeedSave )
		{
			// ������ ��������� ������ ��������� ������
			try { States->Save(); }
			catch (std::bad_alloc &e) {}
		}
		// ��������� MAC-������ � ��� ������������� ��������� �� ����
		if ( UpdateMAC(&SyncStates)&&AutoSaveARP )
		{
			// ������ ��������� ������ ��������� ������
			try { SyncStates.Save(); }
			catch (std::bad_alloc &e) {}
		}
		// ��������� ��������� �������� �������������
		sPCount(Process);

		// ��������� �� ���� �� ��������� ������
		if ( IsStopping.load() ) return;

		// ��������, ���� ������ ������ ���������
		if ( (::GetTickCount()-StartCycleTime)<(SYNC_SendInterval/4) )
		{
			std::this_thread::sleep_for(
				std::chrono::milliseconds(SYNC_SendInterval/4));
/*
			// ������� ��������
			thread_local static unsigned cntr=0;
			if ( (++cntr)%64 == 0 ) BasicRand.event();
*/
		}
	}
}
//---------------------------------------------------------------------------
bool MSync::PollData(SOCKET Socket_)
{
	fd_set WaitSockets;
	timeval WaitTimer;

	// ������� ����� � �������
	FD_ZERO(&WaitSockets);
	FD_SET(Socket_,&WaitSockets);
	// ������������� ������ �������� ������ � ����
    WaitTimer.tv_sec=0;
    WaitTimer.tv_usec=0;

    return ::select(0,&WaitSockets,nullptr,nullptr,&WaitTimer)==1;
}
//---------------------------------------------------------------------------
bool MSync::UpdateMAC(MSyncStates *States_)
{
    PMIB_IPNETTABLE Table=nullptr;
    ULONG TableSize;
	PMIB_IPNETROW Record;
	bool NeedSave=false;

	// ����� ������ �������
	TableSize=0;
	if ( ::GetIpNetTable(nullptr,&TableSize,TRUE)!=
		ERROR_INSUFFICIENT_BUFFER ) goto error;
	// �������� ������ ��� �������
	try { Table=(PMIB_IPNETTABLE)new char[TableSize]; }
	catch (std::bad_alloc &e) { goto error; }
	memset(Table,0,TableSize);
	// ����� �������
	if ( ::GetIpNetTable(Table,&TableSize,TRUE)!=0 ) goto error;

	// ��������� MAC-������ � ������ ���������
	for ( DWORD i=0; i<Table->dwNumEntries; i++ )
	{
		Record=&Table->table[i];
		// ��������� ��� ������ (����� ������ ������������ � �����������)
		if ( (Record->dwType!=MIB_IPNET_TYPE_DYNAMIC)&&
			(Record->dwType!=MIB_IPNET_TYPE_STATIC) ) continue;
		// ��������� ��������� �� ����� MAC-������ � ���������
		if ( Record->dwPhysAddrLen!=MAC_AddrLength ) continue;
		// ���� ���������, ��������������� � ��� �� �������, ��� ���� �� �������
		auto iState=States_->Search(Record->dwAddr);
		if ( iState==States_->end() ) continue;
		// ��������� MAC-�����
		NeedSave|=iState->UpdateMAC(Record->bPhysAddr);
    }

    delete[] Table;
    return NeedSave;
error:
	delete[] Table;
	return false;
}
//---------------------------------------------------------------------------
bool MSyncCl::PollData(SOCKET Socket_)
{
    fd_set WaitSockets;
    timeval WaitTimer;

    // ������� ����� � �������
    FD_ZERO(&WaitSockets);
    FD_SET(Socket_,&WaitSockets);
    // ������������� ������ �������� ������ � ����
	WaitTimer.tv_sec=0;
	WaitTimer.tv_usec=0;

	return ::select(0,&WaitSockets,nullptr,nullptr,&WaitTimer)==1;
}
//---------------------------------------------------------------------------
bool MSyncCl::NetInit(std::uint32_t Code_, MAuth *MAC_)
{
	WSADATA WSAData;
	NetCode=Code_;
	NetMAC=MAC_;
	Init=!::WSAStartup(MAKEWORD(1,1),&WSAData);
    return Init;
}
//---------------------------------------------------------------------------
bool MSyncCl::NetFree()
{
    Init=::WSACleanup();
    return !Init;
}
//---------------------------------------------------------------------------
void MSyncCl::Associate(MStateCl *State_)
{
    State=State_;
}
//---------------------------------------------------------------------------
bool MSyncCl::Start()
{
    sockaddr_in Address;
    BOOL SetValue;

	if (
		(!Init) ||
		Thread.joinable() ) return false;	/// throw ?

	Socket=INVALID_SOCKET;
	// ������� ����� ��� ������/�������� ���������
	if ( (Socket=::socket(AF_INET,SOCK_DGRAM,
		IPPROTO_IP))==INVALID_SOCKET ) goto error;
	// ������ ��������� ������ ������
	memset(&Address,0,sizeof(Address));
	Address.sin_family=AF_INET;
	Address.sin_port=::htons(SYNC_Port);
	Address.sin_addr.s_addr=INADDR_ANY;
	// ������������ �����
	if ( ::bind(Socket,(sockaddr*)&Address,
		sizeof(Address))==SOCKET_ERROR ) goto error;

	//
	LastSendTime=::GetTickCount();
	Process=mspWait;

	IsStopping.store(false);
	try
	{
		// ������� ����� ��� ���������� �������� � ������ ������
		std::thread TmpThread(
			[](MSyncCl *Obj_)
			{
				Obj_->ThreadExecute();
			}, this);
		Thread.swap(TmpThread);
	}
	catch (std::system_error &e)
	{
		goto error;
	}

	return true;

error:
	// ��������� �����
	if ( Socket!=INVALID_SOCKET ) { ::closesocket(Socket); Socket=INVALID_SOCKET; }
	return false;
}
//---------------------------------------------------------------------------
void MSyncCl::Stop()
{
	// ��������� ������ ������
	if ( Thread.joinable() )
	{
		IsStopping.store(true);
		Thread.join();
	}

	// ��������� �����
	if ( Socket!=INVALID_SOCKET ) { ::closesocket(Socket); Socket=INVALID_SOCKET; }
}
//---------------------------------------------------------------------------
void MSyncCl::ThreadExecute()
{
	MSG Message;
	sockaddr_in FromAddr;
	int RecvSize, AddrSize;
	bool NeedSave;
	DWORD StartCycleTime;

	while(true)
	{
		StartCycleTime=::GetTickCount();

        NeedSave=false;
        // ���������� ����� �������
        while(PollData(Socket))
        {
            // ������ ������ �� ������
            memset(&FromAddr,0,sizeof(FromAddr)); AddrSize=sizeof(FromAddr);
            RecvSize=::recvfrom(Socket,(char*)&RcvPacket,sizeof(RcvPacket),
				0,(sockaddr*)&FromAddr,&AddrSize);
			if ( (RecvSize==0)||(RecvSize==SOCKET_ERROR) ) continue;
			// �������� ��� ��������� � �������� ������
			NeedSave|=Recv(&FromAddr,(char*)&RcvPacket,RecvSize);
		}
		NeedSave|=Send(Socket);
		// ��������� ����������� ���������, ���� �����
		if ( NeedSave )
		{
			// ������ ��������� ������ ��������� ������
			try { State->Save(); }
			catch (std::bad_alloc &e) {}
		}

		// ��������� ������� ���������
		if ( IsStopping.load() ) break;

		// ��������, ���� ������ ����������
		if ( (::GetTickCount()-StartCycleTime)<(SYNC_SendInterval/4) )
		{
			std::this_thread::sleep_for(
				std::chrono::milliseconds(SYNC_SendInterval/4));
		}
	}
}
//---------------------------------------------------------------------------
bool MSyncCl::Recv(sockaddr_in *From_, char *Packet_, int PacketSize_)
{
	MSyncPHeader *RecvHeader=(MSyncPHeader*)Packet_;
	bool NeedSave=false;

	if ( Process==mspNone ) goto error;
	// ���� ����� �����, ���������� ������ � ������ IP
	if ( (Process!=mspWait)&&
		(SndAddr.sin_addr.s_addr!=From_->sin_addr.s_addr) ) goto error;
	// ��������� ���������� ������� ������
	if ( (PacketSize_<SYNC_MinSize)||
		(PacketSize_>SYNC_MaxSize) ) goto error;
	// �������������� ���
	BasicDecode(Packet_,PacketSize_,NetCode);
	// ������� ������ � MAC
	if ( (RecvHeader->Version!=SYNC_Version)||
		(!NetMAC->Check(Packet_,PacketSize_-MAC_Size,
		Packet_+PacketSize_-MAC_Size,MAC_Size)) ) goto error;

	// ������������ �����
	switch((int)RecvHeader->Type)
	{
		case mptHelloS:
			// ���� �� �� ���� ����� � ������ �� � ���� ������ ?
			if ( (Process!=mspWait)||
				(PacketSize_!=sizeof(MSyncPHello)) ) break;
			// ���������� ����� �������, � ������� ������ �����
			SndAddr=*From_;
//            memset(&SndAddr,0,sizeof(SndAddr));
//            SndAddr.sin_family=AF_INET;
//            SndAddr.sin_port=From_->sin_port;
//            SndAddr.sin_addr.s_addr=From_->sin_addr.s_addr;
			// ��������� ��������� hello-������ �� ����� random
			SndPacket.Hello.Header.Version=SYNC_Version;
			SndPacket.Hello.Header.Type=mptHelloC;
			SndPacket.Hello.Header.SessId=NextSessId();
			// � ����� ���������� ��������� ID, �.�. random ������� ��� �������
			{
				std::array <decltype(SessId),2> IdV;
				IdV[0]=RecvHeader->SessId;
				IdV[1]=SndPacket.Hello.Header.SessId;
				SessId=fasthash64(&IdV,sizeof(IdV),NetCode);    /// ��� �� ��������� 'NetCode'
			}
			// ��������� � ������ MAC
			NetMAC->Calc((char*)&SndPacket.Hello,
				sizeof(SndPacket.Hello)-sizeof(SndPacket.Hello.MAC),
				SndPacket.Hello.MAC,sizeof(SndPacket.Hello.MAC));
			// ������ ������ ������ ��� �������� � ������� ���
			SndPacketSize=sizeof(SndPacket.Hello);
			BasicEncode((char*)&SndPacket,SndPacketSize,NetCode);
			// �������� ��������
			LastSendTime=::GetTickCount()-SYNC_SendInterval;
			SendCount=0; Process=mspHello;
			break;
		case mptSyncData:
			// ���� �� �� ���� ����� � ������ �� � ���� ������ ?
			if ( (Process!=mspHello)||
				(SendCount==0)||            // ������ �� replay brute-force
				(PacketSize_!=sizeof(MSyncPData))||
				(RecvHeader->SessId!=SessId) ) break;
			// ��������� ����� ����� ������ �������, �� �������� ���
			// ������ ����� �������� ������������� �������
			SyncData=((MSyncPData*)Packet_)->Data;
			// ��������� ����� c ������� �������
			SndPacket.Conf.Header.Version=SYNC_Version;
			SndPacket.Conf.Header.Type=mptSyncConf;
			SndPacket.Conf.Header.SessId=SessId;
			// ��������� � ������ MAC
			NetMAC->Calc((char*)&SndPacket.Conf,
				sizeof(SndPacket.Conf)-sizeof(SndPacket.Conf.MAC),
				SndPacket.Conf.MAC,sizeof(SndPacket.Conf.MAC));
			// ������ ������ ������ ��� �������� � ������� ���
			SndPacketSize=sizeof(SndPacket.Conf);
			BasicEncode((char*)&SndPacket,SndPacketSize,NetCode);
			// �������� ��������
			LastSendTime=::GetTickCount()-SYNC_SendInterval;
			SendCount=0; Process=mspSyncConf;
			break;
		default: break;
	}

error:
	return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncCl::Send(SOCKET Socket_)
{
	bool NeedSave=false;

	switch(Process)
	{
		case mspHello:
		case mspSyncConf:
			// ��������� �� ���� �� ��������� ��������� �����
			if ( (::GetTickCount()-LastSendTime)<SYNC_SendInterval ) break;
			// ��������� �� ���������� �� �������� ���������� �������
			if ( SendCount>=SYNC_SendRetryes )
			{
				// ��������� ����� ���������/��������� ������� (reboot, shutdown),
				// ���� ������ ���� �������� � ������ ��������� ������������� �������
				if ( Process==mspSyncConf )
					NeedSave=State->NewSyncData(SyncData);
				// ��������� � ����� �������� ����� ������
				Process=mspWait;
			} else
			{
				// ���������� �����
				::sendto(Socket_,(char*)&SndPacket,SndPacketSize,
					0,(sockaddr*)&SndAddr,sizeof(SndAddr));
#ifdef _TEST_SYN
				std::cout << "Srv <=[" << Process << "] Cl\r\n";
				::Beep(5000,100);
#endif
				// ��������� ����� �������� ������ � ����������� �������
				LastSendTime=::GetTickCount(); SendCount++;
			}
			break;
		default: break;
	}

	return NeedSave;
}
//---------------------------------------------------------------------------

