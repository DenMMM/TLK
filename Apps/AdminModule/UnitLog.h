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
// Минимальный период ведения файла лога
#define mlpDay          1   // Суточный
#define mlpWeek         2   // Недельный
#define mlpMonth        3   // На месяц
//---------------------------------------------------------------------------
class MLog
{
private:
	__int64 SystemTime;		// Системное время для синхронизации с описателями состояний компьютеров
	__int64 BeginTime;		// Время, когда лог был начат
	std::wstring Directory;	// Директория для хранения файлов логов
	bool Opened;			// Лог был успешно открыт/начат
//    bool Transaction;       // Флаг-не очищать буфер записей до "сброса" на диск
	MLogRecords Records;	// Буфер для записей
	unsigned User;			// Пользователь, открывший смену последним
	DWORD LastError;

    // Добавить в буфер событие/данные
	void AddSimpleEvent(MLogRecords::ItemType Type_);
	void AddStatesData(const MStates &States_);
	void AddTariffsData(const MTariffs &Tariffs_);
	void AddFinesData(const MFines &Fines_);
	void AddUsersData(const MUsers &Users_);
	// Записать в лог простое событие/команду
	bool AddEvent(MLogRecords::ItemType Type_);
	bool AddMode(MLogRecords::ItemType Type_, char Number_, bool Apply_);
	bool AddCmd(MLogRecords::ItemType Type_, char Number_);
    //
    bool Rename() const;

public:
    // Работа с файлом лога
    bool Begin(
		const MShellState &ShellState_,
		const MStates &States_,
		const MTariffs &Tariffs_,
		const MFines &Fines_,
		const MUsers &Users_);				// Создать файл и сохранить первоначальные данные
	bool Open();							// Открыть существующий файл
	bool End();								// Закончить заполнение файла
	bool CheckPeriod(int Period_) const;	// Проверить не пора ли сменить файл лога
	unsigned LastUser() const;				// Вернуть ID последнего залогиненного пользователя

	// Админский модуль
	bool AddStart(
		const MShellState &ShellState_,
		const MStates &States_,
		const MTariffs &Tariffs_,
		const MFines &Fines_,
		const MUsers &Users_);				// Запуск модуля управления
	bool AddStop();							// Остановка модуля управления

	// Настройки модуля управления
	bool AddConfig(bool Open_);             	// Настройки открыты/закрыты
	bool AddComputers(const MStates &States_);	// Изменен список компьютеров
	bool AddTariffs(const MTariffs &Tariffs_);	// Изменен список тарифов
	bool AddFines(const MFines &Fines_);		// Изменен список штрафов
	bool AddUsers(const MUsers &Users_);		// Изменен список пользователей
	bool AddOptions();							// Изменены общие настройки

	// Пользователи
	bool AddLogIn(unsigned UserID_);		// Пользователь начал смену
	bool AddLogOut();						// Пользователь закончил смену

	// Управление буферизацией следующих ниже команд
//    bool Transact();
//    bool Apply();
	// Команды, применяемые к компьютерам
	bool AddRun(const MTariffRunTimesItem &Time_);
	bool AddFine(char Number_, unsigned FineID_, short Time_);
	bool AddExchange(char From_, char To_);
	bool AddLock(char Number_, bool Apply_);
	bool AddPause(char Number_, bool Apply_);
	bool AddOpen(char Number_, bool Apply_);
	bool AddPowerOn(char Number_);
	bool AddReboot(char Number_);
	bool AddShutdown(char Number_);

	void SetDefault(const std::wstring &Dir_, unsigned Code_);
	void Timer(__int64 SystemTime_);
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

