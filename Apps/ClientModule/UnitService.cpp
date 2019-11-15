//---------------------------------------------------------------------------
#include <stdio.h>
#include <memory>
#include <vector>
#include <string>
#include <cwchar>
#pragma hdrstop

#include "UnitService.h"
#include "UnitStates.h"
#include "UnitAuth.h"
#include "UnitSync.h"
#include "UnitSend.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
volatile DWORD ServiceThreadID;
//---------------------------------------------------------------------------
DWORD WINAPI HandlerEx(
  DWORD  dwControl,
  DWORD  dwEventType,
  LPVOID lpEventData,
  LPVOID lpContext
)
{
    switch(dwControl)
    {
        case SERVICE_CONTROL_STOP:
            ::PostThreadMessage(ServiceThreadID,scmSTOP,0,0);
            break;

        case SERVICE_CONTROL_SESSIONCHANGE:
            switch(dwEventType)
            {
                case 0x5:   // WTS_SESSION_LOGON:
                    ::PostThreadMessage(ServiceThreadID,scmLOGON,
                        ((PWTSSESSION_NOTIFICATION)lpEventData)->dwSessionId,0);
                    break;
                case 0x6:   // WTS_SESSION_LOGOFF:
                    ::PostThreadMessage(ServiceThreadID,scmLOGOFF,
                        ((PWTSSESSION_NOTIFICATION)lpEventData)->dwSessionId,0);
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }

    return 0;
}
//---------------------------------------------------------------------------
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    SERVICE_STATUS_HANDLE service_status_handle;
    SERVICE_STATUS service_status;
	std::unique_ptr <MSvcProcess> Svc;

    // ����� ������������� ������ ��� ��� ������������� ������������ �������� �� SCM
    ServiceThreadID=::GetCurrentThreadId();
    // ������� ������� ��������� ��� ������
    ::PeekMessage(nullptr,nullptr,0,0,PM_NOREMOVE);
    // ������������ ���������� �������� �� SCM � ������
    service_status_handle=::RegisterServiceCtrlHandlerEx(SVC_Name, &HandlerEx, nullptr);

    // ����������� SCM � ���, ��� ���� ������ ������
    service_status.dwServiceType=SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState=SERVICE_START_PENDING;
    service_status.dwControlsAccepted=
        SERVICE_ACCEPT_STOP|
        SERVICE_ACCEPT_SESSIONCHANGE;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=5000;
    ::SetServiceStatus(service_status_handle,&service_status);

    try
    {
		Svc.reset(new MSvcProcess);

        if ( Svc->OnStart() )
        {
            // ����������� SCM �� �������� ������� ������
            service_status.dwCurrentState=SERVICE_RUNNING;
            service_status.dwCheckPoint=0;
            service_status.dwWaitHint=0;
            ::SetServiceStatus(service_status_handle,&service_status);

            Svc->OnExecute();
        }
    }
    catch ( std::bad_alloc &e )
    {
        ResMessageBox(nullptr,1,3,MB_SERVICE_NOTIFICATION);
    }
    catch (std::exception &e)
    {
		::MessageBoxA(nullptr, e.what(),
            "TLK - ������",MB_SERVICE_NOTIFICATION);
    }

    service_status.dwCurrentState=SERVICE_STOP_PENDING;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=5000;
    ::SetServiceStatus(service_status_handle,&service_status);

    Svc->OnStop();

    // ����������� SCM �� �������� �������� ������
    service_status.dwCurrentState=SERVICE_STOPPED;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);
}
//---------------------------------------------------------------------------
bool MSvcProcess::CheckUser(HANDLE hUser_, const wchar_t *Login_)
{
    std::vector <char> vUserToken;
	std::vector <wchar_t> vUserName;
    std::vector <wchar_t> vDomainName;
    PTOKEN_USER pUserToken;
    DWORD dwTokenSize;
    DWORD dwUserNameSize;
    DWORD dwDomainNameSize;
    SID_NAME_USE SidNameUse;

    // �������� ������ ��� ������ ������������
    dwTokenSize=0;
    if ( (!::GetTokenInformation(
		hUser_, TokenUser, nullptr,
		0, &dwTokenSize))&&
		(::GetLastError()!=ERROR_INSUFFICIENT_BUFFER) ) goto error;
    if ( dwTokenSize==0 ) goto error;
    vUserToken.resize(dwTokenSize);
	pUserToken=(PTOKEN_USER)vUserToken.data();

    // ����������� ��� � �����
    if ( !::GetTokenInformation(
        hUser_,TokenUser,pUserToken,
        dwTokenSize,&dwTokenSize) ) goto error;

    // �������� ������ ��� ������ ������������ � ����� ������
    dwUserNameSize=0;
    dwDomainNameSize=0;
	if ( (!::LookupAccountSid(
        nullptr,pUserToken->User.Sid,
        nullptr,&dwUserNameSize,
        nullptr,&dwDomainNameSize,
        &SidNameUse))&&
        (::GetLastError()!=ERROR_INSUFFICIENT_BUFFER) ) goto error;
    if ( (dwUserNameSize==0)||(dwDomainNameSize==0) ) goto error;
    vUserName.resize(dwUserNameSize);
    vDomainName.resize(dwDomainNameSize);
    
    // ����������� ����� � ������
	if ( !::LookupAccountSid(
		nullptr, pUserToken->User.Sid,
		vUserName.data(), &dwUserNameSize,
		vDomainName.data(), &dwDomainNameSize,
		&SidNameUse) ) goto error;
	// ��� ���������� ������ ������ ?
    if ( SidNameUse!=SidTypeUser ) goto error;

#ifdef _DEBUG
	::MessageBox(
		nullptr, vUserName.data(),
		L"MSvcProcess::CheckUser()",
		MB_SERVICE_NOTIFICATION);
#endif

    // ���������� �����
//    return strcmpi(vUserName.data(), Login_)==0;    /// ���������� � �� ASCII ?
	return wcscmp(vUserName.data(), Login_)==0;
error:
    return false;
}
//---------------------------------------------------------------------------
HANDLE MSvcProcess::StartChild(HANDLE hProcess_, const wchar_t *Cmd_)
{
    // ���� ������������ �� ���������, �� � ��������� ������
    if ( hUserToken==INVALID_HANDLE_VALUE ) return hProcess_;

    // �������� ��� �� ������� ��� ������� � ������� �� �� ���
    if ( (hProcess_!=INVALID_HANDLE_VALUE)&&
        (::WaitForSingleObject(hProcess_,0)==WAIT_TIMEOUT) ) return hProcess_;

    // ���������� ��������� ������ � �����������
	std::wstring cmd_line;
    cmd_line=ExeName;
	cmd_line+=L" ";
	cmd_line+=Cmd_;
    cmd_line+=L":";
    cmd_line+=Shared.InhToHEX();

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset(&si,0,sizeof(STARTUPINFO));
    si.cb=sizeof(STARTUPINFO);

#ifdef _DEBUG
	::MessageBox(nullptr, cmd_line.c_str(), L"debug", MB_SERVICE_NOTIFICATION);
#endif

	// �������� ������� � ������ ������������� ������������
	std::vector <wchar_t> cmd_line_buff(cmd_line.length()+1);
	wcscpy(cmd_line_buff.data(), cmd_line.c_str());

	LPVOID lpEnv=nullptr;
	if ( ::CreateEnvironmentBlock(
			&lpEnv,
			hUserToken,
			FALSE)&&
		::CreateProcessAsUser(
			hUserToken,
			nullptr, cmd_line_buff.data(),
			nullptr, nullptr, TRUE,
			CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS|CREATE_UNICODE_ENVIRONMENT,
			lpEnv, nullptr, &si, &pi) ) hProcess_=pi.hProcess;
    else hProcess_=INVALID_HANDLE_VALUE;
    if ( lpEnv!=nullptr ) ::DestroyEnvironmentBlock(lpEnv);

    return hProcess_;
}
//---------------------------------------------------------------------------
void MSvcProcess::Route(bool Enable_)
{
    const ULONG TableFlags=
        GAA_FLAG_SKIP_UNICAST|
        GAA_FLAG_SKIP_ANYCAST|
        GAA_FLAG_SKIP_MULTICAST;
    std::vector <char> vTable;
    PIP_ADAPTER_ADDRESSES pAdapter;

    // ����������� ������� �������
    for ( int retryes=3; retryes; retryes-- )
    {
        ULONG TableSize=0;

        // ����������� �� ������
        if ( ::GetAdaptersAddresses(
            AF_INET,TableFlags,nullptr,
            nullptr,&TableSize)!=ERROR_BUFFER_OVERFLOW ) continue;
        if ( TableSize==0 ) continue;
        vTable.resize(TableSize);

        // ����������� �������
        if ( ::GetAdaptersAddresses(
            AF_INET,TableFlags,nullptr,
            (PIP_ADAPTER_ADDRESSES)vTable.data(),
            &TableSize)==ERROR_SUCCESS ) break;
        vTable.clear();
    }
    // �������, ���� ������� ������
    if ( vTable.empty() ) return;

    // ���� ���������� �������
    pAdapter=(PIP_ADAPTER_ADDRESSES)vTable.data();
    while(pAdapter)
    {
        // �������� ������ �� ���� ���� DNS-������
        if ( pAdapter->FirstDnsServerAddress!=nullptr ) break;
        pAdapter=pAdapter->Next;
    }
    // ��������, ���� �� ����� �������
    if ( pAdapter==nullptr ) return;

	sockaddr_in *SockAddr=reinterpret_cast<sockaddr_in*>(
		pAdapter->FirstDnsServerAddress->Address.lpSockaddr);

    // �������� ������ ��� ������� �������������
    MIB_IPFORWARDROW Route;
    memset(&Route,0,sizeof(Route));
    Route.dwForwardDest=0;
    Route.dwForwardMask=0;
    Route.dwForwardPolicy=0;
    Route.dwForwardNextHop=SockAddr->sin_addr.s_addr;
    Route.dwForwardIfIndex=pAdapter->IfIndex;
    Route.dwForwardType=MIB_IPROUTE_TYPE_DIRECT;
    Route.dwForwardProto=MIB_IPPROTO_NETMGMT;
    Route.dwForwardAge=0;
    Route.dwForwardNextHopAS=0;
//    Route.dwForwardMetric1=pAdapter->Ipv4Metric;    // Vista+
    Route.dwForwardMetric1=65535;                   /// �������...
    Route.dwForwardMetric2=-1;
    Route.dwForwardMetric3=-1;
    Route.dwForwardMetric4=-1;
    Route.dwForwardMetric5=-1;

    // ���������/������� ������ � �������
    DWORD result;
    if ( Enable_ ) result=::CreateIpForwardEntry(&Route);
    else result=::DeleteIpForwardEntry(&Route);

#ifdef _DEBUG
{
	wchar_t msg[256+1];
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,nullptr,
		result,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		(LPTSTR)msg,256,nullptr);
	::MessageBox(nullptr, msg, L"MSvcProcess::Route()", MB_SERVICE_NOTIFICATION);
}
#endif
}
//---------------------------------------------------------------------------
bool MSvcProcess::OnStart()
{
    // �������� ��� ����� ������
	if ( !::GetModuleFileName(nullptr, ExeName, MAX_PATH) )
    {
        ResMessageBox(nullptr,1,4,MB_SERVICE_NOTIFICATION,::GetLastError());
        return false;
    }

    // ����������� ����
    {
        wchar_t gms_path[MAX_PATH+7+1], *delim;

		wcscpy(gms_path, ExeName);
		delim=wcsrchr(gms_path, L'\\');
		if ( delim!=nullptr ) delim[1]=0;
		wcscat(gms_path, L"TLK.GMS");

        State.SetDefault(
			HKEY_LOCAL_MACHINE, L"Software\\MMM Groups\\TLK\\3.0\\Client", L"State",    //path//
			HKEY_LOCAL_MACHINE, L"Software\\MMM Groups\\TLK\\3.0\\Client", L"Options",  //path//
			gms_path,ENC_Code);
		Auth.SetDefaultKey(
			HKEY_LOCAL_MACHINE, L"Software\\MMM Groups\\TLK\\3.0\\Client", L"Auth",ENC_Code);    //path//
    }

    // ��������� ���������, ������� �� "��������" ��� �������������
    if ( !Auth.Load() ) Auth.Save();
    if ( !State.Load() ) State.Save();
    if ( !State.GetOptions(&Options) )
    {
        Options.SetShellUser(L"");
        Options.ToEndTime=2;
        Options.MessageTime=15;
        Options.MsgEndTime=10;
        Options.RebootWait=20;
        Options.AutoLockTime=15;
        Options.Flags=0;
        State.NewOptions(&Options);
    }

    // ���������� ����
    if ( !(Sync.NetInit(ENC_Net,&Auth)&&
        Send.NetInit(&State,ENC_Net,&Auth)) )
    {
        ResMessageBox(nullptr,1,12,MB_SERVICE_NOTIFICATION);
        return false;
    }

    if ( !Shared.Create() )
    {
        ResMessageBox(nullptr,1,13,MB_SERVICE_NOTIFICATION);
        return false;
    }

    // ������� ��������� ����� ������������
    Shared.SetTransp(Options.Flags&mcoTransp);

    // ������ �������
    uTimer=::SetTimer(nullptr,0,1000,nullptr);
    if ( uTimer==0 )
    {
        ResMessageBox(nullptr,1,4,MB_SERVICE_NOTIFICATION,::GetLastError());
        return false;
    }

    // ������ ����
    Sync.Associate(&State);
    if ( (!Sync.Start())||
         (!Send.Start()) )
    {
        ResMessageBox(nullptr,1,12,MB_SERVICE_NOTIFICATION);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void MSvcProcess::OnStop()
{
    ::KillTimer(nullptr,uTimer);

    Send.Stop(); Send.NetFree();
    Sync.Stop(); Sync.NetFree();

    Shared.Close();
}
//---------------------------------------------------------------------------
void MSvcProcess::OnExecute()
{
    OnTimer();

    // �������� TLK shell, ���� ������������ ��� ����� � �������
    PWTS_SESSION_INFO pInfo;
    DWORD dwInfoCount;
    if ( !::WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE,
        0,1,&pInfo,&dwInfoCount) )
    {
///        ResMessageBox(nullptr,1,11,MB_SERVICE_NOTIFICATION,::GetLastError());
    } else
    {
        for ( DWORD i=0; i<dwInfoCount; i++ )
        {
//            if (
//                (pInfo[i].State==WTSActive)||
//                (pInfo[i].State==WTSConnected)||
//                (pInfo[i].State==WTSDisconnected) )
                    OnLogOn(pInfo[i].SessionId);
        }
        ::WTSFreeMemory(pInfo);
    }

    MSG msg;
    while(::GetMessage(&msg,nullptr,0,0))
    {
        if ( msg.message==scmSTOP ) break;
        switch(msg.message)
        {
            case WM_TIMER:  OnTimer(); break;
            case scmLOGON:  OnLogOn(msg.wParam); break;
            case scmLOGOFF: OnLogOff(msg.wParam); break;
            default: break;
        }
    }
}
//---------------------------------------------------------------------------
void MSvcProcess::OnTimer()
{
    __int64 SystemTime;
	MStatesInfo Info;

    // ��������� ������ shared-������
    if ( !Shared.Lock() ) return;

    GetLocalTimeInt64(SystemTime);
    Shared.UpdateSysTime(SystemTime);

    // ���� ����� ������ �������������� �� ��������� ������� �������, ������ ���������
    if ( TimeMsgWarn&&((--TimeMsgWarn)==0) ) Shared.ShowWarnMsg(false);
    // ���� ����� ������ ��������� �� ��������� ������� �������, ������ ���������
    if ( TimeMsgEndTime&&((--TimeMsgEndTime)==0) ) Shared.ShowImageMessage(mimLocked);
    // ���� ������ ������������ �����, �������� ��� ����� ���������������
    if ( TimeToReboot&&((--TimeToReboot)==0) ) State.CmdReboot();
    // ��������� ��������� � ������������ �� ��������
    if ( State.Timer(SystemTime) ) State.Save();
    State.StateInfo(&Info);

    // ����������� ����� ���������
    if ( Info.Changes&mdcOptions )
    {
        State.GetOptions(&Options);
        // ������� ����� ������������
        Shared.SetTransp(Options.Flags&mcoTransp);
    }
    // ��������� ����� ����������
    if ( Info.Changes&mdcNumber ) Shared.UpdateCompNum(Info.Number);
    // ����� ������
    if ( Info.Changes&mdcState )
    {
        Shared.SetConfigMode(false);
        if ( Info.State&mcsOpen )
        {
            // ������ ��� ���������
            Shared.SetConfigMode(true);
            TimeToReboot=0;
            TimeMsgEndTime=0;
            Shared.ShowWarnMsg(false);
            Shared.ShowImageMessage(mimNone);
            Shared.ShowGames(mgpAll);
        } else if ( Info.State&mcsFree )
        {
            Shared.ShowWarnMsg(false);
            Shared.ShowGames(mgpNone);
            Shared.UpdateWorkTime(0);
            if ( Info.Changes&mdcWorkTime )
            {
                // ����� �����������
                TimeToReboot=Options.RebootWait;
                TimeMsgEndTime=Options.MsgEndTime;
                Shared.ShowImageMessage(TimeMsgEndTime? mimEndTime: mimLocked);
            } else
            {
                // ��������� ������
                TimeToReboot=0;
                TimeMsgEndTime=0;
                Shared.ShowImageMessage(mimLocked);
            }
        } else if ( Info.State&mcsWork )
        {
            if ( Info.Changes&mdcWorkTime )
            {
                // ��������� �������
                TimeToReboot=0;
                TimeMsgEndTime=0;
                Shared.ShowWarnMsg(false); MsgWarnShowed=false;
            }
            // �������������� ������
            if ( Info.State&mcsPause )
            {
                // ����� ��������������
                Shared.ShowGames(mgpNone);
                Shared.ShowImageMessage(mimTimePaused);
            } else if ( Info.State&mcsLock )
            {
                // �������
                Shared.ShowGames(mgpNone);
                Shared.ShowImageMessage(mimPaused);
            } else if ( Info.State&mcsFine )
            {
                // �����
                Shared.ShowGames(mgpNone);
                Shared.ShowImageMessage(mimFine);
            } else
            {
                // ������
                Shared.ShowImageMessage(mimNone);
                Shared.ShowGames(Info.Programs);
            }
        }

        // ������ � ��������
        if ( Options.Flags&mcoAddRoute )
        {
            // ���������� �������� ��� ������� ������� ��-���������
            bool Enable=(Info.State&mcsOpen)||
                ((Info.State&mcsWork)&&
                (!(Info.State&(mcsFine|mcsPause)))&&
                (Info.Programs&mgpRoute));
            Route(Enable);
        }
    } else if ( Info.Changes&mdcPrograms )
    {
        // ��������� ������ �������� ��� ��������� ������ ������
        if ( Info.State==mcsWork ) Shared.ShowGames(Info.Programs);
        else if ( Info.State&mcsOpen ) Shared.ShowGames(mgpAll);
    }

    //
    if ( Info.Changes&mdcWorkTime ) MsgWarnShowed=false;
    // ������� ���������� �����
    if ( Info.State&mcsWork )
    {
        Shared.UpdateWorkTime(Info.ToEndWork);
        // ���� �� ��������� ������� �������� ����� ���� �����, ��������� ����� ���������
        if ( (!MsgWarnShowed)&&Options.ToEndTime&&
            (Info.ToEndWork<=Options.ToEndTime) )
        {
            MsgWarnShowed=true;
            TimeMsgWarn=Options.MessageTime;
            Shared.ShowWarnMsg(true);
        }
    }

    //
    if ( Info.Changes&mdcCommands )
    {
        if ( Info.Commands&mccShutdown )
        {
            State.Save();
#ifndef _DEBUG
            WinExit(EWX_POWEROFF|EWX_FORCE);  /// EWX_FORCEIFHUNG �� ��������
#endif
        } else if ( Info.Commands&mccReboot )
        {
            State.Save();
#ifndef _DEBUG
            WinExit(EWX_REBOOT|EWX_FORCE);
#endif
        }
    }

    Shared.UnLock();

    // ������������ TLK shell, �� ������ ��� "�������"
    StartShell();
}
//---------------------------------------------------------------------------
void MSvcProcess::OnLogOn(DWORD dwSessionId_)
{
    HANDLE hUser;

    // �������� ��� ���� �� ������������� ��������� TLK shell
    if ( !::WTSQueryUserToken(dwSessionId_,&hUser) )
    {
//        ResMessageBox(nullptr,1,10,MB_SERVICE_NOTIFICATION,::GetLastError());
        return;
    }
	if ( !CheckUser(hUser, Options.ShellUser.c_str()) ) return;

    // �������� handle "������" ������������ � �������� ��������
    hUserToken=hUser;
    StartShell();

    // �������� ��������� �� ������� ����������� (����� ������� �� ������...)
    if ( Options.Flags&mcoAutoRun )
    {
        RegExecList(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
            hUserToken);
    }
}
//---------------------------------------------------------------------------
void MSvcProcess::OnLogOff(DWORD dwSessionId_)
{
    if ( hUserToken==INVALID_HANDLE_VALUE ) return;

    // �������� "���" �� ������������ ����� �� �������
    HANDLE hUser;
    if ( !::WTSQueryUserToken(dwSessionId_,&hUser) )
    {
//        ResMessageBox(nullptr,1,10,MB_SERVICE_NOTIFICATION,::GetLastError());
        return;
    }
    if ( hUser!=hUserToken ) return;

    // ������� �������� � ������������ � ���������� �������� ���������
    hUserToken=INVALID_HANDLE_VALUE;
    hProcessShell=INVALID_HANDLE_VALUE;
}
//---------------------------------------------------------------------------

