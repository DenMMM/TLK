//---------------------------------------------------------------------------
#ifndef UnitComputersH
#define UnitComputersH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
#include "UnitGames.h"
//#include "UnitThreadNetSync.h"
#include <Classes.hpp>
#include <winsock2.h>
//---------------------------------------------------------------------------
#define MAX_Computers ((int)100)  // ����������� ����� ������������� �����������
#define MAX_NetTimeOut ((int)10)  // ����� �������� �� �������� ������� ���������� � ���� (� ��������)
#define NUM_SyncThreads ((int)5)  // ���������� �������, ������������ ������������� ����������
//---------------------------------------------------------------------------
#define mgcNone ((int)-1)  //
#define mgcAqua ((int)0)  //
#define mgcLime ((int)1)  //
#define mgcRed ((int)2)  //
#define mgcYellow ((int)3)  //
//---------------------------------------------------------------------------
#define mcsFree ((unsigned int)1)  // ��������
#define mcsWork ((unsigned int)2)  // � ������ (�����)
#define mcsFine ((unsigned int)4)  // � ������, �� ����������
#define mcsLock ((unsigned int)8)  // � ������, �� �������
#define mcsPause ((unsigned int)16)  // ��������� ������ � ����� �����������
#define mcsAuto ((unsigned int)32)  // ��������� ������ ��� ������������ � ����� �����������
#define mcsWtLocker ((unsigned int)64)  // ����� ��� ��������� TimeLocker'�
#define mcsNotUse ((unsigned int)128)  // ��������� �� ������������ � ������
#define mcsReboot ((unsigned int)256)  // ��������� ������ ���������������
#define mcsShutdown ((unsigned int)512)  // ��������� ������ �����������
//---------------------------------------------------------------------------
#define mnsState ((unsigned int)1)  // ����� ������, ��������� ������� � �.�.
#define mnsConfig ((unsigned int)2)  //
#define mnsGames ((unsigned int)4)  //
//---------------------------------------------------------------------------
#define mdcState ((unsigned int)1)  //
#define mdcTariff ((unsigned int)2)  //
#define mdcWorkTime ((unsigned int)4)  //
#define mdcFineTime ((unsigned int)8)  //
#define mdcNetState ((unsigned int)16)  //
//---------------------------------------------------------------------------
extern CRITICAL_SECTION CS_ComputersStateFile;
//---------------------------------------------------------------------------
class MCompParam;
class MCompParams;
//---------------------------------------------------------------------------
class MComputer;
class MComputers;
//---------------------------------------------------------------------------
class MThreadNetSync;
//---------------------------------------------------------------------------
class MCompParam:public MListItem
{
public:
    unsigned int ID;
    AnsiString Address;
    int Number;
    int GroupColor;

    MCompParam &operator=(MCompParam &CompParam_);
    MCompParam();
    ~MCompParam();
};
//---------------------------------------------------------------------------
class MCompParams:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MCompParam); }
    void delete_(MListItem *ListItem_) { delete ((MCompParam*)ListItem_); }
public:
    MCompParams() { constructor(); }
    ~MCompParams() { destructor(); }
};
//---------------------------------------------------------------------------
class MComputer:public MListItem
{
private:
    // ������������ ��� ������������� ������� ������ �������
    CRITICAL_SECTION CS_Main;  // ����� ������, ����� � �.�.
    CRITICAL_SECTION CS_Games;  // ������ ���
    // ������� �������� ��������� ������� ������ � ������� ������
    bool ControlWorkTime();
    bool ControlFineTime();
public:
    // ��������� ����������
    unsigned int ID;  // ���������� ����� ����������
    AnsiString Address;  // IP-����� ���������� � ����
    int Number;  // ����� ���������� � �����
    int GroupColor;  // ���� ������, � ������� ������� ���������
    // ����� ��������� ����������
    unsigned int State;  // ��������� ����������
    double StartWorkTime;  // ����� ������� ���������� � ������ (���������� �����)
    int SizeWorkTime;  // �� ����� ����� ������� ��������� (� �������)
    double StartFineTime;  // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;  // ����� �������� � ������ ������ (� �������)
    double StopTimerTime;  // ����� ��������� ������� ������� (���������� �����)
    //
    int FaceType;
    unsigned int GamesPages;
    unsigned int TariffID;  // ID-����� ������, �� �������� ��������� ��������
    // ��������� ����
    unsigned int StateVer;  // ������ ��������� (������������ ��� ��������� �������������)
    unsigned int NetSync;  // ���������, ��������� ������������� �� ����
    unsigned int NotSync;  // ���������, ��� ������ ��� �������������, �� ��� �� ������������
    int NetTimeOut;  // �����, ��������� � ������� ���������� �������� �� ����
    bool NetPresent;  // ����������� ���������� � ���� (��������� ���������� ��������)
    bool NeedConnect;  //
    bool SyncProcess;
    //
    unsigned int DataChanges;

    // �������, ����������� � �����������
    bool CmdRun(unsigned int TariffID_, unsigned int GamesPages_, int WorkTime_);
    void CmdFine(int FineSize_);
    void CmdExchange(MComputer *Computer_);
    void CmdLock(bool Apply_);
    void CmdPause(bool Apply_);
    void CmdAuto(bool Apply_);
    void CmdWtLocker(bool Apply_);
    void CmdNotUse(bool Apply_);
    void CmdReboot();
    void CmdShutdown();
    //
    bool Save();
    bool SetGames(MGames *Games_);
    bool GetGames(MGames *Games_);
    //
    void Timer();
    void NextStateVer();
    // ������� ������������� ������� ������ ������� � ������
    void Lock() { ::EnterCriticalSection(&CS_Main); }
    void UnLock() { ::LeaveCriticalSection(&CS_Main); }
    void LockGames() { ::EnterCriticalSection(&CS_Games); }
    void UnLockGames() { ::LeaveCriticalSection(&CS_Games); }

    MComputer();
    ~MComputer();
};
//---------------------------------------------------------------------------
class MComputers:public MList
{
private:
    MThreadNetSync *ThreadNetSync[NUM_SyncThreads];

    MListItem *new_() { return (MListItem*)(new MComputer); }
    void delete_(MListItem *ListItem_) { delete ((MComputer*)ListItem_); }
public:
    MComputer *SearchID(unsigned int ID_);
    MComputer *SearchIP(AnsiString &IP_);
    //
    bool Save();
    bool Load();
    void Get(MCompParams *CompParams_);
    void Set(MCompParams *CompParams_);
    void Timer();
    int GetPosition();

    MComputers();
    ~MComputers();
};
//---------------------------------------------------------------------------
class MThreadNetSync : public TThread
{
private:
    CRITICAL_SECTION CS_Main;
    CRITICAL_SECTION CS_Computers;
    bool PausedSync;
    bool ExitThread;
    //
    MComputer *FirstComputer;
    //
    SOCKET RemoteSocket;
    bool Waiting;
    int WaitTime;
    //
    int Position;

    //
    int GetComputerDataSize();
    char *SetComputerData(char *Data_, MComputer *Computer_);
    //
    void Lock() { ::EnterCriticalSection(&CS_Main); }
    void UnLock() { ::LeaveCriticalSection(&CS_Main); }
    void LockComputers() { ::EnterCriticalSection(&CS_Computers); }
    void UnLockComputers() { ::LeaveCriticalSection(&CS_Computers); }
    void CloseSocket();
    bool Send(char *Buffer_, int Size_);
    void StartWaiting(int Time_);
    void StopWaiting();
    void Process(MComputer *Computer_);
    void __fastcall Terminate(TObject *Sender);
protected:
    void __fastcall Execute();
public:
    void Init(MComputers *Computers_);
    void Pause(bool Set_);
    void Exit();
    void Timer();
    int GetPosition();
    __fastcall MThreadNetSync(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif

