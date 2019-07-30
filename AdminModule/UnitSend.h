//---------------------------------------------------------------------------
#ifndef UnitSendH
#define UnitSendH
//---------------------------------------------------------------------------
#include <windows.h>
#include <winsock2.h>
//---------------------------------------------------------------------------
#include "UnitComputers.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#define SEND_Version        0x30    // ������ �������� ����������
#define SEND_SyncPort       7005    // ����� ����� ��� �������� ������
#define SEND_MinData        8       // 
#define SEND_MaxData        32768   //
//---------------------------------------------------------------------------
#define mssWait         0
#define mssSend         1
#define mssGetGames     2
#define mssGetConfig    3
//---------------------------------------------------------------------------
// Main Send Type
#define mstAccept           0x01020304
//#define mstDeny             0x04030201
#define mstGames            1
#define mstConfig           2
#define mstGetGames         3
#define mstGetConfig        4
//---------------------------------------------------------------------------
// Main Send Event
#define mseConnecting       1   // ��������������� ����������
#define mseSending          2   // �������� ������
#define mseReceiving        3   // ����� ������
#define mseDisconnecting    4   // ������������
#define mseExecute          5   // ����� ���������� �������
#define mseNotConnect       6   // �� ������� ����������� � ��������
#define msePrclError        7   // ������ ��������� (������ ������ ��� ������� �� ������ TLK)
#define mseFreeParam        8   // ����� ������� �������� ��� ��������� ��������� ������
//---------------------------------------------------------------------------
class MSend
{
private:
    HWND Window;                // ���� ��� ��������� ��������� � �������� ��������
    UINT MinMsg;                //
    HANDLE Thread;              // ���������� ������, ��������������� ��������/�����
    DWORD ThreadID;             // ������������� ������
    SOCKET Socket;              // �����
    int Mode;
    MComputer **Computers;
    MGames *Games;
    MClOptions *Options;
    char *Data;                 // ������ ��� ��������
    unsigned DataSize;          // ������ ������
    bool Break;

    inline void Event(MComputer *Computer_, int Event_);
    bool Create();
    bool Connect(char *IP_, unsigned Time_);
    bool Send(char *Data_, unsigned Size_, unsigned Time_);
    bool Recv(char *Data_, unsigned Size_, unsigned Time_);
    bool Disconnect(unsigned Time_);
    void Close();
    static DWORD WINAPI ThreadFunc(LPVOID Data);
    void ThreadSend();
    void ThreadGet();
public:
    void SetShell(HWND Window_, UINT MinMsg_);
    bool Send(MComputer **Computers_, MGames *Games_, MClOptions *Options_);
    bool Get(MComputer *Computer_, MGames *Games_, MClOptions *Options_);
    void Stop();

    MSend();
    ~MSend();
};
//---------------------------------------------------------------------------
#endif

