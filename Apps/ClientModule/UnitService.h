//---------------------------------------------------------------------------
#ifndef UnitServiceH
#define UnitServiceH
//---------------------------------------------------------------------------
class MSvcProcess;
//---------------------------------------------------------------------------
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

//#include <windows.h>
#include <winsock2.h>
#include <wtsapi32.h>
#include <iphlpapi.h>
#include <userenv.h>

#include "UnitClOptions.h"
#include "UnitStates.h"
#include "UnitSync.h"
#include "UnitSend.h"
#include "UnitAuth.h"
#include "UnitShared.h"
//---------------------------------------------------------------------------
#define SVC_Name            "TLKClient"
#define SVC_Desc            "TLK Client"
//---------------------------------------------------------------------------
#define scmSTOP             WM_USER+1
#define scmLOGON            WM_USER+2
#define scmLOGOFF           WM_USER+3
//---------------------------------------------------------------------------
extern volatile DWORD ServiceThreadID;
//---------------------------------------------------------------------------
DWORD WINAPI HandlerEx(
  DWORD  dwControl,
  DWORD  dwEventType,
  LPVOID lpEventData,
  LPVOID lpContext);
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
//---------------------------------------------------------------------------
class MSvcProcess
{
private:
    MClOptions Options;
    MStateCl State;
    MAuth Auth;
    MSyncCl Sync;
    MSendCl Send;
    MShared Shared;

    char ExeName[MAX_PATH+1];
    HANDLE hUserToken;
    HANDLE hProcessShell;
    UINT uTimer;
    int TimeMsgWarn;
    bool MsgWarnShowed;
    int TimeMsgEndTime;
    int TimeToReboot;

    static bool CheckUser(HANDLE hUser_, const char *Login_);
    HANDLE StartChild(HANDLE hProcess_, const char *Cmd_);
    void StartShell() { hProcessShell=StartChild(hProcessShell,"/shell"); }
    void Route(bool Enable_);

    void OnTimer();
    void OnLogOn(DWORD dwSessionId_);
    void OnLogOff(DWORD dwSessionId_);

public:
    bool OnStart();
    void OnExecute();
    void OnStop();

    MSvcProcess()
    {
        hUserToken=INVALID_HANDLE_VALUE;
        hProcessShell=INVALID_HANDLE_VALUE;
        uTimer=0;
        TimeMsgWarn=0;
        MsgWarnShowed=false;
        TimeMsgEndTime=0;
        TimeToReboot=0;
    }
};
//---------------------------------------------------------------------------
#endif

