//---------------------------------------------------------------------------
#include <winsock2.h>
#include <ip2string.h>
#include <ntstatus.h>
#pragma hdrstop

#include "UnitSend.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool MSend::NetInit(unsigned Code_, MAuth *MAC_)
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
        if ( ::PeekMessage(&Msg,(HWND)-1,0,0,PM_REMOVE) )
        {
            Break=true;
            goto exit;
        }
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
        if ( ::PeekMessage(&Msg,(HWND)-1,0,0,PM_REMOVE) )
        {
            Break=true;
            goto exit;
        }
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
bool MSend::Snd(char *Data_, unsigned Size_, unsigned Time_)
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
        if ( ::PeekMessage(&Msg,(HWND)-1,0,0,PM_REMOVE) )
        {
            Break=true;
            goto exit;
        }
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
bool MSend::Rcv(char *Data_, unsigned Size_, unsigned Time_)
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
        if ( ::PeekMessage(&Msg,(HWND)-1,0,0,PM_REMOVE) )
        {
            Break=true;
            goto exit;
        }
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
        if ( ::PeekMessage(&Msg,(HWND)-1,0,0,PM_REMOVE) )
        {
            Break=true;
            goto exit;
        }
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
bool MSend::SndHello(unsigned Seed_)
{
    // ��������� hello-���� �� ����� random
    Packet.Hello.Version=SEND_Version;
    Packet.Hello.Seed=Seed_;
    // ��������� MAC � �������
    NetMAC->Calc((char*)&Packet.Hello,
        sizeof(Packet.Hello)-sizeof(Packet.Hello.MAC),
        Packet.Hello.MAC,sizeof(Packet.Hello.MAC));
    BasicEncode((char*)&Packet.Hello,sizeof(Packet.Hello),NetCode);

    // ���������� ������� hello
    return Snd((char*)&Packet.Hello,sizeof(Packet.Hello),3);
}
//---------------------------------------------------------------------------
bool MSend::RcvHello(unsigned *Seed_)
{
    if ( !Rcv((char*)&Packet.Hello,sizeof(Packet.Hello),3) ) goto error;
    BasicDecode((char*)&Packet.Hello,sizeof(Packet.Hello),NetCode);
    // ������� ������ ���������� � MAC
    if ( (Packet.Hello.Version!=SEND_Version)||
        (!NetMAC->Check((char*)&Packet.Hello,
        sizeof(Packet.Hello)-sizeof(Packet.Hello.MAC),
        Packet.Hello.MAC,sizeof(Packet.Hello.MAC))) ) goto error;
    // ���������� ���������
    *Seed_=Packet.Hello.Seed;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::SndRequest(unsigned char Type_, unsigned Seed_, unsigned Size_)
{
    // ��������� ����-��������
    Packet.Request.Type=Type_;
    Packet.Request.Seed=Seed_;
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
bool MSend::RcvRequest(unsigned char *Type_, unsigned Seed_, unsigned *Size_)
{
    if ( !Rcv((char*)&Packet.Request,sizeof(Packet.Request),3) ) goto error;
    BasicDecode((char*)&Packet.Request,sizeof(Packet.Request),NetCode);
    // ������� seed � MAC
    if ( (Packet.Request.Seed!=Seed_)||
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
bool MSend::SndObject(obj_type *Obj_, unsigned Type_, unsigned Seed_)
{
	Marray <char> Data;
	char *pt;
	unsigned Size;

	try
	{
		// ���������� ������ ������ � �������� ������ ��� ������ (+ seed,MAC)
		Size=Obj_->GetAllDataSize()+sizeof(unsigned)+MAC_Size;
		Data.Alloc(Size);
		// ��������� ���
		pt=static_cast<char*>(MemSet(&Data[0],(unsigned)Seed_));
		pt=static_cast<char*>(Obj_->SetAllData(pt));
		// ��������� MAC � �������
		NetMAC->Calc(&Data[0],pt-&Data[0],pt,&Data[0]+Size-pt);
		BasicEncode(&Data[0],Size,NetCode);

		// ���������� ������� �������� � ������
		if ( (!SndRequest(Type_,Seed_,Size))||
			(!Snd(&Data[0],Size,10)) ) goto error;
	}
	catch (std::bad_alloc &e) { goto error; }

	return true;
error:
	return false;
}
//---------------------------------------------------------------------------
template <typename obj_type>
bool MSend::RcvObject(obj_type *Obj_, unsigned Size_, unsigned Seed_)
{
    Marray <char> Data;

    try
    {
        // ��������� ������������ ������� ������ � �������� ������
        if ( (Size_<SEND_MinData)||
            (Size_>SEND_MaxData) ) goto error;
        Data.Alloc(Size_);

        // ��������� �� � ��������������
        if ( !Rcv(&Data[0],Size_,10) ) goto error;
        BasicDecode(&Data[0],Size_,NetCode);
        // ������� seed � MAC
        if ( (*((unsigned*)&Data[0])!=Seed_)||
            (!NetMAC->Check(&Data[0],Size_-MAC_Size,
            &Data[0]+(Size_-MAC_Size),MAC_Size)) ) goto error;

        // ��������������� ������ �� �������� ������ (�� ������� seed � MAC)
        if ( !Obj_->GetAllData(&Data[0]+sizeof(unsigned),
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
DWORD WINAPI MSend::ThreadF(LPVOID Data)
{
    MSG msg;
    
    // ������� ������� ��������� ��� ������
///    ::PeekMessage(nullptr,(HWND)-1,0,0,PM_NOREMOVE);
    ::PeekMessage(&msg,nullptr,WM_USER,WM_USER,PM_NOREMOVE);
    ((MSend*)Data)->Break=false;
    // ��������� ���� ������
    ((MSend*)Data)->ThreadP();
    // ��������� ������ ������
    ::ExitThread(0);
    return 0;
}
//---------------------------------------------------------------------------
bool MSend::Start()
{
    if ( !Init ) return false;
    // ������� ����� ��� ���������� �������� � ������ ������
    return (Thread=::CreateThread(nullptr,0,&ThreadF,
        (LPVOID)this,0,&ThreadID))!=nullptr;
}
//---------------------------------------------------------------------------
void MSend::Stop()
{
    DWORD ExitCode;
    // ��������� �������� �� �����
    if ( Thread==nullptr ) return;
    if ( ::GetExitCodeThread(Thread,&ExitCode)&&(ExitCode==STILL_ACTIVE) )
    {
        // �������� ������ ��������� � ���� ���������� ��� ������
        ::PostThreadMessage(ThreadID,WM_USER,0,0);
        ::WaitForSingleObject(Thread,INFINITE);
    }
    // ��������� ��������� ������
    ::CloseHandle(Thread);
    Thread=nullptr;
    ThreadID=0;
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
	unsigned Seed, RmtSeed, Size;
    unsigned char Type;
	MComputersItem *pComp;

	for ( unsigned i=0; i<Comps->Count(); i++ )
	{
		// ������� ����� ��� ���������� ����������
		if ( !Create(true) ) goto next;
		pComp=(*Comps)[i];
		// ������������� ����������
		Event(pComp,mseConnecting);
		if ( !Connect(pComp->Address.c_str(),2) ) { Event(pComp,mseNotConnect); goto next; }
		Event(pComp,mseSending);

		// �������� ����� hello � ���������� ID ������
		Seed=BasicRand();
		if ( (!SndHello(Seed))||
			(!RcvHello(&RmtSeed)) ) { Event(pComp,mseProtError); goto next; }
		Seed=BasicMix(Seed,RmtSeed);

		// ���������� ����-�������� � ������
		switch(Mode)
		{
			case mssSendGames:
				if ( !SndObject(ObjGames,mstSendGames,Seed) ) {
					Event(pComp,mseProtError); goto next; }
				break;
			case mssSendConfig:
				if ( !SndObject(ObjOptions,mstSendConfig,Seed) ) {
					Event(pComp,mseProtError); goto next; }
				break;
			default:
				goto next;
		}

		// ��������� �������������
		if ( !RcvRequest(&Type,Seed,&Size) ) { Event(pComp,mseProtError); goto next; }
		if ( (Type!=mstAccept)||(Size!=0) ) { Event(pComp,mseProtError); goto next; }

		// ��������� ����������
		Event(pComp,mseDisconnecting);
		if ( !Disconnect(10) ) { Event(pComp,mseProtError); goto next; }

		Event(pComp,mseExecute);
next:
		rClose();
		if ( Break ) break;
	}
}
//---------------------------------------------------------------------------
void MSendSrv::ThreadGet()
{
	unsigned Seed, RmtSeed, Size;
	unsigned char Type;

	if ( !Create(true) ) goto error;
	// ������������� ����������
	Event(Comp,mseConnecting);
	if ( !Connect((Comp)->Address.c_str(),2) ) { Event(Comp,mseNotConnect); goto error; }
	Event(Comp,mseReceiving);

	// �������� ����� hello � ���������� ID ������
	Seed=BasicRand();
	if ( (!SndHello(Seed))||
		(!RcvHello(&RmtSeed)) ) { Event(Comp,mseProtError); goto error; }
	Seed=BasicMix(Seed,RmtSeed);

	// ���������� ������ �� ������
	switch(Mode)
	{
		case mssGetGames:   Type=mstGetGames; break;
		case mssGetConfig:  Type=mstGetConfig; break;
		default: goto error;
	}
	if ( !SndRequest(Type,Seed,0) ) { Event(Comp,mseProtError); goto error; }

	// ��������� ����� � ������
	if ( (!RcvRequest(&Type,Seed,&Size))||
		(Type!=mstAccept) ) { Event(Comp,mseProtError); goto error; }
	switch(Mode)
	{
		case mssGetGames:
			if ( !RcvObject(ObjGames,Size,Seed) ) {
				Event(Comp,mseProtError); goto error; }
			break;
		case mssGetConfig:
			if ( !RcvObject(ObjOptions,Size,Seed) ) {
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
bool MSendSrv::Send(Marray <MComputersItem*> *Computers_, MGames *Games_, MClOptions *Options_)
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
    unsigned Seed, RmtSeed, Size;
    unsigned char Type;

    // ������� ����������� (���������) �����
    if ( !Create(false) ) return;
    // ������������ ����������� ����������
    while(Listen())
    {
        // ��������� ����������
        if ( !Accept() ) continue;

        // ��������� hello � random �������
        if ( !RcvHello(&RmtSeed) ) goto next;
        // ���������� ����
        Seed=BasicRand();
        if ( !SndHello(Seed) ) goto next;
        // ��������� ��������� ID � ��������� ������
        Seed=BasicMix(Seed,RmtSeed);
        if ( !RcvRequest(&Type,Seed,&Size) ) goto next;

        // ������������ ������
        switch(Type)
        {
            case mstSendGames:
                if ( (!RcvObject(&Games,Size,Seed))||
                    (!State->NewGames(&Games))||
                    (!SndRequest(mstAccept,Seed,0)) ) goto next;
                break;
            case mstSendConfig:
                if ( (!RcvObject(&Options,Size,Seed))||
                    (!State->NewOptions(&Options))||
                    (!SndRequest(mstAccept,Seed,0)) ) goto next;
                break;
            case mstGetGames:
                if ( (!State->GetGames(&Games))||
                    (!SndObject(&Games,mstAccept,Seed)) ) goto next;
                break;
            case mstGetConfig:
                if ( (!State->GetOptions(&Options))||
                    (!SndObject(&Options,mstAccept,Seed)) ) goto next;
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

