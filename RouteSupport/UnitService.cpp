//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitService.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
DWORD ServiceThreadID;
SERVICE_STATUS_HANDLE service_status_handle;
SERVICE_STATUS service_status;
//---------------------------------------------------------------------------
MState State;
//---------------------------------------------------------------------------
VOID WINAPI Handler(DWORD fdwControl)
{
    UINT Message;
    switch(fdwControl)
    {
        case SERVICE_CONTROL_STOP: Message=scmSTOP; break;
//        case SERVICE_CONTROL_INTERROGATE: Message=scmINTERROGATE; break;
        case SERVICE_CONTROL_SHUTDOWN: Message=scmSHUTDOWN; break;
        default: Message=0; break;
    }
    if ( Message ) ::PostThreadMessage(ServiceThreadID,Message,0,0);
}
//---------------------------------------------------------------------------
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    HKEY Key=NULL;
    HANDLE Event;
    MSG Message;

    // Берем идентификатор потока для его использования обработчиком запросов от SCM
    ServiceThreadID=::GetCurrentThreadId();
    // Создаем очередь сообщений для потока
    ::PeekMessage(NULL,NULL,0,0,PM_NOREMOVE);
    // Регистрируем обработчик запросов от SCM к службе
    service_status_handle=::RegisterServiceCtrlHandler("TLKRoute",&Handler);

    // Информируем SCM о том, что идет запуск службы
    service_status.dwServiceType=SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState=SERVICE_START_PENDING;
    service_status.dwControlsAccepted=//SERVICE_CONTROL_INTERROGATE+
        SERVICE_ACCEPT_STOP+SERVICE_ACCEPT_SHUTDOWN;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=1;
    service_status.dwWaitHint=5000;
    ::SetServiceStatus(service_status_handle,&service_status);

    //
    if ( (Event=::CreateEvent(NULL,FALSE,FALSE,"TLK_RegChange_Event"))==NULL ) goto exit;
    if ( (::RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\MMM Groups\\TLK\\3.0\\Client",
        0,KEY_NOTIFY|KEY_QUERY_VALUE,&Key))!=ERROR_SUCCESS ) goto exit;
//    ServiceStartProcess();

    // Считываем настройки клиента
    if ( StateLoad(&State) ) Route(&State);

    // Информируем SCM об успешном запуске службы
    service_status.dwCurrentState=SERVICE_RUNNING;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);

    while(true)
    {
        // Регистрируем ожидание изменений в реестре
        if ( (::RegNotifyChangeKeyValue(Key,FALSE,
            REG_NOTIFY_CHANGE_LAST_SET,Event,TRUE))!=ERROR_SUCCESS ) break;
        // Ждем события и периодически проверяем очередь сообщений потока
        while(true)
        {
            if ( ::WaitForSingleObject(Event,200)==WAIT_OBJECT_0 )
                { StateLoad(&State); break; }
            else if ( Timer(&State) ) break;
            // Проверяем очередь сообщений
            if ( !::PeekMessage(&Message,NULL,0,0,PM_REMOVE) ) continue;
            //
            if ( (Message.message==scmSTOP)||
                (Message.message==scmSHUTDOWN) ) goto exit;
//            if ( Message.message==scmINTERROGATE ) ;
        }
        Route(&State);
    }

exit:
    service_status.dwCurrentState=SERVICE_STOP_PENDING;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);

    //
    ::CloseHandle(Event);
    ::RegCloseKey(Key);
//    ServiceStartProcess();

    // Информируем SCM об успешном останове службы
    service_status.dwCurrentState=SERVICE_STOPPED;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);
}
//---------------------------------------------------------------------------
void ServiceStartProcess()
{
    service_status.dwCheckPoint++;
    service_status.dwWaitHint=5000;
    ::SetServiceStatus(service_status_handle,&service_status);
}
//---------------------------------------------------------------------------
void BasicEncode(char *Data_, unsigned DataSize_, unsigned Code_)
{
    Data_+=DataSize_-sizeof(unsigned);

    while(DataSize_>=sizeof(unsigned))
    {
        *((unsigned*)Data_)-=Code_;
        Data_--; DataSize_--;
    }
}
//---------------------------------------------------------------------------
bool StateLoad(MState *State_)
{
    HKEY key=NULL;
    unsigned long data_size;

    // Открываем
    if ( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\MMM Groups\\TLK\\3.0\\Client",
        NULL,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS ) goto error;
    // Определяем размер
    if ( ::RegQueryValueEx(key,"State",NULL,NULL,NULL,&data_size)!=ERROR_SUCCESS ) goto error;
    // Проверяем его на допустимость
    if ( data_size!=sizeof(MState) ) goto error;
    // Считываем данные
    if ( ::RegQueryValueEx(key,"State",NULL,NULL,(char*)State_,&data_size)!=ERROR_SUCCESS ) goto error;
    ::RegCloseKey(key); key=NULL;

    BasicEncode((char*)State_,data_size,0xE2B7C9F9);
    // Дополнительные проверки
    if ( (State_->Size!=(data_size-sizeof(unsigned)*2))||(State_->Zero!=0) ) goto error;

    return true;
error:
    if ( key!=NULL ) ::RegCloseKey(key);
    return false;
}
//---------------------------------------------------------------------------
bool GetLocalTimeInt64(__int64 *lpInt64)
{
    SYSTEMTIME ssTime;
    FILETIME FileTime;
    LARGE_INTEGER LargeInteger;
    ::GetLocalTime(&ssTime);
    if ( !::SystemTimeToFileTime(&ssTime,&FileTime) ) return false;
    memcpy(&LargeInteger,&FileTime,sizeof(LargeInteger));
    *lpInt64=LargeInteger.QuadPart;
    return true;
}
//---------------------------------------------------------------------------
bool Timer(MState *State_)
{
    __int64 SystemTime;

    // Берем системное время
    if ( !GetLocalTimeInt64(&SystemTime) ) return false;
    // Проверяем окончание времени работы и времени штрафа
    if ( (!(State_->State&mcsWork))||(State_->State&(mcsPause|mcsOpen))||
        (SystemTime<(State_->StartWorkTime+State_->SizeWorkTime*60*10000000i64)) )
    {
        if ( (!(State_->State&mcsFine))||
            (SystemTime<(State_->StartFineTime+State_->SizeFineTime*60*10000000i64)) ) return false;
        State_->StartFineTime=0; State_->SizeFineTime=0;
        State_->State&=~mcsFine;
    } else
    {
        State_->State=mcsFree|(State_->State&mcsOpen);
        State_->StartWorkTime=0; State_->SizeWorkTime=0;
        State_->StartFineTime=0; State_->SizeFineTime=0;
        State_->StopTimerTime=0;
        State_->Programs=0;
    }
    return true;
}
//---------------------------------------------------------------------------
void Route(MState *State_)
{
    ULONG TableSize;
    PMIB_IFTABLE TableIF=NULL;
    PMIB_IFROW RecordIF;
    PIP_PER_ADAPTER_INFO AdapterInfo;
    MIB_IPFORWARDROW Record;
    bool Enable;

    // Определяем добавить или удалить маршрут
    Enable=(State_->State&mcsOpen)||
        ((State_->State&mcsWork)&&
        (!(State_->State&(mcsFine|mcsPause)))&&
        (State.Programs&mgpRoute));

    // Запрашиваем размер таблицы интерфейсов
    TableSize=0;
    if ( ::GetIfTable(NULL,&TableSize,TRUE)!=ERROR_INSUFFICIENT_BUFFER ) goto error;
    // Выделяем память под таблицу
    if ( (TableIF=(PMIB_IFTABLE)new char[TableSize])==NULL ) goto error;
    memset(TableIF,0,TableSize);
    // Берем таблицу
    if ( ::GetIfTable(TableIF,&TableSize,TRUE)!=0 ) goto error;

    // Ищем в таблице интерфейсов первый типа Ethernet
    for ( DWORD i=0; i<TableIF->dwNumEntries; i++ )
    {
        RecordIF=&TableIF->table[i];
        if ( RecordIF->dwType!=IF_TYPE_ETHERNET_CSMACD ) continue;
        // Запрашиваем дополнительные параметры конфигурации адаптера
        TableSize=0;
        if ( ::GetPerAdapterInfo(RecordIF->dwIndex,NULL,&TableSize)!=
            ERROR_BUFFER_OVERFLOW ) continue;
        if ( (AdapterInfo=(PIP_PER_ADAPTER_INFO)new char[TableSize])==NULL ) goto error2;
        if ( ::GetPerAdapterInfo(RecordIF->dwIndex,AdapterInfo,&TableSize)!=0 ) goto error2;
        // Подготавливаем запись для таблицы маршрутизации
        memset(&Record,0,sizeof(Record));
        Record.dwForwardIfIndex=RecordIF->dwIndex;
        Record.dwForwardDest=0;
        Record.dwForwardMask=0;
        Record.dwForwardNextHop=::inet_addr(AdapterInfo->DnsServerList.IpAddress.String);
        // Проверяем был ли указан корректный адрес DNS-сервера
        if ( Record.dwForwardNextHop==INADDR_NONE ) goto error2;
        Record.dwForwardPolicy=0;
//        Record.dwForwardType=4;
//        Record.dwForwardProto=3;
        // Добавляем/удаляем запись в таблицу
        if ( Enable ) ::CreateIpForwardEntry(&Record);
        else ::DeleteIpForwardEntry(&Record);
error2:
        delete[] AdapterInfo;
        break;
    }

error:
    delete[] TableIF;
}
//---------------------------------------------------------------------------

