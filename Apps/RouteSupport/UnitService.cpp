//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitService.h"
#include "UnitStates.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
DWORD ServiceThreadID;
SERVICE_STATUS_HANDLE service_status_handle;
SERVICE_STATUS service_status;
//---------------------------------------------------------------------------
MState State;
//---------------------------------------------------------------------------
VOID WINAPI Handler(DWORD fdwControl)
{
    UINT Message;
    switch(fdwControl)
    {
        case SERVICE_CONTROL_STOP: Message=scmSTOP; break;
//        case SERVICE_CONTROL_INTERROGATE: Message=scmINTERROGATE; break;
        case SERVICE_CONTROL_SHUTDOWN: Message=scmSHUTDOWN; break;
        default: Message=0; break;
    }
    if ( Message ) ::PostThreadMessage(ServiceThreadID,Message,0,0);
}
//---------------------------------------------------------------------------
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    HKEY Key=NULL;
    HANDLE Event;
    MSG Message;
    MStateCl State;
    __int64 SystemTime;

    // ����� ������������� ������ ��� ��� ������������� ������������ �������� �� SCM
    ServiceThreadID=::GetCurrentThreadId();
    // ������� ������� ��������� ��� ������
    ::PeekMessage(NULL,NULL,0,0,PM_NOREMOVE);
    // ������������ ���������� �������� �� SCM � ������
    service_status_handle=::RegisterServiceCtrlHandler("TLKRoute",&Handler);

    // ����������� SCM � ���, ��� ���� ������ ������
    service_status.dwServiceType=SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState=SERVICE_START_PENDING;
    service_status.dwControlsAccepted=//SERVICE_CONTROL_INTERROGATE+
        SERVICE_ACCEPT_STOP+SERVICE_ACCEPT_SHUTDOWN;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=1;
    service_status.dwWaitHint=5000;
    ::SetServiceStatus(service_status_handle,&service_status);

    //
    State.SetDefault(HKEY_LOCAL_MACHINE,                    //path//
        "Software\\MMM Groups\\TLK\\3.0\\Client",           //path//
        "State", 0 , "", "", "", ENC_Code);
    //
    if ( (Event=::CreateEvent(NULL,FALSE,FALSE,"TLK_RegChange_Event"))==NULL ) goto exit;
    if ( (::RegOpenKeyEx(HKEY_LOCAL_MACHINE,                //path//
        "Software\\MMM Groups\\TLK\\3.0\\Client",           //path//
        0,KEY_NOTIFY|KEY_QUERY_VALUE,&Key))!=ERROR_SUCCESS ) goto exit;

    // ����������� SCM �� �������� ������� ������
    service_status.dwCurrentState=SERVICE_RUNNING;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);

    while(true)
    {
        // ��������� ��������� ������� � ����������� �������������
        if ( State.Load() ) Route(&State);

        // ��������� ������� ��������� � �������
        if ( (::RegNotifyChangeKeyValue(Key,FALSE,
            REG_NOTIFY_CHANGE_LAST_SET,Event,TRUE))!=ERROR_SUCCESS ) break;

        // ���� ������� ������������ �������� ������� ���������
        while(::WaitForSingleObject(Event,200)!=WAIT_OBJECT_0)
        {
            // ������������ ����� ������
            if ( GetLocalTimeInt64(&SystemTime)&&
                State.Timer(SystemTime) ) Route(&State);
            // ��������� ������� ���������
            if ( !::PeekMessage(&Message,NULL,0,0,PM_REMOVE) ) continue;
            //
            if ( (Message.message==scmSTOP)||
                (Message.message==scmSHUTDOWN) ) goto exit;
//            if ( Message.message==scmINTERROGATE ) ;
        }
    }

exit:
    service_status.dwCurrentState=SERVICE_STOP_PENDING;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);

    //
    ::CloseHandle(Event);
    ::RegCloseKey(Key);
//    ServiceStartProcess();

    // ����������� SCM �� �������� �������� ������
    service_status.dwCurrentState=SERVICE_STOPPED;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);
}
//---------------------------------------------------------------------------
void Route(MStateCl *State_)
{
    ULONG TableSize;
    PMIB_IFTABLE TableIF=NULL;
    PMIB_IFROW RecordIF;
    PIP_PER_ADAPTER_INFO AdapterInfo;
    MIB_IPFORWARDROW Record;
    MStateInfo Info;
    bool Enable;

    State_->StateInfo(&Info);
    // ���������� �������� ��� ������� �������
    Enable=(Info.State&mcsOpen)||
        ((Info.State&mcsWork)&&
        (!(Info.State&(mcsFine|mcsPause)))&&
        (Info.Programs&mgpRoute));

    // ����������� ������ ������� �����������
    TableSize=0;
    if ( ::GetIfTable(NULL,&TableSize,TRUE)!=ERROR_INSUFFICIENT_BUFFER ) goto error;
    // �������� ������ ��� �������
    if ( (TableIF=(PMIB_IFTABLE)new char[TableSize])==NULL ) goto error;
    memset(TableIF,0,TableSize);
    // ����� �������
    if ( ::GetIfTable(TableIF,&TableSize,TRUE)!=0 ) goto error;

    // ���� � ������� ����������� ������ ���� Ethernet
    for ( DWORD i=0; i<TableIF->dwNumEntries; i++ )
    {
        RecordIF=&TableIF->table[i];
        if ( RecordIF->dwType!=IF_TYPE_ETHERNET_CSMACD ) continue;
        // ����������� �������������� ��������� ������������ ��������
        TableSize=0;
        if ( ::GetPerAdapterInfo(RecordIF->dwIndex,NULL,&TableSize)!=
            ERROR_BUFFER_OVERFLOW ) continue;
        if ( (AdapterInfo=(PIP_PER_ADAPTER_INFO)new char[TableSize])==NULL ) goto error2;
        if ( ::GetPerAdapterInfo(RecordIF->dwIndex,AdapterInfo,&TableSize)!=0 ) goto error2;
        // �������������� ������ ��� ������� �������������
        memset(&Record,0,sizeof(Record));
        Record.dwForwardIfIndex=RecordIF->dwIndex;
        Record.dwForwardDest=0;
        Record.dwForwardMask=0;
        Record.dwForwardNextHop=::inet_addr(AdapterInfo->DnsServerList.IpAddress.String);
        // ��������� ��� �� ������ ���������� ����� DNS-�������
        if ( Record.dwForwardNextHop==INADDR_NONE ) goto error2;
        Record.dwForwardPolicy=0;
//        Record.dwForwardType=4;
//        Record.dwForwardProto=3;
        // ���������/������� ������ � �������
        if ( Enable ) ::CreateIpForwardEntry(&Record);
        else ::DeleteIpForwardEntry(&Record);
error2:
        delete[] AdapterInfo;
        break;
    }

error:
    delete[] TableIF;
}
//---------------------------------------------------------------------------

