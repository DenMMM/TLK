//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <string.h>
//#include "UnitThreadNetSync.h"
#include "UnitComputers.h"
#include "UnitCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall MThreadNetSync::MThreadNetSync(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    ::InitializeCriticalSection(&CS_Main);
    ::InitializeCriticalSection(&CS_Computers);
    ExitThread=false;
    PausedSync=true;
    FirstComputer=NULL;
    RemoteSocket=INVALID_SOCKET;
    Waiting=false;
    Position=0;
    OnTerminate=&Terminate;
}
//---------------------------------------------------------------------------
void __fastcall MThreadNetSync::Terminate(TObject *Sender)
{
    ::DeleteCriticalSection(&CS_Main);
    ::DeleteCriticalSection(&CS_Computers);
}
//---------------------------------------------------------------------------
void __fastcall MThreadNetSync::Execute()
{
    while(true)
    {
        ::Sleep(300);
        Lock();
        // ��������� �� ��������� �� ��������� ����� � �� �������������� �� ��������
        if ( ExitThread ) { UnLock(); break; }
        // ��������� �� ������������� �� ������� �������������
        if ( PausedSync ) { UnLock(); continue; }
        UnLock();
        // ���� ���������� ��� �������������
        LockComputers();
        for ( MComputer *Computer=FirstComputer; Computer;
            Computer=(MComputer*)Computer->NextItem )
        {
            // ��������� �� ������������� �� ������� �������������
            Lock(); if ( PausedSync ) { UnLock(); break; }
            else Position++; UnLock();
            // ��������� ����� �� ������������� � ��������� �����������
            Computer->Lock();
            if ( Computer->NeedConnect&&(!Computer->SyncProcess) )
            {
                Computer->NeedConnect=false;
                Computer->SyncProcess=true;
                Computer->UnLock(); Process(Computer);
                Computer->Lock(); Computer->SyncProcess=false;
                Computer->DataChanges|=mdcNetState;
                Computer->UnLock();
            } else Computer->UnLock();
        }
        UnLockComputers();
        Lock(); Position=0; UnLock();
    }
}
//---------------------------------------------------------------------------
void MThreadNetSync::Init(MComputers *Computers_)
{
    bool OldPausedSync;

    // ������������� ������� ��������
    Lock(); OldPausedSync=PausedSync; PausedSync=true; CloseSocket(); UnLock();
    //
    LockComputers(); FirstComputer=(MComputer*)Computers_->FirstItem; UnLockComputers();
    // ��������������� ������� ��������
    Lock(); PausedSync=OldPausedSync; UnLock();
}
//---------------------------------------------------------------------------
void MThreadNetSync::Pause(bool Set_)
{
    // ������������� ������� ��������
    Lock(); PausedSync=Set_; CloseSocket(); UnLock();
    // ���� ������� ���������� ������� ��������
    LockComputers(); UnLockComputers();
}
//---------------------------------------------------------------------------
void MThreadNetSync::Exit()
{
    // ������������� ������� ��������
    Lock(); PausedSync=true; CloseSocket(); UnLock();
    // ���� ������� ���������� ������� ��������
    LockComputers(); UnLockComputers();
    // ��������, ��� ���� ��������� ������
    Lock(); ExitThread=true; UnLock();
}
//---------------------------------------------------------------------------
void MThreadNetSync::CloseSocket()
{
    ::shutdown(RemoteSocket,SD_BOTH);
    ::closesocket(RemoteSocket);
    RemoteSocket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
void MThreadNetSync::StartWaiting(int Time_)
{
    Lock(); Waiting=true; WaitTime=Time_; UnLock();
}
//---------------------------------------------------------------------------
void MThreadNetSync::StopWaiting()
{
    Lock(); Waiting=false; UnLock();
}
//---------------------------------------------------------------------------
void MThreadNetSync::Timer()
{
    Lock();
    if ( Waiting&&((--WaitTime)<=0) )
    {
        Waiting=false;
        CloseSocket();
    }
    UnLock();
}
//---------------------------------------------------------------------------
int MThreadNetSync::GetPosition()
{
    int position;
    Lock(); position=Position; UnLock();
    return position;
}
//---------------------------------------------------------------------------
void MThreadNetSync::Process(MComputer *Computer_)
{
    unsigned int SyncData;
    char *data=NULL;
    int data_size, result;
    sockaddr_in RemoteAddr;

    // ������� �����
    Lock(); RemoteSocket=::socket(PF_INET,SOCK_STREAM,IPPROTO_IP);
    if ( RemoteSocket==INVALID_SOCKET ) { UnLock(); goto error; } UnLock();
    // blocking �����
    {
        unsigned long a=0;
        result=::ioctlsocket(RemoteSocket,FIONBIO,&a);
        if ( result==SOCKET_ERROR ) goto error;
    }

    // ������ ��������� ��������
    RemoteAddr.sin_family=PF_INET;
    RemoteAddr.sin_port=::htons(6714);
    Computer_->Lock();
    RemoteAddr.sin_addr.s_addr=::inet_addr(Computer_->Address.c_str());
    Computer_->UnLock();

    // ������������� ����������
    StartWaiting(7);
    result=::connect(RemoteSocket,(sockaddr*)&RemoteAddr,sizeof(sockaddr_in));
    StopWaiting();
    // ������� �������� �� ��������� �� ����
    Computer_->Lock(); Computer_->NetPresent=!(result==SOCKET_ERROR);
    Computer_->DataChanges|=mdcNetState; Computer_->UnLock();
    // ��������� ����������� ���������� ��� ������� ��� �������
    if ( result==SOCKET_ERROR ) goto error;

    // ����� ������ � ���, ��� ���� ����������������
    Computer_->Lock(); SyncData=Computer_->NetSync|Computer_->NotSync;
    Computer_->NotSync=SyncData; Computer_->NetSync=0; Computer_->UnLock();

    if ( SyncData&mnsState )
    {
        // �������� ������ ��� ������
        data_size=GetComputerDataSize();
        data=new char[data_size];
        if ( data==NULL ) goto error;
        // ������� ������ ��� ��������
        Computer_->Lock();
        SetComputerData(data,Computer_);
        Computer_->UnLock();

        // ���������� ������ ������
        StartWaiting(10);
        result=::send(RemoteSocket,(char*)&data_size,sizeof(int),0);
        StopWaiting();
        if ( (result==SOCKET_ERROR)||(result!=sizeof(int)) ) goto error;
        // ���������� ������
        StartWaiting(30);
        result=::send(RemoteSocket,data,data_size,0);
        StopWaiting();
        if ( (result==SOCKET_ERROR)||(result!=data_size) ) goto error;

        delete[] data; data=NULL;
        // ��������, ��� ������ � ��������� ����������
        SyncData&=~mnsState;
    } else
    {
        // ���������� ������� ������ ������
        data_size=0;
        StartWaiting(10);
        result=::send(RemoteSocket,(char*)&data_size,sizeof(int),0);
        StopWaiting();
        if ( (result==SOCKET_ERROR)||(result!=sizeof(int)) ) goto error;
    }

    if ( SyncData&mnsGames )
    {
        char file_name[MAX_PATH];
        HANDLE file=NULL;
        DWORD rw_size;

        Computer_->LockGames();
        strcpy(file_name,PrgDir.c_str());
        sprintf(file_name+strlen(file_name),"%s%08X%s","COMPUTERS\\",Computer_->ID,".GMS");
        // ��������� ���� �� ������� ���
        if ( (file=::CreateFile(file_name,GENERIC_READ,0,NULL,OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE )
            { Computer_->UnLockGames(); goto err; }
        // ���������� ��� ������
        if ( (data_size=::GetFileSize(file,NULL))==0xFFFFFFFF )
            { Computer_->UnLockGames(); goto err; }
        // �������� ������ ��� ������
        if ( (data=new char[data_size])==NULL )
            { Computer_->UnLockGames(); goto err; }
        // ��������� ������ �� �����
        if ( ::ReadFile(file,data,data_size,&rw_size,NULL)==NULL )
            { Computer_->UnLockGames(); goto err; }
        ::CloseHandle(file); file=NULL;
        Computer_->UnLockGames();

        //
        BasicEncode(data,data_size,0x5DCF39EA);
        // ���������� ������ ������
        StartWaiting(10);
        result=::send(RemoteSocket,(char*)&data_size,sizeof(int),0);
        StopWaiting();
        if ( (result==SOCKET_ERROR)||(result!=sizeof(int)) ) goto error;
        // ���������� ������
        StartWaiting(30);
        result=::send(RemoteSocket,data,data_size,0);
        StopWaiting();
        if ( (result==SOCKET_ERROR)||(result!=data_size) ) goto error;

        delete[] data; data=NULL;
        // ��������, ��� ������ � ��������� ����������
        SyncData&=~mnsGames;
        goto norm;
err:
        delete[] data; data=NULL;
        if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
norm:
    } else
    {
        // ���������� ������� ������ ������
        data_size=0;
        StartWaiting(10);
        result=::send(RemoteSocket,(char*)&data_size,sizeof(int),0);
        StopWaiting();
        if ( (result==SOCKET_ERROR)||(result!=sizeof(int)) ) goto error;
    }

    // ��������� �����
    Lock(); CloseSocket(); UnLock();
    // ��������� ���������� ��������� ����������
    Computer_->Lock(); Computer_->NetSync|=SyncData; Computer_->State&=~(mcsReboot|mcsShutdown);
    Computer_->NotSync=0; Computer_->Save();
    Computer_->UnLock();
    return;
error:
    delete[] data;
    // ��������� �����
    Lock(); CloseSocket(); UnLock();
}
//---------------------------------------------------------------------------
int MThreadNetSync::GetComputerDataSize()
{
    return
        sizeof(__int64)+  // ��������� �����
        sizeof(int)+  // ����� ���������� � �����
        sizeof(unsigned int)+  // ��������� ����������
        sizeof(double)+  // ����� ������� ���������� � ������ (���������� �����)
        sizeof(int)+  // �� ����� ����� ������� ��������� (� �������)
        sizeof(double)+  // �����, ����� ��� �������� ����� (���������� �����)
        sizeof(int)+  // ����� �������� � ������ ������ (� �������)
        sizeof(double)+  // ����� ��������� ������� ������� (���������� �����)
        sizeof(unsigned int)+  // ����������� ��� ������� ������ ��������
        sizeof(unsigned int);  // ������ ���������
}
//---------------------------------------------------------------------------
char *MThreadNetSync::SetComputerData(char *Data_, MComputer *Computer_)
{
    SYSTEMTIME system_time;
    __int64 SystemTime;

    ::GetLocalTime(&system_time);
    ::SystemTimeToFileTime(&system_time,(FILETIME*)&SystemTime);
    *((__int64*)Data_)=SystemTime; Data_+=sizeof(__int64);
    *((int*)Data_)=Computer_->Number; Data_+=sizeof(int);
    *((unsigned int*)Data_)=Computer_->State; Data_+=sizeof(unsigned int);
    *((double*)Data_)=Computer_->StartWorkTime; Data_+=sizeof(double);
    *((int*)Data_)=Computer_->SizeWorkTime; Data_+=sizeof(int);
    *((double*)Data_)=Computer_->StartFineTime; Data_+=sizeof(double);
    *((int*)Data_)=Computer_->SizeFineTime; Data_+=sizeof(int);
    *((double*)Data_)=Computer_->StopTimerTime; Data_+=sizeof(double);
    *((unsigned int*)Data_)=Computer_->GamesPages; Data_+=sizeof(unsigned int);
    *((unsigned int*)Data_)=-1; Data_+=sizeof(unsigned int);

    return Data_;
}
//---------------------------------------------------------------------------

