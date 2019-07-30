//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitLogRecords.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

MListItem *MLogRecords::operator_new(unsigned Type_)
{
    MListItem *Item=NULL;

    switch(Type_)
    {
        // Файл лога
        case mlrBegin: Item=new MLogRecordBegin; break;
        case mlrEnd: Item=new MLogRecordEnd; break;
        // Админский модуль
        case mlrStart: Item=new MLogRecordStart; break;
        case mlrWork: Item=new MLogRecordWork; break;
        case mlrStop: Item=new MLogRecordStop; break;
        // Настройки админского модуля
        case mlrConfig: Item=new MLogRecordConfig; break;
        case mlrComputers: Item=new MLogRecordComputers; break;
        case mlrTariffs: Item=new MLogRecordTariffs; break;
        case mlrFines: Item=new MLogRecordFines; break;
        case mlrUsers: Item=new MLogRecordUsers; break;
        case mlrOptions: Item=new MLogRecordOptions; break;
        // Пользователи
        case mlrLogIn: Item=new MLogRecordLogIn; break;
        case mlrLogOut: Item=new MLogRecordLogOut; break;
        // Команды компьютерам
        case mlrRun: Item=new MLogRecordRun; break;
        case mlrFine: Item=new MLogRecordFine; break;
        case mlrExchange: Item=new MLogRecordExchange; break;
        case mlrLock: Item=new MLogRecordLock; break;
        case mlrPause: Item=new MLogRecordPause; break;
        case mlrOpen: Item=new MLogRecordOpen; break;
        case mlrWtLocker: Item=new MLogRecordWtLocker; break;
        case mlrPowerOn: Item=new MLogRecordPowerOn; break;
        case mlrReboot: Item=new MLogRecordReboot; break;
        case mlrShutdown: Item=new MLogRecordShutdown; break;
        // Служебные данные
        case mlrDataShellState: Item=new MLogRecordDataShellState; break;
        case mlrDataStates: Item=new MLogRecordDataStates; break;
        case mlrDataTariffs: Item=new MLogRecordDataTariffs; break;
        case mlrDataFines: Item=new MLogRecordDataFines; break;
        case mlrDataUsers: Item=new MLogRecordDataUsers; break;
        default: break;
    }

    return Item;
}

void MLogRecords::operator_delete(MListItem *DelItem_)
{
    switch(DelItem_->TypeID())
    {
        // Файл лога
        case mlrBegin: delete (MLogRecordBegin*)DelItem_; break;
        case mlrEnd: delete (MLogRecordEnd*)DelItem_; break;
        // Админский модуль
        case mlrStart: delete (MLogRecordStart*)DelItem_; break;
        case mlrWork: delete (MLogRecordWork*)DelItem_; break;
        case mlrStop: delete (MLogRecordStop*)DelItem_; break;
        // Настройки админского модуля
        case mlrConfig: delete (MLogRecordConfig*)DelItem_; break;
        case mlrComputers: delete (MLogRecordComputers*)DelItem_; break;
        case mlrTariffs: delete(MLogRecordTariffs*)DelItem_; break;
        case mlrFines: delete (MLogRecordFines*)DelItem_; break;
        case mlrUsers: delete (MLogRecordUsers*)DelItem_; break;
        case mlrOptions: delete (MLogRecordOptions*)DelItem_; break;
        // Пользователи
        case mlrLogIn: delete (MLogRecordLogIn*)DelItem_; break;
        case mlrLogOut: delete (MLogRecordLogOut*)DelItem_; break;
        // Команды компьютерам
        case mlrRun: delete (MLogRecordRun*)DelItem_; break;
        case mlrFine: delete (MLogRecordFine*)DelItem_; break;
        case mlrExchange: delete (MLogRecordExchange*)DelItem_; break;
        case mlrLock: delete (MLogRecordLock*)DelItem_; break;
        case mlrPause: delete (MLogRecordPause*)DelItem_; break;
        case mlrOpen: delete (MLogRecordOpen*)DelItem_; break;
        case mlrWtLocker: delete (MLogRecordWtLocker*)DelItem_; break;
        case mlrPowerOn: delete (MLogRecordPowerOn*)DelItem_; break;
        case mlrReboot: delete (MLogRecordReboot*)DelItem_; break;
        case mlrShutdown: delete (MLogRecordShutdown*)DelItem_; break;
        // Служебные данные
        case mlrDataShellState: delete (MLogRecordDataShellState*)DelItem_; break;
        case mlrDataStates: delete (MLogRecordDataStates*)DelItem_; break;
        case mlrDataTariffs: delete (MLogRecordDataTariffs*)DelItem_; break;
        case mlrDataFines: delete (MLogRecordDataFines*)DelItem_; break;
        case mlrDataUsers: delete (MLogRecordDataUsers*)DelItem_; break;
        default: break;
    }
}

//---------------------------------------------------------------------------

MLogRecordEvent::MLogRecordEvent()
{
    SystemTime=0;
}

MLogRecordEvent::~MLogRecordEvent()
{
}

bool MLogRecordEvent::Copy(MListItem *SrcItem_)
{
    MLogRecordEvent *RecordEvent_=(MLogRecordEvent*)SrcItem_;
    SystemTime=RecordEvent_->SystemTime;
    return true;
}

unsigned MLogRecordEvent::GetDataSize()
{
    return
        sizeof(SystemTime);
}

char *MLogRecordEvent::SetData(char *Data_)
{
    Data_=MemSet(Data_,SystemTime);
    return Data_;
}

char *MLogRecordEvent::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordConfig::MLogRecordConfig()
{
    SystemTime=0;
    Open=false;
}

MLogRecordConfig::~MLogRecordConfig()
{
}

bool MLogRecordConfig::Copy(MListItem *SrcItem_)
{
    MLogRecordConfig *RecordConfig_=(MLogRecordConfig*)SrcItem_;
    SystemTime=RecordConfig_->SystemTime;
    Open=RecordConfig_->Open;
    return true;
}

unsigned MLogRecordConfig::GetDataSize()
{
    return
        sizeof(SystemTime)+
        sizeof(Open);
}

char *MLogRecordConfig::SetData(char *Data_)
{
    Data_=MemSet(Data_,SystemTime);
    return Data_;
}

char *MLogRecordConfig::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordLogIn::MLogRecordLogIn()
{
    SystemTime=0;
    User=0;
}

MLogRecordLogIn::~MLogRecordLogIn()
{
}

bool MLogRecordLogIn::Copy(MListItem *SrcItem_)
{
    MLogRecordLogIn *RecordLogIn_=(MLogRecordLogIn*)SrcItem_;
    SystemTime=RecordLogIn_->SystemTime;
    User=RecordLogIn_->User;
    return true;
}

unsigned MLogRecordLogIn::GetDataSize()
{
    return
        sizeof(SystemTime)+
        sizeof(User);
}

char *MLogRecordLogIn::SetData(char *Data_)
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,User);
    return Data_;
}

char *MLogRecordLogIn::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&User,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordRun::MLogRecordRun()
{
    SystemTime=0;
    Number=0;
    Tariff=0;
    StartTime=0;
    Type=mttUndefined;
    BeginTime=EndTime=0;
    SizeTime=WorkTime=0;
    Cost=0.;
}

MLogRecordRun::~MLogRecordRun()
{
}

bool MLogRecordRun::Copy(MListItem *SrcItem_)
{
    MLogRecordRun *RecordRun_=(MLogRecordRun*)SrcItem_;
    SystemTime=RecordRun_->SystemTime;
    Number=RecordRun_->Number;
    Tariff=RecordRun_->Tariff;
    StartTime=RecordRun_->StartTime;
    Type=RecordRun_->Type;
    BeginTime=RecordRun_->BeginTime;
    EndTime=RecordRun_->EndTime;
    SizeTime=RecordRun_->SizeTime;
    WorkTime=RecordRun_->WorkTime;
    Cost=RecordRun_->Cost;
    return true;
}

unsigned MLogRecordRun::GetDataSize()
{
    return
        sizeof(SystemTime)+
        sizeof(Number)+
        sizeof(Tariff)+
        sizeof(StartTime)+
        sizeof(Type)+
        sizeof(BeginTime)+
        sizeof(EndTime)+
        sizeof(SizeTime)+
        sizeof(WorkTime)+
        sizeof(Cost);
}

char *MLogRecordRun::SetData(char *Data_)
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,Tariff);
    Data_=MemSet(Data_,StartTime);
    Data_=MemSet(Data_,Type);
    Data_=MemSet(Data_,BeginTime);
    Data_=MemSet(Data_,EndTime);
    Data_=MemSet(Data_,SizeTime);
    Data_=MemSet(Data_,WorkTime);
    Data_=MemSet(Data_,Cost);
    return Data_;
}

char *MLogRecordRun::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Number,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Tariff,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&StartTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Type,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&BeginTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&EndTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&SizeTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&WorkTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Cost,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordFine::MLogRecordFine()
{
    SystemTime=0;
    Number=0;
    Fine=0;
    Time=0;
}

MLogRecordFine::~MLogRecordFine()
{
}

bool MLogRecordFine::Copy(MListItem *SrcItem_)
{
    MLogRecordFine *RecordFine_=(MLogRecordFine*)SrcItem_;
    SystemTime=RecordFine_->SystemTime;
    Number=RecordFine_->Number;
    Fine=RecordFine_->Fine;
    Time=RecordFine_->Time;
    return true;
}

unsigned MLogRecordFine::GetDataSize()
{
    return
        sizeof(SystemTime)+
        sizeof(Number)+
        sizeof(Fine)+
        sizeof(Time);
}

char *MLogRecordFine::SetData(char *Data_)
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,Fine);
    Data_=MemSet(Data_,Time);
    return Data_;
}

char *MLogRecordFine::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Number,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Fine,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Time,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordExchange::MLogRecordExchange()
{
    SystemTime=0;
    From=To=0;
}

MLogRecordExchange::~MLogRecordExchange()
{
}

bool MLogRecordExchange::Copy(MListItem *SrcItem_)
{
    MLogRecordExchange *RecordExchange_=(MLogRecordExchange*)SrcItem_;
    SystemTime=RecordExchange_->SystemTime;
    From=RecordExchange_->From;
    To=RecordExchange_->To;
    return true;
}

unsigned MLogRecordExchange::GetDataSize()
{
    return
        sizeof(SystemTime)+
        sizeof(From)+
        sizeof(To);
}

char *MLogRecordExchange::SetData(char *Data_)
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,From);
    Data_=MemSet(Data_,To);
    return Data_;
}

char *MLogRecordExchange::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&From,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&To,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordMode::MLogRecordMode()
{
    SystemTime=0;
    Number=0;
    Apply=false;
}

MLogRecordMode::~MLogRecordMode()
{
}

bool MLogRecordMode::Copy(MListItem *SrcItem_)
{
    MLogRecordMode *RecordMode_=(MLogRecordMode*)SrcItem_;
    SystemTime=RecordMode_->SystemTime;
    Number=RecordMode_->Number;
    Apply=RecordMode_->Apply;
    return true;
}

unsigned MLogRecordMode::GetDataSize()
{
    return
        sizeof(SystemTime)+
        sizeof(Number)+
        sizeof(Apply);
}

char *MLogRecordMode::SetData(char *Data_)
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,Apply);
    return Data_;
}

char *MLogRecordMode::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Number,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Apply,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordCmd::MLogRecordCmd()
{
    SystemTime=0;
    Number=0;
}

MLogRecordCmd::~MLogRecordCmd()
{
}

bool MLogRecordCmd::Copy(MListItem *SrcItem_)
{
    MLogRecordCmd *RecordCmd_=(MLogRecordCmd*)SrcItem_;
    SystemTime=RecordCmd_->SystemTime;
    Number=RecordCmd_->Number;
    return true;
}

unsigned MLogRecordCmd::GetDataSize()
{
    return
        sizeof(SystemTime)+
        sizeof(Number);
}

char *MLogRecordCmd::SetData(char *Data_)
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,Number);
    return Data_;
}

char *MLogRecordCmd::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Number,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordDataShellState::MLogRecordDataShellState()
{
    SystemTime=0;
    State=0;
    User=0;
}

MLogRecordDataShellState::~MLogRecordDataShellState()
{
}

bool MLogRecordDataShellState::Copy(MListItem *SrcItem_)
{
    MLogRecordDataShellState *RecordDataShellState_=(MLogRecordDataShellState*)SrcItem_;
    SystemTime=RecordDataShellState_->SystemTime;
    State=RecordDataShellState_->State;
    User=RecordDataShellState_->User;
    return true;
}

unsigned MLogRecordDataShellState::GetDataSize()
{
    return
        sizeof(SystemTime)+
        sizeof(State)+
        sizeof(User);
}

char *MLogRecordDataShellState::SetData(char *Data_)
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,State);
    Data_=MemSet(Data_,User);
    return Data_;
}

char *MLogRecordDataShellState::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&State,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&User,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------
MLogRecordDataStates::MLogRecordDataStates()
{
    SystemTime=0;
    NumStates=0;
    States=NULL;
}

MLogRecordDataStates::~MLogRecordDataStates()
{
    delete[] States; States=NULL;
}

bool MLogRecordDataStates::Copy(MListItem *SrcItem_)
{
    MLogRecordDataStates *RecordStates_=(MLogRecordDataStates*)SrcItem_;
    SystemTime=RecordStates_->SystemTime;
    NumStates=RecordStates_->NumStates;
    delete[] States; if ( (States=new MStateData[NumStates])==NULL ) return false;
    memcpy(States,RecordStates_->States,sizeof(MStateData)*NumStates);
    return true;
}

unsigned MLogRecordDataStates::GetDataSize()
{
    unsigned Size;

    Size=sizeof(SystemTime)+sizeof(NumStates);
    if ( NumStates>0 )
        Size+=(
            sizeof(States->Number)+
            sizeof(States->State)+
            sizeof(States->TariffID)+
            sizeof(States->StartWorkTime)+
            sizeof(States->SizeWorkTime)+
            sizeof(States->StartFineTime)+
            sizeof(States->SizeFineTime)+
            sizeof(States->StopTimerTime))*NumStates;

    return Size;
}

char *MLogRecordDataStates::SetData(char *Data_)
{
    MStateData *State=States;

    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,NumStates);
    for ( unsigned i=0; i<NumStates; i++, State++ )
    {
        Data_=MemSet(Data_,State->Number);
        Data_=MemSet(Data_,State->State);
        Data_=MemSet(Data_,State->TariffID);
        Data_=MemSet(Data_,State->StartWorkTime);
        Data_=MemSet(Data_,State->SizeWorkTime);
        Data_=MemSet(Data_,State->StartFineTime);
        Data_=MemSet(Data_,State->SizeFineTime);
        Data_=MemSet(Data_,State->StopTimerTime);
    }

    return Data_;
}

char *MLogRecordDataStates::GetData(char *Data_, char *Limit_)
{
    MStateData *State;

    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&NumStates,Limit_))==NULL ) goto error;
    if ( NumStates>MAX_Computers ) goto error;

    delete[] States; if ( (States=new MStateData[NumStates])==NULL ) goto error;

    State=States;
    for ( unsigned i=0; i<NumStates; i++, State++ )
    {
        if ( (Data_=MemGet(Data_,&State->Number,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&State->State,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&State->TariffID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&State->StartWorkTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&State->SizeWorkTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&State->StartFineTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&State->SizeFineTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&State->StopTimerTime,Limit_))==NULL ) goto error;
    }

    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordDataTariffs::MLogRecordDataTariffs()
{
    SystemTime=0;
    NumTariffs=0;
    Tariffs=NULL;
}

MLogRecordDataTariffs::~MLogRecordDataTariffs()
{
    delete[] Tariffs; Tariffs=NULL;
}

bool MLogRecordDataTariffs::Copy(MListItem *SrcItem_)
{
    MLogRecordDataTariffs *RecordTariffs_=(MLogRecordDataTariffs*)SrcItem_;
    SystemTime=RecordTariffs_->SystemTime;
    NumTariffs=RecordTariffs_->NumTariffs;
    delete[] Tariffs; if ( (Tariffs=new MTariffData[NumTariffs])==NULL ) return false;
    memcpy(Tariffs,RecordTariffs_->Tariffs,sizeof(MTariffData)*NumTariffs);
    return true;
}

unsigned MLogRecordDataTariffs::GetDataSize()
{
    unsigned Size; MTariffData *Tariff=Tariffs;

    Size=sizeof(SystemTime)+sizeof(NumTariffs);
    for ( unsigned i=0; i<NumTariffs; i++, Tariff++ )
    {
        Size+=
            sizeof(Tariff->ID)+
            strlen(Tariff->Name)+1;
    }

    return Size;
}

char *MLogRecordDataTariffs::SetData(char *Data_)
{
    MTariffData *Tariff=Tariffs;

    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,NumTariffs);
    for ( unsigned i=0; i<NumTariffs; i++, Tariff++ )
    {
        Data_=MemSet(Data_,Tariff->ID);
        Data_=MemSetCLine(Data_,Tariff->Name);
    }

    return Data_;
}

char *MLogRecordDataTariffs::GetData(char *Data_, char *Limit_)
{
    MTariffData *Tariff;

    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&NumTariffs,Limit_))==NULL ) goto error;
    if ( NumTariffs>MAX_Tariffs ) goto error;

    delete[] Tariffs; if ( (Tariffs=new MTariffData[NumTariffs])==NULL ) goto error;

    Tariff=Tariffs;
    for ( unsigned i=0; i<NumTariffs; i++, Tariff++ )
    {
        if ( (Data_=MemGet(Data_,&Tariff->ID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,Tariff->Name,MAX_TariffNameLength,Limit_))==NULL ) goto error;
    }

    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordDataFines::MLogRecordDataFines()
{
    SystemTime=0;
    NumFines=0;
    Fines=NULL;
}

MLogRecordDataFines::~MLogRecordDataFines()
{
    delete[] Fines; Fines=NULL;
}

bool MLogRecordDataFines::Copy(MListItem *SrcItem_)
{
    MLogRecordDataFines *RecordFines_=(MLogRecordDataFines*)SrcItem_;
    SystemTime=RecordFines_->SystemTime;
    NumFines=RecordFines_->NumFines;
    delete[] Fines; if ( (Fines=new MFineData[NumFines])==NULL ) return false;
    memcpy(Fines,RecordFines_->Fines,sizeof(MFineData)*NumFines);
    return true;
}

unsigned MLogRecordDataFines::GetDataSize()
{
    unsigned Size; MFineData *Fine=Fines;

    Size=sizeof(SystemTime)+sizeof(NumFines);
    for ( unsigned i=0; i<NumFines; i++, Fine++ )
    {
        Size+=
            sizeof(Fine->ID)+
            strlen(Fine->Description)+1;
    }

    return Size;
}

char *MLogRecordDataFines::SetData(char *Data_)
{
    MFineData *Fine=Fines;

    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,NumFines);
    for ( unsigned i=0; i<NumFines; i++, Fine++ )
    {
        Data_=MemSet(Data_,Fine->ID);
        Data_=MemSetCLine(Data_,Fine->Description);
    }

    return Data_;
}

char *MLogRecordDataFines::GetData(char *Data_, char *Limit_)
{
    MFineData *Fine;

    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&NumFines,Limit_))==NULL ) goto error;
    if ( NumFines>MAX_Fines ) goto error;

    delete[] Fines; if ( (Fines=new MFineData[NumFines])==NULL ) goto error;

    Fine=Fines;
    for ( unsigned i=0; i<NumFines; i++, Fine++ )
    {
        if ( (Data_=MemGet(Data_,&Fine->ID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,Fine->Description,MAX_FineDescLength,Limit_))==NULL ) goto error;
    }

    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

MLogRecordDataUsers::MLogRecordDataUsers()
{
    SystemTime=0;
    NumUsers=0;
    Users=NULL;
}

MLogRecordDataUsers::~MLogRecordDataUsers()
{
    delete[] Users; Users=NULL;
}

bool MLogRecordDataUsers::Copy(MListItem *SrcItem_)
{
    MLogRecordDataUsers *RecordUsers_=(MLogRecordDataUsers*)SrcItem_;
    SystemTime=RecordUsers_->SystemTime;
    NumUsers=RecordUsers_->NumUsers;
    delete[] Users; if ( (Users=new MUserData[NumUsers])==NULL ) return false;
    memcpy(Users,RecordUsers_->Users,sizeof(MUserData)*NumUsers);
    return true;
}

unsigned MLogRecordDataUsers::GetDataSize()
{
    unsigned Size; MUserData *User=Users;

    Size=sizeof(SystemTime)+sizeof(NumUsers);
    for ( unsigned i=0; i<NumUsers; i++, User++ )
    {
        Size+=
            sizeof(User->ID)+
            strlen(User->Login)+1+
            strlen(User->Name)+1;
    }

    return Size;
}

char *MLogRecordDataUsers::SetData(char *Data_)
{
    MUserData *User=Users;

    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,NumUsers);
    for ( unsigned i=0; i<NumUsers; i++, User++ )
    {
        Data_=MemSet(Data_,User->ID);
        Data_=MemSetCLine(Data_,User->Login);
        Data_=MemSetCLine(Data_,User->Name);
    }

    return Data_;
}

char *MLogRecordDataUsers::GetData(char *Data_, char *Limit_)
{
    MUserData *User;

    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&NumUsers,Limit_))==NULL ) goto error;
    if ( NumUsers>MAX_Users ) goto error;

    delete[] Users; if ( (Users=new MUserData[NumUsers])==NULL ) goto error;

    User=Users;
    for ( unsigned i=0; i<NumUsers; i++, User++ )
    {
        if ( (Data_=MemGet(Data_,&User->ID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,User->Login,MAX_UserLoginLength,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,User->Name,MAX_UserNameLength,Limit_))==NULL ) goto error;
    }

    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------

