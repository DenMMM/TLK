//---------------------------------------------------------------------------
#ifndef UnitLogH
#define UnitLogH
//---------------------------------------------------------------------------
#include <string>
#include "UnitLogRecords.h"
#include "UnitShellState.h"
#include "UnitStates.h"
#include "UnitTariffs.h"
#include "UnitFines.h"
#include "UnitUsers.h"
//---------------------------------------------------------------------------
// ����������� ������ ������� ����� ����
#define mlpDay          1   // ��������
#define mlpWeek         2   // ���������
#define mlpMonth        3   // �� �����
//---------------------------------------------------------------------------
class MLog
{
private:
    __int64 SystemTime;     // ��������� ����� ��� ������������� � ����������� ��������� �����������
    __int64 BeginTime;      // �����, ����� ��� ��� �����
    std::string Directory;  // ���������� ��� �������� ������ �����
    bool Opened;            // ��� ��� ������� ������/�����
//    bool Transaction;       // ����-�� ������� ����� ������� �� "������" �� ����
    MLogRecords Records;    // ����� ��� �������
    unsigned User;          // ������������, ��������� ����� ���������
    DWORD LastError;

    // �������� � ����� �������/������
    void AddSimpleEvent(unsigned char Type_);
    void AddStatesData(MStates *States_);
    void AddTariffsData(MTariffs *Tariffs_);
    void AddFinesData(MFines *Fines_);
    void AddUsersData(MUsers *Users_);
    // �������� � ��� ������� �������/�������
    bool AddEvent(unsigned char Type_);
    bool AddMode(unsigned char Type_, char Number_, bool Apply_);
    bool AddCmd(unsigned char Type_, char Number_);
    //
    bool Rename() const;

public:
    // ������ � ������ ����
    bool Begin(
        MShellState *ShellState_,
        MStates *States_,
        MTariffs *Tariffs_,
        MFines *Fines_,
        MUsers *Users_);                    // ������� ���� � ��������� �������������� ������
    bool Open();                            // ������� ������������ ����
    bool End();                             // ��������� ���������� �����
    bool CheckPeriod(int Period_) const;    // ��������� �� ���� �� ������� ���� ����
    unsigned LastUser() const;              // ������� ID ���������� ������������� ������������

    // ��������� ������
    bool AddStart(
        MShellState *ShellState_,
        MStates *States_,
        MTariffs *Tariffs_,
        MFines *Fines_,
        MUsers *Users_);                    // ������ ������ ����������
    bool AddStop();                         // ��������� ������ ����������

    // ��������� ������ ����������
    bool AddConfig(bool Open_);             // ��������� �������/�������
    bool AddComputers(MStates *States_);    // ������� ������ �����������
    bool AddTariffs(MTariffs *Tariffs_);    // ������� ������ �������
    bool AddFines(MFines *Fines_);          // ������� ������ �������
    bool AddUsers(MUsers *Users_);          // ������� ������ �������������
    bool AddOptions();                      // �������� ����� ���������

    // ������������
    bool AddLogIn(unsigned UserID_);        // ������������ ����� �����
    bool AddLogOut();                       // ������������ �������� �����

    // ���������� ������������ ��������� ���� ������
//    bool Transact();        
//    bool Apply();
    // �������, ����������� � �����������
    bool AddRun(MRunTime *Time_);
    bool AddFine(char Number_, unsigned FineID_, short Time_);
    bool AddExchange(char From_, char To_);
    bool AddLock(char Number_, bool Apply_);
    bool AddPause(char Number_, bool Apply_);
    bool AddOpen(char Number_, bool Apply_);
    bool AddPowerOn(char Number_);
    bool AddReboot(char Number_);
    bool AddShutdown(char Number_);

    void SetDefault(const std::string &Dir_, unsigned Code_);
    void Timer(__int64 SystemTime_);
    DWORD gLastErr() const { return Records.gLastErr(); }

    MLog();
};
//---------------------------------------------------------------------------
#endif

