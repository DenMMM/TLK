//---------------------------------------------------------------------------
#ifndef UnitSendH
#define UnitSendH
//---------------------------------------------------------------------------
#include <windows.h>
#include <winsock2.h>
//---------------------------------------------------------------------------
#include "UnitComputers.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#define SEND_Version        0x30    // Версия сетевого интерфейса
#define SEND_SyncPort       7005    // Номер порта для отправки данных
#define SEND_MinData        8       // 
#define SEND_MaxData        32768   //
//---------------------------------------------------------------------------
#define mssWait         0
#define mssSend         1
#define mssGetGames     2
#define mssGetConfig    3
//---------------------------------------------------------------------------
// Main Send Type
#define mstAccept           0x01020304
//#define mstDeny             0x04030201
#define mstGames            1
#define mstConfig           2
#define mstGetGames         3
#define mstGetConfig        4
//---------------------------------------------------------------------------
// Main Send Event
#define mseConnecting       1   // Устанавливается соединение
#define mseSending          2   // Отправка данных
#define mseReceiving        3   // Прием данных
#define mseDisconnecting    4   // Разъединение
#define mseExecute          5   // Сеанс завершился успешно
#define mseNotConnect       6   // Не удалось соединиться с клиентом
#define msePrclError        7   // Ошибка протокола (разные версии или партнер не клиент TLK)
#define mseFreeParam        8   // Можно удалить поданные для групповой обработки данные
//---------------------------------------------------------------------------
class MSend
{
private:
    HWND Window;                // Окно для обработки сообщений о процессе отправки
    UINT MinMsg;                //
    HANDLE Thread;              // Дескриптор потока, осуществляющего отправку/прием
    DWORD ThreadID;             // Идентификатор потока
    SOCKET Socket;              // Сокет
    int Mode;
    MComputer **Computers;
    MGames *Games;
    MClOptions *Options;
    char *Data;                 // Данные для отправки
    unsigned DataSize;          // Размер данных
    bool Break;

    inline void Event(MComputer *Computer_, int Event_);
    bool Create();
    bool Connect(char *IP_, unsigned Time_);
    bool Send(char *Data_, unsigned Size_, unsigned Time_);
    bool Recv(char *Data_, unsigned Size_, unsigned Time_);
    bool Disconnect(unsigned Time_);
    void Close();
    static DWORD WINAPI ThreadFunc(LPVOID Data);
    void ThreadSend();
    void ThreadGet();
public:
    void SetShell(HWND Window_, UINT MinMsg_);
    bool Send(MComputer **Computers_, MGames *Games_, MClOptions *Options_);
    bool Get(MComputer *Computer_, MGames *Games_, MClOptions *Options_);
    void Stop();

    MSend();
    ~MSend();
};
//---------------------------------------------------------------------------
#endif

