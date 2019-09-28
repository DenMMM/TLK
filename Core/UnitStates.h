//---------------------------------------------------------------------------
#ifndef UnitStatesH
#define UnitStatesH
//---------------------------------------------------------------------------
#include <winsock2.h>
//#include <windows.h>
#include <string>

#include "UnitWinAPI.h"
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
class MStatesItem:
	public MSLListItem::Simple <
		MSLListItem::Proxy <MSLListItem, MStatesItem>,
		MStatesItem>
{
protected:
	virtual void Copy(const MListItem *SrcItem_) override {}
	// ������� ��������� ����������/�������� ������
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

	mutable MWAPI::CRITICAL_SECTION CS_Main;    // ������ ��� ������������� ������� ������� � ������
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

	// ������� �������� ��������� ������� ������ � ������� ������
	bool ControlWorkTime();
	bool ControlFineTime();

public:
	// �������, ����������� � �����������
	bool CmdRun(
		MTariffsItem *Tariff_,
		MTariffRunTimesItem *Time_,
		bool Check_);
	bool CmdFine(short FineSize_, bool Check_);
	bool CmdExchange(MStatesItem *State_, bool Check_);
	bool CmdLock(bool Apply_, bool Check_);
	bool CmdPause(bool Apply_, bool Check_);
	bool CmdOpen(bool Apply_, bool Check_);
	bool CmdPowerOn(bool Check_);
	bool CmdReboot(bool Check_);
	bool CmdShutdown(bool Check_);

	// ��������������� �������
	void RunParam(MTariffRunTimesItem *RunTime_);
	void StateInfo(MStatesInfo *Info_);
	bool Timer(__int64 SystemTime_);

	// ���������� � �������� ������ � ������� �������
	int Associated() const { return Number; }
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
	struct LogData
	{
		char Number;                // ����� ����������
		unsigned State;             // ��������� ���������� (������ ������)
		unsigned TariffID;          // ID-����� ������, �� �������� �������� ���������
		__int64 StartWorkTime;      // ����� ������� ���������� � ������ (���������� �����)
		short SizeWorkTime;         // �� ����� ����� ������� ��������� (� �������)
		__int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
		short SizeFineTime;         // ����� �������� � ������ ������ (� �������)
		__int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)

		LogData &operator=(const MStatesItem &State_)
		{
			MWAPI::CRITICAL_SECTION::Lock lckObj(State_.CS_Main);

			Number=State_.Number;
			State=State_.State;
			TariffID=State_.TariffID;
			StartWorkTime=State_.StartWorkTime;
			SizeWorkTime=State_.SizeWorkTime;
			StartFineTime=State_.StartFineTime;
			SizeFineTime=State_.SizeFineTime;
			StopTimerTime=State_.StopTimerTime;

			return *this;
		}
	};

	MStatesItem &operator=(const LogData &Data_)
	{
		MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

		Number=Data_.Number;
		State=Data_.State;
		TariffID=Data_.TariffID;
		StartWorkTime=Data_.StartWorkTime;
		SizeWorkTime=Data_.SizeWorkTime;
		StartFineTime=Data_.StartFineTime;
		SizeFineTime=Data_.SizeFineTime;
		StopTimerTime=Data_.StopTimerTime;
		Changes=mdcAll;

		return *this;
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

	virtual ~MStatesItem() override = default;
};
//---------------------------------------------------------------------------
class MStates:
	public MSLList::Simple <MSLList, MStates, MStatesItem>
{
private:
	MWAPI::CRITICAL_SECTION CS_File;   // ������ ��� �������������

public:
	// ��������������� �������
	MStatesItem *Search(int Number_) const;
	bool Update(MComputers *Computers_);
	bool Timer(__int64 SystemTime_);

	// �������������� ������� ���������� ������ � ����
	bool Save();

	MStates() = default;
	~MStates() = default;
};
//---------------------------------------------------------------------------
class MStateCl: public MSLList          /// ��������� ������ ������������
{
private:
    // ������� ��������� ����������/�������� ������
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

	HKEY OptKey;				//
	std::wstring OptPath;		//
	std::wstring OptValue;		//
	std::wstring PrgFile;		// ���� ��� �������� ������ ��������
	unsigned AutoLockTime;		// ����� ���������� ����� � �������� �� ��������������

    mutable MWAPI::CRITICAL_SECTION CS_Main;   // ������ ��� ������������� ������� ������� � ������
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

    // ������� �������� ��������� ������� ������ � ������� ������
    bool ControlWorkTime();
    bool ControlFineTime();
    bool ControlSyncTime();

public:
    // ��������������� �������
    void StateInfo(MStatesInfo *Info_);
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
    void SetDefault(
        HKEY RegKey_,
        const std::wstring &RegPath_,
        const std::wstring &RegValue_,
        HKEY OptKey_,
        const std::wstring &OptPath_,
        const std::wstring &OptValue_,
        const std::wstring &PrgFile_,
        unsigned RegCode_);

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

	~MStateCl() = default;
};
//---------------------------------------------------------------------------
#endif

