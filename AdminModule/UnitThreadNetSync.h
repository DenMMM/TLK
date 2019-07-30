/*//---------------------------------------------------------------------------
#ifndef UnitThreadNetSyncH
#define UnitThreadNetSyncH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <winsock2.h>
//---------------------------------------------------------------------------
//#include "UnitComputers.h"
//---------------------------------------------------------------------------
class MThreadNetSync;
//---------------------------------------------------------------------------
class MThreadNetSync : public TThread
{
private:
    CRITICAL_SECTION CS_Main;
    CRITICAL_SECTION CS_Computers;
    HANDLE H_EventSyncExit;
    bool PausedSync;
    bool ExitThread;
    //
    MComputer *FirstComputer;
    //
    SOCKET RemoteSocket;
    bool Waiting;
    int WaitTime;
    
//    fd_set RemoteSocketRW;
//    fd_set RemoteSocketE;
//    timeval TimeOut;
    //
    int GetComputerDataSize();
    char *SetComputerData(char *Data_, MComputer *Computer_);
    //
    void Lock() { ::EnterCriticalSection(&CS_Main); }
    void UnLock() { ::LeaveCriticalSection(&CS_Main); }
    void LockComputers() { ::EnterCriticalSection(&CS_Computers); }
    void UnLockComputers() { ::LeaveCriticalSection(&CS_Computers); }
    void CloseSocket();
    void StartWaiting(int Time_);
    void StopWaiting();
    void Process(MComputer *Computer_);
    void __fastcall Terminate(TObject *Sender);
protected:
    void __fastcall Execute();
public:
    void Init(MComputers *Computers_);
    void Pause(bool Set_);
    void Sync();
    void Exit();
    void Timer();
    __fastcall MThreadNetSync(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
*/
