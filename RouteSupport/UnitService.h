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
#define mcsWtLocker ((unsigned int)1)   // Режим без активного TimeLocker'а
#define mcsFree ((unsigned int)2)       // Свободен
#define mcsWork ((unsigned int)4)       // В работе
#define mcsFine ((unsigned int)8)       // В работе, но оштрафован (дополнительный флаг)
#define mcsLock ((unsigned int)16)      // В работе, но прикрыт (дополнительный флаг)
#define mcsPause ((unsigned int)32)     // Компьютер закрыт и время остановлено (дополнительный флаг)
#define mcsOpen ((unsigned int)64)      // Компьютер открыт для обслуживания и время остановлено (дополнительный флаг)
//---------------------------------------------------------------------------
// Main Games Pages
#define mgpRoute    256
//---------------------------------------------------------------------------
extern struct MState
{
    unsigned Size;
    int Number;                 // Номер компьютера
    unsigned State;             // Состояние компьютера (режимы работы)
    __int64 StartWorkTime;      // Время запуска компьютера в работу (абсолютное время)
    int SizeWorkTime;           // На какое время запущен компьютер (в минутах)
    __int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
    int SizeFineTime;           // Время ожидания в режиме штрафа (в минутах)
    __int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
    unsigned Programs;          // Групы программ, разрешенных для запуска
    unsigned Zero;
} State;

void BasicEncode(char *Data_, unsigned DataSize_, unsigned Code_);
bool StateLoad(MState *State_);
bool GetLocalTimeInt64(__int64 *lpInt64);
bool Timer(MState *State_);
void Route(MState *State_);
//---------------------------------------------------------------------------
#endif

