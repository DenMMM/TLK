//---------------------------------------------------------------------------
#include <stdlib.h>
#include <windows.h>
#include <iphlpapi.h>
#pragma hdrstop

#include "UnitSync.h"
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
    SendCount=0;
    LastSendTime=0;
    SNumber=0;
    PacketSize=0;
}
//---------------------------------------------------------------------------
MSyncState::~MSyncState()
{
//
}
//---------------------------------------------------------------------------
unsigned MSyncState::GetDataSize()
{
    return KnownMAC? sizeof(IP)+sizeof(MAC): 0;
}
//---------------------------------------------------------------------------
char *MSyncState::SetData(char *Data_)
{
    if ( !KnownMAC ) return Data_;
    Data_=MemSet(Data_,IP);
    Data_=((char*)memcpy(Data_,MAC,sizeof(MAC)))+sizeof(MAC);
    return Data_;
}
//---------------------------------------------------------------------------
char *MSyncState::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&IP,Limit_))==NULL ) goto error;
    if ( (Data_+sizeof(MAC))>Limit_ ) goto error;
    memcpy(MAC,Data_,sizeof(MAC)); Data_+=sizeof(MAC); KnownMAC=true;
    return Data_;
error:
    return NULL;
}
//---------------------------------------------------------------------------
unsigned int MSyncState::NextSNumber()
{
    static unsigned SNumber=0;
    do { SNumber+=random(MAX_Computers)+1; } while(SNumber==0);
    return SNumber;
}
//---------------------------------------------------------------------------
void MSyncState::Associate(MState *State_, u_long IP_)
{
    State=State_;
    KnownMAC=(IP==IP_); IP=IP_;
    Process=mspNone;
}
//---------------------------------------------------------------------------
u_long MSyncState::GetIP()
{
    return IP;
}
//---------------------------------------------------------------------------
bool MSyncState::UpdateMAC(unsigned char *MAC_)
{
    bool NeedSave;
    // ��������� ����� �� �������� MAC-�����
    NeedSave=(!KnownMAC)||memcmp(MAC,MAC_,MAC_AddrLength);
    // ��� ������������� ��������� MAC-�����
    if ( NeedSave ) { memcpy(MAC,MAC_,MAC_AddrLength); KnownMAC=true; }
    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncState::Start()
{
    if ( (State==NULL)||(IP==INADDR_NONE) ) return false;
    Process=mspWait; LastSendTime=::GetTickCount()-SYNC_WaitInterval;
    SNumber=NextSNumber();
    return true;
}
//---------------------------------------------------------------------------
bool MSyncState::Stop()
{
    bool NeedSave=false;
    switch(Process)
    {
        case mspSend1: case mspMagic:
            NeedSave=State->NetEnd(); break;
        default: break;
    }
    Process=mspNone;
    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncState::Send(SOCKET Socket_, SOCKET SocketBC_)
{
    bool NeedSave=false;

    switch(Process)
    {
        case mspWait:
            if ( !State->NetBegin() ) break;
            if ( KnownMAC&&State->NetPwrOnNeed() )
            {
                memset(&Address,0,sizeof(Address));
                Address.sin_family=AF_INET;
                Address.sin_port=::htons(0);
                Address.sin_addr.s_addr=INADDR_BROADCAST;
                //
                Packet.PacketPwrOn.ZeroFirst=0;
                // ��������� ����������������� ������������������
                memset(Packet.PacketPwrOn.Sync,0xFF,MAC_AddrLength);
                // ��������� ������� �� �������������� 16 ��� MAC-������
                for ( int i=0; i<16; i++ )
                    memcpy(Packet.PacketPwrOn.Magic[i],MAC,MAC_AddrLength);
                // ������ ������ ������
                PacketSize=sizeof(Packet.PacketPwrOn);
                // �������� ����� �������� "Magic Packet"
                LastSendTime=::GetTickCount()-SYNC_SendInterval;
                SendCount=0; Process=mspMagic;
//            } else if ( (Sync&&State->NetSyncNewData())||
            } else if ( State->NetSyncNewData()||
                ((::GetTickCount()-LastSendTime)>=SYNC_WaitInterval) )
            {
                // ������ ����� ����� ������
                SNumber=NextSNumber();
                //
                memset(&Address,0,sizeof(Address));
                Address.sin_family=AF_INET;
                Address.sin_port=::htons(SYNC_SyncPort);
                Address.sin_addr.s_addr=IP;
                // ��������� ��������� ������
                Packet.PacketData.Header.Version=SYNC_Version;
                Packet.PacketData.Header.SNumber=SNumber;
                Packet.PacketData.Header.Type=mptSyncData;
                // ����������� ����� ������ ��� �������������
                State->NetSyncData(&Packet.PacketData.Data);
                // ������� �����
                BasicCode((char*)&Packet.PacketData,sizeof(Packet.PacketData),0);
                // ������ ������ ������
                PacketSize=sizeof(Packet.PacketData);
                // �������� ����� �������� ������ � ���������
                LastSendTime=::GetTickCount()-SYNC_SendInterval;
                SendCount=0; Process=mspSend1;
            } else NeedSave=State->NetEnd();
            break;
        case mspSend1:
        case mspMagic:
            // ��������� �� ���� �� ��������� ��������� �����
            if ( (::GetTickCount()-LastSendTime)<SYNC_SendInterval ) break;
            // ��������� �� ���������� �� �������� ���������� �������
            if ( SendCount>=SYNC_SendRetryes )
            {
                // ��������� ������� �������� � �����������
                if ( Process==mspMagic ) State->NetPwrOnExecuted();
                else State->NetSyncExecuted(false);
                NeedSave=State->NetEnd();
                // ��������� � ����� �������� ����� ������
                Process=mspWait;
            } else
            {
                // ���������� �����
                ::sendto(Process==mspMagic?SocketBC_:Socket_,(char*)&Packet,PacketSize,
                    0,(sockaddr*)&Address,sizeof(Address));
                // ��������� ����� �������� ������ � ����������� ������� �������
                LastSendTime=::GetTickCount(); SendCount++;
            }
            break;
        default: break;
    }

    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncState::Recv(char *Packet_, int PacketSize_)
{
    MPacketBasic *RecvPacket=(MPacketBasic*)Packet_;
    bool NeedSave=false;

    //
    if ( Process==mspNone ) goto error;
    // ��������� ������ ������
    if ( PacketSize_!=sizeof(MPacketBasic) ) goto error;
    // �������������� �����
    BasicEncode(Packet_,PacketSize_,0);
    // ��������� ������ ������ ���������� � ������
    if ( (RecvPacket->Header.Version!=SYNC_Version)||
        (RecvPacket->Header.SNumber!=SNumber) ) goto error;

    // ������������ �����
    switch(RecvPacket->Header.Type)
    {
        case mptDataConfirm:
            // ��������� ������� ��������� �������� ������
            if ( Process!=mspSend1 ) break;
            // ��������� ������� �������� � �����������
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
unsigned MSyncState::GetCounter()
{
    return Process==mspWait? 0: SendCount+1;
}
//---------------------------------------------------------------------------
MSyncState *MSyncStates::Search(u_long IP_)
{
    MSyncState *SyncState=(MSyncState*)First;
    while(SyncState)
    {
        if ( SyncState->GetIP()==IP_ ) break;
        SyncState=(MSyncState*)SyncState->Next;
    }
    return SyncState;
}
//---------------------------------------------------------------------------
bool MSyncStates::Associate(MStates *States_, MComputers *Computers_)
{
    MComputer *Computer;
    u_long IP;
    MSyncState *SyncState, *LastSyncState=(MSyncState*)First;

    for ( MState *State=(MState*)States_->First;
        State; State=(MState*)State->Next )
    {
        // ���� ���������, � ������� ������������� ���������
        Computer=Computers_->Search(State->Associated()); if ( Computer==NULL ) continue;
        // ��������� ��� IP-�����
        IP=::inet_addr(Computer->Address); if ( IP==INADDR_NONE ) continue;
        // ���� �� ����� ������ ��������� �������������
        // (����� ��������� �������� ��������� MAC-�����) ��� ��������� �����
        SyncState=Search(IP); if ( SyncState==NULL ) SyncState=(MSyncState*)Add();
        // ����������� ��������� ������������� � ���������� ���������� � ��� �������
        SyncState->Associate(State,IP);
        // ���� ��������� �� ��������� � ������, ��������� ��� � ������ ������
        if ( LastSyncState!=NULL ) Exchange(LastSyncState,SyncState);
        LastSyncState=(MSyncState*)SyncState->Next;
    }
    // ������� �� ������ �� �������������� ������
    while(LastSyncState)
    {
        SyncState=(MSyncState*)LastSyncState->Next;
        Delete(LastSyncState);
        LastSyncState=SyncState;
    }

    return true;
}
//---------------------------------------------------------------------------
bool MSyncStates::Start()
{
    for ( MSyncState *SyncState=(MSyncState*)First;
        SyncState; SyncState=(MSyncState*)SyncState->Next )
        if ( !SyncState->Start() ) return false;
    return true;
}
//---------------------------------------------------------------------------
bool MSyncStates::Stop()
{
    bool NeedSave=false;
    for ( MSyncState *SyncState=(MSyncState*)First; SyncState;
        SyncState=(MSyncState*)SyncState->Next ) NeedSave|=SyncState->Stop();
    return NeedSave;
}
//---------------------------------------------------------------------------
MSync::MSync()
{
    Init=false;
    AutoSaveARP=false;
    Thread=NULL;
    ThreadID=0;
    States=NULL;
    Socket=INVALID_SOCKET;
    SocketBC=INVALID_SOCKET;
    ::InitializeCriticalSection(&CS_PCounter);
    Counter=0;
}
//---------------------------------------------------------------------------
MSync::~MSync()
{
    SyncStates.Clear();
    ::DeleteCriticalSection(&CS_PCounter);
}
//---------------------------------------------------------------------------
bool MSync::SetARPFile(char *File_, unsigned Code_)
{
    return SyncStates.SetDefaultFile(File_,Code_);
}
//---------------------------------------------------------------------------
void MSync::SetAutoSaveARP(bool AutoSave_)
{
    AutoSaveARP=AutoSave_;
}
//---------------------------------------------------------------------------
bool MSync::SaveARP()
{
    return SyncStates.Save();
}
//---------------------------------------------------------------------------
bool MSync::LoadARP()
{
    return SyncStates.Load();
}
//---------------------------------------------------------------------------
bool MSync::NetInit()
{
    WSADATA WSAData;
    Init=!::WSAStartup(MAKEWORD(1,1),&WSAData);
    return Init;
}
//---------------------------------------------------------------------------
bool MSync::NetFree()
{
    Init=::WSACleanup();
    return Init;
}
//---------------------------------------------------------------------------
bool MSync::Associate(MStates *States_, MComputers *Computers_)
{
    States=NULL;
    if ( (!SyncStates.Associate(States_,Computers_))||
        (!SyncStates.Save()) ) return false;
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
    Address.sin_port=::htons(0);
    Address.sin_addr.s_addr=INADDR_ANY;
    // ������������ �����
    if ( ::bind(Socket,(sockaddr*)&Address,
        sizeof(Address))==SOCKET_ERROR ) goto error;

    // ������� ����� ��� �������� broadcast-���������
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
    if ( Socket!=INVALID_SOCKET ) ::closesocket(Socket);
    if ( SocketBC!=INVALID_SOCKET ) ::closesocket(SocketBC);
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
    ::CloseHandle(Thread); Thread=NULL; ThreadID=0;
    //
    if ( SyncStates.Stop() ) States->Save();
    // ��������� ������
    ::closesocket(Socket);
    ::closesocket(SocketBC);
    //
    PCounterSet(0);
}
//---------------------------------------------------------------------------
DWORD WINAPI MSync::ThreadFunc(LPVOID Data)
{
    // ������� ������� ��������� ��� ������
    ::PeekMessage(NULL,(HANDLE)-1,0,0,PM_NOREMOVE);

    ((MSync*)Data)->ThreadExecute();

    // ��������� ������ ������
    ::ExitThread(0);
    return 0;
}
//---------------------------------------------------------------------------
void MSync::ThreadExecute()
{
    MSG Message;
    sockaddr_in Address;
    int RecvSize, AddrSize;
    bool NeedSave;
    DWORD StartCycleTime;
    unsigned Process;

    while(true)
    {
        StartCycleTime=::GetTickCount();

        // ��������� ������� ���������
        if ( ::PeekMessage(&Message,(HANDLE)-1,0,0,PM_REMOVE) ) break;
        // ��������� ���� ��������/������ ������
        NeedSave=false; Process=0;
        for ( MSyncState *SyncState=(MSyncState*)SyncStates.First, *RecvState;
            SyncState; SyncState=(MSyncState*)SyncState->Next )
        {
            // ���������� ������
            NeedSave|=SyncState->Send(Socket,SocketBC);
            //
            Process+=SyncState->GetCounter();
            PCounterSet(Process);
            // ��������� ��� �� ������ ��� ������
            if ( !PollData(Socket) ) continue;
            // ������ ������ �� ������
            memset(&Address,0,sizeof(Address)); AddrSize=sizeof(Address);
            RecvSize=::recvfrom(Socket,(char*)&Packet,sizeof(Packet),
                0,(sockaddr*)&Address,&AddrSize);
            if ( (RecvSize==0)||(RecvSize==SOCKET_ERROR) ) continue;
            // ���� ��������� ������������� �� ������, � �������� ������ �����
            RecvState=SyncStates.Search(Address.sin_addr.s_addr);
            // �������� ��� ����� ��� ���������
            if ( RecvState!=NULL ) NeedSave|=RecvState->Recv((char*)&Packet,RecvSize);
        }
        // ��������� ����� �� ��������� ����������� ��������� �����������
        if ( NeedSave ) States->Save();
        // ��������� MAC-������ � ��� ������������� ��������� �� �� ����
        if ( UpdateMAC(&SyncStates)&&AutoSaveARP ) SyncStates.Save();

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
    Table=(PMIB_IPNETTABLE)new char[TableSize];
    if ( Table==NULL ) goto error;
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
void MSync::PCounterSet(unsigned Value_)
{
    ::EnterCriticalSection(&CS_PCounter); Counter=Value_;
    ::LeaveCriticalSection(&CS_PCounter);
}
//---------------------------------------------------------------------------
unsigned MSync::GetCounterMax()
{
    return SyncStates.Count*SYNC_SendRetryes;
}
//---------------------------------------------------------------------------
unsigned MSync::GetCounter()
{
    unsigned result;
    ::EnterCriticalSection(&CS_PCounter); result=Counter;
    ::LeaveCriticalSection(&CS_PCounter);
    return result;
}
//---------------------------------------------------------------------------

