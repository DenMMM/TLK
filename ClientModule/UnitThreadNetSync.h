//---------------------------------------------------------------------------
#ifndef UnitThreadNetSyncH
#define UnitThreadNetSyncH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <winsock2.h>
//---------------------------------------------------------------------------
class MThreadNetSync : public TThread
{
private:
    //
    CRITICAL_SECTION CS_Main;
    bool PausedSync;
    //
    SOCKET LocalSocket;
    SOCKET RemoteSocket;
    sockaddr_in LocalAddr;
    sockaddr_in RemoteAddr;
    //
    void __fastcall Terminate(TObject *Sender);
    void Lock();
    void UnLock();
    void CloseSocket();
    bool Recv(char *Buffer_, int Size_);
    void Process();
protected:
    void __fastcall Execute();
public:
    void Pause(bool Set_);
    void Exit();
    __fastcall MThreadNetSync(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif

