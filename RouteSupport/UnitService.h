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
// Main Computer State
#define mcsWtLocker ((unsigned int)1)   // ����� ��� ��������� TimeLocker'�
#define mcsFree ((unsigned int)2)       // ��������
#define mcsWork ((unsigned int)4)       // � ������
#define mcsFine ((unsigned int)8)       // � ������, �� ���������� (�������������� ����)
#define mcsLock ((unsigned int)16)      // � ������, �� ������� (�������������� ����)
#define mcsPause ((unsigned int)32)     // ��������� ������ � ����� ����������� (�������������� ����)
#define mcsOpen ((unsigned int)64)      // ��������� ������ ��� ������������ � ����� ����������� (�������������� ����)
//---------------------------------------------------------------------------
// Main Games Pages
#define mgpRoute    256
//---------------------------------------------------------------------------
extern struct MState
{
    unsigned Size;
    int Number;                 // ����� ����������
    unsigned State;             // ��������� ���������� (������ ������)
    __int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
    int SizeWorkTime;           // �� ����� ����� ������� ��������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;           // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Zero;
} State;

void BasicEncode(char *Data_, unsigned DataSize_, unsigned Code_);
bool StateLoad(MState *State_);
bool GetLocalTimeInt64(__int64 *lpInt64);
bool Timer(MState *State_);
void Route(MState *State_);
//---------------------------------------------------------------------------
#endif

