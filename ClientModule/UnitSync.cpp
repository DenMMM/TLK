//---------------------------------------------------------------------------
#include <stdlib.h>
#include <windows.h>
#pragma hdrstop

#include "UnitSync.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MSync::MSync()
{
    Init=false;
    Thread=NULL;
    ThreadID=0;
    Socket=INVALID_SOCKET;
    State=NULL;
    Process=mspNone;
    SendCount=0;
    LastSendTime=0;
    SNumber=0;
}
//---------------------------------------------------------------------------
MSync::~MSync()
{
//
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
void MSync::Associate(MState *State_)
{
    State=State_;
}
//---------------------------------------------------------------------------
bool MSync::Start()
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
    Address.sin_port=::htons(SYNC_SyncPort);
    Address.sin_addr.s_addr=INADDR_ANY;
    // ������������ �����
    if ( ::bind(Socket,(sockaddr*)&Address,
        sizeof(Address))==SOCKET_ERROR ) goto error;

    //
    Process=mspWait; LastSendTime=::GetTickCount();

    // ������� ����� ��� ���������� �������� � ������ ������
    Thread=::CreateThread(NULL,0,&ThreadFunc,(LPVOID)this,0,&ThreadID);
    if ( Thread==NULL ) goto error;

    return true;
error:
    // ��������� �����
    if ( Socket!=INVALID_SOCKET ) ::closesocket(Socket);
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
    // ��������� ������
    ::closesocket(Socket);
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
//    sockaddr_in Address;
    int RecvSize, AddrSize;
    bool NeedSave;
    DWORD StartCycleTime;

    while(true)
    {
        StartCycleTime=::GetTickCount();

        // ��������� ������� ���������
        if ( ::PeekMessage(&Message,(HANDLE)-1,0,0,PM_REMOVE) ) break;
        NeedSave=false;
        // ��������� ��� �� ������ ��� ������
        if ( PollData(Socket) )
        {
            // ������ ������ �� ������
            memset(&Address,0,sizeof(Address)); AddrSize=sizeof(Address);
            RecvSize=::recvfrom(Socket,(char*)&RecvPacket,sizeof(RecvPacket),
                0,(sockaddr*)&Address,&AddrSize);
            if ( !((RecvSize==0)||(RecvSize==SOCKET_ERROR)) )
            {
                // �������� ����� ��� ���������
                NeedSave|=Recv(&Address,(char*)&RecvPacket,RecvSize);
            }
        }
        // ���������� ������
        NeedSave|=Send(Socket);
        // ��������� ����� �� ��������� ����������� ���������
        if ( NeedSave ) State->Save();

        if ( (::GetTickCount()-StartCycleTime)<(SYNC_SendInterval/4) )
            ::Sleep(SYNC_SendInterval/4);
    }
}
//---------------------------------------------------------------------------
bool MSync::Recv(sockaddr_in *From_, char *Packet_, int PacketSize_)
{
    MPacketBasic *RecvPacket=(MPacketBasic*)Packet_;
    bool NeedSave=false;

    //
    if ( Process==mspNone ) goto error;
    // ��������� ����� �������, ������������ �����
    if ( (Process!=mspWait)&&
        (Address.sin_addr.s_addr!=From_->sin_addr.s_addr) ) goto error;
    // ��������� ����������� ������ ������
    if ( PacketSize_<sizeof(MPacketBasic) ) goto error;
    // �������������� �����
    BasicEncode(Packet_,PacketSize_,0);
    // ��������� ������ ������ ���������� � ������
    if ( (RecvPacket->Header.Version!=SYNC_Version)||
        ((Process!=mspWait)&&(RecvPacket->Header.SNumber!=SNumber)) ) goto error;

    // ������������ �����
    switch(RecvPacket->Header.Type)
    {
        case mptSyncData:
            // ��������� ������� ��������� �������� ������
            if ( Process!=mspWait ) break;
            // ��������� ������ ������
            if ( PacketSize_!=sizeof(MPacketData) ) break;
            //
            SyncData=((MPacketData*)RecvPacket)->Data;
            // ���������� ����� ������ ��� ���������� �������� �������
            SNumber=RecvPacket->Header.SNumber;
            //
            memset(&Address,0,sizeof(Address));
            Address.sin_family=AF_INET;
            Address.sin_port=::htons(SYNC_SyncPort+1);
            Address.sin_addr.s_addr=From_->sin_addr.s_addr;
            // ��������� ��������� ������
            SendPacket.Header.Version=SYNC_Version;
            SendPacket.Header.SNumber=SNumber;
            SendPacket.Header.Type=mptDataConfirm;
            // ��������� �������� ������������� � ���������
            LastSendTime=::GetTickCount()-SYNC_SendInterval;
            SendCount=0; Process=mspSend1;
            break;
        default: break;
    }

error:
    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSync::Send(SOCKET Socket_)
{
    bool NeedSave=false;

    switch(Process)
    {
        case mspWait: break;
        case mspSend1:
            // ��������� �� ���� �� ��������� ��������� �����
            if ( (::GetTickCount()-LastSendTime)<SYNC_SendInterval ) break;
            // ��������� �� ���������� �� �������� ���������� �������
            if ( SendCount>=SYNC_SendRetryes )
            {
                // ��������� ������� �������� � �����������
                NeedSave=State->NewSyncData(&SyncData);
                // ��������� � ����� �������� ����� ������
                Process=mspWait;
            } else
            {
                // ���������� �����
                ::sendto(Socket_,(char*)&SendPacket,sizeof(SendPacket),
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

