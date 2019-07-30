//---------------------------------------------------------------------------
#ifndef UnitStateH
#define UnitStateH
//---------------------------------------------------------------------------
#define MaxPingTime ((int)300)  // Время ожидания связи с админским модулем до блокировки клиента
//---------------------------------------------------------------------------
#define mcsFree ((unsigned int)1)  // Свободен
#define mcsWork ((unsigned int)2)  // В работе (занят)
#define mcsFine ((unsigned int)4)  // В работе, но оштрафован
#define mcsLock ((unsigned int)8)  // В работе, но прикрыт
#define mcsPause ((unsigned int)16)  // Компьютер закрыт и время остановлено
#define mcsAuto ((unsigned int)32)  // Компьютер открыт для обслуживания и время остановлено
#define mcsWtLocker ((unsigned int)64)  // Режим без активного TimeLocker'а
#define mcsNotUse ((unsigned int)128)  // Компьютер не используется в работе
#define mcsReboot ((unsigned int)256)  // Компьютер должен перезагрузиться
#define mcsShutdown ((unsigned int)512)  // Компьютер должен выключиться
//---------------------------------------------------------------------------
#define mgp1 ((unsigned int)1)
#define mgp2 ((unsigned int)2)
#define mgp3 ((unsigned int)4)
#define mgp4 ((unsigned int)8)
#define mgp5 ((unsigned int)16)
#define mgpAll (mgp1|mgp2|mgp3|mgp4|mgp5)
#define mgpNone ((unsigned int)0)
//---------------------------------------------------------------------------
class MState;
//---------------------------------------------------------------------------
class MState
{
private:
    // Используются для синхронизации доступа разных потоков
    LPCRITICAL_SECTION CS_Main;  // Режим работы, время и т.п.
    LPCRITICAL_SECTION CS_Games;  // Список игр
public:
    int Number;  // Номер компьютера в клубе
    unsigned int State;  // Состояние компьютера
    double StartWorkTime;  // Время запуска компьютера в работу (абсолютное время)
    int SizeWorkTime;  // На какое время запущен компьютер (в минутах)
    double StartFineTime;  // Время, когда был применен штраф (абсолютное время)
    int SizeFineTime;  // Время ожидания в режиме штрафа (в минутах)
    double StopTimerTime;  // Время остановки отсчета времени (абсолютное время)
    unsigned int GamesPages;
    unsigned int StateVer;

    int NotPingTime;
    bool NeedUpdate;

    // Функции проверки окончания времени работы и времени штрафа
    bool ControlWorkTime();
    bool ControlFineTime();

    bool ControlPingTime();

    // Функции синхронизации доступа разных потоков к данным
    void InitCriticalSection();
    void DelCriticalSection();
    void Lock() { ::EnterCriticalSection(CS_Main); }
    void UnLock() { ::LeaveCriticalSection(CS_Main); }
    void LockGames() { ::EnterCriticalSection(CS_Games); }
    void UnLockGames() { ::LeaveCriticalSection(CS_Games); }

    MState();
    ~MState();
};
//---------------------------------------------------------------------------
#endif

