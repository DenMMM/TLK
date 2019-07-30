//---------------------------------------------------------------------------
#ifndef UnitStatesH
#define UnitStatesH
//---------------------------------------------------------------------------
struct MStateData;
struct MStateInfo;

class MState;
class MStates;
class MStateCl;
//---------------------------------------------------------------------------
#include <winsock2.h>
//#include <windows.h>
#include "UnitSLList.h"
#include "UnitComputers.h"
#include "UnitTariffs.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
#include "UnitSync.h"
//---------------------------------------------------------------------------
#define MAX_TimeShift   20      // ������������ ����������� ������� ������� � ������� (������)
//---------------------------------------------------------------------------
// ��������� ����������
#define mcsWtTLK        1       // ����� ��� ��������� TLK
#define mcsFree         2       // ��������
#define mcsWork         4       // � ������, ����� �������������
#define mcsFine         8       // � ������, �� ���������� (���. ����)
#define mcsLock         16      // � ������, �� ������� (���. ����)
#define mcsPause        32      // ��������� ������ � ����� ����������� (���. ����)
#define mcsOpen         64      // ��������� ������ ��� ������������ � ����� ����������� (���. ����)
//#define mcsService      128     // ��������� ����� ��� ������������ (���. ����)
//---------------------------------------------------------------------------
// ������� ��� ����������
#define mccPowerOn      1       // �������� (WOL)
#define mccReboot       2       // �������������
#define mccShutdown     4       // ���������
//---------------------------------------------------------------------------
// ��������� �������� ������ � �����������
#define mnsPresent      1       // ��������� ��������� �������� �� ������� �� ����
#define mnsSyncNeed     2       // ��������� ������������� ������ ������ ������ � ��������
#define mnsSyncData     4       // ���� ����� ������ ��� �������� �������� ����������
#define mnsLock         8       //
#define mnsNeedSave     16      //
//---------------------------------------------------------------------------
// ��������� � ��������� ���������� � ������� ���������� ������� (����������� ��� GUI)
#define mdcNumber       1       // ����� ����������
#define mdcState        2       // ����� ������
#define mdcTariff       4       // ����� (������)
#define mdcWorkTime     8       // ����� ������
#define mdcFineTime     16      // ����� ������
#define mdcCommands     32      // �������������� �������
#define mdcNetState     64      // ��������� ���� (������)
//#define mdcServiceState     128         //
#define mdcPrograms     256     // ��������� ������ ��������� �������� (������)
#define mdcOptions      512     // ���������� ��������� ������� (������)
#define mdcAll (mdcNumber|mdcState|mdcTariff|mdcWorkTime|mdcFineTime|mdcCommands|mdcNetState)
//#define mdcAllCl (mdcNumber|mdcState|mdcWorkTime|mdcFineTime|mdcPrograms|mdcCommands|mdcOptions)
//---------------------------------------------------------------------------
struct MStateData
{
    int Number;                 // ����� ����������
    unsigned State;             // ��������� ���������� (������ ������)
    unsigned TariffID;          // ID-����� ������, �� �������� �������� ���������
    __int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
    int SizeWorkTime;           // �� ����� ����� ������� ��������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;           // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
};

struct MStateInfo
{
    int Number;                 // ����� ����������
    unsigned State;             // ��������� ���������� (������ ������)
    unsigned TariffID;          // ID-����� ������, �� �������� �������� ���������
    int WorkTime;               // �� ����� ����� ��� ������� ��������� (� �������)
    int ToEndWork;              // ���������� ����� ������ (� �������)
    int EndWorkTime;            // �� ������� ��� ����� ��������� ������ (� ������� � ������ �����)
    int FineTime;               // ����� �������� � ������ ������ (� �������)
    int ToEndFine;              // ���������� ����� ������ (� �������)
    int EndFineTime;            // �� ������� ��� ����� ��������� ������ (� ������� � ������ �����)
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� ������� ��� ����������
    unsigned NetState;          // ��������� ����
    unsigned Changes;           // ��������� ������ ��� �������� �� ������� �� ���������� �������
};
//---------------------------------------------------------------------------
class MState:public MSLListItem
{
private:
    CRITICAL_SECTION CS_Main;   // ������ ��� ������������� ������� ������� � ������
    __int64 SystemTime;         // ��������� �����, ������������ ��� ���� ��������

    int Number;                 // ����� ����������, � ������� ������������� ���������
    unsigned State;             // ��������� ���������� (������ ������)
    unsigned TariffID;          // ID-����� ������, �� �������� �������� ���������
    __int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
    int SizeWorkTime;           // �� ����� ����� ������� ��������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;           // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� ������� ��� ����������
    unsigned CmdsToReset;       // ������� ��������� ������ �� ��������� ����������
    unsigned NetState;          // ��������� ����
    unsigned Changes;           // ��������� ������ ��� �������� �� ������� �� ���������� �������

    // ������� ������������� ������� ������ ������� � ������
    inline void Lock() { ::EnterCriticalSection(&CS_Main); }
    inline void UnLock() { ::LeaveCriticalSection(&CS_Main); }

    // ������� �������� ��������� ������� ������ � ������� ������
    bool ControlWorkTime();
    bool ControlFineTime();

public:
    bool Copy(MListItem *SrcItem_);

    // �������, ����������� � �����������
    bool CmdRun(MTariff *Tariff_, MRunTime *Time_, bool Check_);
    bool CmdFine(int FineSize_, bool Check_);
    bool CmdExchange(MState *State_, bool Check_);
    bool CmdLock(bool Apply_, bool Check_);
    bool CmdPause(bool Apply_, bool Check_);
    bool CmdOpen(bool Apply_, bool Check_);
    bool CmdWtLocker(bool Apply_, bool Check_);
    bool CmdPowerOn(bool Check_);
    bool CmdReboot(bool Check_);
    bool CmdShutdown(bool Check_);

    // ��������������� �������
    void RunParam(MRunTime *RunTime_);
    void StateInfo(MStateInfo *Info_);
    bool Timer(__int64 SystemTime_);

    // ���������� � �������� ������ � ������� �������
    int Associated();
    void Associate(int Number_);

    // �������� ��� �������� ����������
    bool NetBegin();
    bool NetEnd();

    bool NetSyncNewData();                  // ���� �� ��� ��������� ������� ?
    void NetSyncData(MSyncData *Data_);     // ������ ������ ��� �������� �������
    void NetSyncExecuted(bool Executed_);   //

    bool NetPwrOnNeed();
    void NetPwrOnExecuted();

    // ��������� �����
    void GetStateData(MStateData *Data_);
    void SetStateData(MStateData *Data_);

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MState();
    ~MState();
};
//---------------------------------------------------------------------------
class MStates:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MState; }
    void operator_delete(MListItem *DelItem_) { delete (MState*)DelItem_; }
private:
    CRITICAL_SECTION CS_File;   // ������ ��� ������������� ������� ������� � ������

    // ������� ������������� ������� ������ ������� � ������
    inline void Lock() { ::EnterCriticalSection(&CS_File); }
    inline void UnLock() { ::LeaveCriticalSection(&CS_File); }
public:
    // ��������������� �������
    MState *Search(int Number_);
    bool Update(MComputers *Computers_);
    bool Timer(__int64 SystemTime_);

    // �������������� ������� ���������� ������ � ����
    bool Save();

    MStates() { constructor(); ::InitializeCriticalSection(&CS_File); }
    ~MStates() { ::DeleteCriticalSection(&CS_File); destructor(); }
};
//---------------------------------------------------------------------------
class MStateCl:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return NULL; }
    void operator_delete(MListItem *DelItem_) { }
private:
    HKEY OptKey;                //
    char *OptPath;              //
    char *OptValue;             //
    char *PrgFile;              // ���� ��� �������� ������ ��������
    unsigned AutoLockTime;      // ����� ���������� ����� � �������� �� ��������������

    CRITICAL_SECTION CS_Main;   // ������ ��� ������������� ������� ������� � ������
    __int64 SystemTime;         // ��������� �����, ������������ ��� ���� ��������

    int Number;                 // ����� ����������
    unsigned State;             // ��������� ���������� (������ ������)
    __int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
    int SizeWorkTime;           // �� ����� ����� ������� ��������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;           // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
    __int64 LastSyncTime;       // ����� ���������� ����������� ������ �� ���������� ������
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� ������� ��� ����������
    unsigned Changes;           // ��������� ������ ��� �������� �� ������� �� ���������� �������

    // ������� ������������� ������� ������ ������� � ������
    inline void Lock() { ::EnterCriticalSection(&CS_Main); }
    inline void UnLock() { ::LeaveCriticalSection(&CS_Main); }

    // ������� �������� ��������� ������� ������ � ������� ������
    bool ControlWorkTime();
    bool ControlFineTime();
    bool ControlSyncTime();

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

public:
    // ��������������� �������
    void StateInfo(MStateInfo *Info_);
    bool GetOptions(MClOptions *Options_);
    bool GetGames(MGames *Games_);
    bool Timer(__int64 SystemTime_);
    void CmdReboot();                       // ����� �� ?

    // �������� ��� �������� ����������
    bool NewSyncData(MSyncData *Data_);
    bool NewGames(MGames *Games_);
    bool NewOptions(MClOptions *Options_);

    // ������������ ��������� ��� ��������/���������� � ������� �������� ������ MSLList
    bool SetDefault(HKEY RegKey_, char *RegPath_, char *RegValue_,
        HKEY OptKey_, char *OptPath_, char *OptValue_, char *PrgFile_, unsigned RegCode_);

    MStateCl();
    ~MStateCl();
};
//---------------------------------------------------------------------------
#endif

