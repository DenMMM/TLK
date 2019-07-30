//---------------------------------------------------------------------------
#ifndef UnitStateH
#define UnitStateH
//---------------------------------------------------------------------------
#define MaxPingTime ((int)300)  // ����� �������� ����� � ��������� ������� �� ���������� �������
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
#define mgp1 ((unsigned int)1)
#define mgp2 ((unsigned int)2)
#define mgp3 ((unsigned int)4)
#define mgp4 ((unsigned int)8)
#define mgp5 ((unsigned int)16)
#define mgpAll (mgp1|mgp2|mgp3|mgp4|mgp5)
#define mgpNone ((unsigned int)0)
//---------------------------------------------------------------------------
class MState;
//---------------------------------------------------------------------------
class MState
{
private:
    // ������������ ��� ������������� ������� ������ �������
    LPCRITICAL_SECTION CS_Main;  // ����� ������, ����� � �.�.
    LPCRITICAL_SECTION CS_Games;  // ������ ���
public:
    int Number;  // ����� ���������� � �����
    unsigned int State;  // ��������� ����������
    double StartWorkTime;  // ����� ������� ���������� � ������ (���������� �����)
    int SizeWorkTime;  // �� ����� ����� ������� ��������� (� �������)
    double StartFineTime;  // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;  // ����� �������� � ������ ������ (� �������)
    double StopTimerTime;  // ����� ��������� ������� ������� (���������� �����)
    unsigned int GamesPages;
    unsigned int StateVer;

    int NotPingTime;
    bool NeedUpdate;

    // ������� �������� ��������� ������� ������ � ������� ������
    bool ControlWorkTime();
    bool ControlFineTime();

    bool ControlPingTime();

    // ������� ������������� ������� ������ ������� � ������
    void InitCriticalSection();
    void DelCriticalSection();
    void Lock() { ::EnterCriticalSection(CS_Main); }
    void UnLock() { ::LeaveCriticalSection(CS_Main); }
    void LockGames() { ::EnterCriticalSection(CS_Games); }
    void UnLockGames() { ::LeaveCriticalSection(CS_Games); }

    MState();
    ~MState();
};
//---------------------------------------------------------------------------
#endif

