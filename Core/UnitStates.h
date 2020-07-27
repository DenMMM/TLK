//---------------------------------------------------------------------------
#ifndef UnitStatesH
#define UnitStatesH
//---------------------------------------------------------------------------
#include <string>
#include <mutex>
#include <cstdint>
#include <winsock2.h>
//#include <windows.h>

#include "UnitSLList.h"
#include "UnitComputers.h"
#include "UnitTariffs.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
#include "UnitSyncMsgs.h"
//---------------------------------------------------------------------------
struct MStatesInfo;
class MStatesItem;
class MStates;
class MStateCl;
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
struct MStatesInfo
{
	std::uint8_t Number;		// ����� ����������
	std::uint32_t State;		// ��������� ���������� (������ ������)
	std::uint32_t TariffID;		// ID-����� ������, �� �������� �������� ���������
	std::int16_t WorkTime;		// �� ����� ����� ��� ������� ��������� (� �������)
	std::int16_t ToEndWork;		// ���������� ����� ������ (� �������)
	std::int16_t EndWorkTime;	// �� ������� ��� ����� ��������� ������ (� ������� � ������ �����)
	std::int16_t FineTime;		// ����� �������� � ������ ������ (� �������)
	std::int16_t ToEndFine;		// ���������� ����� ������ (� �������)
	std::int16_t EndFineTime;	// �� ������� ��� ����� ��������� ������ (� ������� � ������ �����)
	std::uint32_t Programs;		// ����� ��������, ����������� ��� �������
	std::uint32_t Commands;		// �������������� ������� ��� ����������
	std::uint32_t NetState;		// ��������� ����
	std::uint32_t Changes;		// ��������� ������ ��� �������� �� ������� �� ���������� �������
};
//---------------------------------------------------------------------------
class MStatesItem:
	public MSLListItemSimple <
		MSLListItem <MStates, MStatesItem>,
		MStatesItem>
{
public:
	// ������� ��������� ����������/�������� ������
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

private:
	mutable std::mutex mtxMain;	// ������ ��� ������������� ������� ������� � ������
	std::int64_t SystemTime;	// ��������� �����, ������������ ��� ���� ��������

	std::int8_t Number;			// ����� ����������, � ������� ������������� ���������
	std::uint32_t State;		// ��������� ���������� (������ ������)
	std::uint32_t TariffID;		// ID-����� ������, �� �������� �������� ���������
	std::int64_t StartWorkTime;	// ����� ������� ���������� � ������ (���������� �����)
	std::int16_t SizeWorkTime;	// �� ����� ����� ������� ��������� (� �������)
	std::int64_t StartFineTime;	// �����, ����� ��� �������� ����� (���������� �����)
	std::int16_t SizeFineTime;	// ����� �������� � ������ ������ (� �������)
	std::int64_t StopTimerTime;	// ����� ��������� ������� ������� (���������� �����)
	std::uint32_t Programs;		// ����� ��������, ����������� ��� �������
	std::uint32_t Commands;		// �������������� ������� ��� ����������
	std::uint32_t CmdsToReset;	// ������� ��������� ������ �� ��������� ����������
	std::uint32_t NetState;		// ��������� ����
	std::uint32_t Changes;		// ��������� ������ ��� �������� �� ������� �� ���������� �������

	// ������� �������� ��������� ������� ������ � ������� ������
	bool ControlWorkTime();
	bool ControlFineTime();

public:
	// �������, ����������� � �����������
	bool CmdRun(
		const MTariffsItem &Tariff_,
		const MTariffRunTimesItem &Time_,
		bool Check_);
	bool CmdFine(short FineSize_, bool Check_);
	bool CmdExchange(MStatesItem &State_, bool Check_);
	bool CmdLock(bool Apply_, bool Check_);
	bool CmdPause(bool Apply_, bool Check_);
	bool CmdOpen(bool Apply_, bool Check_);
	bool CmdPowerOn(bool Check_);
	bool CmdReboot(bool Check_);
	bool CmdShutdown(bool Check_);

	// ��������������� �������
	MTariffRunTimesItem GetRunParam() const;
	MStatesInfo GetInfo();
	bool Timer(std::int64_t SystemTime_);

	// ���������� � �������� ������ � ������� �������
	std::int8_t Associated() const { return Number; }
	void Associate(std::int8_t Number_);

	// �������� ��� �������� ����������
	bool NetBegin();
	bool NetEnd();

	bool NetSyncNewData();					// ���� �� ��� ��������� ������� ?
	MSyncData NetSyncData();				// ������ ������ ��� �������� �������
	void NetSyncExecuted(bool Executed_);	//

	bool NetPwrOnNeed();
	void NetPwrOnExecuted();

	// ��������� �����
	struct LogData
	{
		std::uint8_t Number;		// ����� ����������
		std::uint32_t State;		// ��������� ���������� (������ ������)
		std::uint32_t TariffID;		// ID-����� ������, �� �������� �������� ���������
		std::int64_t StartWorkTime;	// ����� ������� ���������� � ������ (���������� �����)
		std::int16_t SizeWorkTime;	// �� ����� ����� ������� ��������� (� �������)
		std::int64_t StartFineTime;	// �����, ����� ��� �������� ����� (���������� �����)
		std::int16_t SizeFineTime;	// ����� �������� � ������ ������ (� �������)
		std::int64_t StopTimerTime;	// ����� ��������� ������� ������� (���������� �����)
	};

	LogData gLogData() const
	{
		std::lock_guard lckObj(mtxMain);

		LogData ld;
		ld.Number=Number;
		ld.State=State;
		ld.TariffID=TariffID;
		ld.StartWorkTime=StartWorkTime;
		ld.SizeWorkTime=SizeWorkTime;
		ld.StartFineTime=StartFineTime;
		ld.SizeFineTime=SizeFineTime;
		ld.StopTimerTime=StopTimerTime;

		return ld;
	}

	void sFromLog(const LogData &Data_)
	{
		std::lock_guard lckObj(mtxMain);

		Number=Data_.Number;
		State=Data_.State;
		TariffID=Data_.TariffID;
		StartWorkTime=Data_.StartWorkTime;
		SizeWorkTime=Data_.SizeWorkTime;
		StartFineTime=Data_.StartFineTime;
		SizeFineTime=Data_.SizeFineTime;
		StopTimerTime=Data_.StopTimerTime;
		Changes=mdcAll;
	}

	MStatesItem():
		SystemTime(0),
		Number(0),
		State(0),
		TariffID(0),
		StartWorkTime(0),
		SizeWorkTime(0),
		StartFineTime(0),
		SizeFineTime(0),
		StopTimerTime(0),
		Programs(0),
		Commands(0),
		CmdsToReset(0),
		NetState(0),
		Changes(0)
	{
	}

	MStatesItem(const MStatesItem&) { throw; }
	MStatesItem(MStatesItem&&) { throw; }
	MStatesItem& operator=(const MStatesItem&) { throw; return *this; }
	MStatesItem& operator=(MStatesItem&&) { throw; return *this; }
};
//---------------------------------------------------------------------------
class MStates:
	public MSLListSimple <
		MSLList <MStates, MStatesItem>,
		MStatesItem>
{
private:
	std::mutex mtxFile;			// ������ ��� �������������

public:
	// ��������������� �������
	const_iterator Search(std::int8_t Number_) const;
	iterator Search(std::int8_t Number_)
	{
		const auto *const_this=this;
		return const_cast_iter(const_this->Search(Number_));
	}

	bool Update(const MComputers &Computers_);
	bool Timer(std::int64_t SystemTime_);

	// �������������� ������� ���������� ������ � ����
	bool Save();

	MStates() = default;
	MStates(const MStates&) = delete;
	MStates(MStates&&) = delete;
	MStates& operator=(const MStates&) = delete;
	MStates& operator=(MStates&&) = delete;
	~MStates() = default;
};
//---------------------------------------------------------------------------
class MStateClStub:
	public MSLListItem <MStateCl, MStateClStub> {};

class MStateCl:
	public MSLList <MStateCl, MStateClStub>     /// private ?
{
public:
	// ������� ��������� ����������/�������� ������
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

private:
	HKEY OptKey;				//
	std::wstring OptPath;		//
	std::wstring OptValue;		//
	std::wstring PrgFile;		// ���� ��� �������� ������ ��������
	std::uint32_t AutoLockTime;	// ����� ���������� ����� � �������� �� ��������������

	mutable std::mutex mtxMain;	// ������ ��� ������������� ������� ������� � ������
	std::int64_t SystemTime;	// ��������� �����, ������������ ��� ���� ��������

	std::uint8_t Number;		// ����� ����������
	std::uint32_t State;		// ��������� ���������� (������ ������)
	std::int64_t StartWorkTime;	// ����� ������� ���������� � ������ (���������� �����)
	std::int16_t SizeWorkTime;	// �� ����� ����� ������� ��������� (� �������)
	std::int64_t StartFineTime;	// �����, ����� ��� �������� ����� (���������� �����)
	std::int16_t SizeFineTime;	// ����� �������� � ������ ������ (� �������)
	std::int64_t StopTimerTime;	// ����� ��������� ������� ������� (���������� �����)
	std::int64_t LastSyncTime;	// ����� ���������� ����������� ������ �� ���������� ������
	std::uint32_t Programs;		// ����� ��������, ����������� ��� �������
	std::uint32_t Commands;		// �������������� ������� ��� ����������
	std::uint32_t Changes;		// ��������� ������ ��� �������� �� ������� �� ���������� �������

    // ������� �������� ��������� ������� ������ � ������� ������
    bool ControlWorkTime();
    bool ControlFineTime();
    bool ControlSyncTime();

public:
    // ��������������� �������
	MStatesInfo GetInfo();
	bool GetOptions(MClOptions &Options_);
    bool GetGames(MGames &Games_);
    bool Timer(std::int64_t SystemTime_);
    void CmdReboot();                       // ����� �� ?
    void CmdShutdown();

    // �������� ��� �������� ����������
	bool NewSyncData(MSyncData &Data_);
	bool NewGames(const MGames &Games_);
	bool NewOptions(const MClOptions &Options_);

    // ������������ ��������� ��� ��������/���������� � ������� �������� ������ MSLList
    void SetDefault(
        HKEY RegKey_,
        const std::wstring &RegPath_,
        const std::wstring &RegValue_,
        HKEY OptKey_,
        const std::wstring &OptPath_,
        const std::wstring &OptValue_,
        const std::wstring &PrgFile_,
        std::uint32_t RegCode_);

	MStateCl():
		OptKey(nullptr),
//	    OptCode=PrgCode=0;
		AutoLockTime(0),
		Number(0),
		State(mcsFree),
		StartWorkTime(0),
		SizeWorkTime(0),
		StartFineTime(0),
		SizeFineTime(0),
		StopTimerTime(0),
		Programs(0),
		Commands(0),
		Changes(mdcNumber|mdcState)
	{
		SystemTime=LastSyncTime=::GetTickCount();
	}

	MStateCl(const MStateCl&) = delete;
	MStateCl(MStateCl&&) = delete;
	MStateCl& operator=(const MStateCl&) = delete;
	MStateCl& operator=(MStateCl&&) = delete;
	~MStateCl() = default;
};
//---------------------------------------------------------------------------
#endif

