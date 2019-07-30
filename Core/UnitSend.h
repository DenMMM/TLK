//---------------------------------------------------------------------------
#ifndef UnitSendH
#define UnitSendH
//---------------------------------------------------------------------------
#include <winsock2.h>
//#include <windows.h>
//---------------------------------------------------------------------------
#include "UnitStates.h"
#include "UnitComputers.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#define SEND_Version        0x31    // Версия сетевого интерфейса
#define SEND_Port           7005    // Номер порта клиента
#define SEND_MinData        8       // Ограничения на размер данных
#define SEND_MaxData        32768   // в процессе обмена 
//---------------------------------------------------------------------------
// Что сервер хочет отправить клиенту/скачать с него
#define mstAccept           0x01020304  // Подтверждение клиентом успешной доставки
//#define mstDeny             0x04030201
#define mstGames            1       // Список игр для клиента
#define mstConfig           2       // Настройки для клиента
#define mstGetGames         3       // Запрос списка игр
#define mstGetConfig        4       // Запрос настроек клиента
//---------------------------------------------------------------------------
#define mssWait             0
#define mssSend             1
#define mssGetGames         2
#define mssGetConfig        3
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
    SOCKET Socket;              // Сокет для соединения с клиентом
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
class MSendCl
{
private:
    HANDLE Thread;              // Дескриптор потока, осуществляющего отправку/прием
    DWORD ThreadID;             // Идентификатор потока
    SOCKET Socket;              // Сокет для ожидания соединений
    SOCKET RemoteSocket;        // Сокет для примема/отправки после принятия соединения
    char *Data;                 // Данные для приема/отправки
    MStateCl *State;            // Кому сообщать об обновлении списка игр, настроек  

    bool Create();
    bool Listen();
    bool Accept();
    bool Send(char *Data_, unsigned Size_, unsigned Time_);
    bool Recv(char *Data_, unsigned Size_, unsigned Time_);
    bool Disconnect(unsigned Time_);
    void CloseRemote();
    void Close();
    static DWORD WINAPI ThreadFunc(LPVOID Data);
    void ThreadProc();
    bool ProcessRecv(unsigned Type_);
    bool ProcessSend(unsigned Type_);
public:
    void Associate(MStateCl *State_);
    bool Start();
    void Stop();

    MSendCl();
    ~MSendCl();
};
//---------------------------------------------------------------------------
#endif

