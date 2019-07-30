//---------------------------------------------------------------------------
#ifndef UnitServiceH
#define UnitServiceH
//---------------------------------------------------------------------------
#include <stdio.h>
#include <winsock2.h>
//#include <windows.h>
#include <iphlpapi.h>
#include "UnitStates.h"
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
//---------------------------------------------------------------------------
void Route(MStateCl *State_);
//---------------------------------------------------------------------------
#endif

