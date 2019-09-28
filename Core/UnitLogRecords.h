//---------------------------------------------------------------------------
#ifndef UnitLogRecordsH
#define UnitLogRecordsH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitStates.h"
#include "UnitTariffs.h"
#include "UnitFines.h"
#include "UnitUsers.h"
//---------------------------------------------------------------------------
class MLogRecordsItem;
class MLogRecords;
//---------------------------------------------------------------------------
class MLogRecordsItem:
	public MSLListItem::Proxy <MSLListItem, MLogRecordsItem>
{
public:
	__int64 SystemTime;     // ����� �������� ������

	MLogRecordsItem():
		SystemTime(0)
	{
	}

	virtual ~MLogRecordsItem() override = default;
};

class MLogRecords:
	public MSLList::Typed <MSLList, MLogRecords, MLogRecordsItem>
{
public:
	enum ItemType {
		mlrBegin			= 0x11,	// ��� �����
		mlrEnd				= 0x12,	// ��� ������
		mlrDataShState		= 0x13,	// ������ ��������� ��������
		mlrDataStates		= 0x14,	// ������ ��������� �����������
		mlrDataTariffs		= 0x15,	// ������ �������
		mlrDataFines		= 0x16,	// ������ �������
		mlrDataUsers		= 0x17,	// ������ �������������

		mlrStart			= 0x21,	// ������ ���������� ������
		mlrStop				= 0x22,	// ��������� ���������� ������
		mlrLogIn			= 0x23,	// ������������ ����� ������
		mlrLogOut			= 0x24,	// ������������ �������� ������
		mlrConfig			= 0x25,	// ��������� �������/�������
		mlrComputers		= 0x26,	// ������� ������ �����������
		mlrTariffs			= 0x27,	// ������� ������ �������
		mlrFines			= 0x28,	// ������� ������ �������
		mlrUsers			= 0x29,	// ������� ������ �������������
		mlrOptions			= 0x2A,	// �������� ����� ���������

		mlrRun				= 0x31,	// ������ ����������
		mlrFine				= 0x32,	// ����������� ����������
		mlrExchange			= 0x33,	// ��������� � ������ ���������� �� ������
		mlrLock				= 0x34,	// ���������/�������� ����������
		mlrPause			= 0x35,	// ������������/������ �������
		mlrOpen				= 0x36,	// ��������/�������� ���������� ��� ������������
		mlrPowerOn			= 0x37,	// ��������� ������� ���������� (WOL)
		mlrReboot			= 0x38,	// ������������ ����������
		mlrShutdown			= 0x39,	// ���������� ������� ����������
		mlrTABLE_SIZE
	};


protected:
	class EventBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		virtual void Copy(const MListItem *SrcItem_) override;

		EventBase() = default;
		virtual ~EventBase() override = default;
	};


	class ModeBase: public MLogRecordsItem        /// ���������� � CmdBase ?
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		char Number;            // ����� ����������
		bool Apply;				// ����� ��� ����������/����

		virtual void Copy(const MListItem *SrcItem_) override;

		ModeBase():
			Number(0),
			Apply(false)
		{
		}

		virtual ~ModeBase() override = default;
	};


	class CmdBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		unsigned GetDataSize() const override;
		void *SetData(void *Data_) const override;
		const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		char Number;            // ����� ����������

		virtual void Copy(const MListItem *SrcItem_) override;

		CmdBase():
			Number(0)
		{
		}

		virtual ~CmdBase() override = default;
	};


	class DataShellBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		unsigned GetDataSize() const override;
		void *SetData(void *Data_) const override;
		const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		unsigned State;         // ��������� ��������
		unsigned User;          // ������� ������������ (��� ����� �������)

		virtual void Copy(const MListItem *SrcItem_) override;

		DataShellBase():
			State(0),
			User(0)
		{
		}

		virtual ~DataShellBase() override = default;
	};


	class DataStatesBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		Marray <MStatesItem::LogData> States;    	// ������ ��������� �����������

		virtual void Copy(const MListItem *SrcItem_) override;

		DataStatesBase() = default;
		virtual ~DataStatesBase() override = default;
	};


	class DataTariffsBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		Marray <MTariffsItem::LogData> Items;  	// ������ �������� �������

		virtual void Copy(const MListItem *SrcItem_) override;

		DataTariffsBase() = default;
		virtual ~DataTariffsBase() override = default;
	};


	class DataFinesBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		Marray <MFinesItem::LogData> Items;	// ������ �������� �������

		virtual void Copy(const MListItem *SrcItem_) override;

		DataFinesBase() = default;
		virtual ~DataFinesBase() override = default;
	};


	class DataUsersBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		Marray <MUsersItem::LogData> Items;	// ������ ������ � �������������

		virtual void Copy(const MListItem *SrcItem_) override;

		DataUsersBase() = default;
		virtual ~DataUsersBase() override = default;
	};


	class AppConfigBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override ;
		virtual void *SetData(void *Data_) const override ;
		virtual const void *GetData(const void *Data_, const void *Limit_) override ;

	public:
		bool Opened;			// ��������� �������/�������

		virtual void Copy(const MListItem *SrcItem_) override ;

		AppConfigBase():
			Opened(false)
		{
		}

		virtual ~AppConfigBase() override = default;
	};


	class AppLogInBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		unsigned User;          // ID-����� ������������

		virtual void Copy(const MListItem *SrcItem_) override;

		AppLogInBase():
			User(0)
		{
		}

		virtual ~AppLogInBase() override = default;
	};


	class CompRunBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		char Number;                            // ����� ����������
		unsigned Tariff;                        // ID-����� ������
		__int64 StartTime;                      // �����, ������������ �������� ��������� ���������
		char Type;                              // =
		short BeginTime;                        // =    ���������� � ������
		short EndTime;                          // =
		short SizeTime;                         // =
		short WorkTime;                         // ������������ ����� ������
		double Cost;                            // ��������� ������ �� ���������� � ������� 'WorkTime'

		virtual void Copy(const MListItem *SrcItem_) override;

		CompRunBase():
			Number(0),
			Tariff(0),
			StartTime(0),
			Type(mttUndefined),
			BeginTime(0),
			EndTime(0),
			SizeTime(0),
			WorkTime(0),
			Cost(0.)
		{
		}

		virtual ~CompRunBase() override = default;
	};


	class CompFineBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		char Number;                        // ����� ����������
		unsigned Fine;                      // ID-����� ������
		short Time;                         // ����� ������

		virtual void Copy(const MListItem *SrcItem_) override;

		CompFineBase():
			Number(0),
			Fine(0),
			Time(0)
		{
		}

		virtual ~CompFineBase() override = default;
	};


	class CompExchangeBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		char From;                          // � ������ ���������� ���� ����� �����
		char To;                            // �� ����� ��������� ����� ���� ����������

		virtual void Copy(const MListItem *SrcItem_) override;

		CompExchangeBase():
			From(0),
			To(0)
		{
		}

		virtual ~CompExchangeBase() override = default;
	};


public:
	class Event:        public EventBase {};
	class Mode:			public ModeBase {};
	class Cmd:			public CmdBase {};

	class LogBegin:		public MLogRecordsItem::Typed <Event, LogBegin, mlrBegin> {};
	class LogEnd:		public MLogRecordsItem::Typed <Event, LogEnd, mlrEnd> {};

	class AppStart:		public MLogRecordsItem::Typed <Event, AppStart, mlrStart> {};
	class AppStop:		public MLogRecordsItem::Typed <Event, AppStop, mlrStop> {};

	class AppConfig:	public MLogRecordsItem::Typed <AppConfigBase, AppConfig, mlrConfig> {};
	class AppLogIn:		public MLogRecordsItem::Typed <AppLogInBase, AppLogIn, mlrLogIn> {};
	class AppLogOut:	public MLogRecordsItem::Typed <Event, AppLogOut, mlrLogOut> {};

	class ChComputers:	public MLogRecordsItem::Typed <Event, ChComputers, mlrComputers> {};
	class ChTariffs:	public MLogRecordsItem::Typed <Event, ChTariffs, mlrTariffs> {};
	class ChFines:		public MLogRecordsItem::Typed <Event, ChFines, mlrFines> {};
	class ChUsers:		public MLogRecordsItem::Typed <Event, ChUsers, mlrUsers> {};
	class ChOptions:	public MLogRecordsItem::Typed <Event, ChOptions, mlrOptions> {};

	class DataShell:	public MLogRecordsItem::Typed <DataShellBase, DataShell, mlrDataShState> {};
	class DataStates:	public MLogRecordsItem::Typed <DataStatesBase, DataStates, mlrDataStates> {};
	class DataTariffs:	public MLogRecordsItem::Typed <DataTariffsBase, DataTariffs, mlrDataTariffs> {};
	class DataFines:	public MLogRecordsItem::Typed <DataFinesBase, DataFines, mlrDataFines> {};
	class DataUsers:	public MLogRecordsItem::Typed <DataUsersBase, DataUsers, mlrDataUsers> {};

	class CompRun:		public MLogRecordsItem::Typed <CompRunBase, CompRun, mlrRun> {};
	class CompFine:		public MLogRecordsItem::Typed <CompFineBase, CompFine, mlrFine> {};
	class CompExchange:	public MLogRecordsItem::Typed <CompExchangeBase, CompExchange, mlrExchange> {};

	class ModeLock:		public MLogRecordsItem::Typed <Mode, ModeLock, mlrLock> {};
	class ModePause:	public MLogRecordsItem::Typed <Mode, ModePause, mlrPause> {};
	class ModeOpen:		public MLogRecordsItem::Typed <Mode, ModeOpen, mlrOpen> {};

	class CmdPowerOn:	public MLogRecordsItem::Typed <Cmd, CmdPowerOn, mlrPowerOn> {};
	class CmdReboot:	public MLogRecordsItem::Typed <Cmd, CmdReboot, mlrReboot> {};
	class CmdShutdown:	public MLogRecordsItem::Typed <Cmd, CmdShutdown, mlrShutdown> {};

public:
	MLogRecords()
	{
		TypesIDPrepare(mlrTABLE_SIZE);

		TypesIDSet(LogBegin::TypeID,  		reinterpret_cast<MListItem*(*)()>(&LogBegin::New));
		TypesIDSet(LogEnd::TypeID,			reinterpret_cast<MListItem*(*)()>(&LogEnd::New));
		TypesIDSet(AppStart::TypeID,  		reinterpret_cast<MListItem*(*)()>(&AppStart::New));
		TypesIDSet(AppStop::TypeID,			reinterpret_cast<MListItem*(*)()>(&AppStop::New));
		TypesIDSet(AppConfig::TypeID,		reinterpret_cast<MListItem*(*)()>(&AppConfig::New));
		TypesIDSet(AppLogIn::TypeID,  		reinterpret_cast<MListItem*(*)()>(&AppLogIn::New));
		TypesIDSet(AppLogOut::TypeID,		reinterpret_cast<MListItem*(*)()>(&AppLogOut::New));
		TypesIDSet(ChComputers::TypeID,		reinterpret_cast<MListItem*(*)()>(&ChComputers::New));
		TypesIDSet(ChTariffs::TypeID,		reinterpret_cast<MListItem*(*)()>(&ChTariffs::New));
		TypesIDSet(ChFines::TypeID,			reinterpret_cast<MListItem*(*)()>(&ChFines::New));
		TypesIDSet(ChUsers::TypeID,			reinterpret_cast<MListItem*(*)()>(&ChUsers::New));
		TypesIDSet(ChOptions::TypeID,		reinterpret_cast<MListItem*(*)()>(&ChOptions::New));
		TypesIDSet(DataShell::TypeID,		reinterpret_cast<MListItem*(*)()>(&DataShell::New));
		TypesIDSet(DataStates::TypeID,		reinterpret_cast<MListItem*(*)()>(&DataStates::New));
		TypesIDSet(DataTariffs::TypeID,		reinterpret_cast<MListItem*(*)()>(&DataTariffs::New));
		TypesIDSet(DataFines::TypeID,		reinterpret_cast<MListItem*(*)()>(&DataFines::New));
		TypesIDSet(DataUsers::TypeID,		reinterpret_cast<MListItem*(*)()>(&DataUsers::New));
		TypesIDSet(CompRun::TypeID,			reinterpret_cast<MListItem*(*)()>(&CompRun::New));
		TypesIDSet(CompFine::TypeID,  		reinterpret_cast<MListItem*(*)()>(&CompFine::New));
		TypesIDSet(CompExchange::TypeID,	reinterpret_cast<MListItem*(*)()>(&CompExchange::New));
		TypesIDSet(ModeLock::TypeID,		reinterpret_cast<MListItem*(*)()>(&ModeLock::New));
		TypesIDSet(ModePause::TypeID,		reinterpret_cast<MListItem*(*)()>(&ModePause::New));
		TypesIDSet(ModeOpen::TypeID,		reinterpret_cast<MListItem*(*)()>(&ModeOpen::New));
		TypesIDSet(CmdPowerOn::TypeID,		reinterpret_cast<MListItem*(*)()>(&CmdPowerOn::New));
		TypesIDSet(CmdReboot::TypeID,		reinterpret_cast<MListItem*(*)()>(&CmdReboot::New));
		TypesIDSet(CmdShutdown::TypeID,		reinterpret_cast<MListItem*(*)()>(&CmdShutdown::New));
	}

	~MLogRecords() = default;
};
//---------------------------------------------------------------------------
#endif

