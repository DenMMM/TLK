//---------------------------------------------------------------------------
#ifndef UnitLogH
#define UnitLogH
//---------------------------------------------------------------------------
#include <string>
#include <cstdint>

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
	std::int64_t SystemTime;	// ��������� ����� ��� ������������� � ����������� ��������� �����������
	std::int64_t BeginTime;		// �����, ����� ��� ��� �����
	std::wstring Directory;		// ���������� ��� �������� ������ �����
	bool Opened;				// ��� ��� ������� ������/�����
//    bool Transaction;			// ����-�� ������� ����� ������� �� "������" �� ����
	MLogRecords Records;		// ����� ��� �������
	std::uint32_t User;			// ������������, ��������� ����� ���������
	DWORD LastError;

    // �������� � ����� �������/������
	void AddSimpleEvent(MLogRecords::ItemType Type_);
	void AddStatesData(const MStates &States_);
	void AddTariffsData(const MTariffs &Tariffs_);
	void AddFinesData(const MFines &Fines_);
	void AddUsersData(const MUsers &Users_);
	// �������� � ��� ������� �������/�������
	bool AddEvent(MLogRecords::ItemType Type_);
	bool AddMode(MLogRecords::ItemType Type_, char Number_, bool Apply_);
	bool AddCmd(MLogRecords::ItemType Type_, char Number_);
    //
    bool Rename() const;

public:
    // ������ � ������ ����
    bool Begin(
		const MShellState &ShellState_,
		const MStates &States_,
		const MTariffs &Tariffs_,
		const MFines &Fines_,
		const MUsers &Users_);				// ������� ���� � ��������� �������������� ������
	bool Open();							// ������� ������������ ����
	bool End();								// ��������� ���������� �����
	bool CheckPeriod(int Period_) const;	// ��������� �� ���� �� ������� ���� ����
	std::uint32_t LastUser() const;			// ������� ID ���������� ������������� ������������

	// ��������� ������
	bool AddStart(
		const MShellState &ShellState_,
		const MStates &States_,
		const MTariffs &Tariffs_,
		const MFines &Fines_,
		const MUsers &Users_);				// ������ ������ ����������
	bool AddStop();							// ��������� ������ ����������

	// ��������� ������ ����������
	bool AddConfig(bool Open_);             	// ��������� �������/�������
	bool AddComputers(const MStates &States_);	// ������� ������ �����������
	bool AddTariffs(const MTariffs &Tariffs_);	// ������� ������ �������
	bool AddFines(const MFines &Fines_);		// ������� ������ �������
	bool AddUsers(const MUsers &Users_);		// ������� ������ �������������
	bool AddOptions();							// �������� ����� ���������

	// ������������
	bool AddLogIn(std::uint32_t UserID_);	// ������������ ����� �����
	bool AddLogOut();						// ������������ �������� �����

	// ���������� ������������ ��������� ���� ������
//    bool Transact();
//    bool Apply();
	// �������, ����������� � �����������
	bool AddRun(const MTariffRunTimesItem &Time_);
	bool AddFine(std::int8_t Number_, std::uint32_t FineID_, std::int16_t Time_);
	bool AddExchange(std::int8_t From_, std::int8_t To_);
	bool AddLock(std::int8_t Number_, bool Apply_);
	bool AddPause(std::int8_t Number_, bool Apply_);
	bool AddOpen(std::int8_t Number_, bool Apply_);
	bool AddPowerOn(std::int8_t Number_);
	bool AddReboot(std::int8_t Number_);
	bool AddShutdown(std::int8_t Number_);

	void SetDefault(const std::wstring &Dir_, unsigned Code_);
	void Timer(std::int64_t SystemTime_);
	DWORD gLastErr() const { return Records.gLastErr(); }

	MLog();
	MLog(const MLog&) = delete;
	MLog(MLog&&) = delete;
	MLog& operator=(const MLog&) = delete;
	MLog& operator=(MLog&&) = delete;
	~MLog() = default;
};
//---------------------------------------------------------------------------
#endif

