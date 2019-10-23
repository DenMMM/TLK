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
    Opened=false;
    User=0;
}
//---------------------------------------------------------------------------
void MLog::AddSimpleEvent(MLogRecords::ItemType Type_)
{
	// Добавляем запись в буфер и заполняем атрибуты
	auto record=&dynamic_cast<MLogRecords::Event&>(
		*Records.Add(Type_));
    record->SystemTime=SystemTime;
}
//---------------------------------------------------------------------------
void MLog::AddStatesData(MStates *States_)
{
	// Добавляем запись в буфер и заполняем атрибуты
	auto record=&dynamic_cast<MLogRecords::DataStates&>(
		*Records.Add(MLogRecords::mlrDataStates));
	record->SystemTime=SystemTime;
	// Создаем массив состояний компьютеров и заполняем его
	record->Items.clear();
	record->Items.reserve(States_->gCount());
	for ( const auto &State: *States_ )
	{
		record->Items.push_back(State.gLogData());
	}
}
//---------------------------------------------------------------------------
void MLog::AddTariffsData(MTariffs *Tariffs_)
{
	// Добавляем запись в буфер и заполняем атрибуты
	auto record=&dynamic_cast<MLogRecords::DataTariffs&>(
		*Records.Add(MLogRecords::mlrDataTariffs));
	record->SystemTime=SystemTime;
	// Создаем массив тарифов и заполняем его
	record->Items.clear();
	record->Items.reserve(Tariffs_->gCount());
	for ( const auto &Tariff: *Tariffs_ )
	{
		record->Items.push_back(Tariff.gLogData());
	}
}
//---------------------------------------------------------------------------
void MLog::AddFinesData(MFines *Fines_)
{
	// Добавляем запись в буфер и заполняем атрибуты
	auto record=&dynamic_cast<MLogRecords::DataFines&>(
		*Records.Add(MLogRecords::mlrDataFines));
	record->SystemTime=SystemTime;
	// Создаем массив штрафов и заполняем его
	record->Items.clear();
	record->Items.reserve(Fines_->gCount());
	for ( const auto &Fine: *Fines_ )
	{
		record->Items.push_back(Fine.gLogData());
	}
}
//---------------------------------------------------------------------------
void MLog::AddUsersData(MUsers *Users_)
{
	// Добавляем запись в буфер и заполняем атрибуты
	auto record=&dynamic_cast<MLogRecords::DataUsers&>(
		*Records.Add(MLogRecords::mlrDataUsers));
	record->SystemTime=SystemTime;
	// Создаем массив пользователей и заполняем его
	record->Items.clear();
	record->Items.reserve(Users_->ActiveCount());
	for ( const auto &User: *Users_ )
	{
		if ( !User.Active ) continue;
		record->Items.push_back(User.gLogData());
	}
}
//---------------------------------------------------------------------------
bool MLog::AddEvent(MLogRecords::ItemType Type_)
{
	bool result;

	if ( !Opened ) return false;

	Records.Clear();
	// Добавляем запись в буфер и дописываем к файлу лога безопасно
    AddSimpleEvent(Type_);
    result=Records.Attach(true);
    // Очищаем буфер
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddMode(MLogRecords::ItemType Type_, char Number_, bool Apply_)
{
    bool result=false;
    MLogRecords::Mode *record;

    if ( !Opened ) return false;

    Records.Clear();
    // Добавляем запись в буфер и заполняем атрибуты
	record=&dynamic_cast<MLogRecords::Mode&>(*Records.Add(Type_));
    record->SystemTime=SystemTime;
    record->Number=Number_;
    record->Apply=Apply_;
    // Добавляем к файлу безопасно
    result=Records.Attach(true);
    // Очищаем буфер
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddCmd(MLogRecords::ItemType Type_, char Number_)
{
    bool result=false;
	MLogRecords::Cmd *record;

	if ( !Opened ) return false;

	Records.Clear();
	// Добавляем запись в буфер и заполняем атрибуты
	record=&dynamic_cast<MLogRecords::Cmd&>(*Records.Add(Type_));
	record->SystemTime=SystemTime;
	record->Number=Number_;
	// Добавляем к файлу безопасно
	result=Records.Attach(true);
	// Очищаем буфер
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::Rename() const
{
    SYSTEMTIME begin_time, end_time;
    wchar_t file_name[MAX_PATH];
    int name_length;

    // Конвертируем время начала и окончания (текущее) лога
    if ( !(Int64ToSystemTime(&BeginTime,&begin_time)&&
        Int64ToSystemTime(&SystemTime,&end_time)) ) goto error;

    // Путь к файлу
	name_length=swprintf(
		file_name, sizeof(file_name),
		L"%s\\", Directory.c_str());
	// Полная дата создания файла
	name_length+=swprintf(
		file_name+name_length, sizeof(file_name)-name_length,
		L"%4d.%02d.%02d - ", begin_time.wYear, begin_time.wMonth, begin_time.wDay);
    // Если года отличаются, то добавляем год закрытия файла
    if ( begin_time.wYear!=end_time.wYear )
		name_length+=swprintf(
			file_name+name_length, sizeof(file_name)-name_length,
			L"%4d.", end_time.wYear);
    // Месяц и число закрытия файла
	name_length+=swprintf(
		file_name+name_length, sizeof(file_name)-name_length,
		L"%02d.%02d", end_time.wMonth, end_time.wDay);
    // Если файл открыт и закрыт в один день, добавляем метку уникальности
    if ( (begin_time.wYear==end_time.wYear)&&
        (begin_time.wMonth==end_time.wMonth)&&
        (begin_time.wDay==end_time.wDay) )
    {
		name_length+=swprintf(
			file_name+name_length, sizeof(file_name)-name_length,
			L" (%03x)", end_time.wMilliseconds);
    }
    // Окончание имени файла
	swprintf(
		file_name+name_length, sizeof(file_name)-name_length,
		L".TLG");
    // Переименовываем файл
	if ( !::MoveFile(Records.DefaultFile.c_str(), file_name) ) goto error;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MLog::Begin(
    MShellState *ShellState_,
    MStates *States_,
    MTariffs *Tariffs_,
    MFines *Fines_,
    MUsers *Users_)
{
    Opened=false;

    Records.Clear();
    // Запоминаем время начала лога и переименуем файл, если он все еще существует
    BeginTime=SystemTime;
    Rename();
    // Сохраняем обязательные данные _без перезаписи_ файла
	AddSimpleEvent(
		MLogRecords::mlrBegin);			// Время начала нового лога
	AddStatesData(States_);             // Состояния компьютеров
    AddTariffsData(Tariffs_);           // Описания тарифов
    AddFinesData(Fines_);               // Описания штрафов
    AddUsersData(Users_);               // Данные о пользователях
    Opened=Records.Save(false,true);    // Без перезаписи файла и без кеша
    // Очищаем буфер
    Records.Clear();

    return Opened;
}
//---------------------------------------------------------------------------
bool MLog::Open()
{
	Opened=false;
	Records.Clear();

	// Загружаем все записи
	if ( !Records.Load() ) return false;	// goto error;

	// Производим простейшую проверку
	auto iRecord=Records.cbegin();
	auto iEnd=Records.cend();

	if ( (iRecord==iEnd)||
		(iRecord->gTypeID() != MLogRecords::LogBegin::TypeID) ) goto error;
	// Запоминаем время, когда лог был начат
	BeginTime=dynamic_cast<MLogRecords::LogBegin&>(*iRecord).SystemTime;
	//
	++iRecord;
	if ( (iRecord==iEnd)||
		(iRecord->gTypeID() != MLogRecords::DataStates::TypeID) ) goto error;

	++iRecord;
	if ( (iRecord==iEnd)||
		(iRecord->gTypeID() != MLogRecords::DataTariffs::TypeID) ) goto error;

	++iRecord;
	if ( (iRecord==iEnd)||
		(iRecord->gTypeID() != MLogRecords::DataFines::TypeID) ) goto error;

	++iRecord;
	if ( (iRecord==iEnd)||
		(iRecord->gTypeID() != MLogRecords::DataUsers::TypeID) ) goto error;

	++iRecord;
	if ( iRecord->gTypeID() == MLogRecords::LogEnd::TypeID ) goto error;
//	if ( Records.gLast()->gTypeID() == MLogRecords::LogEnd::TypeID ) goto error;
	Opened=true;

    // Определяем какой пользователь последним открыл смену
    User=0;
	for ( MLogRecordsItem *record=Records.gLast();
		record; record=record->gPrev() )
    {
		unsigned char type=record->gTypeID();
		if ( type == MLogRecords::AppLogOut::TypeID ) break;
		else if ( type == MLogRecords::AppLogIn::TypeID )
        {
			User=dynamic_cast<MLogRecords::AppLogIn&>(*record).User;
            break;
        }
    }
    // Очищаем буфер
    Records.Clear();

error:
    return Opened;
}
//---------------------------------------------------------------------------
bool MLog::End()
{
    if ( !Opened ) return false;
    
    // Добавляем запись о закрытии файла лога
	Opened=!AddEvent(MLogRecords::mlrEnd);
    // Если добавилась, пытаемся переименовать файл лога
    return (!Opened)&&Rename();
}
//---------------------------------------------------------------------------
bool MLog::CheckPeriod(int Period_) const
{
    bool result=false;
    SYSTEMTIME begin, current;

    if ( !Opened ) return false;
    
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
        default: break;
    }

error:
    return result;
}
//---------------------------------------------------------------------------
unsigned MLog::LastUser() const
{
    return Opened? User: 0;
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

    if ( !Opened ) return false;

    Records.Clear();
    // Добавляем записи в список и дописываем данные к файлу лога
	AddSimpleEvent(
		MLogRecords::mlrStart);				// Запуск админского модуля
	AddStatesData(States_);         		// Состояния компьютеров
	AddTariffsData(Tariffs_);       		// Описания тарифов
	AddFinesData(Fines_);           		// Описания штрафов
	AddUsersData(Users_);           		// Данные о пользователях
	result=Records.Attach(true);    		// Сохраняем новые записи в файле
    // Очищаем буфер
    Records.Clear();
    
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddStop()
{
    return AddEvent(MLogRecords::mlrStop);
}
//---------------------------------------------------------------------------
bool MLog::AddConfig(bool Open_)
{
    bool result=false;
    MLogRecords::AppConfig *record;

    if ( !Opened ) return false;
    
    Records.Clear();
    // Добавляем запись в буфер
	record=&dynamic_cast<MLogRecords::AppConfig&>(
		*Records.Add(MLogRecords::mlrConfig));
    // Заполняем атрибуты
    record->SystemTime=SystemTime;
    record->Opened=Open_;
    // Добавляем к файлу безопасно
    result=Records.Attach(true);
    // Очищаем буфер
    Records.Clear();

error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddComputers(MStates *States_)
{
    bool result;

    if ( !Opened ) return false;

    Records.Clear();
    // Добавляем записи в список и дописываем данные к файлу лога
	AddSimpleEvent(
		MLogRecords::mlrComputers);		// Изменен список компьютеров
    AddStatesData(States_);             // Данные состояний компьютеров
    result=Records.Attach(true);        // Сохраняем новые записи в файле
    // Очищаем буфер
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddTariffs(MTariffs *Tariffs_)
{
    bool result;

    if ( !Opened ) return false;

    Records.Clear();
    // Добавляем записи в список и дописываем данные к файлу лога
	AddSimpleEvent(
		MLogRecords::mlrTariffs);		// Изменен список тарифов
    AddTariffsData(Tariffs_);           // Данные тарифов
    result=Records.Attach(true);        // Сохраняем новые записи в файле
    // Очищаем буфер
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddFines(MFines *Fines_)
{
    bool result;

    if ( !Opened ) return false;

    Records.Clear();
    // Добавляем записи в список и дописываем данные к файлу лога
	AddSimpleEvent(
		MLogRecords::mlrFines);			// Изменен список штрафов
    AddFinesData(Fines_);               // Данные штрафов
    result=Records.Attach(true);        // Сохраняем новые записи в файле
    // Очищаем буфер
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddUsers(MUsers *Users_)
{
    bool result;

    if ( !Opened ) return false;

    Records.Clear();
    // Добавляем записи в список и дописываем данные к файлу лога
	AddSimpleEvent(
		MLogRecords::mlrUsers);			// Изменен список пользователей
    AddUsersData(Users_);               // Данные пользователей
    result=Records.Attach(true);        // Сохраняем новые записи в файле
    // Очищаем буфер
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddOptions()
{
    return AddEvent(MLogRecords::mlrOptions);
}
//---------------------------------------------------------------------------
bool MLog::AddLogIn(unsigned UserID_)
{
    bool result=false;
    MLogRecords::AppLogIn *record;

    if ( !Opened ) return false;

    Records.Clear();
    // Добавляем запись в буфер и заполняем атрибуты
	record=&dynamic_cast<MLogRecords::AppLogIn&>(
		*Records.Add(MLogRecords::mlrLogIn));
	record->SystemTime=SystemTime;
    record->User=UserID_;
    // Добавляем к файлу безопасно
    result=Records.Attach(true);
    // Очищаем буфер
    Records.Clear();

    if ( result ) User=UserID_;
    
error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddLogOut()
{
    bool result=AddEvent(MLogRecords::mlrLogOut);
    if ( result ) User=0;
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddRun(MTariffRunTimesItem *Time_)
{
    bool result=false;
    MLogRecords::CompRun *record;

    if ( !Opened ) return false;

    Records.Clear();
    // Добавляем запись в буфер
	record=&dynamic_cast<MLogRecords::CompRun&>(
		*Records.Add(MLogRecords::mlrRun));
    // Заполняем атрибуты
    record->SystemTime=SystemTime;
    record->Number=Time_->Number;
    record->Tariff=Time_->TariffID;
    record->StartTime=Time_->StartTime;
    record->Type=Time_->Type;
    record->BeginTime=Time_->BeginTime;
    record->EndTime=Time_->EndTime;
    record->SizeTime=Time_->SizeTime;
    record->WorkTime=Time_->WorkTime;
    record->Cost=Time_->Cost;
    // Добавляем к файлу безопасно
    result=Records.Attach(true);
    // Очищаем буфер
    Records.Clear();

error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddFine(char Number_, unsigned FineID_, short Time_)
{
    bool result=false;
    MLogRecords::CompFine *record;

    if ( !Opened ) return false;

    Records.Clear();
    // Добавляем запись
	record=&dynamic_cast<MLogRecords::CompFine&>(
		*Records.Add(MLogRecords::mlrFine));
    // Заполняем атрибуты
    record->SystemTime=SystemTime;
    record->Number=Number_;
    record->Fine=FineID_;
    record->Time=Time_;
    // Добавляем к файлу безопасно
    result=Records.Attach(true);
    // Очищаем буфер
    Records.Clear();

error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddExchange(char From_, char To_)
{
    bool result=false;
    MLogRecords::CompExchange *record;

    if ( !Opened ) return false;

    Records.Clear();
    // Добавляем запись
	record=&dynamic_cast<MLogRecords::CompExchange&>(
		*Records.Add(MLogRecords::mlrExchange));
    // Заполняем атрибуты
    record->SystemTime=SystemTime;
    record->From=From_;
    record->To=To_;
    // Добавляем к файлу безопасно
    result=Records.Attach(true);
    // Очищаем буфер
    Records.Clear();

error:
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddLock(char Number_, bool Apply_)
{
	return AddMode(MLogRecords::mlrLock, Number_, Apply_);
}
//---------------------------------------------------------------------------
bool MLog::AddPause(char Number_, bool Apply_)
{
	return AddMode(MLogRecords::mlrPause, Number_, Apply_);
}
//---------------------------------------------------------------------------
bool MLog::AddOpen(char Number_, bool Apply_)
{
    return AddMode(MLogRecords::mlrOpen, Number_, Apply_);
}
//---------------------------------------------------------------------------
bool MLog::AddPowerOn(char Number_)
{
    return true;
//    return AddCmd(MLogRecords::mlrPowerOn, Number_);   /// отключил, чтобы не засорять лог
}
//---------------------------------------------------------------------------
bool MLog::AddReboot(char Number_)
{
    return true;
//    return AddCmd(MLogRecords::mlrReboot, Number_);
}
//---------------------------------------------------------------------------
bool MLog::AddShutdown(char Number_)
{
	return true;
//    return AddCmd(MLogRecords::mlrShutdown, Number_);
}
//---------------------------------------------------------------------------
void MLog::SetDefault(const std::wstring &Dir_, unsigned Code_)
{
    Directory=Dir_;
    Records.SetDefaultFile(Dir_+L"\\CURRENT.TLG",Code_);
}
//---------------------------------------------------------------------------
void MLog::Timer(__int64 SystemTime_)
{
    SystemTime=SystemTime_;
}
//---------------------------------------------------------------------------

