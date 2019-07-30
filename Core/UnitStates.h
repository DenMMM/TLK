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
#define mcsFree         1       // ��������
#define mcsWork         2       // � ������, ����� �������������
#define mcsFine         4       // � ������, �� ���������� (���. ����)
#define mcsLock         8       // � ������, �� ������� (���. ����)
#define mcsPause        16      // ��������� ������ � ����� ����������� (���. ����)
#define mcsOpen         32      // ��������� ������ ��� ������������ � ����� ����������� (���. ����)
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
    char Number;                // ����� ����������
    unsigned State;             // ��������� ���������� (������ ������)
    unsigned TariffID;          // ID-����� ������, �� �������� �������� ���������
    __int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
    short SizeWorkTime;         // �� ����� ����� ������� ��������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    short SizeFineTime;         // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
};

struct MStateInfo
{
    char Number;                // ����� ����������
    unsigned State;             // ��������� ���������� (������ ������)
    unsigned TariffID;          // ID-����� ������, �� �������� �������� ���������
    short WorkTime;             // �� ����� ����� ��� ������� ��������� (� �������)
    short ToEndWork;            // ���������� ����� ������ (� �������)
    short EndWorkTime;          // �� ������� ��� ����� ��������� ������ (� ������� � ������ �����)
    short FineTime;             // ����� �������� � ������ ������ (� �������)
    short ToEndFine;            // ���������� ����� ������ (� �������)
    short EndFineTime;          // �� ������� ��� ����� ��������� ������ (� ������� � ������ �����)
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� ������� ��� ����������
    unsigned NetState;          // ��������� ����
    unsigned Changes;           // ��������� ������ ��� �������� �� ������� �� ���������� �������
};
//---------------------------------------------------------------------------
class MState:public MSLListItem
{
private:
    void Copy(const MListItem *SrcItem_) {}
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

    mutable CRITICAL_SECTION CS_Main;   // ������ ��� ������������� ������� ������� � ������
    __int64 SystemTime;         // ��������� �����, ������������ ��� ���� ��������

    char Number;                // ����� ����������, � ������� ������������� ���������
    unsigned State;             // ��������� ���������� (������ ������)
    unsigned TariffID;          // ID-����� ������, �� �������� �������� ���������
    __int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
    short SizeWorkTime;         // �� ����� ����� ������� ��������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    short SizeFineTime;         // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� ������� ��� ����������
    unsigned CmdsToReset;       // ������� ��������� ������ �� ��������� ����������
    unsigned NetState;          // ��������� ����
    unsigned Changes;           // ��������� ������ ��� �������� �� ������� �� ���������� �������

    // ������� ������������� ������� ������ ������� � ������
    void Lock() const { ::EnterCriticalSection(&CS_Main); }
    void UnLock() const { ::LeaveCriticalSection(&CS_Main); }

    // ������� �������� ��������� ������� ������ � ������� ������
    bool ControlWorkTime();
    bool ControlFineTime();

public:
    // �������, ����������� � �����������
    bool CmdRun(MTariff *Tariff_, MRunTime *Time_, bool Check_);
    bool CmdFine(short FineSize_, bool Check_);
    bool CmdExchange(MState *State_, bool Check_);
    bool CmdLock(bool Apply_, bool Check_);
    bool CmdPause(bool Apply_, bool Check_);
    bool CmdOpen(bool Apply_, bool Check_);
    bool CmdPowerOn(bool Check_);
    bool CmdReboot(bool Check_);
    bool CmdShutdown(bool Check_);

    // ��������������� �������
    void RunParam(MRunTime *RunTime_);
    void StateInfo(MStateInfo *Info_);
    bool Timer(__int64 SystemTime_);

    // ���������� � �������� ������ � ������� �������
    int MState::Associated() const { return Number; }
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

    MState();
    ~MState();
};
//---------------------------------------------------------------------------
class MStates:public MSLList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MState; }
    void item_del(MListItem *Item_) const { delete (MState*)Item_; }

    CRITICAL_SECTION CS_File;   // ������ ��� �������������

    // ������� ������������� ������� ������ ������� � ������
    void Lock() { ::EnterCriticalSection(&CS_File); }
    void UnLock() { ::LeaveCriticalSection(&CS_File); }

public:
    // ��������������� �������
    MState *Search(int Number_);
    bool Update(MComputers *Computers_);
    bool Timer(__int64 SystemTime_);

    // �������������� ������� ���������� ������ � ����
    bool Save();

    MStates() { ::InitializeCriticalSection(&CS_File); }
    ~MStates() { ::DeleteCriticalSection(&CS_File); Clear(); }
};
//---------------------------------------------------------------------------
class MStateCl:public MSLList
{
private:
    // ��������, �.�. ������ �� ����� ���� ��� - ������ "���������"
    MListItem *item_new(unsigned char TypeID_) const { return NULL; }
    void item_del(MListItem *Item_) const { return; }

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);
    
    HKEY OptKey;                //
    char *OptPath;              //
    char *OptValue;             //
    char *PrgFile;              // ���� ��� �������� ������ ��������
    unsigned AutoLockTime;      // ����� ���������� ����� � �������� �� ��������������

    mutable CRITICAL_SECTION CS_Main;   // ������ ��� ������������� ������� ������� � ������
    __int64 SystemTime;         // ��������� �����, ������������ ��� ���� ��������

    char Number;                // ����� ����������
    unsigned State;             // ��������� ���������� (������ ������)
    __int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
    short SizeWorkTime;         // �� ����� ����� ������� ��������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    short SizeFineTime;         // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
    __int64 LastSyncTime;       // ����� ���������� ����������� ������ �� ���������� ������
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� ������� ��� ����������
    unsigned Changes;           // ��������� ������ ��� �������� �� ������� �� ���������� �������

    // ������� ������������� ������� ������ ������� � ������
    void Lock() const { ::EnterCriticalSection(&CS_Main); }
    void UnLock() const { ::LeaveCriticalSection(&CS_Main); }

    // ������� �������� ��������� ������� ������ � ������� ������
    bool ControlWorkTime();
    bool ControlFineTime();
    bool ControlSyncTime();

public:
    // ��������������� �������
    void StateInfo(MStateInfo *Info_);
    bool GetOptions(MClOptions *Options_);
    bool GetGames(MGames *Games_);
    bool Timer(__int64 SystemTime_);
    void CmdReboot();                       // ����� �� ?
    void CmdShutdown();

    // �������� ��� �������� ����������
    bool NewSyncData(MSyncData *Data_);
    bool NewGames(MGames *Games_);
    bool NewOptions(MClOptions *Options_);

    // ������������ ��������� ��� ��������/���������� � ������� �������� ������ MSLList
    void SetDefault(HKEY RegKey_, char *RegPath_, char *RegValue_,
        HKEY OptKey_, char *OptPath_, char *OptValue_, char *PrgFile_, unsigned RegCode_);

    MStateCl();
    ~MStateCl();
};
//---------------------------------------------------------------------------
#endif

