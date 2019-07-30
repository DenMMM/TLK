//---------------------------------------------------------------------------
#ifndef UnitStatesH
#define UnitStatesH
//---------------------------------------------------------------------------
#include <windows.h>
#include "UnitSLList.h"
#include "UnitComputers.h"
#include "UnitTariffs.h"
//---------------------------------------------------------------------------
// Main Computer State
#define mcsWtLocker ((unsigned int)1)   // Режим без активного TimeLocker'а
#define mcsFree ((unsigned int)2)       // Свободен
#define mcsWork ((unsigned int)4)       // В работе
#define mcsFine ((unsigned int)8)       // В работе, но оштрафован (дополнительный флаг)
#define mcsLock ((unsigned int)16)      // В работе, но прикрыт (дополнительный флаг)
#define mcsPause ((unsigned int)32)     // Компьютер закрыт и время остановлено (дополнительный флаг)
#define mcsOpen ((unsigned int)64)      // Компьютер открыт для обслуживания и время остановлено (дополнительный флаг)
//#define mcsService ((unsigned int)128)  // Компьютер занят для обслуживания (дополнительный флаг)
//---------------------------------------------------------------------------
// Main Computer Command
#define mccPowerOn ((unsigned int)1)    // Компьютер должен включиться
#define mccReboot ((unsigned int)2)     // Компьютер должен перезагрузиться
#define mccShutdown ((unsigned int)4)   // Компьютер должен выключиться
//---------------------------------------------------------------------------
// Main Net State
#define mnsPresent ((unsigned int)1)        // Компьютер корректно отвечает на запросы по сети
#define mnsSyncNeed ((unsigned int)2)       // Требуется синхронизация нового режима работы с клиентом
#define mnsSyncData ((unsigned int)4)       // Есть новые данные для передачи сетевому интерфейсу
#define mnsLock ((unsigned int)8)           //
#define mnsNeedSave ((unsigned int)16)      //
//---------------------------------------------------------------------------
// Main Data Change
#define mdcNumber ((unsigned int)1)     // Изменился номер компьютера (состояние асоциировано с другим компьютером)
#define mdcState ((unsigned int)2)      // Изменился режим работы
#define mdcTariff ((unsigned int)4)     // Изменился тариф
#define mdcWorkTime ((unsigned int)8)   // Изменилось время работы
#define mdcFineTime ((unsigned int)16)  // Изменилось время штрафа
#define mdcCommands ((unsigned int)32)  // Изменились дополнительные команды
#define mdcNetState ((unsigned int)64)  // Изменилось состояние сети
#define mdcAll (mdcNumber|mdcState|mdcTariff|mdcWorkTime|mdcFineTime|mdcCommands|mdcNetState)
//#define mdcServiceState ((unsigned int)64)  //
//---------------------------------------------------------------------------
class MState;
class MStates;
struct MSyncData;
struct MStateInfo;
struct MStateData;
//---------------------------------------------------------------------------
class MState:public MSLListItem
{
public:
    bool Copy(MListItem *SrcItem_);
private:
    CRITICAL_SECTION CS_Main;   // Объект для синхронизации доступа потоков к данным
    __int64 SystemTime;         // Системное время, используемое при всех расчетах

    int Number;                 // Номер компьютера, с которым ассоциировано состояние
    unsigned State;             // Состояние компьютера (режимы работы)
    unsigned TariffID;          // ID-номер тарифа, по которому работает компьютер
    __int64 StartWorkTime;      // Время запуска компьютера в работу (абсолютное время)
    int SizeWorkTime;           // На какое время запущен компьютер (в минутах)
    __int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
    int SizeFineTime;           // Время ожидания в режиме штрафа (в минутах)
    __int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
    unsigned Programs;          // Групы программ, разрешенных для запуска
    unsigned Commands;          // Дополнительные команды для компьютера
    unsigned CmdsToReset;       // Команды требующие отмены по окончании выполнения
    unsigned NetState;          // Состояние сети
    unsigned Changes;           // Изменения данных для оболочки со времени их последнего запроса

    // Функции синхронизации доступа разных потоков к данным
    void Lock() { ::EnterCriticalSection(&CS_Main); }
    void UnLock() { ::LeaveCriticalSection(&CS_Main); }

    // Функции проверки окончания времени работы и времени штрафа
    bool ControlWorkTime();
    bool ControlFineTime();

public:
    // Команды, применяемые к компьютерам
    bool CmdRun(MTariff *Tariff_, MRunTime *Time_, bool Check_);
    bool CmdFine(int FineSize_, bool Check_);
    bool CmdExchange(MState *State_, bool Check_);
    bool CmdLock(bool Apply_, bool Check_);
    bool CmdPause(bool Apply_, bool Check_);
    bool CmdOpen(bool Apply_, bool Check_);
    bool CmdWtLocker(bool Apply_, bool Check_);
    bool CmdPowerOn(bool Check_);
    bool CmdReboot(bool Check_);
    bool CmdShutdown(bool Check_);

    // Вспомогательные функции
    void RunParam(MRunTime *RunTime_);
    void StateInfo(MStateInfo *Info_);
    bool Timer(__int64 SystemTime_);

    // Назначение и проверка связей с другими данными
    int Associated();
    void Associate(int Number_);

    // Операции для сетевого интерфейса
    bool NetBegin();
    bool NetEnd();

    bool NetSyncNewData();
    void NetSyncData(MSyncData *Data_);
    void NetSyncExecuted(bool Executed_);

    bool NetPwrOnNeed();
    void NetPwrOnExecuted();

    // Поддержка логов
    void GetStateData(MStateData *Data_);
    void SetStateData(MStateData *Data_);

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MState();
    ~MState();
};
//---------------------------------------------------------------------------
class MStates:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MState; }
    void operator_delete(MListItem *DelItem_) { delete (MState*)DelItem_; }
private:
    CRITICAL_SECTION CS_File;   // Объект для синхронизации доступа потоков к данным

    // Функции синхронизации доступа разных потоков к данным
    void Lock() { ::EnterCriticalSection(&CS_File); }
    void UnLock() { ::LeaveCriticalSection(&CS_File); }
public:
    // Вспомогательные функции
    MState *Search(int Number_);
    bool Update(MComputers *Computers_);
    bool Timer(__int64 SystemTime_);

    // Переопределяем функцию сохранения списка в файл
    bool Save();

    MStates() { constructor(); ::InitializeCriticalSection(&CS_File); }
    ~MStates() { ::DeleteCriticalSection(&CS_File); destructor(); }
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
    unsigned TariffID;          // ID-номер тарифа, по которому работает компьютер
    int WorkTime;               // На какое время был запущен компьютер (в минутах)
    int ToEndWork;              // Оставшееся время работы (в минутах)
    int EndWorkTime;            // До скольки еще ждать окончания работы (в минутах с начала суток)
    int FineTime;               // Время ожидания в режиме штрафа (в минутах)
    int ToEndFine;              // Оставшееся время штрафа (в минутах)
    int EndFineTime;            // До скольки еще ждать окончания штрафа (в минутах с начала суток)
    unsigned Commands;          // Дополнительные команды для компьютера
    unsigned NetState;          // Состояние сети
    unsigned Changes;           // Изменения данных для оболочки со времени их последнего запроса
};
//---------------------------------------------------------------------------
struct MStateData
{
    int Number;                 // Номер компьютера
    unsigned int State;         // Состояние компьютера (режимы работы)
    unsigned TariffID;          // ID-номер тарифа, по которому работает компьютер
    __int64 StartWorkTime;      // Время запуска компьютера в работу (абсолютное время)
    int SizeWorkTime;           // На какое время запущен компьютер (в минутах)
    __int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
    int SizeFineTime;           // Время ожидания в режиме штрафа (в минутах)
    __int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
};
//---------------------------------------------------------------------------
#endif

