//---------------------------------------------------------------------------
#ifndef UnitStatesH
#define UnitStatesH
//---------------------------------------------------------------------------
#include <windows.h>
#include "UnitSLList.h"
#include "UnitComputers.h"
#include "UnitTariffs.h"
//---------------------------------------------------------------------------
// Main Computer State
#define mcsWtLocker ((unsigned int)1)   // ����� ��� ��������� TimeLocker'�
#define mcsFree ((unsigned int)2)       // ��������
#define mcsWork ((unsigned int)4)       // � ������
#define mcsFine ((unsigned int)8)       // � ������, �� ���������� (�������������� ����)
#define mcsLock ((unsigned int)16)      // � ������, �� ������� (�������������� ����)
#define mcsPause ((unsigned int)32)     // ��������� ������ � ����� ����������� (�������������� ����)
#define mcsOpen ((unsigned int)64)      // ��������� ������ ��� ������������ � ����� ����������� (�������������� ����)
//#define mcsService ((unsigned int)128)  // ��������� ����� ��� ������������ (�������������� ����)
//---------------------------------------------------------------------------
// Main Computer Command
#define mccPowerOn ((unsigned int)1)    // ��������� ������ ����������
#define mccReboot ((unsigned int)2)     // ��������� ������ ���������������
#define mccShutdown ((unsigned int)4)   // ��������� ������ �����������
//---------------------------------------------------------------------------
// Main Net State
#define mnsPresent ((unsigned int)1)        // ��������� ��������� �������� �� ������� �� ����
#define mnsSyncNeed ((unsigned int)2)       // ��������� ������������� ������ ������ ������ � ��������
#define mnsSyncData ((unsigned int)4)       // ���� ����� ������ ��� �������� �������� ����������
#define mnsLock ((unsigned int)8)           //
#define mnsNeedSave ((unsigned int)16)      //
//---------------------------------------------------------------------------
// Main Data Change
#define mdcNumber ((unsigned int)1)     // ��������� ����� ���������� (��������� ������������ � ������ �����������)
#define mdcState ((unsigned int)2)      // ��������� ����� ������
#define mdcTariff ((unsigned int)4)     // ��������� �����
#define mdcWorkTime ((unsigned int)8)   // ���������� ����� ������
#define mdcFineTime ((unsigned int)16)  // ���������� ����� ������
#define mdcCommands ((unsigned int)32)  // ���������� �������������� �������
#define mdcNetState ((unsigned int)64)  // ���������� ��������� ����
#define mdcAll (mdcNumber|mdcState|mdcTariff|mdcWorkTime|mdcFineTime|mdcCommands|mdcNetState)
//#define mdcServiceState ((unsigned int)64)  //
//---------------------------------------------------------------------------
class MState;
class MStates;
struct MSyncData;
struct MStateInfo;
struct MStateData;
//---------------------------------------------------------------------------
class MState:public MSLListItem
{
public:
    bool Copy(MListItem *SrcItem_);
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
    void Lock() { ::EnterCriticalSection(&CS_Main); }
    void UnLock() { ::LeaveCriticalSection(&CS_Main); }

    // ������� �������� ��������� ������� ������ � ������� ������
    bool ControlWorkTime();
    bool ControlFineTime();

public:
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

    bool NetSyncNewData();
    void NetSyncData(MSyncData *Data_);
    void NetSyncExecuted(bool Executed_);

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
    void Lock() { ::EnterCriticalSection(&CS_File); }
    void UnLock() { ::LeaveCriticalSection(&CS_File); }
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
struct MSyncData
{
    __int64 SystemTime;         // ��������� �����, ������������ ��� ���� ��������
    int Number;                 // ����� ����������, � ������� ������������� ���������
    unsigned State;             // ��������� ���������� (������ ������)
    __int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
    int SizeWorkTime;           // �� ����� ����� ������� ��������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;           // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� ������� ��� ����������
};
//---------------------------------------------------------------------------
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
    unsigned Commands;          // �������������� ������� ��� ����������
    unsigned NetState;          // ��������� ����
    unsigned Changes;           // ��������� ������ ��� �������� �� ������� �� ���������� �������
};
//---------------------------------------------------------------------------
struct MStateData
{
    int Number;                 // ����� ����������
    unsigned int State;         // ��������� ���������� (������ ������)
    unsigned TariffID;          // ID-����� ������, �� �������� �������� ���������
    __int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
    int SizeWorkTime;           // �� ����� ����� ������� ��������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;           // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
};
//---------------------------------------------------------------------------
#endif

