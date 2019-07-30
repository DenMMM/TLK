//---------------------------------------------------------------------------
#include <stdlib.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdexcept.h>
#pragma hdrstop

#include "UnitSync.h"
#include "UnitAuth.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MSyncState::MSyncState()
{
    State=NULL;
    IP=INADDR_NONE;
    KnownMAC=false;
    Process=mspNone;
    PacketSize=0;
    SendCount=0;
    LastSendTime=0;
    Seed=0;
}
//---------------------------------------------------------------------------
MSyncState::~MSyncState()
{
//
}
//---------------------------------------------------------------------------
unsigned MSyncState::GetDataSize() const
{
    return KnownMAC?
        sizeof(IP)+
        sizeof(MAC): 0;
}
//---------------------------------------------------------------------------
char *MSyncState::SetData(char *Data_) const
{
    if ( !KnownMAC ) return Data_;
    Data_=MemSet(Data_,IP);
    Data_=MemSetBLine(Data_,MAC,sizeof(MAC));
    return Data_;
}
//---------------------------------------------------------------------------
const char *MSyncState::GetData(const char *Data_, const char *Limit_)
{
    if ( (Data_=MemGet(Data_,&IP,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetBLine(Data_,MAC,sizeof(MAC),Limit_))==NULL ) goto error;
    KnownMAC=true;
error:
    return Data_;
}
//---------------------------------------------------------------------------
void MSyncState::Associate(MState *State_, u_long IP_)
{
    State=State_;
    KnownMAC=(IP==IP_);
    IP=IP_;
    Process=mspNone;
}
//---------------------------------------------------------------------------
bool MSyncState::UpdateMAC(unsigned char *MAC_)
{
    bool NeedSave;
    // ��������� ����� �� �������� MAC-�����
    NeedSave=(!KnownMAC)||(memcmp(MAC,MAC_,MAC_AddrLength)!=0);
    // ��� ������������� ��������� MAC-�����
    if ( NeedSave ) { memcpy(MAC,MAC_,MAC_AddrLength); KnownMAC=true; }
    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncState::Start()
{
    if ( (State==NULL)||(IP==INADDR_NONE) ) return false;
    Process=mspWait;
    LastSendTime=::GetTickCount()-SYNC_WaitInterval;
    return true;
}
//---------------------------------------------------------------------------
bool MSyncState::Stop()
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
bool MSyncState::Send(SOCKET Socket_, SOCKET SocketBC_, unsigned Code_, MAuth *MAC_)
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
                State->NetSyncData(&SyncData);
                //
                Seed=BasicRand();
                // ��������� ��������� hello-������ �� ����� random
                Packet.Hello.Header.Version=SYNC_Version;
                Packet.Hello.Header.Type=mptHello;
                Packet.Hello.Header.Seed=Seed;
                // ��������� MAC
                MAC_->Calc((char*)&Packet.Hello,
                    sizeof(Packet.Hello)-sizeof(Packet.Hello.MAC),
                    Packet.Hello.MAC,sizeof(Packet.Hello.MAC));
                // ������ ������ ������ ��� �������� � ������� ���
                PacketSize=sizeof(Packet.Hello);
                BasicEncode((char*)&Packet,PacketSize,Code_);
                // �������� ��������, ������� �� �� ��������� �����
                LastSendTime=::GetTickCount()-Seed%SYNC_SendInterval;
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
                // ��������� ����� �������� ������ � ����������� �������
                LastSendTime=::GetTickCount(); SendCount++;
            }
            break;
        default: break;
    }

    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncState::Recv(char *Packet_, int PacketSize_, unsigned Code_, MAuth *MAC_)
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
        case mptHello:
            // ���� �� �� ���� ����� � ������ �� � ���� ������ ?
            if ( (Process!=mspHello)||
                (PacketSize_!=sizeof(MSyncPHello)) ) break;
            // ����� ���������� �� �������, �� �������� ��� mspHello
//            memset(&Address,0,sizeof(Address));
//            Address.sin_family=AF_INET;
//            Address.sin_port=::htons(SYNC_Port);
//            Address.sin_addr.s_addr=::inet_addr(IP);
            // ����� ������������� ��������� ID �� ������ random � �������
            Seed=BasicMix(Seed,RecvHeader->Seed);
            // ��������� ��������� ������
            Packet.Data.Header.Version=SYNC_Version;
            Packet.Data.Header.Type=mptSyncData;
            Packet.Data.Header.Seed=Seed;
            // �������� � ����� ����������� ��� �� ����� hello ������ ��� �������������
            Packet.Data.Data=SyncData;
            // ��������� MAC
            MAC_->Calc((char*)&Packet.Data,
                sizeof(Packet.Data)-sizeof(Packet.Data.MAC),
                Packet.Data.MAC,sizeof(Packet.Data.MAC));
            // ������ ������ ������ ��� �������� � ������� ���
            PacketSize=sizeof(Packet.Data);
            BasicEncode((char*)&Packet,PacketSize,Code_);
            //  �������� ��������
            LastSendTime=::GetTickCount()-SYNC_SendInterval;
            SendCount=0; Process=mspSyncData;
            break;
        case mptSyncConf:
            // ���� �� �� ���� �����, ������ �� � ���� ������ � seed ?
            if ( (Process!=mspSyncData)||
                (PacketSize_!=sizeof(MSyncPConf))||
                (RecvHeader->Seed!=Seed) ) break;
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
    MSyncState *SyncState, *LastSyncState;
    MComputer *Computer;
    u_long IP;

    LastSyncState=(MSyncState*)gFirst();
    for ( MState *State=(MState*)States_->gFirst();
        State; State=(MState*)State->gNext() )
    {
        // ���� ���������, � ������� ������������� ���������
        Computer=Computers_->Search(State->Associated());
        if ( Computer==NULL ) continue;
        // ������������ IP-����� �� ������ � u_long
        IP=::inet_addr(Computer->Address);
        if ( IP==INADDR_NONE ) continue;
        // ���� �� ����� ������ ��������� �������������
        // (����� ��������� �������� ��������� MAC-�����)
        // ��� ��������� �����
        SyncState=(SyncState=Search(IP))!=NULL? SyncState: (MSyncState*)Add();
        // ����������� ��������� ������������� � ���������� ���������� � ��� �������
        SyncState->Associate(State,IP);
        // ���� ��������� �� ��������� � ������, ��������� ��� � ������
        if ( LastSyncState!=NULL ) Exch(LastSyncState,SyncState);
        LastSyncState=(MSyncState*)SyncState->gNext();
    }
    // ������� ���������� �� ������� �������
    while(LastSyncState)
    {
        SyncState=(MSyncState*)LastSyncState->gNext();
        Del(LastSyncState);
        LastSyncState=SyncState;
    }
}
//---------------------------------------------------------------------------
bool MSyncStates::Start() const
{
    for ( MSyncState *SyncState=(MSyncState*)gFirst();
        SyncState; SyncState=(MSyncState*)SyncState->gNext() )
        if ( !SyncState->Start() ) return false;
    return true;
}
//---------------------------------------------------------------------------
bool MSyncStates::Stop() const
{
    bool NeedSave=false;
    for ( MSyncState *SyncState=(MSyncState*)gFirst(); SyncState;
        SyncState=(MSyncState*)SyncState->gNext() ) NeedSave|=SyncState->Stop();
    return NeedSave;
}
//---------------------------------------------------------------------------
MSyncState *MSyncStates::Search(u_long IP_) const
{
    MSyncState *SyncState=(MSyncState*)gFirst();
    while(SyncState)
    {
        if ( SyncState->gIP()==IP_ ) break;
        SyncState=(MSyncState*)SyncState->gNext();
    }
    return SyncState;
}
//---------------------------------------------------------------------------
MSync::MSync()
{
    Init=false;
    AutoSaveARP=false;
    Thread=NULL;
    ThreadID=0;
    NetCode=0;
    NetMAC=NULL;
    States=NULL;
    Socket=INVALID_SOCKET;
    SocketBC=INVALID_SOCKET;
    ::InitializeCriticalSection(&CS_PCount);
    PCount=0;
}
//---------------------------------------------------------------------------
MSync::~MSync()
{
    ::DeleteCriticalSection(&CS_PCount);
}
//---------------------------------------------------------------------------
void MSync::SetARPFile(char *File_, unsigned Code_, bool AutoSave_)
{
    AutoSaveARP=AutoSave_;
    SyncStates.SetDefaultFile(File_,Code_);
}
//---------------------------------------------------------------------------
bool MSync::NetInit(unsigned Code_, MAuth *MAC_)
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
    States=NULL;
    SyncStates.Associate(States_,Computers_);
    if ( !SyncStates.Save() ) return false;
    States=States_;
    return true;
}
//---------------------------------------------------------------------------
bool MSync::Start()
{
    sockaddr_in Address;
    BOOL SetValue;

    if ( (!Init)||(States==NULL)||(Thread!=NULL) ) return false;

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
    if ( !SyncStates.Start() ) goto error;

    // ������� ����� ��� ���������� �������� � ������ ������
    Thread=::CreateThread(NULL,0,&ThreadFunc,(LPVOID)this,0,&ThreadID);
    if ( Thread==NULL ) goto error;

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
    if ( Thread==NULL ) return;
    // �������� ������ ���������
    ::PostThreadMessage(ThreadID,WM_USER,0,0);
    // ���� ���������� ������
    ::WaitForSingleObject(Thread,INFINITE);
    // ��������� ��������� ������
    ::CloseHandle(Thread);
    Thread=NULL; ThreadID=0;
    // ��������� ������
    ::closesocket(Socket); Socket=INVALID_SOCKET;
    ::closesocket(SocketBC); SocketBC=INVALID_SOCKET;
    // ��� ������������� ��������� ��������� �����������
    if ( SyncStates.Stop() ) States->Save();
}
//---------------------------------------------------------------------------
DWORD WINAPI MSync::ThreadFunc(LPVOID Data)
{
    // ������� ������� ��������� ��� ������
    ::PeekMessage(NULL,(HANDLE)-1,0,0,PM_NOREMOVE);
    // ��������� ����������
    ((MSync*)Data)->ThreadExecute();
    // ���������� ��������� - ������� �������
    ((MSync*)Data)->sPCount(0);
    // ��������� ������ ������
    ::ExitThread(0);
    return 0;
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
        for ( MSyncState *SyncState=(MSyncState*)SyncStates.gFirst(), *RecvState;
            SyncState; SyncState=(MSyncState*)SyncState->gNext() )
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
                RecvState=SyncStates.Search(FromAddr.sin_addr.s_addr);
                // �������� ��� ��� ��������� � �������� ������ ����� ��
                if ( RecvState!=NULL )
                {
                    NeedSave|=RecvState->Recv((char*)&Packet,RecvSize,NetCode,NetMAC);
                    NeedSave|=SyncState->Send(Socket,SocketBC,NetCode,NetMAC);
                }
            }
            // ���� ���������� ������ "��-����������"
            NeedSave|=SyncState->Send(Socket,SocketBC,NetCode,NetMAC);
            Process+=SyncState->gPCount();
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
        if ( ::PeekMessage(&Message,(HANDLE)-1,0,0,PM_REMOVE) ) return;

        // ��������, ���� ������ ������ ���������
        if ( (::GetTickCount()-StartCycleTime)<(SYNC_SendInterval/4) )
            ::Sleep(SYNC_SendInterval/4);
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

    return ::select(0,&WaitSockets,NULL,NULL,&WaitTimer)==1;
}
//---------------------------------------------------------------------------
bool MSync::UpdateMAC(MSyncStates *States_)
{
    PMIB_IPNETTABLE Table=NULL;
    ULONG TableSize;
    PMIB_IPNETROW Record;
    MSyncState *State;
    bool NeedSave=false;

    // ����� ������ �������
    TableSize=0;
    if ( ::GetIpNetTable(NULL,&TableSize,TRUE)!=
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
        if ( (State=States_->Search(Record->dwAddr))==NULL ) continue;
        // ��������� MAC-�����
        NeedSave|=State->UpdateMAC(Record->bPhysAddr);
    }

    delete[] Table;
    return NeedSave;
error:
    delete[] Table;
    return false;
}
//---------------------------------------------------------------------------
void MSync::sPCount(unsigned Value_)
{
    ::EnterCriticalSection(&CS_PCount);
    PCount=Value_;
    ::LeaveCriticalSection(&CS_PCount);
}
//---------------------------------------------------------------------------
unsigned MSync::gPCountMax() const
{
    return SyncStates.gCount()*SYNC_SendRetryes;
}
//---------------------------------------------------------------------------
unsigned MSync::gPCount() const
{
    unsigned result;
    ::EnterCriticalSection(&CS_PCount);
    result=PCount;
    ::LeaveCriticalSection(&CS_PCount);
    return result;
}
//---------------------------------------------------------------------------
MSyncCl::MSyncCl()
{
    Init=false;
    Thread=NULL;
    ThreadID=0;
    Socket=INVALID_SOCKET;
    State=NULL;
    Process=mspNone;
    SendCount=0;
    LastSendTime=0;
    NetCode=0;
    NetMAC=NULL;
}
//---------------------------------------------------------------------------
MSyncCl::~MSyncCl()
{
//
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

    return ::select(0,&WaitSockets,NULL,NULL,&WaitTimer)==1;
}
//---------------------------------------------------------------------------
bool MSyncCl::NetInit(unsigned Code_, MAuth *MAC_)
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

    if ( (!Init)||(Thread!=NULL) ) return false;

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

    // ������� ����� ��� ���������� �������� � ������ ������
    Thread=::CreateThread(NULL,0,&ThreadFunc,(LPVOID)this,0,&ThreadID);
    if ( Thread==NULL ) goto error;

    return true;
error:
    // ��������� �����
    if ( Socket!=INVALID_SOCKET ) { ::closesocket(Socket); Socket=INVALID_SOCKET; }
    return false;
}
//---------------------------------------------------------------------------
void MSyncCl::Stop()
{
    if ( Thread==NULL ) return;
    // �������� ������ ���������
    ::PostThreadMessage(ThreadID,WM_USER,0,0);
    // ���� ���������� ������
    ::WaitForSingleObject(Thread,INFINITE);
    // ��������� ��������� ������
    ::CloseHandle(Thread); Thread=NULL; ThreadID=0;
    // ��������� �����
    ::closesocket(Socket); Socket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
DWORD WINAPI MSyncCl::ThreadFunc(LPVOID Data)
{
    // ������� ������� ��������� ��� ������
    ::PeekMessage(NULL,(HANDLE)-1,0,0,PM_NOREMOVE);

    ((MSyncCl*)Data)->ThreadExecute();

    // ��������� ������ ������
    ::ExitThread(0);
    return 0;
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
        if ( ::PeekMessage(&Message,(HANDLE)-1,0,0,PM_REMOVE) ) break;

        // ��������, ���� ������ ����������
        if ( (::GetTickCount()-StartCycleTime)<(SYNC_SendInterval/4) )
            ::Sleep(SYNC_SendInterval/4);
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
        case mptHello:
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
            SndPacket.Hello.Header.Type=mptHello;
            SndPacket.Hello.Header.Seed=BasicRand();
            // � ����� ���������� ��������� ID, �.�. random ������� ��� �������
            Seed=BasicMix(SndPacket.Hello.Header.Seed,RecvHeader->Seed);
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
                (RecvHeader->Seed!=Seed) ) break;
            // ��������� ����� ����� ������ �������, �� �������� ���
            // ������ ����� �������� ������������� �������
            SyncData=((MSyncPData*)Packet_)->Data;
            // ��������� ����� c ������� �������
            SndPacket.Conf.Header.Version=SYNC_Version;
            SndPacket.Conf.Header.Type=mptSyncConf;
            SndPacket.Conf.Header.Seed=Seed;
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
                    NeedSave=State->NewSyncData(&SyncData);
                // ��������� � ����� �������� ����� ������
                Process=mspWait;
            } else
            {
                // ���������� �����
                ::sendto(Socket_,(char*)&SndPacket,SndPacketSize,
                    0,(sockaddr*)&SndAddr,sizeof(SndAddr));
                // ��������� ����� �������� ������ � ����������� �������
                LastSendTime=::GetTickCount(); SendCount++;
            }
            break;
        default: break;
    }

    return NeedSave;
}
//---------------------------------------------------------------------------

