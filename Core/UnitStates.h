//---------------------------------------------------------------------------
#ifndef UnitStatesH
#define UnitStatesH
//---------------------------------------------------------------------------
#include <string>
#include <mutex>
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
#define MAX_TimeShift   20      // Максимальное расхождение времени сервера и клиента (секунд)
//---------------------------------------------------------------------------
// Состояние компьютера
#define mcsFree         1       // Свободен
#define mcsWork         2       // В работе, время отсчитывается
#define mcsFine         4       // В работе, но оштрафован (доп. флаг)
#define mcsLock         8       // В работе, но прикрыт (доп. флаг)
#define mcsPause        16      // Компьютер закрыт и время остановлено (доп. флаг)
#define mcsOpen         32      // Компьютер открыт для обслуживания и время остановлено (доп. флаг)
//---------------------------------------------------------------------------
// Команды для компьютера
#define mccPowerOn      1       // Включить (WOL)
#define mccReboot       2       // Перезагрузить
#define mccShutdown     4       // Выключить
//---------------------------------------------------------------------------
// Состояние сетевого обмена с компьютером
#define mnsPresent      1       // Компьютер корректно отвечает на запросы по сети
#define mnsSyncNeed     2       // Требуется синхронизация нового режима работы с клиентом
#define mnsSyncData     4       // Есть новые данные для передачи сетевому интерфейсу
#define mnsLock         8       //
#define mnsNeedSave     16      //
//---------------------------------------------------------------------------
// Изменения в состоянии компьютера с момента последнего запроса (оптимизация для GUI)
#define mdcNumber       1       // Номер компьютера
#define mdcState        2       // Режим работы
#define mdcTariff       4       // Тариф (сервер)
#define mdcWorkTime     8       // Время работы
#define mdcFineTime     16      // Время штрафа
#define mdcCommands     32      // Дополнительные команды
#define mdcNetState     64      // Состояние сети (сервер)
//#define mdcServiceState     128         //
#define mdcPrograms     256     // Изменился список доступных программ (клиент)
#define mdcOptions      512     // Изменились настройки клиента (клиент)
#define mdcAll (mdcNumber|mdcState|mdcTariff|mdcWorkTime|mdcFineTime|mdcCommands|mdcNetState)
//#define mdcAllCl (mdcNumber|mdcState|mdcWorkTime|mdcFineTime|mdcPrograms|mdcCommands|mdcOptions)
//---------------------------------------------------------------------------
struct MStatesInfo
{
	char Number;                // Номер компьютера
	unsigned State;             // Состояние компьютера (режимы работы)
	unsigned TariffID;          // ID-номер тарифа, по которому работает компьютер
	short WorkTime;             // На какое время был запущен компьютер (в минутах)
	short ToEndWork;            // Оставшееся время работы (в минутах)
	short EndWorkTime;          // До скольки еще ждать окончания работы (в минутах с начала суток)
	short FineTime;             // Время ожидания в режиме штрафа (в минутах)
	short ToEndFine;            // Оставшееся время штрафа (в минутах)
	short EndFineTime;          // До скольки еще ждать окончания штрафа (в минутах с начала суток)
	unsigned Programs;          // Групы программ, разрешенных для запуска
	unsigned Commands;          // Дополнительные команды для компьютера
	unsigned NetState;          // Состояние сети
	unsigned Changes;           // Изменения данных для оболочки со времени их последнего запроса
};
//---------------------------------------------------------------------------
class MStatesItem:
	public MSLListItemSimple <
		MSLListItem <MStates, MStatesItem>,
		MStatesItem>
{
public:
	// Функции механизма сохранения/загрузки данных
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

private:
	mutable std::mutex mtxMain;	// Объект для синхронизации доступа потоков к данным
	__int64 SystemTime;         // Системное время, используемое при всех расчетах

	char Number;                // Номер компьютера, с которым ассоциировано состояние
	unsigned State;             // Состояние компьютера (режимы работы)
	unsigned TariffID;          // ID-номер тарифа, по которому работает компьютер
	__int64 StartWorkTime;      // Время запуска компьютера в работу (абсолютное время)
	short SizeWorkTime;         // На какое время запущен компьютер (в минутах)
	__int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
	short SizeFineTime;         // Время ожидания в режиме штрафа (в минутах)
	__int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
	unsigned Programs;          // Групы программ, разрешенных для запуска
	unsigned Commands;          // Дополнительные команды для компьютера
	unsigned CmdsToReset;       // Команды требующие отмены по окончании выполнения
	unsigned NetState;          // Состояние сети
	unsigned Changes;           // Изменения данных для оболочки со времени их последнего запроса

	// Функции проверки окончания времени работы и времени штрафа
	bool ControlWorkTime();
	bool ControlFineTime();

public:
	// Команды, применяемые к компьютерам
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

	// Вспомогательные функции
	MTariffRunTimesItem GetRunParam() const;
	MStatesInfo GetInfo();
	bool Timer(__int64 SystemTime_);

	// Назначение и проверка связей с другими данными
	int Associated() const { return Number; }
	void Associate(int Number_);

	// Операции для сетевого интерфейса
	bool NetBegin();
	bool NetEnd();

	bool NetSyncNewData();					// Есть ли что отправить клиенту ?
	MSyncData NetSyncData();				// Запрос данных для отправки клиенту
	void NetSyncExecuted(bool Executed_);	//

	bool NetPwrOnNeed();
	void NetPwrOnExecuted();

	// Поддержка логов
	struct LogData
	{
		char Number;                // Номер компьютера
		unsigned State;             // Состояние компьютера (режимы работы)
		unsigned TariffID;          // ID-номер тарифа, по которому работает компьютер
		__int64 StartWorkTime;      // Время запуска компьютера в работу (абсолютное время)
		short SizeWorkTime;         // На какое время запущен компьютер (в минутах)
		__int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
		short SizeFineTime;         // Время ожидания в режиме штрафа (в минутах)
		__int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
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
	std::mutex mtxFile;			// Объект для синхронизации

public:
	// Вспомогательные функции
	const_iterator Search(int Number_) const;
	iterator Search(int Number_)
	{
		const auto *const_this=this;
		return const_cast_iter(const_this->Search(Number_));
	}

	bool Update(const MComputers &Computers_);
	bool Timer(__int64 SystemTime_);

	// Переопределяем функцию сохранения списка в файл
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
	// Функции механизма сохранения/загрузки данных
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

private:
	HKEY OptKey;				//
	std::wstring OptPath;		//
	std::wstring OptValue;		//
	std::wstring PrgFile;		// Файл для хранения списка программ
	unsigned AutoLockTime;		// Время отсутствия связи с сервером до автоблокировки

	mutable std::mutex mtxMain;	// Объект для синхронизации доступа потоков к данным
    __int64 SystemTime;         // Системное время, используемое при всех расчетах

    char Number;                // Номер компьютера
    unsigned State;             // Состояние компьютера (режимы работы)
    __int64 StartWorkTime;      // Время запуска компьютера в работу (абсолютное время)
    short SizeWorkTime;         // На какое время запущен компьютер (в минутах)
    __int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
    short SizeFineTime;         // Время ожидания в режиме штрафа (в минутах)
    __int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
    __int64 LastSyncTime;       // Время последнего поступления данных от админского модуля
    unsigned Programs;          // Групы программ, разрешенных для запуска
    unsigned Commands;          // Дополнительные команды для компьютера
    unsigned Changes;           // Изменения данных для оболочки со времени их последнего запроса

    // Функции проверки окончания времени работы и времени штрафа
    bool ControlWorkTime();
    bool ControlFineTime();
    bool ControlSyncTime();

public:
    // Вспомогательные функции
	MStatesInfo GetInfo();
	bool GetOptions(MClOptions &Options_);
    bool GetGames(MGames &Games_);
    bool Timer(__int64 SystemTime_);
    void CmdReboot();                       // Нужна ли ?
    void CmdShutdown();

    // Операции для сетевого интерфейса
	bool NewSyncData(MSyncData &Data_);
	bool NewGames(const MGames &Games_);
	bool NewOptions(const MClOptions &Options_);

    // Переадресуем обращения для загрузки/сохранения к методам базового класса MSLList
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

	MStateCl(const MStateCl&) = delete;
	MStateCl(MStateCl&&) = delete;
	MStateCl& operator=(const MStateCl&) = delete;
	MStateCl& operator=(MStateCl&&) = delete;
	~MStateCl() = default;
};
//---------------------------------------------------------------------------
#endif

