//---------------------------------------------------------------------------
#ifndef UnitServiceH
#define UnitServiceH
//---------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <iphlpapi.h>
//---------------------------------------------------------------------------
#define scmSTOP WM_USER+1
#define scmINTERROGATE WM_USER+2
#define scmSHUTDOWN WM_USER+3
//---------------------------------------------------------------------------
extern DWORD ServiceThreadID;
extern SERVICE_STATUS_HANDLE service_status_handle;
extern SERVICE_STATUS service_status;

VOID WINAPI Handler(DWORD fdwControl);
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
void ServiceStartProcess();
//---------------------------------------------------------------------------
#define mgpRoute ((unsigned int)32)
//---------------------------------------------------------------------------
struct MState
{
    int Number;
    unsigned int State;
    double StartWorkTime;
    int SizeWorkTime;
    double StartFineTime;
    int SizeFineTime;
    double StopTimerTime;
    unsigned int GamesPages;
    unsigned int StateVer;
};
//---------------------------------------------------------------------------
extern MState State;

void BasicEncode(char *Data_, int DataSize_, unsigned int Code_);
bool StateLoad(MState *State_);
void Route(bool Enable_);
//---------------------------------------------------------------------------
#endif

