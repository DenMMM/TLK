//---------------------------------------------------------------------------
#ifndef UnitSharedH
#define UnitSharedH
//---------------------------------------------------------------------------
#include <string>
//#include <winnt.h>
#include <winsock2.h>
//---------------------------------------------------------------------------
class MShared;
//---------------------------------------------------------------------------
#define SHRD_LockWait       500     // Время ожидания доступа к общим данным (мсек)
//---------------------------------------------------------------------------
// Сообщения (картинки)
#define mimNone             0       // Скрыть картинку с сообщением
#define mimEndTime          1       // Время закончилось
#define mimLocked           2       // Компьютер закрыт
#define mimPaused           3       // Компьютер прикрыт адмиинстратором или аварийно
#define mimFine             4       // Применен штраф
#define mimTimePaused       5       // Время приостановлено
//---------------------------------------------------------------------------
class MShared
{
private:
    struct MSharedData
    {
        volatile __int64 SysTime;       // Системное время
        volatile unsigned SysTimeVer;
        volatile int CompNum;           // Номер компьютера
        volatile unsigned CompNumVer;
        volatile int WorkTime;          // Оставшееся время работы
        volatile unsigned WorkTimeVer;
//        volatile int FineTime;          // Оставшееся время штрафа
//        volatile unsigned FineTimeVer;
        volatile bool WarnMsg;          // Флаг - отображать предупреждение об окончании времени
        volatile unsigned WarnMsgVer;
        volatile int ImageMsg;          // Сообщение экрана блокировки
        volatile unsigned ImageMsgVer;
        volatile unsigned Games;        // Разрешенные для запуска группы программ
        volatile unsigned GamesVer;
        volatile bool ConfigMode;       // Флаг - режим настройки
        volatile unsigned ConfigModeVer;
        volatile bool Transp;           // Флаг - прозрачность TLK Shell
        volatile unsigned TranspVer;
    } *pData;                   // Указатель на данные в общем блоке памяти

    // Увеличить счетчик версии shared-поля с пропуском нуля
    void NextVer(volatile unsigned *Ver_)
    {
        if ( (++(*Ver_))==0 ) ++(*Ver_);
    }

    HANDLE hMap;                // Handle общего блока памяти
    HANDLE hMtxMap;             // Handle мьютекса для синхронизации доступа к нему
    HANDLE hMtxLive;            // Handle мьютекса-маркера существования службы

#pragma pack(push, 1)
    // Передаваемые дочерним процессам параметры
    struct MInheritData
    {
        HANDLE hMap;
        HANDLE hMtxMap;
        HANDLE hMtxLive;
    };
#pragma pack(pop)
    
public:
    // Вернуть HEX-строку с параметрами для дочернего процесса (служба)
	std::wstring InhToHEX() const;

    // Создать shared-секцию (служба)
    bool Create();
    // Открыть shared-секцию (дочерний процесс)
    bool Open(wchar_t *InhLine_);
    // Закрыть shared-секцию и освободить системные ресурсы
    void Close();

    bool IsLive() const {
        return ::WaitForSingleObject(hMtxLive,0)!=WAIT_ABANDONED; }
    bool Lock() const {
        return ::WaitForSingleObject(hMtxMap,SHRD_LockWait)==WAIT_OBJECT_0; }
    bool UnLock() const {
        return ::ReleaseMutex(hMtxMap); }

    // Обновление shared-полей управляющей службой
    void UpdateSysTime(__int64 SysTime_);
    void UpdateCompNum(int Num_);
    void UpdateWorkTime(int Time_);
    void ShowWarnMsg(bool Show_);
    void ShowImageMessage(int Msg_);
    void ShowGames(unsigned Games_);
    void SetConfigMode(bool Mode_);
    void SetTransp(bool Transp_);

    // Проверка версии поля и получение его значения дочерним процессом
	bool CheckSysTime(__int64 &SysTime_, unsigned &SysTimeVer_) const;
	bool CheckCompNum(int &Num_, unsigned &NumVer_) const;
	bool CheckWorkTime(int &Time_, unsigned &TimeVer_) const;
	bool CheckWarnMsg(bool &Msg_, unsigned &MsgVer_) const;
	bool CheckImageMessage(int &Msg_, unsigned &MsgVer_) const;
	bool CheckGames(unsigned &Games_, unsigned &GamesVer_) const;
	bool CheckConfigMode(bool &Mode_, unsigned &ModeVer_) const;
	bool CheckTransp(bool &Transp_, unsigned &TranspVer_) const;

    MShared()
    {
        pData=NULL;
        hMap=NULL;
        hMtxMap=NULL;
        hMtxLive=NULL;
    }
    ~MShared()
    {
        Close();
    }
};
//---------------------------------------------------------------------------
#endif
