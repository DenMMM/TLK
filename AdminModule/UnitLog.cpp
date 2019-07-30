//---------------------------------------------------------------------------
#include <stdio.h>
#pragma hdrstop

#include "UnitLog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MLog::MLog()
{
    SystemTime=0;
    BeginTime=0;
    Directory=NULL;
}
//---------------------------------------------------------------------------
MLog::~MLog()
{
    delete[] Directory;
    Records.Clear();
}
//---------------------------------------------------------------------------
void MLog::Timer(__int64 SystemTime_)
{
    SystemTime=SystemTime_;
}
//---------------------------------------------------------------------------
bool MLog::SetDefault(char *Dir_, unsigned Code_)
{
    char file_name[MAX_PATH];

    delete[] Directory;
    Directory=new char[strlen(Dir_)+1];
    if ( Directory==NULL ) goto error;
    strcpy(Directory,Dir_);

    sprintf(file_name,"%s\\CURRENT.TLG",Dir_);
    return Records.SetDefaultFile(file_name,Code_);
error:
    return false;
}
//---------------------------------------------------------------------------
bool MLog::AddStatesData(MStates *States_)
{
    MLogRecordDataStates *RecordStates;
    MStateData *StateData;

    RecordStates=(MLogRecordDataStates*)Records.Add(mlrDataStates);
    if ( RecordStates==NULL ) goto error;
    RecordStates->SystemTime=SystemTime;
    RecordStates->NumStates=States_->Count;
    RecordStates->States=new MStateData[RecordStates->NumStates];
    if ( RecordStates->States==NULL ) goto error;
    StateData=RecordStates->States;
    for ( MState *State=(MState*)States_->First; State;
        State=(MState*)State->Next )
    {
        State->GetStateData(StateData);
        StateData++;
    }

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MLog::AddTariffsData(MTariffs *Tariffs_)
{
    MLogRecordDataTariffs *RecordTariffs;
    MTariffData *TariffData;

    RecordTariffs=(MLogRecordDataTariffs*)Records.Add(mlrDataTariffs);
    if ( RecordTariffs==NULL ) goto error;
    RecordTariffs->SystemTime=SystemTime;
    RecordTariffs->NumTariffs=Tariffs_->Count;
    RecordTariffs->Tariffs=new MTariffData[RecordTariffs->NumTariffs];
    if ( RecordTariffs->Tariffs==NULL ) goto error;
    TariffData=RecordTariffs->Tariffs;
    for ( MTariff *Tariff=(MTariff*)Tariffs_->First; Tariff;
        Tariff=(MTariff*)Tariff->Next )
    {
        Tariff->GetTariffData(TariffData);
        TariffData++;
    }

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MLog::AddFinesData(MFines *Fines_)
{
    MLogRecordDataFines *RecordFines;
    MFineData *FineData;

    RecordFines=(MLogRecordDataFines*)Records.Add(mlrDataFines);
    if ( RecordFines==NULL ) goto error;
    RecordFines->SystemTime=SystemTime;
    RecordFines->NumFines=Fines_->Count;
    RecordFines->Fines=new MFineData[RecordFines->NumFines];
    if ( RecordFines->Fines==NULL ) goto error;
    FineData=RecordFines->Fines;
    for ( MFine *Fine=(MFine*)Fines_->First; Fine;
        Fine=(MFine*)Fine->Next )
    {
        Fine->GetFineData(FineData);
        FineData++;
    }

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MLog::AddUsersData(MUsers *Users_)
{
    MLogRecordDataUsers *RecordUsers;
    MUserData *UserData;

    RecordUsers=(MLogRecordDataUsers*)Records.Add(mlrDataUsers);
    if ( RecordUsers==NULL ) goto error;
    RecordUsers->SystemTime=SystemTime;
    RecordUsers->NumUsers=0;
    for ( MUser *User=(MUser*)Users_->First; User;
        User=(MUser*)User->Next ) if ( User->Active ) RecordUsers->NumUsers++;
    if ( RecordUsers->NumUsers )
    {
        RecordUsers->Users=new MUserData[RecordUsers->NumUsers];
        if ( RecordUsers->Users==NULL ) goto error;
        UserData=RecordUsers->Users;
        for ( MUser *User=(MUser*)Users_->First; User;
            User=(MUser*)User->Next )
        {
            if ( !User->Active ) continue;
            User->GetUserData(UserData);
            UserData++;
        }
    } else RecordUsers->Users=NULL;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MLog::AddSimpleEvent(unsigned Type_)
{
    MLogRecordEvent *RecordEvent;
    if ( (RecordEvent=(MLogRecordEvent*)Records.Add(Type_))==NULL ) return false;
    RecordEvent->SystemTime=SystemTime;
    return true;
}
//---------------------------------------------------------------------------
bool MLog::AddEvent(unsigned Type_)
{
    bool result;
    // Добавляем запись в список и дописываем данные к файлу лога
    result=AddSimpleEvent(Type_)&&Records.Attach();
    // Очищаем список записей лога
    Records.Clear();
    return result;
}
//---------------------------------------------------------------------------
bool MLog::Begin(
    MShellState *ShellState_,
    MStates *States_,
    MTariffs *Tariffs_,
    MFines *Fines_,
    MUsers *Users_)
{
    // На всякий случай очищаем список записей лога
    Records.Clear();
    // Сохраняем в файле начальные обязательные записи
    if ( !(AddSimpleEvent(mlrBegin)&&           // Время начала нового лога
        AddStatesData(States_)&&                // Состояния компьютеров
        AddTariffsData(Tariffs_)&&              // Описания тарифов
        AddFinesData(Fines_)&&                  // Описания штрафов
        AddUsersData(Users_)&&                  // Данные о пользователях
        Records.Save()) ) goto error;           // Сохраняем новые записи в файле
    // Сохраняем время начала заполения лога
    BeginTime=SystemTime;
    // Очищаем список записей лога
    Records.Clear();

    return true;
error:
    Records.Clear();
    return false;
}
//---------------------------------------------------------------------------
bool MLog::Open()
{
    MLogRecord *Record;

    // На всякий случай очищаем список записей лога
    Records.Clear();
    // Загружаем все записи
    if ( !Records.Load() ) goto error;
    // Производим простейшую проверку
    Record=(MLogRecord*)Records.First;
    if ( (Record==NULL)||(Record->TypeID()!=mlrBegin) ) goto error;
    BeginTime=((MLogRecordBegin*)Record)->SystemTime;
    Record=(MLogRecord*)Record->Next;
    if ( (Record==NULL)||(Record->TypeID()!=mlrDataStates) ) goto error; Record=(MLogRecord*)Record->Next;
    if ( (Record==NULL)||(Record->TypeID()!=mlrDataTariffs) ) goto error; Record=(MLogRecord*)Record->Next;
    if ( (Record==NULL)||(Record->TypeID()!=mlrDataFines) ) goto error; Record=(MLogRecord*)Record->Next;
    if ( (Record==NULL)||(Record->TypeID()!=mlrDataUsers) ) goto error;
    if ( Records.Last->TypeID()==mlrEnd ) goto error;
    // Определяем какой пользователь последним открыл смену
    User=0; Record=(MLogRecord*)Records.Last;
    while(Record)
    {
        if ( Record->TypeID()==mlrLogOut ) break;
        else if ( Record->TypeID()==mlrLogIn )
            { User=((MLogRecordLogIn*)Record)->User; break; }
        Record=(MLogRecord*)Record->Prev;
    }
    // Очищаем список записей лога
    Records.Clear();

    return true;
error:
    Records.Clear();
    return false;
}
//---------------------------------------------------------------------------
unsigned MLog::LastUser()
{
    return User;
}
//---------------------------------------------------------------------------
bool MLog::CheckPeriod(int Period_)
{
    bool result=false;
    SYSTEMTIME begin, current;

    if ( !(Int64ToSystemTime(&BeginTime,&begin)&&
        Int64ToSystemTime(&SystemTime,&current)) ) goto error;

    switch(Period_)
    {
        case mlpDay:
            if ( (SystemTime/(24*3600*10000000i64))!=
                (BeginTime/(24*3600*10000000i64)) ) result=true;
            break;
        case mlpWeek:
            if ( (SystemTime/(7*24*3600*10000000i64))!=
                (BeginTime/(7*24*3600*10000000i64)) ) result=true;
            break;
        case mlpMonth:
            if ( (begin.wYear!=current.wYear)||
                (begin.wMonth!=current.wMonth) ) result=true;
            break;
        case mlpQuarter:
            if ( (begin.wYear!=current.wYear)||
                ((begin.wMonth/3)!=(current.wMonth/3)) ) result=true;
            break;
        case mlpYear:
            if ( begin.wYear!=current.wYear ) result=true;
            break;
        default: break;
    }

error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::End()
{
    MLogRecordEnd *RecordEnd;
    SYSTEMTIME begin_time, end_time;
    char file_name[MAX_PATH];
    int name_length;

    // Добавляем запись о закрытии файла лога
    if ( !AddEvent(mlrEnd) ) goto error;

    // Конвертируем время начала и окончания лога
    if ( !(Int64ToSystemTime(&BeginTime,&begin_time)&&
        Int64ToSystemTime(&SystemTime,&end_time)) ) goto error;

    // Путь к файлу
    name_length=sprintf(file_name,"%s\\",Directory);
    // Полная дата создания файла
    name_length+=sprintf(file_name+name_length,"%4d.%02d.%02d - ",
        begin_time.wYear,begin_time.wMonth,begin_time.wDay);
    // Если года отличаются, то добавляем год закрытия файла
    if ( begin_time.wYear!=end_time.wYear )
        name_length+=sprintf(file_name+name_length,
            "%4d.",end_time.wYear);
    // Месяц и число закрытия файла
    name_length+=sprintf(file_name+name_length,
        "%02d.%02d",end_time.wMonth,end_time.wDay);
    // Если, вдруг, файл открыт и закрыт в один день, добавляем метку уникальности
    if ( (begin_time.wYear==end_time.wYear)&&
        (begin_time.wMonth==end_time.wMonth)&&
        (begin_time.wDay==end_time.wDay) )
    {
        name_length+=sprintf(file_name+name_length,
            " (%03x)",end_time.wMilliseconds);
    }
    // Окончание имени файла
    sprintf(file_name+name_length,".TLG");
    // Переименовываем файл
    if ( !::MoveFile(Records.DefaultFile,file_name) ) goto error;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MLog::AddStart(
    MShellState *ShellState_,
    MStates *States_,
    MTariffs *Tariffs_,
    MFines *Fines_,
    MUsers *Users_)
{
    bool result;
    // Добавляем записи в список и дописываем данные к файлу лога
    result=AddSimpleEvent(mlrStart)&&   // Запуск админского модуля
        AddStatesData(States_)&&        // Состояния компьютеров
        AddTariffsData(Tariffs_)&&      // Описания тарифов
        AddFinesData(Fines_)&&          // Описания штрафов
        AddUsersData(Users_)&&          // Данные о пользователях
        Records.Attach();               // Сохранаяем новые записи в файле
    // Очищаем список записей лога
    Records.Clear();
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddWork()
{
    return AddEvent(mlrWork);
}
//---------------------------------------------------------------------------
bool MLog::AddStop()
{
    return AddEvent(mlrStop);
}
//---------------------------------------------------------------------------
bool MLog::AddConfig(bool Open_)
{
    bool result=false;
    MLogRecordConfig *RecordConfig;

    // Добавляем запись
    if ( (RecordConfig=(MLogRecordConfig*)Records.Add(mlrConfig))==NULL ) goto error;
    RecordConfig->SystemTime=SystemTime;
    RecordConfig->Open=Open_;
    // Сохранаяем новые записи в файле
    result=Records.Attach();
    // Очищаем список записей лога
    Records.Clear();
error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddComputers(MStates *States_)
{
    bool result;
    // Добавляем записи в список и дописываем данные к файлу лога
    result=AddSimpleEvent(mlrComputers)&&   // Изменен список компьютеров
        AddStatesData(States_)&&            // Данные состояний компьютеров
        Records.Attach();                   // Сохранаяем новые записи в файле
    // Очищаем список записей лога
    Records.Clear();
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddTariffs(MTariffs *Tariffs_)
{
    bool result;
    // Добавляем записи в список и дописываем данные к файлу лога
    result=AddSimpleEvent(mlrTariffs)&&     // Изменен список тарифов
        AddTariffsData(Tariffs_)&&          // Данные тарифов
        Records.Attach();                   // Сохранаяем новые записи в файле
    // Очищаем список записей лога
    Records.Clear();
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddFines(MFines *Fines_)
{
    bool result;
    // Добавляем записи в список и дописываем данные к файлу лога
    result=AddSimpleEvent(mlrFines)&&   // Изменен список штрафов
        AddFinesData(Fines_)&&          // Данные штрафов
        Records.Attach();               // Сохранаяем новые записи в файле
    // Очищаем список записей лога
    Records.Clear();
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddUsers(MUsers *Users_)
{
    bool result;
    // Добавляем записи в список и дописываем данные к файлу лога
    result=AddSimpleEvent(mlrUsers)&&   // Изменен список пользователей
        AddUsersData(Users_)&&          // Данные пользователей
        Records.Attach();               // Сохранаяем новые записи в файле
    // Очищаем список записей лога
    Records.Clear();
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddOptions()
{
    return AddEvent(mlrOptions);
}
//---------------------------------------------------------------------------
bool MLog::AddLogIn(unsigned ID_)
{
    bool result=false;
    MLogRecordLogIn *RecordLogIn;

    // Добавляем запись
    if ( (RecordLogIn=(MLogRecordLogIn*)Records.Add(mlrLogIn))==NULL ) goto error;
    RecordLogIn->SystemTime=SystemTime;
    RecordLogIn->User=ID_;
    // Сохранаяем новые записи в файле
    result=Records.Attach();
    // Очищаем список записей лога
    Records.Clear();
error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddLogOut()
{
    return AddEvent(mlrLogOut);
}
//---------------------------------------------------------------------------
bool MLog::AddRun(MRunTime *Time_)
{
    bool result=false;
    MLogRecordRun *RecordRun;

    // Добавляем запись
    if ( (RecordRun=(MLogRecordRun*)Records.Add(mlrRun))==NULL ) goto error;
    RecordRun->SystemTime=SystemTime;
    RecordRun->Number=Time_->Number;
    RecordRun->Tariff=Time_->TariffID;
    RecordRun->StartTime=Time_->StartTime;
    RecordRun->Type=Time_->Type;
    RecordRun->BeginTime=Time_->BeginTime;
    RecordRun->EndTime=Time_->EndTime;
    RecordRun->SizeTime=Time_->SizeTime;
    RecordRun->WorkTime=Time_->WorkTime;
    RecordRun->Cost=Time_->Cost;
    // Сохранаяем новые записи в файле
    result=Records.Attach();
    // Очищаем список записей лога
    Records.Clear();
error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddFine(int Number_, unsigned Fine_, int Time_)
{
    bool result=false;
    MLogRecordFine *RecordFine;

    // Добавляем запись
    if ( (RecordFine=(MLogRecordFine*)Records.Add(mlrFine))==NULL ) goto error;
    RecordFine->SystemTime=SystemTime;
    RecordFine->Number=Number_;
    RecordFine->Fine=Fine_;
    RecordFine->Time=Time_;
    // Сохранаяем новые записи в файле
    result=Records.Attach();
    // Очищаем список записей лога
    Records.Clear();
error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddExchange(int From_, int To_)
{
    bool result=false;
    MLogRecordExchange *RecordExchange;

    // Добавляем запись
    if ( (RecordExchange=(MLogRecordExchange*)Records.Add(mlrExchange))==NULL ) goto error;
    RecordExchange->SystemTime=SystemTime;
    RecordExchange->From=From_;
    RecordExchange->To=To_;
    // Сохранаяем новые записи в файле
    result=Records.Attach();
    // Очищаем список записей лога
    Records.Clear();
error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddMode(unsigned Type_, int Number_, bool Apply_)
{
    bool result=false;
    MLogRecordMode *RecordMode;

    // Добавляем запись
    if ( (RecordMode=(MLogRecordMode*)Records.Add(Type_))==NULL ) goto error;
    RecordMode->SystemTime=SystemTime;
    RecordMode->Number=Number_;
    RecordMode->Apply=Apply_;
    // Сохранаяем новые записи в файле
    result=Records.Attach();
    // Очищаем список записей лога
    Records.Clear();
error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddLock(int Number_, bool Apply_)
{
    return AddMode(mlrLock,Number_,Apply_);
}
//---------------------------------------------------------------------------
bool MLog::AddPause(int Number_, bool Apply_)
{
    return AddMode(mlrPause,Number_,Apply_);
}
//---------------------------------------------------------------------------
bool MLog::AddOpen(int Number_, bool Apply_)
{
    return AddMode(mlrOpen,Number_,Apply_);
}
//---------------------------------------------------------------------------
bool MLog::AddWtLocker(int Number_, bool Apply_)
{
    return AddMode(mlrWtLocker,Number_,Apply_);
}
//---------------------------------------------------------------------------
bool MLog::AddCmd(unsigned Type_, int Number_)
{
    bool result=false;
    MLogRecordCmd *RecordCmd;

    // Добавляем запись
    if ( (RecordCmd=(MLogRecordCmd*)Records.Add(Type_))==NULL ) goto error;
    RecordCmd->SystemTime=SystemTime;
    RecordCmd->Number=Number_;
    // Сохранаяем новые записи в файле
    result=Records.Attach();
    // Очищаем список записей лога
    Records.Clear();
error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddPowerOn(int Number_)
{
    return AddCmd(mlrPowerOn,Number_);
}
//---------------------------------------------------------------------------
bool MLog::AddReboot(int Number_)
{
    return AddCmd(mlrReboot,Number_);
}
//---------------------------------------------------------------------------
bool MLog::AddShutdown(int Number_)
{
    return AddCmd(mlrShutdown,Number_);
}
//---------------------------------------------------------------------------

