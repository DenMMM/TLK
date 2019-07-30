//---------------------------------------------------------------------------
#ifndef UnitStateH
#define UnitStateH
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#define MAX_TimeShift 20        // ���������� ���������� ������� �� ������� ������������ ���������� (������)
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
// Main Computer Command
#define mccPowerOn ((unsigned int)1)    // ��������� ������ ����������
#define mccReboot ((unsigned int)2)     // ��������� ������ ���������������
#define mccShutdown ((unsigned int)4)   // ��������� ������ �����������
//---------------------------------------------------------------------------
// Main Data Change
#define mdcNumber ((unsigned int)1)     // ��������� ����� ����������
#define mdcState ((unsigned int)2)      // ��������� ����� ������
#define mdcWorkTime ((unsigned int)4)   // ���������� ����� ������
#define mdcFineTime ((unsigned int)8)   // ���������� ����� ������
#define mdcPrograms ((unsigned int)16)  // ��������� ������ ��������� ��������
#define mdcCommands ((unsigned int)32)  // ���������� �������������� �������
#define mdcOptions  ((unsigned int)64)  // ���������� ��������� �������
#define mdcAll (mdcNumber|mdcState|mdcWorkTime|mdcFineTime|mdcPrograms|mdcCommands|mdcOptions)
//---------------------------------------------------------------------------
class MState;
struct MSyncData;
struct MStateInfo;
//---------------------------------------------------------------------------
class MState:private MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return NULL; }
    void operator_delete(MListItem *DelItem_) { }
private:
    HKEY OptKey;                //
    char *OptPath;              //
    char *OptValue;             //
    unsigned OptCode;           // ��� ��� ���������� ��������
    char *PrgFile;              // ���� ��� �������� ������ ��������
    unsigned PrgCode;           // ��� ��� ���������� ������ ��������
    unsigned AutoLockTime;

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
    void Lock() { ::EnterCriticalSection(&CS_Main); }
    void UnLock() { ::LeaveCriticalSection(&CS_Main); }

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
    void CmdReboot();

    // �������� ��� �������� ����������
    bool NewSyncData(MSyncData *Data_);
    bool NewGames(MGames *Games_);
    bool NewOptions(MClOptions *Options_);

    // ������������ ��������� ��� ��������/���������� � ������� �������� ������ MSLList
    bool SetDefault(HKEY RegKey_, char *RegPath_, char *RegValue_, unsigned RegCode_,
        HKEY OptKey_, char *OptPath_, char *OptValue_, unsigned OptCode_,
        char *PrgFile_, unsigned PrgCode_);

    bool Save() { return MSLList::Store(); }
    bool Load() { return MSLList::Query(); }

    MState();
    ~MState();
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
    int WorkTime;               // �� ����� ����� ��� ������� ��������� (� �������)
    int ToEndWork;              // ���������� ����� ������ (� �������)
    int EndWorkTime;            // �� ������� ��� ����� ��������� ������ (� ������� � ������ �����)
    int FineTime;               // ����� �������� � ������ ������ (� �������)
    int ToEndFine;              // ���������� ����� ������ (� �������)
    int EndFineTime;            // �� ������� ��� ����� ��������� ������ (� ������� � ������ �����)
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� ������� ��� ����������
    unsigned Changes;           // ��������� ������ ��� �������� �� ������� �� ���������� �������
};
//---------------------------------------------------------------------------
#endif

