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
void MLog::AddSimpleEvent(unsigned char Type_)
{
    MLogRecordEvent *record;

    // ��������� ������ � ����� � ��������� ��������
    record=(MLogRecordEvent*)Records.Add(Type_);
    record->SystemTime=SystemTime;
}
//---------------------------------------------------------------------------
void MLog::AddStatesData(MStates *States_)
{
    MLogRecordDataStates *record;
    unsigned i;

    // ��������� ������ � ����� � ��������� ��������
    record=(MLogRecordDataStates*)Records.Add(mlrDataStates);
    record->SystemTime=SystemTime;
    // ������� ������ ��������� ����������� � ��������� ���
    record->States.Alloc(States_->gCount());
    i=0;
    for ( MState *state=(MState*)States_->gFirst(); state;
        state=(MState*)state->gNext(), i++ )
    {
        record->States[i]=*state;
    }
}
//---------------------------------------------------------------------------
void MLog::AddTariffsData(MTariffs *Tariffs_)
{
    MLogRecordDataTariffs *record;
    unsigned i;

    // ��������� ������ � ����� � ��������� ��������
    record=(MLogRecordDataTariffs*)Records.Add(mlrDataTariffs);
    record->SystemTime=SystemTime;
    // ������� ������ ������� � ��������� ���
    record->Tariffs.Alloc(Tariffs_->gCount());
    i=0;
    for ( MTariff *tariff=(MTariff*)Tariffs_->gFirst(); tariff;
        tariff=(MTariff*)tariff->gNext(), i++ )
    {
        record->Tariffs[i]=*tariff;
    }
}
//---------------------------------------------------------------------------
void MLog::AddFinesData(MFines *Fines_)
{
    MLogRecordDataFines *record;
    unsigned i;

    // ��������� ������ � ����� � ��������� ��������
    record=(MLogRecordDataFines*)Records.Add(mlrDataFines);
    record->SystemTime=SystemTime;
    // ������� ������ ������� � ��������� ���
    record->Fines.Alloc(Fines_->gCount());
    i=0;
    for ( MFine *fine=(MFine*)Fines_->gFirst();
        fine; fine=(MFine*)fine->gNext(), i++ )
    {
        record->Fines[i]=*fine;
    }
}
//---------------------------------------------------------------------------
void MLog::AddUsersData(MUsers *Users_)
{
    MLogRecordDataUsers *record;
    unsigned i;

    // ��������� ������ � ����� � ��������� ��������
    record=(MLogRecordDataUsers*)Records.Add(mlrDataUsers);
    record->SystemTime=SystemTime;
    // ������� ������ ������������� � ��������� ���
    record->Users.Alloc(Users_->ActiveCount());
    i=0;
    for ( MUser *user=(MUser*)Users_->gFirst(); user;
        user=(MUser*)user->gNext(), i++ )
    {
        if ( !user->Active ) continue;
        record->Users[i]=*user;
    }
}
//---------------------------------------------------------------------------
bool MLog::AddEvent(unsigned char Type_)
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
bool MLog::AddMode(unsigned char Type_, char Number_, bool Apply_)
{
    bool result=false;
    MLogRecordMode *record;

    if ( !Opened ) return false;

    Records.Clear();
    // ��������� ������ � ����� � ��������� ��������
    record=(MLogRecordMode*)Records.Add(Type_);
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
bool MLog::AddCmd(unsigned char Type_, char Number_)
{
    bool result=false;
    MLogRecordCmd *RecordCmd;

    if ( !Opened ) return false;

    Records.Clear();
    // ��������� ������ � ����� � ��������� ��������
    RecordCmd=(MLogRecordCmd*)Records.Add(Type_);
    RecordCmd->SystemTime=SystemTime;
    RecordCmd->Number=Number_;
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
    char file_name[MAX_PATH];
    int name_length;

    // ������������ ����� ������ � ��������� (�������) ����
    if ( !(Int64ToSystemTime(&BeginTime,&begin_time)&&
        Int64ToSystemTime(&SystemTime,&end_time)) ) goto error;

    // ���� � �����
    name_length=sprintf(file_name,"%s\\",Directory);
    // ������ ���� �������� �����
    name_length+=sprintf(file_name+name_length,"%4d.%02d.%02d - ",
        begin_time.wYear,begin_time.wMonth,begin_time.wDay);
    // ���� ���� ����������, �� ��������� ��� �������� �����
    if ( begin_time.wYear!=end_time.wYear )
        name_length+=sprintf(file_name+name_length,
            "%4d.",end_time.wYear);
    // ����� � ����� �������� �����
    name_length+=sprintf(file_name+name_length,
        "%02d.%02d",end_time.wMonth,end_time.wDay);
    // ���� ���� ������ � ������ � ���� ����, ��������� ����� ������������
    if ( (begin_time.wYear==end_time.wYear)&&
        (begin_time.wMonth==end_time.wMonth)&&
        (begin_time.wDay==end_time.wDay) )
    {
        name_length+=sprintf(file_name+name_length,
            " (%03x)",end_time.wMilliseconds);
    }
    // ��������� ����� �����
    sprintf(file_name+name_length,".TLG");
    // ��������������� ����
    if ( !::MoveFile(Records.DefaultFile.c_str(),file_name) ) goto error;

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
    AddSimpleEvent(mlrBegin);           // ����� ������ ������ ����
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
    MLogRecord *record;

    Opened=false;
    Records.Clear();
    // ��������� ��� ������
    if ( !Records.Load() ) goto error;

    // ���������� ���������� ��������
    record=(MLogRecord*)Records.gFirst();
    if ( (record==NULL)||(record->gTypeID()!=mlrBegin) ) goto error;
    // ���������� �����, ����� ��� ��� �����
    BeginTime=((MLogRecordBegin*)record)->SystemTime;
    //
    record=(MLogRecord*)record->gNext();
    if ( (record==NULL)||(record->gTypeID()!=mlrDataStates) ) goto error;
    record=(MLogRecord*)record->gNext();
    if ( (record==NULL)||(record->gTypeID()!=mlrDataTariffs) ) goto error;
    record=(MLogRecord*)record->gNext();
    if ( (record==NULL)||(record->gTypeID()!=mlrDataFines) ) goto error;
    record=(MLogRecord*)record->gNext();
    if ( (record==NULL)||(record->gTypeID()!=mlrDataUsers) ) goto error;
    if ( Records.gLast()->gTypeID()==mlrEnd ) goto error;
    Opened=true;

    // ���������� ����� ������������ ��������� ������ �����
    User=0;
    for ( record=(MLogRecord*)Records.gLast(); record;
        record=(MLogRecord*)record->gPrev() )
    {
        unsigned char type=record->gTypeID();
        if ( type==mlrLogOut ) break;
        else if ( type==mlrLogIn )
        {
            User=((MLogRecordLogIn*)record)->User;
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
    Opened=!AddEvent(mlrEnd);
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
    AddSimpleEvent(mlrStart);       // ������ ���������� ������
    AddStatesData(States_);         // ��������� �����������
    AddTariffsData(Tariffs_);       // �������� �������
    AddFinesData(Fines_);           // �������� �������
    AddUsersData(Users_);           // ������ � �������������
    result=Records.Attach(true);    // ��������� ����� ������ � �����
    // ������� �����
    Records.Clear();
    
    return result;
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
    MLogRecordConfig *record;

    if ( !Opened ) return false;
    
    Records.Clear();
    // ��������� ������ � �����
    record=(MLogRecordConfig*)Records.Add(mlrConfig);
    if ( record==NULL ) goto error;
    // ��������� ��������
    record->SystemTime=SystemTime;
    record->Open=Open_;
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
    AddSimpleEvent(mlrComputers);       // ������� ������ �����������
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
    AddSimpleEvent(mlrTariffs);         // ������� ������ �������
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
    AddSimpleEvent(mlrFines);           // ������� ������ �������
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
    AddSimpleEvent(mlrUsers);           // ������� ������ �������������
    AddUsersData(Users_);               // ������ �������������
    result=Records.Attach(true);        // ��������� ����� ������ � �����
    // ������� �����
    Records.Clear();

    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddOptions()
{
    return AddEvent(mlrOptions);
}
//---------------------------------------------------------------------------
bool MLog::AddLogIn(unsigned UserID_)
{
    bool result=false;
    MLogRecordLogIn *record;

    if ( !Opened ) return false;

    Records.Clear();
    // ��������� ������ � ����� � ��������� ��������
    record=(MLogRecordLogIn*)Records.Add(mlrLogIn);
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
    bool result=AddEvent(mlrLogOut);
    if ( result ) User=0;
    return result;
}
//---------------------------------------------------------------------------
bool MLog::AddRun(MRunTime *Time_)
{
    bool result=false;
    MLogRecordRun *record;

    if ( !Opened ) return false;

    Records.Clear();
    // ��������� ������ � �����
    record=(MLogRecordRun*)Records.Add(mlrRun);
    if ( record==NULL ) goto error;
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
    MLogRecordFine *record;

    if ( !Opened ) return false;

    Records.Clear();
    // ��������� ������
    record=(MLogRecordFine*)Records.Add(mlrFine);
    if ( record==NULL ) goto error;
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
    MLogRecordExchange *record;

    if ( !Opened ) return false;

    Records.Clear();
    // ��������� ������
    record=(MLogRecordExchange*)Records.Add(mlrExchange);
    if ( record==NULL ) goto error;
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
    return AddMode(mlrLock,Number_,Apply_);
}
//---------------------------------------------------------------------------
bool MLog::AddPause(char Number_, bool Apply_)
{
    return AddMode(mlrPause,Number_,Apply_);
}
//---------------------------------------------------------------------------
bool MLog::AddOpen(char Number_, bool Apply_)
{
    return AddMode(mlrOpen,Number_,Apply_);
}
//---------------------------------------------------------------------------
bool MLog::AddPowerOn(char Number_)
{
    return true;
//    return AddCmd(mlrPowerOn,Number_);   /// ��������, ����� �� �������� ���
}
//---------------------------------------------------------------------------
bool MLog::AddReboot(char Number_)
{
    return true;
//    return AddCmd(mlrReboot,Number_);
}
//---------------------------------------------------------------------------
bool MLog::AddShutdown(char Number_)
{
    return true;
//    return AddCmd(mlrShutdown,Number_);
}
//---------------------------------------------------------------------------
void MLog::SetDefault(const std::string &Dir_, unsigned Code_)
{
    Directory=Dir_;
    Records.SetDefaultFile(Dir_+"\\CURRENT.TLG",Code_);
}
//---------------------------------------------------------------------------
void MLog::Timer(__int64 SystemTime_)
{
    SystemTime=SystemTime_;
}
//---------------------------------------------------------------------------

