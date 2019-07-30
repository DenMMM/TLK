//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitThreadNetSync.h"
#include "UnitState.h"
#include "UnitFormMain.h"
#include "UnitOptionsLoadSave.h"
#include "UnitCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall MThreadNetSync::MThreadNetSync(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    ::InitializeCriticalSection(&CS_Main);
    LocalSocket=RemoteSocket=INVALID_SOCKET;
    PausedSync=true;
    OnTerminate=&Terminate;
}
//---------------------------------------------------------------------------
void __fastcall MThreadNetSync::Terminate(TObject *Sender)
{
    ::DeleteCriticalSection(&CS_Main);
}
//---------------------------------------------------------------------------
void __fastcall MThreadNetSync::Execute()
{
    int result;
    
    // ������� �����
    Lock(); LocalSocket=::socket(PF_INET,SOCK_STREAM,IPPROTO_IP);
    if ( LocalSocket==INVALID_SOCKET ) { UnLock(); goto error; } UnLock();
    // blocking �����
    {
        unsigned long a=0;
        result=::ioctlsocket(LocalSocket,FIONBIO,&a);
        if ( result==SOCKET_ERROR ) goto error;
    }
    // ������ ��������� ������ ������
    LocalAddr.sin_family=PF_INET;
    LocalAddr.sin_port=::htons(6714);
    LocalAddr.sin_addr.s_addr=INADDR_ANY;
    //
    result=::bind(LocalSocket,(sockaddr*)&LocalAddr,sizeof(sockaddr_in));
    if ( result==SOCKET_ERROR ) goto error;
    //
    result=::listen(LocalSocket,0);
    if ( result==SOCKET_ERROR ) goto error;
    //
    while(true)
    {
        // ��������� ����������
//        Lock(); RemoteSocket=::accept(LocalSocket,(sockaddr*)&RemoteAddr,sizeof(sockaddr_in));
        RemoteSocket=::accept(LocalSocket,NULL,NULL);
        Lock();
        if ( LocalSocket==INVALID_SOCKET ) { UnLock(); break; }
        if ( RemoteSocket==INVALID_SOCKET ) { UnLock(); continue; }
        if ( PausedSync ) { CloseSocket(); UnLock(); continue; }
        UnLock();
        Process();
    }
error:
    ::shutdown(LocalSocket,SD_BOTH);
    ::closesocket(LocalSocket);
}
//---------------------------------------------------------------------------
void MThreadNetSync::Pause(bool Set_)
{
    Lock(); PausedSync=Set_; if ( Set_ ) CloseSocket(); UnLock();
}
//---------------------------------------------------------------------------
void MThreadNetSync::Exit()
{
    Lock(); PausedSync=true; CloseSocket();
    ::shutdown(LocalSocket,SD_BOTH); ::closesocket(LocalSocket);
    LocalSocket=INVALID_SOCKET; UnLock();
}
//---------------------------------------------------------------------------
void MThreadNetSync::Lock() { ::EnterCriticalSection(&CS_Main); }
//---------------------------------------------------------------------------
void MThreadNetSync::UnLock() { ::LeaveCriticalSection(&CS_Main); }
//---------------------------------------------------------------------------
void MThreadNetSync::CloseSocket()
{
    ::shutdown(RemoteSocket,SD_BOTH);
    ::closesocket(RemoteSocket);
    RemoteSocket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
bool MThreadNetSync::Recv(char *Buffer_, int Size_)
{
    int result;

    while(Size_)
    {
        result=::recv(RemoteSocket,Buffer_,Size_,0);
        if ( (result==0)||(result==SOCKET_ERROR) ) return false;
        Size_-=result; Buffer_+=result;
    }

    return true;
}
//---------------------------------------------------------------------------
void MThreadNetSync::Process()
{
    char *data=NULL, *current_data;
    int data_size, result;
    HANDLE file=NULL;

    // blocking �����
    {
        unsigned long a=0;
        result=::ioctlsocket(RemoteSocket,FIONBIO,&a);
        if ( result==SOCKET_ERROR ) goto error;
    }

    // ���������� ������ �������� �������� � ��������� �������
    State->Lock(); State->NotPingTime=0; State->UnLock();

    // ��������� ������ ������
    if ( !Recv((char*)&data_size,sizeof(int)) ) goto error;
    if ( data_size!=0 )
    {
        __int64 NewSystemTime;
        int NewNumber, NewSizeWorkTime, NewSizeFineTime;
        unsigned int NewState, NewGamesPages, NewStateVer;
        double NewStartWorkTime, NewStartFineTime, NewStopTimerTime;
        SYSTEMTIME system_time;

        if ( data_size!=(sizeof(__int64)+sizeof(int)*3+sizeof(unsigned int)*3+sizeof(double)*3) ) goto error;
        // �������� ������ ��� ������
        data=new char[data_size];
        if ( data==NULL ) goto error;
        // ��������� ������
        if ( !Recv(data,data_size) ) goto error;

        current_data=data;
        // ��������� ������
        NewSystemTime=*((__int64*)current_data); current_data+=sizeof(__int64);
        NewNumber=*((int*)current_data); current_data+=sizeof(int);
        NewState=*((unsigned int*)current_data); current_data+=sizeof(unsigned int);
        NewStartWorkTime=*((double*)current_data); current_data+=sizeof(double);
        NewSizeWorkTime=*((int*)current_data); current_data+=sizeof(int);
        NewStartFineTime=*((double*)current_data); current_data+=sizeof(double);
        NewSizeFineTime=*((int*)current_data); current_data+=sizeof(int);
        NewStopTimerTime=*((double*)current_data); current_data+=sizeof(double);
        NewGamesPages=*((unsigned int*)current_data); current_data+=sizeof(unsigned int);
        NewStateVer=*((unsigned int*)current_data); current_data+=sizeof(unsigned int);

        delete[] data; data=NULL;

        // ���� ������ ��������� �� ������� ������ ��� ������ ���������
        // �� ���������, �� ��������� ����� ���������
        if ( (NewStateVer==-1)||(NewStateVer!=State->StateVer) )
        {
            ::FileTimeToSystemTime((FILETIME*)&NewSystemTime,&system_time);
            ::SetLocalTime(&system_time);
            State->Lock();
            State->Number=NewNumber;
            State->State=NewState;
            State->StartWorkTime=NewStartWorkTime;
            State->SizeWorkTime=NewSizeWorkTime;
            State->StartFineTime=NewStartFineTime;
            State->SizeFineTime=NewSizeFineTime;
            State->StopTimerTime=NewStopTimerTime;
            State->GamesPages=NewGamesPages;
            if ( NewStateVer!=-1 ) State->StateVer=NewStateVer;
            StateSave(State); State->NeedUpdate=true;
            State->UnLock();
        }
    }

    // ��������� ������ ������
    if ( !Recv((char*)&data_size,sizeof(int)) ) goto error;
    if ( data_size!=0 )
    {
        MState NewState;
        char file_name[MAX_PATH];
        DWORD rw_size;

        // �������� ������ ��� ������
        data=new char[data_size];
        if ( data==NULL ) goto error;
        // ��������� ������
        if ( !Recv(data,data_size) ) goto error;
        BasicCode(data,data_size,0xEAF39ED4);

        State->LockGames();
        strcpy(file_name,win_dir);
        strcat(file_name,"TLK.GMS");
        // ������� ���� ��� ���������� ������ ���
        if ( (file=::CreateFile(file_name,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) { State->UnLockGames(); goto error; }
        // ��������� ������ � �����
        if ( ::WriteFile(file,data,data_size,&rw_size,NULL)==NULL ) { State->UnLockGames(); goto error; }
        ::CloseHandle(file);
        State->UnLockGames();

        State->Lock(); State->NeedUpdate=true; State->UnLock();
        delete[] data; data=NULL;
    }

    // ��������� �����
    Lock(); CloseSocket(); UnLock();
    return;
error:
    delete[] data;
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    // ��������� �����
    Lock(); CloseSocket(); UnLock();
}
//---------------------------------------------------------------------------

