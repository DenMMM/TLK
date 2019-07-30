//---------------------------------------------------------------------------
#ifndef UnitComputersH
#define UnitComputersH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
#include "UnitGames.h"
//#include "UnitThreadNetSync.h"
#include <Classes.hpp>
#include <winsock2.h>
//---------------------------------------------------------------------------
#define MAX_Computers ((int)100)  // Масимальное число обслуживаемых компьютеров
#define MAX_NetTimeOut ((int)10)  // Время ожидания до проверки наличия компьютера в сети (в секундах)
#define NUM_SyncThreads ((int)5)  // Количество потоков, одновременно обслуживающих компьютеры
//---------------------------------------------------------------------------
#define mgcNone ((int)-1)  //
#define mgcAqua ((int)0)  //
#define mgcLime ((int)1)  //
#define mgcRed ((int)2)  //
#define mgcYellow ((int)3)  //
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
#define mnsState ((unsigned int)1)  // Режим работы, параметры времени и т.п.
#define mnsConfig ((unsigned int)2)  //
#define mnsGames ((unsigned int)4)  //
//---------------------------------------------------------------------------
#define mdcState ((unsigned int)1)  //
#define mdcTariff ((unsigned int)2)  //
#define mdcWorkTime ((unsigned int)4)  //
#define mdcFineTime ((unsigned int)8)  //
#define mdcNetState ((unsigned int)16)  //
//---------------------------------------------------------------------------
extern CRITICAL_SECTION CS_ComputersStateFile;
//---------------------------------------------------------------------------
class MCompParam;
class MCompParams;
//---------------------------------------------------------------------------
class MComputer;
class MComputers;
//---------------------------------------------------------------------------
class MThreadNetSync;
//---------------------------------------------------------------------------
class MCompParam:public MListItem
{
public:
    unsigned int ID;
    AnsiString Address;
    int Number;
    int GroupColor;

    MCompParam &operator=(MCompParam &CompParam_);
    MCompParam();
    ~MCompParam();
};
//---------------------------------------------------------------------------
class MCompParams:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MCompParam); }
    void delete_(MListItem *ListItem_) { delete ((MCompParam*)ListItem_); }
public:
    MCompParams() { constructor(); }
    ~MCompParams() { destructor(); }
};
//---------------------------------------------------------------------------
class MComputer:public MListItem
{
private:
    // Используются для синхронизации доступа разных потоков
    CRITICAL_SECTION CS_Main;  // Режим работы, время и т.п.
    CRITICAL_SECTION CS_Games;  // Список игр
    // Функции проверки окончания времени работы и времени штрафа
    bool ControlWorkTime();
    bool ControlFineTime();
public:
    // Параметры компьютера
    unsigned int ID;  // Уникальный номер компьютера
    AnsiString Address;  // IP-адрес компьютера в сети
    int Number;  // Номер компьютера в клубе
    int GroupColor;  // Цвет группы, к которой отнесен компьютер
    // Общее состояние компьютера
    unsigned int State;  // Состояние компьютера
    double StartWorkTime;  // Время запуска компьютера в работу (абсолютное время)
    int SizeWorkTime;  // На какое время запущен компьютер (в минутах)
    double StartFineTime;  // Время, когда был применен штраф (абсолютное время)
    int SizeFineTime;  // Время ожидания в режиме штрафа (в минутах)
    double StopTimerTime;  // Время остановки отсчета времени (абсолютное время)
    //
    int FaceType;
    unsigned int GamesPages;
    unsigned int TariffID;  // ID-номер тарифа, по которому компьютер работает
    // Состояние сети
    unsigned int StateVer;  // Версия состояния (используется для аварийной синхронизации)
    unsigned int NetSync;  // Параметры, требующие синхронизации по сети
    unsigned int NotSync;  // Параметры, уже взятые для синхронизации, но еще не отправленные
    int NetTimeOut;  // Время, прошедшее с момента последнего контакта по сети
    bool NetPresent;  // Присутствие компьютера в сети (результат последнего контакта)
    bool NeedConnect;  //
    bool SyncProcess;
    //
    unsigned int DataChanges;

    // Команды, применяемые к компьютерам
    bool CmdRun(unsigned int TariffID_, unsigned int GamesPages_, int WorkTime_);
    void CmdFine(int FineSize_);
    void CmdExchange(MComputer *Computer_);
    void CmdLock(bool Apply_);
    void CmdPause(bool Apply_);
    void CmdAuto(bool Apply_);
    void CmdWtLocker(bool Apply_);
    void CmdNotUse(bool Apply_);
    void CmdReboot();
    void CmdShutdown();
    //
    bool Save();
    bool SetGames(MGames *Games_);
    bool GetGames(MGames *Games_);
    //
    void Timer();
    void NextStateVer();
    // Функции синхронизации доступа разных потоков к данным
    void Lock() { ::EnterCriticalSection(&CS_Main); }
    void UnLock() { ::LeaveCriticalSection(&CS_Main); }
    void LockGames() { ::EnterCriticalSection(&CS_Games); }
    void UnLockGames() { ::LeaveCriticalSection(&CS_Games); }

    MComputer();
    ~MComputer();
};
//---------------------------------------------------------------------------
class MComputers:public MList
{
private:
    MThreadNetSync *ThreadNetSync[NUM_SyncThreads];

    MListItem *new_() { return (MListItem*)(new MComputer); }
    void delete_(MListItem *ListItem_) { delete ((MComputer*)ListItem_); }
public:
    MComputer *SearchID(unsigned int ID_);
    MComputer *SearchIP(AnsiString &IP_);
    //
    bool Save();
    bool Load();
    void Get(MCompParams *CompParams_);
    void Set(MCompParams *CompParams_);
    void Timer();
    int GetPosition();

    MComputers();
    ~MComputers();
};
//---------------------------------------------------------------------------
class MThreadNetSync : public TThread
{
private:
    CRITICAL_SECTION CS_Main;
    CRITICAL_SECTION CS_Computers;
    bool PausedSync;
    bool ExitThread;
    //
    MComputer *FirstComputer;
    //
    SOCKET RemoteSocket;
    bool Waiting;
    int WaitTime;
    //
    int Position;

    //
    int GetComputerDataSize();
    char *SetComputerData(char *Data_, MComputer *Computer_);
    //
    void Lock() { ::EnterCriticalSection(&CS_Main); }
    void UnLock() { ::LeaveCriticalSection(&CS_Main); }
    void LockComputers() { ::EnterCriticalSection(&CS_Computers); }
    void UnLockComputers() { ::LeaveCriticalSection(&CS_Computers); }
    void CloseSocket();
    bool Send(char *Buffer_, int Size_);
    void StartWaiting(int Time_);
    void StopWaiting();
    void Process(MComputer *Computer_);
    void __fastcall Terminate(TObject *Sender);
protected:
    void __fastcall Execute();
public:
    void Init(MComputers *Computers_);
    void Pause(bool Set_);
    void Exit();
    void Timer();
    int GetPosition();
    __fastcall MThreadNetSync(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif

