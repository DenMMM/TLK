//---------------------------------------------------------------------------
#ifndef UnitStateH
#define UnitStateH
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#define MAX_TimeShift 20        // Отклонение системного времени от времени управляющего компьютера (секунд)
//---------------------------------------------------------------------------
// Main Computer State
#define mcsWtLocker ((unsigned int)1)   // Режим без активного TimeLocker'а
#define mcsFree ((unsigned int)2)       // Свободен
#define mcsWork ((unsigned int)4)       // В работе
#define mcsFine ((unsigned int)8)       // В работе, но оштрафован (дополнительный флаг)
#define mcsLock ((unsigned int)16)      // В работе, но прикрыт (дополнительный флаг)
#define mcsPause ((unsigned int)32)     // Компьютер закрыт и время остановлено (дополнительный флаг)
#define mcsOpen ((unsigned int)64)      // Компьютер открыт для обслуживания и время остановлено (дополнительный флаг)
//---------------------------------------------------------------------------
// Main Computer Command
#define mccPowerOn ((unsigned int)1)    // Компьютер должен включиться
#define mccReboot ((unsigned int)2)     // Компьютер должен перезагрузиться
#define mccShutdown ((unsigned int)4)   // Компьютер должен выключиться
//---------------------------------------------------------------------------
// Main Data Change
#define mdcNumber ((unsigned int)1)     // Изменился номер компьютера
#define mdcState ((unsigned int)2)      // Изменился режим работы
#define mdcWorkTime ((unsigned int)4)   // Изменилось время работы
#define mdcFineTime ((unsigned int)8)   // Изменилось время штрафа
#define mdcPrograms ((unsigned int)16)  // Изменился список доступных программ
#define mdcCommands ((unsigned int)32)  // Изменились дополнительные команды
#define mdcOptions  ((unsigned int)64)  // Изменились настройки клиента
#define mdcAll (mdcNumber|mdcState|mdcWorkTime|mdcFineTime|mdcPrograms|mdcCommands|mdcOptions)
//---------------------------------------------------------------------------
class MState;
struct MSyncData;
struct MStateInfo;
//---------------------------------------------------------------------------
class MState:private MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return NULL; }
    void operator_delete(MListItem *DelItem_) { }
private:
    HKEY OptKey;                //
    char *OptPath;              //
    char *OptValue;             //
    unsigned OptCode;           // Код для шифрования настроек
    char *PrgFile;              // Файл для хранения списка программ
    unsigned PrgCode;           // Код для шифрования списка программ
    unsigned AutoLockTime;

    CRITICAL_SECTION CS_Main;   // Объект для синхронизации доступа потоков к данным
    __int64 SystemTime;         // Системное время, используемое при всех расчетах

    int Number;                 // Номер компьютера
    unsigned State;             // Состояние компьютера (режимы работы)
    __int64 StartWorkTime;      // Время запуска компьютера в работу (абсолютное время)
    int SizeWorkTime;           // На какое время запущен компьютер (в минутах)
    __int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
    int SizeFineTime;           // Время ожидания в режиме штрафа (в минутах)
    __int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
    __int64 LastSyncTime;       // Время последнего поступления данных от админского модуля
    unsigned Programs;          // Групы программ, разрешенных для запуска
    unsigned Commands;          // Дополнительные команды для компьютера
    unsigned Changes;           // Изменения данных для оболочки со времени их последнего запроса

    // Функции синхронизации доступа разных потоков к данным
    void Lock() { ::EnterCriticalSection(&CS_Main); }
    void UnLock() { ::LeaveCriticalSection(&CS_Main); }

    // Функции проверки окончания времени работы и времени штрафа
    bool ControlWorkTime();
    bool ControlFineTime();
    bool ControlSyncTime();

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

public:
    // Вспомогательные функции
    void StateInfo(MStateInfo *Info_);
    bool GetOptions(MClOptions *Options_);
    bool GetGames(MGames *Games_);
    bool Timer(__int64 SystemTime_);
    void CmdReboot();

    // Операции для сетевого интерфейса
    bool NewSyncData(MSyncData *Data_);
    bool NewGames(MGames *Games_);
    bool NewOptions(MClOptions *Options_);

    // Переадресуем обращения для загрузки/сохранения к методам базового класса MSLList
    bool SetDefault(HKEY RegKey_, char *RegPath_, char *RegValue_, unsigned RegCode_,
        HKEY OptKey_, char *OptPath_, char *OptValue_, unsigned OptCode_,
        char *PrgFile_, unsigned PrgCode_);

    bool Save() { return MSLList::Store(); }
    bool Load() { return MSLList::Query(); }

    MState();
    ~MState();
};
//---------------------------------------------------------------------------
struct MSyncData
{
    __int64 SystemTime;         // Системное время, используемое при всех расчетах
    int Number;                 // Номер компьютера, с которым ассоциировано состояние
    unsigned State;             // Состояние компьютера (режимы работы)
    __int64 StartWorkTime;      // Время запуска компьютера в работу (абсолютное время)
    int SizeWorkTime;           // На какое время запущен компьютер (в минутах)
    __int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
    int SizeFineTime;           // Время ожидания в режиме штрафа (в минутах)
    __int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
    unsigned Programs;          // Групы программ, разрешенных для запуска
    unsigned Commands;          // Дополнительные команды для компьютера
};
//---------------------------------------------------------------------------
struct MStateInfo
{
    int Number;                 // Номер компьютера
    unsigned State;             // Состояние компьютера (режимы работы)
    int WorkTime;               // На какое время был запущен компьютер (в минутах)
    int ToEndWork;              // Оставшееся время работы (в минутах)
    int EndWorkTime;            // До скольки еще ждать окончания работы (в минутах с начала суток)
    int FineTime;               // Время ожидания в режиме штрафа (в минутах)
    int ToEndFine;              // Оставшееся время штрафа (в минутах)
    int EndFineTime;            // До скольки еще ждать окончания штрафа (в минутах с начала суток)
    unsigned Programs;          // Групы программ, разрешенных для запуска
    unsigned Commands;          // Дополнительные команды для компьютера
    unsigned Changes;           // Изменения данных для оболочки со времени их последнего запроса
};
//---------------------------------------------------------------------------
#endif

