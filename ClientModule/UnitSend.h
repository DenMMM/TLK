//---------------------------------------------------------------------------
#ifndef UnitSendH
#define UnitSendH
//---------------------------------------------------------------------------
#include <winsock2.h>
//---------------------------------------------------------------------------
#include "UnitState.h"
#include "UnitClOptions.h"
#include "UnitGames.h"
//---------------------------------------------------------------------------
#define SEND_Version        0x30    // Версия сетевого интерфейса
#define SEND_SyncPort       7005    // Номер порта для ожидания данных
#define SEND_MinData        8       // 
#define SEND_MaxData        32768   //
//---------------------------------------------------------------------------
// Main Send Type
#define mstAccept       0x01020304
//#define mstDeny         0x04030201
#define mstGames        1
#define mstConfig       2
#define mstGetGames     3
#define mstGetConfig    4
//---------------------------------------------------------------------------
class MSend
{
private:
    HANDLE Thread;              // Дескриптор потока, осуществляющего отправку/прием
    DWORD ThreadID;             // Идентификатор потока
    SOCKET Socket;              // Сокет
    SOCKET RemoteSocket;
    char *Data;                 // Данные для приема
    MState *State;

    bool Create();
    bool Listen();
    bool Accept();
    bool Send(char *Data_, unsigned Size_, unsigned Time_);
    bool Recv(char *Data_, unsigned Size_, unsigned Time_);
    bool Disconnect(unsigned Time_);
    void CloseRemote();
    void Close();
    static DWORD WINAPI ThreadFunc(LPVOID Data);
    void ThreadProc();
    bool ProcessRecv(unsigned Type_);
    bool ProcessSend(unsigned Type_);
public:
    void Associate(MState *State_);
    bool Start();
    void Stop();

    MSend();
    ~MSend();
};
//---------------------------------------------------------------------------
#endif

