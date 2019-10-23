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
	// ��������� ������ � ����� � ��������� ��������
	auto record=&dynamic_cast<MLogRecords::Event&>(
		*Records.Add(Type_));
    record->SystemTime=SystemTime;
}
//---------------------------------------------------------------------------
void MLog::AddStatesData(MStates *States_)
{
	// ��������� ������ � ����� � ��������� ��������
	auto record=&dynamic_cast<MLogRecords::DataStates&>(
		*Records.Add(MLogRecords::mlrDataStates));
	record->SystemTime=SystemTime;
	// ������� ������ ��������� ����������� � ��������� ���
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
	// ��������� ������ � ����� � ��������� ��������
	auto record=&dynamic_cast<MLogRecords::DataTariffs&>(
		*Records.Add(MLogRecords::mlrDataTariffs));
	record->SystemTime=SystemTime;
	// ������� ������ ������� � ��������� ���
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
	// ��������� ������ � ����� � ��������� ��������
	auto record=&dynamic_cast<MLogRecords::DataFines&>(
		*Records.Add(MLogRecords::mlrDataFines));
	record->SystemTime=SystemTime;
	// ������� ������ ������� � ��������� ���
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
	// ��������� ������ � ����� � ��������� ��������
	auto record=&dynamic_cast<MLogRecords::DataUsers&>(
		*Records.Add(MLogRecords::mlrDataUsers));
	record->SystemTime=SystemTime;
	// ������� ������ ������������� � ��������� ���
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
	// ��������� ������ � ����� � ���������� � ����� ���� ���������
    AddSimpleEvent(Type_);
    result=Records.Attach(true);
    // ������� �����
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
    // ��������� ������ � ����� � ��������� ��������
	record=&dynamic_cast<MLogRecords::Mode&>(*Records.Add(Type_));
    record->SystemTime=SystemTime;
    record->Number=Number_;
    record->Apply=Apply_;
    // ��������� � ����� ���������
    result=Records.Attach(true);
    // ������� �����
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
	// ��������� ������ � ����� � ��������� ��������
	record=&dynamic_cast<MLogRecords::Cmd&>(*Records.Add(Type_));
	record->SystemTime=SystemTime;
	record->Number=Number_;
	// ��������� � ����� ���������
	result=Records.Attach(true);
	// ������� �����
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::Rename() const
{
    SYSTEMTIME begin_time, end_time;
    wchar_t file_name[MAX_PATH];
    int name_length;

    // ������������ ����� ������ � ��������� (�������) ����
    if ( !(Int64ToSystemTime(&BeginTime,&begin_time)&&
        Int64ToSystemTime(&SystemTime,&end_time)) ) goto error;

    // ���� � �����
	name_length=swprintf(
		file_name, sizeof(file_name),
		L"%s\\", Directory.c_str());
	// ������ ���� �������� �����
	name_length+=swprintf(
		file_name+name_length, sizeof(file_name)-name_length,
		L"%4d.%02d.%02d - ", begin_time.wYear, begin_time.wMonth, begin_time.wDay);
    // ���� ���� ����������, �� ��������� ��� �������� �����
    if ( begin_time.wYear!=end_time.wYear )
		name_length+=swprintf(
			file_name+name_length, sizeof(file_name)-name_length,
			L"%4d.", end_time.wYear);
    // ����� � ����� �������� �����
	name_length+=swprintf(
		file_name+name_length, sizeof(file_name)-name_length,
		L"%02d.%02d", end_time.wMonth, end_time.wDay);
    // ���� ���� ������ � ������ � ���� ����, ��������� ����� ������������
    if ( (begin_time.wYear==end_time.wYear)&&
        (begin_time.wMonth==end_time.wMonth)&&
        (begin_time.wDay==end_time.wDay) )
    {
		name_length+=swprintf(
			file_name+name_length, sizeof(file_name)-name_length,
			L" (%03x)", end_time.wMilliseconds);
    }
    // ��������� ����� �����
	swprintf(
		file_name+name_length, sizeof(file_name)-name_length,
		L".TLG");
    // ��������������� ����
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
    // ���������� ����� ������ ���� � ����������� ����, ���� �� ��� ��� ����������
    BeginTime=SystemTime;
    Rename();
    // ��������� ������������ ������ _��� ����������_ �����
	AddSimpleEvent(
		MLogRecords::mlrBegin);			// ����� ������ ������ ����
	AddStatesData(States_);             // ��������� �����������
    AddTariffsData(Tariffs_);           // �������� �������
    AddFinesData(Fines_);               // �������� �������
    AddUsersData(Users_);               // ������ � �������������
    Opened=Records.Save(false,true);    // ��� ���������� ����� � ��� ����
    // ������� �����
    Records.Clear();

    return Opened;
}
//---------------------------------------------------------------------------
bool MLog::Open()
{
	Opened=false;
	Records.Clear();

	// ��������� ��� ������
	if ( !Records.Load() ) return false;	// goto error;

	// ���������� ���������� ��������
	auto iRecord=Records.cbegin();
	auto iEnd=Records.cend();

	if ( (iRecord==iEnd)||
		(iRecord->gTypeID() != MLogRecords::LogBegin::TypeID) ) goto error;
	// ���������� �����, ����� ��� ��� �����
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

    // ���������� ����� ������������ ��������� ������ �����
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
    // ������� �����
    Records.Clear();

error:
    return Opened;
}
//---------------------------------------------------------------------------
bool MLog::End()
{
    if ( !Opened ) return false;
    
    // ��������� ������ � �������� ����� ����
	Opened=!AddEvent(MLogRecords::mlrEnd);
    // ���� ����������, �������� ������������� ���� ����
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
    // ��������� ������ � ������ � ���������� ������ � ����� ����
	AddSimpleEvent(
		MLogRecords::mlrStart);				// ������ ���������� ������
	AddStatesData(States_);         		// ��������� �����������
	AddTariffsData(Tariffs_);       		// �������� �������
	AddFinesData(Fines_);           		// �������� �������
	AddUsersData(Users_);           		// ������ � �������������
	result=Records.Attach(true);    		// ��������� ����� ������ � �����
    // ������� �����
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
    // ��������� ������ � �����
	record=&dynamic_cast<MLogRecords::AppConfig&>(
		*Records.Add(MLogRecords::mlrConfig));
    // ��������� ��������
    record->SystemTime=SystemTime;
    record->Opened=Open_;
    // ��������� � ����� ���������
    result=Records.Attach(true);
    // ������� �����
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
    // ��������� ������ � ������ � ���������� ������ � ����� ����
	AddSimpleEvent(
		MLogRecords::mlrComputers);		// ������� ������ �����������
    AddStatesData(States_);             // ������ ��������� �����������
    result=Records.Attach(true);        // ��������� ����� ������ � �����
    // ������� �����
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddTariffs(MTariffs *Tariffs_)
{
    bool result;

    if ( !Opened ) return false;

    Records.Clear();
    // ��������� ������ � ������ � ���������� ������ � ����� ����
	AddSimpleEvent(
		MLogRecords::mlrTariffs);		// ������� ������ �������
    AddTariffsData(Tariffs_);           // ������ �������
    result=Records.Attach(true);        // ��������� ����� ������ � �����
    // ������� �����
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddFines(MFines *Fines_)
{
    bool result;

    if ( !Opened ) return false;

    Records.Clear();
    // ��������� ������ � ������ � ���������� ������ � ����� ����
	AddSimpleEvent(
		MLogRecords::mlrFines);			// ������� ������ �������
    AddFinesData(Fines_);               // ������ �������
    result=Records.Attach(true);        // ��������� ����� ������ � �����
    // ������� �����
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddUsers(MUsers *Users_)
{
    bool result;

    if ( !Opened ) return false;

    Records.Clear();
    // ��������� ������ � ������ � ���������� ������ � ����� ����
	AddSimpleEvent(
		MLogRecords::mlrUsers);			// ������� ������ �������������
    AddUsersData(Users_);               // ������ �������������
    result=Records.Attach(true);        // ��������� ����� ������ � �����
    // ������� �����
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
    // ��������� ������ � ����� � ��������� ��������
	record=&dynamic_cast<MLogRecords::AppLogIn&>(
		*Records.Add(MLogRecords::mlrLogIn));
	record->SystemTime=SystemTime;
    record->User=UserID_;
    // ��������� � ����� ���������
    result=Records.Attach(true);
    // ������� �����
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
    // ��������� ������ � �����
	record=&dynamic_cast<MLogRecords::CompRun&>(
		*Records.Add(MLogRecords::mlrRun));
    // ��������� ��������
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
    // ��������� � ����� ���������
    result=Records.Attach(true);
    // ������� �����
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
    // ��������� ������
	record=&dynamic_cast<MLogRecords::CompFine&>(
		*Records.Add(MLogRecords::mlrFine));
    // ��������� ��������
    record->SystemTime=SystemTime;
    record->Number=Number_;
    record->Fine=FineID_;
    record->Time=Time_;
    // ��������� � ����� ���������
    result=Records.Attach(true);
    // ������� �����
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
    // ��������� ������
	record=&dynamic_cast<MLogRecords::CompExchange&>(
		*Records.Add(MLogRecords::mlrExchange));
    // ��������� ��������
    record->SystemTime=SystemTime;
    record->From=From_;
    record->To=To_;
    // ��������� � ����� ���������
    result=Records.Attach(true);
    // ������� �����
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
//    return AddCmd(MLogRecords::mlrPowerOn, Number_);   /// ��������, ����� �� �������� ���
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

