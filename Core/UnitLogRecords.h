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
	__int64 SystemTime;     // Время создания записи

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
		mlrBegin			= 0x11,	// Лог начат
		mlrEnd				= 0x12,	// Лог закрыт
		mlrDataShState		= 0x13,	// Данные состояния оболочки
		mlrDataStates		= 0x14,	// Данные состояний компьютеров
		mlrDataTariffs		= 0x15,	// Данные тарифов
		mlrDataFines		= 0x16,	// Данные штрафов
		mlrDataUsers		= 0x17,	// Данные пользователей

		mlrStart			= 0x21,	// Запуск админского модуля
		mlrStop				= 0x22,	// Остановка админского модуля
		mlrLogIn			= 0x23,	// Пользователь начал работу
		mlrLogOut			= 0x24,	// Пользователь закончил работу
		mlrConfig			= 0x25,	// Настройки открыты/закрыты
		mlrComputers		= 0x26,	// Изменен список компьютеров
		mlrTariffs			= 0x27,	// Изменен список тарифов
		mlrFines			= 0x28,	// Изменен список штрафов
		mlrUsers			= 0x29,	// Изменен список пользователей
		mlrOptions			= 0x2A,	// Изменены общие настройки

		mlrRun				= 0x31,	// Запуск компьютера
		mlrFine				= 0x32,	// Штрафование компьютера
		mlrExchange			= 0x33,	// Пересадка с одного компьютера на другой
		mlrLock				= 0x34,	// Прикрытие/открытие компьютера
		mlrPause			= 0x35,	// Приостановка/запуск времени
		mlrOpen				= 0x36,	// Открытие/закрытие компьютера для обслуживания
		mlrPowerOn			= 0x37,	// Включение питания компьютера (WOL)
		mlrReboot			= 0x38,	// Перезагрузка компьютера
		mlrShutdown			= 0x39,	// Выключение питания компьютера
		mlrTABLE_SIZE
	};


protected:
	class EventBase: public MLogRecordsItem
	{
	private:
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;
	};


	class ModeBase: public MLogRecordsItem        /// Объединить с CmdBase ?
	{
	private:
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		char Number;            // Номер компьютера
		bool Apply;				// Режим был установлен/снят

		ModeBase():
			Number(0),
			Apply(false)
		{
		}
	};


	class CmdBase: public MLogRecordsItem
	{
	private:
		// Функции механизма сохранения/загрузки данных
		unsigned GetDataSize() const override;
		void *SetData(void *Data_) const override;
		const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		char Number;            // Номер компьютера

		CmdBase():
			Number(0)
		{
		}
	};


	class DataShellBase: public MLogRecordsItem
	{
	private:
		// Функции механизма сохранения/загрузки данных
		unsigned GetDataSize() const override;
		void *SetData(void *Data_) const override;
		const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		unsigned State;         // Состояние оболочки
		unsigned User;          // Текущий пользователь (чья смена открыта)

		DataShellBase():
			State(0),
			User(0)
		{
		}
	};


	class DataStatesBase: public MLogRecordsItem
	{
	private:
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		std::vector <MStatesItem::LogData> Items;		// Массив состояний компьютеров
	};


	class DataTariffsBase: public MLogRecordsItem
	{
	private:
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		std::vector <MTariffsItem::LogData> Items;		// Массив описаний тарифов
	};


	class DataFinesBase: public MLogRecordsItem
	{
	private:
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		std::vector <MFinesItem::LogData> Items;		// Массив описаний штрафов
	};


	class DataUsersBase: public MLogRecordsItem
	{
	private:
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		std::vector <MUsersItem::LogData> Items;		// Массив данных о пользователях
	};


	class AppConfigBase: public MLogRecordsItem
	{
	private:
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override ;
		virtual void *SetData(void *Data_) const override ;
		virtual const void *GetData(const void *Data_, const void *Limit_) override ;

	public:
		bool Opened;			// Настройки открыты/закрыты

		AppConfigBase():
			Opened(false)
		{
		}
	};


	class AppLogInBase: public MLogRecordsItem
	{
	private:
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		unsigned User;          // ID-номер пользователя

		AppLogInBase():
			User(0)
		{
		}
	};


	class CompRunBase: public MLogRecordsItem
	{
	private:
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		char Number;                            // Номер компьютера
		unsigned Tariff;                        // ID-номер тарифа
		__int64 StartTime;                      // Время, относительно которого посчитана стоимость
		char Type;                              // =
		short BeginTime;                        // =    Информация о пакете
		short EndTime;                          // =
		short SizeTime;                         // =
		short WorkTime;                         // Поставленное время работы
		double Cost;                            // Стоимость работы на компьютере в течении 'WorkTime'

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
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		char Number;                        // Номер компьютера
		unsigned Fine;                      // ID-номер штрафа
		short Time;                         // Время штрафа

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
		// Функции механизма сохранения/загрузки данных
		virtual unsigned GetDataSize() const override;
		virtual void *SetData(void *Data_) const override;
		virtual const void *GetData(const void *Data_, const void *Limit_) override;

	public:
		char From;                          // С какого компьютера было взято время
		char To;                            // На какой компьютер время было поставлено

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

