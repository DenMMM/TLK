//---------------------------------------------------------------------------
#ifndef UnitLogRecordsH
#define UnitLogRecordsH
//---------------------------------------------------------------------------
#include <vector>
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
	public MSLListItem <MLogRecords, MLogRecordsItem>
{
public:
	__int64 SystemTime;     // ����� �������� ������

	MLogRecordsItem():
		SystemTime(0)
	{
	}
};

class MLogRecords:
	public MSLList <MLogRecords, MLogRecordsItem>
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

		ModeBase():
			Number(0),
			Apply(false)
		{
		}
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

		CmdBase():
			Number(0)
		{
		}
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

		DataShellBase():
			State(0),
			User(0)
		{
		}
	};


	class DataStatesBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		std::vector <MStatesItem::LogData> Items;		// ������ ��������� �����������
	};


	class DataTariffsBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		std::vector <MTariffsItem::LogData> Items;		// ������ �������� �������
	};


	class DataFinesBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		std::vector <MFinesItem::LogData> Items;		// ������ �������� �������
	};


	class DataUsersBase: public MLogRecordsItem
	{
	private:
		// ������� ��������� ����������/�������� ������
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		std::vector <MUsersItem::LogData> Items;		// ������ ������ � �������������
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

		AppConfigBase():
			Opened(false)
		{
		}
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

		AppLogInBase():
			User(0)
		{
		}
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

		CompFineBase():
			Number(0),
			Fine(0),
			Time(0)
		{
		}
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

		CompExchangeBase():
			From(0),
			To(0)
		{
		}
	};


public:
	class Event:        public EventBase {};
	class Mode:			public ModeBase {};
	class Cmd:			public CmdBase {};

	class LogBegin:		public MSLListItemTyped <Event, MLogRecordsItem, LogBegin, mlrBegin> {};
	class LogEnd:		public MSLListItemTyped <Event, MLogRecordsItem, LogEnd, mlrEnd> {};

	class AppStart:		public MSLListItemTyped <Event, MLogRecordsItem, AppStart, mlrStart> {};
	class AppStop:		public MSLListItemTyped <Event, MLogRecordsItem, AppStop, mlrStop> {};

	class AppConfig:	public MSLListItemTyped <AppConfigBase, MLogRecordsItem, AppConfig, mlrConfig> {};
	class AppLogIn:		public MSLListItemTyped <AppLogInBase, MLogRecordsItem, AppLogIn, mlrLogIn> {};
	class AppLogOut:	public MSLListItemTyped <Event, MLogRecordsItem, AppLogOut, mlrLogOut> {};

	class ChComputers:	public MSLListItemTyped <Event, MLogRecordsItem, ChComputers, mlrComputers> {};
	class ChTariffs:	public MSLListItemTyped <Event, MLogRecordsItem, ChTariffs, mlrTariffs> {};
	class ChFines:		public MSLListItemTyped <Event, MLogRecordsItem, ChFines, mlrFines> {};
	class ChUsers:		public MSLListItemTyped <Event, MLogRecordsItem, ChUsers, mlrUsers> {};
	class ChOptions:	public MSLListItemTyped <Event, MLogRecordsItem, ChOptions, mlrOptions> {};

	class DataShell:	public MSLListItemTyped <DataShellBase, MLogRecordsItem, DataShell, mlrDataShState> {};
	class DataStates:	public MSLListItemTyped <DataStatesBase, MLogRecordsItem, DataStates, mlrDataStates> {};
	class DataTariffs:	public MSLListItemTyped <DataTariffsBase, MLogRecordsItem, DataTariffs, mlrDataTariffs> {};
	class DataFines:	public MSLListItemTyped <DataFinesBase, MLogRecordsItem, DataFines, mlrDataFines> {};
	class DataUsers:	public MSLListItemTyped <DataUsersBase, MLogRecordsItem, DataUsers, mlrDataUsers> {};

	class CompRun:		public MSLListItemTyped <CompRunBase, MLogRecordsItem, CompRun, mlrRun> {};
	class CompFine:		public MSLListItemTyped <CompFineBase, MLogRecordsItem, CompFine, mlrFine> {};
	class CompExchange:	public MSLListItemTyped <CompExchangeBase, MLogRecordsItem, CompExchange, mlrExchange> {};

	class ModeLock:		public MSLListItemTyped <Mode, MLogRecordsItem, ModeLock, mlrLock> {};
	class ModePause:	public MSLListItemTyped <Mode, MLogRecordsItem, ModePause, mlrPause> {};
	class ModeOpen:		public MSLListItemTyped <Mode, MLogRecordsItem, ModeOpen, mlrOpen> {};

	class CmdPowerOn:	public MSLListItemTyped <Cmd, MLogRecordsItem, CmdPowerOn, mlrPowerOn> {};
	class CmdReboot:	public MSLListItemTyped <Cmd, MLogRecordsItem, CmdReboot, mlrReboot> {};
	class CmdShutdown:	public MSLListItemTyped <Cmd, MLogRecordsItem, CmdShutdown, mlrShutdown> {};

public:
	MLogRecords()
	{
		TypesIDPrepare(mlrTABLE_SIZE);

		TypesIDSet <LogBegin> ();
		TypesIDSet <LogEnd> ();
		TypesIDSet <AppStart> ();
		TypesIDSet <AppStop> ();
		TypesIDSet <AppConfig> ();
		TypesIDSet <AppLogIn> ();
		TypesIDSet <AppLogOut> ();
		TypesIDSet <ChComputers> ();
		TypesIDSet <ChTariffs> ();
		TypesIDSet <ChFines> ();
		TypesIDSet <ChUsers> ();
		TypesIDSet <ChOptions> ();
		TypesIDSet <DataShell> ();
		TypesIDSet <DataStates> ();
		TypesIDSet <DataTariffs> ();
		TypesIDSet <DataFines> ();
		TypesIDSet <DataUsers> ();
		TypesIDSet <CompRun> ();
		TypesIDSet <CompFine> ();
		TypesIDSet <CompExchange> ();
		TypesIDSet <ModeLock> ();
		TypesIDSet <ModePause> ();
		TypesIDSet <ModeOpen> ();
		TypesIDSet <CmdPowerOn> ();
		TypesIDSet <CmdReboot> ();
		TypesIDSet <CmdShutdown> ();
	}
};
//---------------------------------------------------------------------------
#endif

