//---------------------------------------------------------------------------
#include <winsock2.h>
#include <ip2string.h>
#include <ntstatus.h>
#include <vector>
#pragma hdrstop

#include "UnitSend.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MRandCounter
	MSendSrv::SessIdRand(
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
			rnd_v[3]=0x5652532D444E4553;	// ASCII 'SEND-SRV'

			return fasthash64(&rnd_v, sizeof(rnd_v), 0);
		}()
	);

std::mutex
	MSendSrv::mtxSessId;
//---------------------------------------------------------------------------
bool MSend::NetInit(std::uint32_t Code_, MAuth *MAC_)
{
    WSADATA WSAData;
    NetCode=Code_;
    NetMAC=MAC_;
    Init=!::WSAStartup(MAKEWORD(1,1),&WSAData);
    return Init;
}
//---------------------------------------------------------------------------
bool MSend::NetFree()
{
    Init=::WSACleanup();
    return !Init;
}
//---------------------------------------------------------------------------
bool MSend::Create(bool Srv_)
{
    BOOL a;
    unsigned long b;
    sockaddr_in Address;

    // ������� �����
    if ( Srv_ )
    {
        if ( (rSocket=::socket(PF_INET,SOCK_STREAM,IPPROTO_IP))==INVALID_SOCKET ) goto error;
        if ( ::setsockopt(rSocket,SOL_SOCKET,SO_REUSEADDR,(char*)&a,sizeof(a)) ) goto error;
        b=TRUE; if ( ::ioctlsocket(rSocket,FIONBIO,&b) ) goto error;
    } else
    {
        if ( (lSocket=::socket(PF_INET,SOCK_STREAM,IPPROTO_IP))==INVALID_SOCKET ) goto error;
        if ( ::setsockopt(lSocket,SOL_SOCKET,SO_REUSEADDR,(char*)&a,sizeof(a)) ) goto error;
        b=TRUE; if ( ::ioctlsocket(lSocket,FIONBIO,&b) ) goto error;
        // ������ ����� ��� �������� ����������
        memset(&Address,0,sizeof(Address));
        Address.sin_family=AF_INET;
        Address.sin_port=::htons(SEND_Port);
        Address.sin_addr.s_addr=INADDR_ANY;
        // ������������ �����
        if ( ::bind (lSocket,(sockaddr*)&Address,sizeof(Address))==SOCKET_ERROR ) goto error;
    }

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Listen()
{
    MSG Msg;
    fd_set fds;
    timeval timeout;
    int result;

    // ��������� ����� � ����� ������ ����������
    if ( ::listen(lSocket,1) ) goto exit;
    // ������� ����������
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
		if ( Break.load() ) goto exit;

		FD_ZERO(&fds); FD_SET(lSocket,&fds);
        if ( (result=::select(0,&fds,nullptr,nullptr,&timeout))==SOCKET_ERROR ) goto exit;
    } while((result!=1)||(!FD_ISSET(lSocket,&fds)));

    return true;
exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Accept()
{
    // ��������� ����������
    rSocket=::accept(lSocket,nullptr,nullptr);
    return rSocket!=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
bool MSend::Connect(const wchar_t *IP_, unsigned Time_)
{
    MSG Msg;
    sockaddr_in Address;
    fd_set w_fds, e_fds;
    timeval timeout;
    DWORD stime;
    int result;

    // ������ ����� ����������
    memset(&Address,0,sizeof(Address));
    Address.sin_family=AF_INET;
	Address.sin_port=::htons(SEND_Port);
//	Address.sin_addr.s_addr=::inet_addr(IP_);
//	if ( Address.sin_addr.s_addr==INADDR_NONE ) goto exit;
	const wchar_t *term;
	if ( ::RtlIpv4StringToAddressW(
		IP_,
		TRUE,
		&term,                              /// nullptr allowed ?
		&Address.sin_addr
		) != STATUS_SUCCESS ) goto exit;

	// ��������� ������������ ����������
//    if ( ::connect(rSocket,(sockaddr*)&Address,sizeof(sockaddr_in))==SOCKET_ERROR ) goto exit;
	::connect(rSocket,(sockaddr*)&Address,sizeof(sockaddr_in));
	// ������� ������������ ����������
    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
		if ( Break.load() ) goto exit;

        // ������� ����������� �������� ��� ������
		FD_ZERO(&w_fds); FD_SET(rSocket,&w_fds);
		FD_ZERO(&e_fds); FD_SET(rSocket,&e_fds);
		if ( (result=::select(0,nullptr,&w_fds,&e_fds,&timeout))==0 ) continue;
		if ( (result==SOCKET_ERROR)||FD_ISSET(rSocket,&e_fds) ) goto exit;
		if ( FD_ISSET(rSocket,&w_fds) ) return true;
	} while((::GetTickCount()-stime)<Time_);

exit:
	return false;
}
//---------------------------------------------------------------------------
bool MSend::Snd(char *Data_, std::size_t Size_, unsigned Time_)
{
	MSG Msg;
	fd_set w_fds, e_fds;
	timeval timeout;
	DWORD stime;
	int result;

	Time_*=1000; stime=::GetTickCount();
	timeout.tv_sec=0; timeout.tv_usec=100*1000;
	do
	{
		if ( Break.load() ) goto exit;

		// ������� ����������� �������� ��� ������
        FD_ZERO(&w_fds); FD_SET(rSocket,&w_fds);
        FD_ZERO(&e_fds); FD_SET(rSocket,&e_fds);
        if ( (result=::select(0,nullptr,&w_fds,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(rSocket,&e_fds) ) goto exit;
        // ������� ��������� ������ ������
        if ( (result=::send(rSocket,Data_,Size_,0))==SOCKET_ERROR ) goto exit;
        if ( (Size_-=result)==0 ) return true;
        Data_+=result;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Rcv(char *Data_, std::size_t Size_, unsigned Time_)
{
    MSG Msg;
    fd_set r_fds, e_fds;
    timeval timeout;
    DWORD stime;
    int result;

    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
		if ( Break.load() ) goto exit;

        // ������� ����������� ������� ��� ������
        FD_ZERO(&r_fds); FD_SET(rSocket,&r_fds);
        FD_ZERO(&e_fds); FD_SET(rSocket,&e_fds);
        if ( (result=::select(0,&r_fds,nullptr,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(rSocket,&e_fds) ) goto exit;
        // ��������� ��������� ������ ������
        result=::recv(rSocket,Data_,Size_,0);
        if ( (result==SOCKET_ERROR)||(result==0) ) goto exit;
        if ( (Size_-=result)==0 ) return true;
        Data_+=result;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Disconnect(unsigned Time_)
{
    MSG Msg;
    fd_set r_fds, e_fds;
    timeval timeout;
    DWORD stime;
    int result;
    char buffer[1];

    // ��������� ��������, ��� �������� ������ ������ �� �����
    if ( ::shutdown(rSocket,SD_SEND)==SOCKET_ERROR ) goto exit;
    //
    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
		if ( Break.load() ) goto exit;

        // ������� ����������� ������� ��� ������
        FD_ZERO(&r_fds); FD_SET(rSocket,&r_fds);
        FD_ZERO(&e_fds); FD_SET(rSocket,&e_fds);
        if ( (result=::select(0,&r_fds,nullptr,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(rSocket,&e_fds) ) goto exit;
        // ��������� ������ �� ������ ����� �����
        result=::recv(rSocket,buffer,sizeof(buffer),0);
///        result=::recv(lSocket,buffer,sizeof(buffer),0);  // ������ ?
        if ( result==SOCKET_ERROR ) goto exit;
        if ( result==0 ) return true;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
void MSend::lClose()
{
    // ������������� �����/�������� ������
    ::shutdown(lSocket,SD_BOTH);
    // ��������� �����
    ::closesocket(lSocket);
    lSocket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
void MSend::rClose()
{
    // ������������� �����/�������� ������
    ::shutdown(rSocket,SD_BOTH);
    // ��������� �����
    ::closesocket(rSocket);
    rSocket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
bool MSend::SndHello(std::uint64_t SessId_)
{
	// ��������� hello-���� �� ����� random
	Packet.Hello.Version=SEND_Version;
	Packet.Hello.SessId=SessId_;
	// ��������� MAC � �������
	NetMAC->Calc((char*)&Packet.Hello,
		sizeof(Packet.Hello)-sizeof(Packet.Hello.MAC),
		Packet.Hello.MAC,sizeof(Packet.Hello.MAC));
	BasicEncode((char*)&Packet.Hello,sizeof(Packet.Hello),NetCode);

	// ���������� ������� hello
	return Snd((char*)&Packet.Hello,sizeof(Packet.Hello),3);
}
//---------------------------------------------------------------------------
bool MSend::RcvHello(std::uint64_t *SessId_)
{
    if ( !Rcv((char*)&Packet.Hello,sizeof(Packet.Hello),3) ) goto error;
    BasicDecode((char*)&Packet.Hello,sizeof(Packet.Hello),NetCode);
    // ������� ������ ���������� � MAC
    if ( (Packet.Hello.Version!=SEND_Version)||
        (!NetMAC->Check((char*)&Packet.Hello,
        sizeof(Packet.Hello)-sizeof(Packet.Hello.MAC),
        Packet.Hello.MAC,sizeof(Packet.Hello.MAC))) ) goto error;
    // ���������� ���������
    *SessId_=Packet.Hello.SessId;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::SndRequest(std::uint8_t Type_, std::uint64_t SessId_, std::size_t Size_)
{
    // ��������� ����-��������
    Packet.Request.Type=Type_;
    Packet.Request.SessId=SessId_;
    Packet.Request.Size=Size_;
    // ��������� � ����� MAC � ������� ���
    NetMAC->Calc((char*)&Packet.Request,
        sizeof(Packet.Request)-sizeof(Packet.Request.MAC),
        Packet.Request.MAC,sizeof(Packet.Request.MAC));
    BasicEncode((char*)&Packet.Request,sizeof(Packet.Request),NetCode);

    // ���������� �������
    return Snd((char*)&Packet.Request,sizeof(Packet.Request),3);
}
//---------------------------------------------------------------------------
bool MSend::RcvRequest(std::uint8_t *Type_, std::uint64_t SessId_, std::size_t *Size_)
{
    if ( !Rcv((char*)&Packet.Request,sizeof(Packet.Request),3) ) goto error;
    BasicDecode((char*)&Packet.Request,sizeof(Packet.Request),NetCode);
    // ������� seed � MAC
    if ( (Packet.Request.SessId!=SessId_)||
        (!NetMAC->Check((char*)&Packet.Request,
        sizeof(Packet.Request)-sizeof(Packet.Request.MAC),
        Packet.Request.MAC,sizeof(Packet.Request.MAC))) ) goto error;
    // ���������� ���������
    *Type_=Packet.Request.Type;
    *Size_=Packet.Request.Size;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
template <typename obj_type>
bool MSend::SndObject(obj_type *Obj_, std::uint8_t Type_, std::uint64_t SessId_)
{
	std::vector <char> Data;

	try
	{
		// ���������� ������ ������ � �������� ������ ��� ������ (+ seed,MAC)
		std::size_t Size=Obj_->GetAllDataSize()+sizeof(decltype(SessId_))+MAC_Size;
		Data.resize(Size);
		// ��������� ���
		char *pt=static_cast<char*>(MemSet(&Data[0], SessId_));
		pt=static_cast<char*>(Obj_->SetAllData(pt));
		// ��������� MAC � �������
		NetMAC->Calc(&Data[0],pt-&Data[0],pt,&Data[0]+Size-pt);
		BasicEncode(&Data[0],Size,NetCode);

		// ���������� ������� �������� � ������
		if ( (!SndRequest(Type_,SessId_,Size))||
			(!Snd(&Data[0],Size,10)) ) goto error;
	}
	catch (std::bad_alloc &e) { goto error; }

	return true;
error:
	return false;
}
//---------------------------------------------------------------------------
template <typename obj_type>
bool MSend::RcvObject(obj_type *Obj_, std::size_t Size_, std::uint64_t SessId_)
{
    std::vector <char> Data;

    try
    {
        // ��������� ������������ ������� ������ � �������� ������
        if ( (Size_<SEND_MinData)||
            (Size_>SEND_MaxData) ) goto error;
        Data.resize(Size_);

        // ��������� �� � ��������������
        if ( !Rcv(&Data[0],Size_,10) ) goto error;
        BasicDecode(&Data[0],Size_,NetCode);
        // ������� seed � MAC
        if ( (*((decltype(SessId_)*)&Data[0])!=SessId_)||
            (!NetMAC->Check(&Data[0],Size_-MAC_Size,
            &Data[0]+(Size_-MAC_Size),MAC_Size)) ) goto error;

        // ��������������� ������ �� �������� ������ (�� ������� seed � MAC)
		if ( !Obj_->GetAllData(&Data[0]+sizeof(decltype(SessId_)),
            &Data[0]+(Size_-MAC_Size)) ) goto error;
    }
    catch (std::bad_alloc &e) { goto error; }

    return true;
error:
    // ������� �������� �� ��������� ����������� ������
    Obj_->Clear();
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Start()
{
	if (
		(!Init) ||
		Thread.joinable() ) return false;   /// throw ?
//		(!Init) ) return false;
//	if ( Thread.joinable() ) Thread.join();

	Break.store(false);
	try
	{
		// ������� ����� ��� ���������� �������� � ������ ������
		std::thread TmpThread(
			[](MSend *Obj_)
			{
				Obj_->ThreadP();
			}, this);
		Thread.swap(TmpThread);
	}
	catch (std::system_error &e)
	{
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
void MSend::Stop()
{
	if ( !Thread.joinable() ) return;

	Break.store(true);
	Thread.join();
}
//---------------------------------------------------------------------------
void MSendSrv::Event(MComputersItem *Computer_, int Event_)
{
//    ::Sleep(1000);  /// ��� ���������� ������� �������
    if ( Window==nullptr ) return;
	::PostMessage(Window, MinMsg+Event_, (WPARAM)Computer_, 0);			// ����������
//    ::SendMessage(Window, MinMsg+Event_, (WPARAM)Computer_, nullptr);	// �������� ���������� �������
}
//---------------------------------------------------------------------------
void MSendSrv::ThreadP()
{
    // ��������� ���� ������
    switch(Mode)
    {
        case mssSendGames:
        case mssSendConfig:
            ThreadSend();
            break;
        case mssGetGames:
        case mssGetConfig:
            ThreadGet();
            break;
        default:
            break;
    }
    //
    Mode=mssNone;
    Event(nullptr,mseFreeParam);
}
//---------------------------------------------------------------------------
void MSendSrv::ThreadSend()
{
	std::uint64_t SessId, RmtSessId;
	std::size_t Size;
    std::uint8_t Type;
	MComputersItem *pComp;

	for ( std::size_t i=0; i<Comps->size(); i++ )
	{
		// ������� ����� ��� ���������� ����������
		if ( !Create(true) ) goto next;
		pComp=(*Comps)[i];
		// ������������� ����������
		Event(pComp,mseConnecting);
		if ( !Connect(pComp->Address.c_str(),2) ) { Event(pComp,mseNotConnect); goto next; }
		Event(pComp,mseSending);

		// �������� ����� hello � ���������� ID ������
		SessId=NextSessId();
		if ( (!SndHello(SessId))||
			(!RcvHello(&RmtSessId)) ) { Event(pComp,mseProtError); goto next; }

		// ����� ������������� ��������� ID �� ������ random � �������
		{
			std::array <decltype(SessId),2> IdV;
			IdV[0]=SessId;
			IdV[1]=RmtSessId;
			SessId=fasthash64(&IdV,sizeof(IdV),0);	/// ����� ������������ Hash-IV
		}

		// ���������� ����-�������� � ������
		switch(Mode)
		{
			case mssSendGames:
				if ( !SndObject(ObjGames,mstSendGames,SessId) ) {
					Event(pComp,mseProtError); goto next; }
				break;
			case mssSendConfig:
				if ( !SndObject(ObjOptions,mstSendConfig,SessId) ) {
					Event(pComp,mseProtError); goto next; }
				break;
			default:
				goto next;
		}

		// ��������� �������������
		if ( !RcvRequest(&Type,SessId,&Size) ) { Event(pComp,mseProtError); goto next; }
		if ( (Type!=mstAccept)||(Size!=0) ) { Event(pComp,mseProtError); goto next; }

		// ��������� ����������
		Event(pComp,mseDisconnecting);
		if ( !Disconnect(10) ) { Event(pComp,mseProtError); goto next; }

		Event(pComp,mseExecute);
next:
		rClose();
		if ( Break.load() ) break;
	}
}
//---------------------------------------------------------------------------
void MSendSrv::ThreadGet()
{
	std::uint64_t SessId, RmtSessId;
	std::size_t Size;
	std::uint8_t Type;

	if ( !Create(true) ) goto error;
	// ������������� ����������
	Event(Comp,mseConnecting);
	if ( !Connect((Comp)->Address.c_str(),2) ) { Event(Comp,mseNotConnect); goto error; }
	Event(Comp,mseReceiving);

	// �������� ����� hello � ���������� ID ������
	SessId=NextSessId();
	if ( (!SndHello(SessId))||
		(!RcvHello(&RmtSessId)) ) { Event(Comp,mseProtError); goto error; }

	// ����� ������������� ��������� ID �� ������ random � �������
	{
		std::array <decltype(SessId),2> IdV;
		IdV[0]=SessId;
		IdV[1]=RmtSessId;
		SessId=fasthash64(&IdV,sizeof(IdV),0);	/// ����� ������������ Hash-IV
	}

	// ���������� ������ �� ������
	switch(Mode)
	{
		case mssGetGames:   Type=mstGetGames; break;
		case mssGetConfig:  Type=mstGetConfig; break;
		default: goto error;
	}
	if ( !SndRequest(Type,SessId,0) ) { Event(Comp,mseProtError); goto error; }

	// ��������� ����� � ������
	if ( (!RcvRequest(&Type,SessId,&Size))||
		(Type!=mstAccept) ) { Event(Comp,mseProtError); goto error; }
	switch(Mode)
	{
		case mssGetGames:
			if ( !RcvObject(ObjGames,Size,SessId) ) {
				Event(Comp,mseProtError); goto error; }
			break;
		case mssGetConfig:
			if ( !RcvObject(ObjOptions,Size,SessId) ) {
				Event(Comp,mseProtError); goto error; }
			break;
		default:
			break;
	}

	// ��������� ����������
	Event(Comp,mseDisconnecting);
	if ( !Disconnect(10) ) { Event(Comp,mseProtError); goto error; }

	Event(Comp,mseExecute);
error:
	rClose();
}
//---------------------------------------------------------------------------
bool MSendSrv::NetInit(HWND Window_, UINT MinMsg_, unsigned Code_, MAuth *MAC_)
{
	Window=Window_;
	MinMsg=MinMsg_;
	return MSend::NetInit(Code_,MAC_);
}
//---------------------------------------------------------------------------
bool MSendSrv::NetFree()
{
    return MSend::NetFree();
}
//---------------------------------------------------------------------------
bool MSendSrv::Send(std::vector <MComputersItem*> *Computers_, MGames *Games_, MClOptions *Options_)
{
    if ( Mode!=mssNone ) goto error;
//    // ��������� ������� ������� ��������
//    Stop();

    if ( Computers_==nullptr ) goto error; /// throw ?
    Comps=Computers_;
    if ( Games_!=nullptr )
    {
		ObjGames=Games_;
		ObjOptions=nullptr;
		Mode=mssSendGames;
	} else if ( Options_!=nullptr )
	{
		ObjGames=nullptr;
		ObjOptions=Options_;
		Mode=mssSendConfig;
	} else goto error;                  /// throw ?

	// ������� �����, ������� �������� ������ �����������
	return Start();
error:
	return false;
}
//---------------------------------------------------------------------------
bool MSendSrv::Get(MComputersItem *Computer_, MGames *Games_, MClOptions *Options_)
{
	if ( Mode!=mssNone ) goto error;
//    // ��������� ������� ������� ��������
//    Stop();

	if ( Computer_==nullptr ) goto error;  /// throw ?
	Comp=Computer_;
	if ( Games_!=nullptr )
	{
		ObjGames=Games_;
		ObjOptions=nullptr;
		Mode=mssGetGames;
	} else if ( Options_!=nullptr )
	{
		ObjGames=nullptr;
		ObjOptions=Options_;
        Mode=mssGetConfig;
    } else goto error;                  /// throw ?

    // ������� �����, ������� �������� ������� ������
    return Start();
error:
    return false;
}
//---------------------------------------------------------------------------
void MSendSrv::Stop()
{
    MSend::Stop();
}
//---------------------------------------------------------------------------
void MSendCl::ThreadP()
{
	std::uint64_t SessId, RmtSessId;
	std::size_t Size;
	std::uint8_t Type;

	// ������� ����������� (���������) �����
	if ( !Create(false) ) return;
	// ������������ ����������� ����������
	while(Listen())
	{
		// ��������� ����������
		if ( !Accept() ) continue;

		// ��������� hello � random �������
		if ( !RcvHello(&RmtSessId) ) goto next;
		// ���������� ����
		SessId=NextSessId();
		if ( !SndHello(SessId) ) goto next;
		// ��������� ��������� ID � ��������� ������
		{
			std::array <decltype(SessId),2> IdV;
			IdV[0]=RmtSessId;
			IdV[1]=SessId;
			SessId=fasthash64(&IdV,sizeof(IdV),0);	/// ����� ������������ Hash-IV
		}
		if ( !RcvRequest(&Type,SessId,&Size) ) goto next;

        // ������������ ������
        switch(Type)
        {
            case mstSendGames:
				if ( (!RcvObject(&Games,Size,SessId))||
					(!State->NewGames(Games))||
					(!SndRequest(mstAccept,SessId,0)) ) goto next;
				break;
			case mstSendConfig:
				if ( (!RcvObject(&Options,Size,SessId))||
					(!State->NewOptions(Options))||
					(!SndRequest(mstAccept,SessId,0)) ) goto next;
				break;
			case mstGetGames:
				if ( (!State->GetGames(Games))||
					(!SndObject(&Games,mstAccept,SessId)) ) goto next;
				break;
			case mstGetConfig:
				if ( (!State->GetOptions(Options))||
					(!SndObject(&Options,mstAccept,SessId)) ) goto next;
				break;
            default: goto next;
        }

        // ��������� ����������
        Disconnect(10);
next:
        // ��������� �����
        rClose();
        // ��������� ������ �� ���������� ����������
        Games.Clear();
    }
    // ��������� ����������� �����
    lClose();
}
//---------------------------------------------------------------------------
bool MSendCl::NetInit(MStateCl *State_, unsigned Code_, MAuth *MAC_)
{
    State=State_;
    return MSend::NetInit(Code_,MAC_);
}
//---------------------------------------------------------------------------
bool MSendCl::NetFree()
{
    return MSend::NetFree();
}
//---------------------------------------------------------------------------
bool MSendCl::Start()
{
    return MSend::Start();
}
//---------------------------------------------------------------------------
void MSendCl::Stop()
{
    MSend::Stop();
}
//---------------------------------------------------------------------------

