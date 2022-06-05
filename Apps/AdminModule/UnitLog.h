//---------------------------------------------------------------------------
#ifndef UnitLogH
#define UnitLogH
//---------------------------------------------------------------------------
#include <string>
#include <cstdint>

#include "..\..\Core\UnitLogRecords.h"
#include "..\..\Core\UnitStates.h"
#include "..\..\Core\UnitTariffs.h"
#include "..\..\Core\UnitFines.h"
#include "..\..\Core\UnitUsers.h"
#include "UnitShellState.h"
//---------------------------------------------------------------------------
// Минимальный период ведения файла лога
#define mlpDay          1   // Суточный
#define mlpWeek         2   // Недельный
#define mlpMonth        3   // На месяц
//---------------------------------------------------------------------------
class MLog
{
private:
	std::int64_t SystemTime;	// Системное время для синхронизации с описателями состояний компьютеров
	std::int64_t BeginTime;		// Время, когда лог был начат
	std::wstring Directory;		// Директория для хранения файлов логов
	bool Opened;				// Лог был успешно открыт/начат
//    bool Transaction;			// Флаг-не очищать буфер записей до "сброса" на диск
	MLogRecords Records;		// Буфер для записей
	std::uint32_t User;			// Пользователь, открывший смену последним
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
	std::uint32_t LastUser() const;			// Вернуть ID последнего залогиненного пользователя

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
	bool AddLogIn(std::uint32_t UserID_);	// Пользователь начал смену
	bool AddLogOut();						// Пользователь закончил смену

	// Управление буферизацией следующих ниже команд
//    bool Transact();
//    bool Apply();
	// Команды, применяемые к компьютерам
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

