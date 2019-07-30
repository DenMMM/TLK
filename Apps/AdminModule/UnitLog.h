//---------------------------------------------------------------------------
#ifndef UnitLogH
#define UnitLogH
//---------------------------------------------------------------------------
#include "UnitLogRecords.h"
#include "UnitShellState.h"
#include "UnitStates.h"
#include "UnitTariffs.h"
#include "UnitFines.h"
#include "UnitUsers.h"
//---------------------------------------------------------------------------
// Main Log Period
#define mlpDay          1   // Суточный лог
#define mlpWeek         2   // Недельный лог
#define mlpMonth        3   // Месячный лог
#define mlpQuarter      4   // Квартальный лог
#define mlpYear         5   // Годовой лог
//---------------------------------------------------------------------------
class MLog
{
private:
    __int64 SystemTime;     // Системное время для синхронизации с описателями состояний компьютеров
    __int64 BeginTime;      // Время начала заполнения файла лога
    char *Directory;        // Директория для хранения файлов логов
    MLogRecords Records;    // Записи лога, еще не добавленные к файлу
    unsigned User;          // Пользователь, чья смена была открыта

    bool AddSimpleEvent(unsigned Type_);
    bool AddEvent(unsigned Type_);
    bool AddMode(unsigned Type_, int Number_, bool Apply_);
    bool AddCmd(unsigned Type_, int Number_);
    bool AddStatesData(MStates *States_);
    bool AddTariffsData(MTariffs *Tariffs_);
    bool AddFinesData(MFines *Fines_);
    bool AddUsersData(MUsers *Users_);
public:
    void Timer(__int64 SystemTime_);
    bool SetDefault(char *Dir_, unsigned Code_);

    // Работа с файлом лога
    bool Begin(
        MShellState *ShellState_,
        MStates *States_,
        MTariffs *Tariffs_,
        MFines *Fines_,
        MUsers *Users_);    // Создать файл и сохранить первоначальные данные
    bool Open();            // Открыть существующий файл
    unsigned LastUser();
    bool CheckPeriod(int Period_);     // Проверить не пора ли сменить файл лога
    bool End();             // Закончить заполнение файла

    // Админский модуль
    bool AddStart(
        MShellState *ShellState_,
        MStates *States_,
        MTariffs *Tariffs_,
        MFines *Fines_,
        MUsers *Users_);        // Запуск админского модуля
    bool AddWork();             // Админский модуль работает
    bool AddStop();             // Остановка админского модуля

    // Настройки админского модуля
    bool AddConfig(bool Open_);             // Настройки открыты/закрыты
    bool AddComputers(MStates *States_);    // Изменен список компьютеров
    bool AddTariffs(MTariffs *Tariffs_);    // Изменен список тарифов
    bool AddFines(MFines *Fines_);          // Изменен список штрафов
    bool AddUsers(MUsers *Users_);          // Изменен список пользователей
    bool AddOptions();                      // Изменены общие настройки

    // Пользователи
    bool AddLogIn(unsigned ID_);            // Пользователь начал работу
    bool AddLogOut();                       // Пользователь закончил работу

    // Команды, применяемые к компьютерам
    bool AddRun(MRunTime *Time_);
    bool AddFine(int Number_, unsigned Fine_, int Time_);
    bool AddExchange(int From_, int To_);
    bool AddLock(int Number_, bool Apply_);
    bool AddPause(int Number_, bool Apply_);
    bool AddOpen(int Number_, bool Apply_);
    bool AddWtLocker(int Number_, bool Apply_);
    bool AddPowerOn(int Number_);
    bool AddReboot(int Number_);
    bool AddShutdown(int Number_);

    MLog();
    ~MLog();
};
//---------------------------------------------------------------------------
#endif

