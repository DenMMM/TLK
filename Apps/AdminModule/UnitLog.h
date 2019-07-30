//---------------------------------------------------------------------------
#ifndef UnitLogH
#define UnitLogH
//---------------------------------------------------------------------------
#include "UnitLogRecords.h"
#include "UnitShellState.h"
#include "UnitStates.h"
#include "UnitTariffs.h"
#include "UnitFines.h"
#include "UnitUsers.h"
//---------------------------------------------------------------------------
// Main Log Period
#define mlpDay          1   // �������� ���
#define mlpWeek         2   // ��������� ���
#define mlpMonth        3   // �������� ���
#define mlpQuarter      4   // ����������� ���
#define mlpYear         5   // ������� ���
//---------------------------------------------------------------------------
class MLog
{
private:
    __int64 SystemTime;     // ��������� ����� ��� ������������� � ����������� ��������� �����������
    __int64 BeginTime;      // ����� ������ ���������� ����� ����
    char *Directory;        // ���������� ��� �������� ������ �����
    MLogRecords Records;    // ������ ����, ��� �� ����������� � �����
    unsigned User;          // ������������, ��� ����� ���� �������

    bool AddSimpleEvent(unsigned Type_);
    bool AddEvent(unsigned Type_);
    bool AddMode(unsigned Type_, int Number_, bool Apply_);
    bool AddCmd(unsigned Type_, int Number_);
    bool AddStatesData(MStates *States_);
    bool AddTariffsData(MTariffs *Tariffs_);
    bool AddFinesData(MFines *Fines_);
    bool AddUsersData(MUsers *Users_);
public:
    void Timer(__int64 SystemTime_);
    bool SetDefault(char *Dir_, unsigned Code_);

    // ������ � ������ ����
    bool Begin(
        MShellState *ShellState_,
        MStates *States_,
        MTariffs *Tariffs_,
        MFines *Fines_,
        MUsers *Users_);    // ������� ���� � ��������� �������������� ������
    bool Open();            // ������� ������������ ����
    unsigned LastUser();
    bool CheckPeriod(int Period_);     // ��������� �� ���� �� ������� ���� ����
    bool End();             // ��������� ���������� �����

    // ��������� ������
    bool AddStart(
        MShellState *ShellState_,
        MStates *States_,
        MTariffs *Tariffs_,
        MFines *Fines_,
        MUsers *Users_);        // ������ ���������� ������
    bool AddWork();             // ��������� ������ ��������
    bool AddStop();             // ��������� ���������� ������

    // ��������� ���������� ������
    bool AddConfig(bool Open_);             // ��������� �������/�������
    bool AddComputers(MStates *States_);    // ������� ������ �����������
    bool AddTariffs(MTariffs *Tariffs_);    // ������� ������ �������
    bool AddFines(MFines *Fines_);          // ������� ������ �������
    bool AddUsers(MUsers *Users_);          // ������� ������ �������������
    bool AddOptions();                      // �������� ����� ���������

    // ������������
    bool AddLogIn(unsigned ID_);            // ������������ ����� ������
    bool AddLogOut();                       // ������������ �������� ������

    // �������, ����������� � �����������
    bool AddRun(MRunTime *Time_);
    bool AddFine(int Number_, unsigned Fine_, int Time_);
    bool AddExchange(int From_, int To_);
    bool AddLock(int Number_, bool Apply_);
    bool AddPause(int Number_, bool Apply_);
    bool AddOpen(int Number_, bool Apply_);
    bool AddWtLocker(int Number_, bool Apply_);
    bool AddPowerOn(int Number_);
    bool AddReboot(int Number_);
    bool AddShutdown(int Number_);

    MLog();
    ~MLog();
};
//---------------------------------------------------------------------------
#endif

