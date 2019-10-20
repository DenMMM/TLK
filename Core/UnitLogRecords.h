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
		Marray <MStatesItem::LogData> States;    	// ������ ��������� �����������
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

