//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitLogRecords.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MListItem *MLogRecords::item_new(unsigned char TypeID_) const
{
    MListItem *Item=NULL;

    switch(TypeID_)
    {
        // Файл лога
        case mlrBegin: Item=new MLogRecordBegin; break;
        case mlrEnd: Item=new MLogRecordEnd; break;
        // Админский модуль
        case mlrStart: Item=new MLogRecordStart; break;
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
        case mlrPowerOn: Item=new MLogRecordPowerOn; break;
        case mlrReboot: Item=new MLogRecordReboot; break;
        case mlrShutdown: Item=new MLogRecordShutdown; break;
        // Служебные данные
        case mlrDataShState: Item=new MLogRecordDataShState; break;
        case mlrDataStates: Item=new MLogRecordDataStates; break;
        case mlrDataTariffs: Item=new MLogRecordDataTariffs; break;
        case mlrDataFines: Item=new MLogRecordDataFines; break;
        case mlrDataUsers: Item=new MLogRecordDataUsers; break;
        default: break;
    }

    return Item;
}

void MLogRecords::item_del(MListItem *Item_) const
{
    switch(Item_->gTypeID())
    {
        // Файл лога
        case mlrBegin: delete (MLogRecordBegin*)Item_; break;
        case mlrEnd: delete (MLogRecordEnd*)Item_; break;
        // Админский модуль
        case mlrStart: delete (MLogRecordStart*)Item_; break;
        case mlrStop: delete (MLogRecordStop*)Item_; break;
        // Настройки админского модуля
        case mlrConfig: delete (MLogRecordConfig*)Item_; break;
        case mlrComputers: delete (MLogRecordComputers*)Item_; break;
        case mlrTariffs: delete(MLogRecordTariffs*)Item_; break;
        case mlrFines: delete (MLogRecordFines*)Item_; break;
        case mlrUsers: delete (MLogRecordUsers*)Item_; break;
        case mlrOptions: delete (MLogRecordOptions*)Item_; break;
        // Пользователи
        case mlrLogIn: delete (MLogRecordLogIn*)Item_; break;
        case mlrLogOut: delete (MLogRecordLogOut*)Item_; break;
        // Команды компьютерам
        case mlrRun: delete (MLogRecordRun*)Item_; break;
        case mlrFine: delete (MLogRecordFine*)Item_; break;
        case mlrExchange: delete (MLogRecordExchange*)Item_; break;
        case mlrLock: delete (MLogRecordLock*)Item_; break;
        case mlrPause: delete (MLogRecordPause*)Item_; break;
        case mlrOpen: delete (MLogRecordOpen*)Item_; break;
        case mlrPowerOn: delete (MLogRecordPowerOn*)Item_; break;
        case mlrReboot: delete (MLogRecordReboot*)Item_; break;
        case mlrShutdown: delete (MLogRecordShutdown*)Item_; break;
        // Служебные данные
        case mlrDataShState: delete (MLogRecordDataShState*)Item_; break;
        case mlrDataStates: delete (MLogRecordDataStates*)Item_; break;
        case mlrDataTariffs: delete (MLogRecordDataTariffs*)Item_; break;
        case mlrDataFines: delete (MLogRecordDataFines*)Item_; break;
        case mlrDataUsers: delete (MLogRecordDataUsers*)Item_; break;
        default: break;
    }
}
//---------------------------------------------------------------------------
void MLogRecordEvent::Copy(const MListItem *SrcItem_)
{
    const MLogRecordEvent *record=
        dynamic_cast<const MLogRecordEvent*>(SrcItem_);
    SystemTime=record->SystemTime;
}

unsigned MLogRecordEvent::GetDataSize() const
{
    return
        sizeof(SystemTime);
}

char *MLogRecordEvent::SetData(char *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    return Data_;
}

const char *MLogRecordEvent::GetData(const char *Data_, const char *Limit_)
{
    return
        (Data_=MemGet(Data_,&SystemTime,Limit_))!=NULL
        ? Data_: NULL;
}
//---------------------------------------------------------------------------
MLogRecordConfig::MLogRecordConfig()
{
    SystemTime=0;
    Open=false;
}

void MLogRecordConfig::Copy(const MListItem *SrcItem_)
{
    const MLogRecordConfig *record=
        dynamic_cast<const MLogRecordConfig*>(SrcItem_);

    SystemTime=record->SystemTime;
    Open=record->Open;
}

unsigned MLogRecordConfig::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(Open);
}

char *MLogRecordConfig::SetData(char *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,Open);

    return Data_;
}

const char *MLogRecordConfig::GetData(const char *Data_, const char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Open,Limit_))==NULL ) goto error;

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

void MLogRecordLogIn::Copy(const MListItem *SrcItem_)
{
    const MLogRecordLogIn *record=
        dynamic_cast<const MLogRecordLogIn*>(SrcItem_);

    SystemTime=record->SystemTime;
    User=record->User;
}

unsigned MLogRecordLogIn::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(User);
}

char *MLogRecordLogIn::SetData(char *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,User);

    return Data_;
}

const char *MLogRecordLogIn::GetData(const char *Data_, const char *Limit_)
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

void MLogRecordRun::Copy(const MListItem *SrcItem_)
{
    const MLogRecordRun *record=
        dynamic_cast<const MLogRecordRun*>(SrcItem_);

    SystemTime=record->SystemTime;
    Number=record->Number;
    Tariff=record->Tariff;
    StartTime=record->StartTime;
    Type=record->Type;
    BeginTime=record->BeginTime;
    EndTime=record->EndTime;
    SizeTime=record->SizeTime;
    WorkTime=record->WorkTime;
    Cost=record->Cost;
}

unsigned MLogRecordRun::GetDataSize() const
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

char *MLogRecordRun::SetData(char *Data_) const
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

const char *MLogRecordRun::GetData(const char *Data_, const char *Limit_)
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

void MLogRecordFine::Copy(const MListItem *SrcItem_)
{
    const MLogRecordFine *record=
        dynamic_cast<const MLogRecordFine*>(SrcItem_);

    SystemTime=record->SystemTime;
    Number=record->Number;
    Fine=record->Fine;
    Time=record->Time;
}

unsigned MLogRecordFine::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(Number)+
        sizeof(Fine)+
        sizeof(Time);
}

char *MLogRecordFine::SetData(char *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,Fine);
    Data_=MemSet(Data_,Time);

    return Data_;
}

const char *MLogRecordFine::GetData(const char *Data_, const char *Limit_)
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

void MLogRecordExchange::Copy(const MListItem *SrcItem_)
{
    const MLogRecordExchange *record=
        dynamic_cast<const MLogRecordExchange*>(SrcItem_);

    SystemTime=record->SystemTime;
    From=record->From;
    To=record->To;
}

unsigned MLogRecordExchange::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(From)+
        sizeof(To);
}

char *MLogRecordExchange::SetData(char *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,From);
    Data_=MemSet(Data_,To);

    return Data_;
}

const char *MLogRecordExchange::GetData(const char *Data_, const char *Limit_)
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

void MLogRecordMode::Copy(const MListItem *SrcItem_)
{
    const MLogRecordMode *record=
        dynamic_cast<const MLogRecordMode*>(SrcItem_);

    SystemTime=record->SystemTime;
    Number=record->Number;
    Apply=record->Apply;
}

unsigned MLogRecordMode::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(Number)+
        sizeof(Apply);
}

char *MLogRecordMode::SetData(char *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,Apply);

    return Data_;
}

const char *MLogRecordMode::GetData(const char *Data_, const char *Limit_)
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

void MLogRecordCmd::Copy(const MListItem *SrcItem_)
{
    const MLogRecordCmd *record=
        dynamic_cast<const MLogRecordCmd*>(SrcItem_);

    SystemTime=record->SystemTime;
    Number=record->Number;
}

unsigned MLogRecordCmd::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(Number);
}

char *MLogRecordCmd::SetData(char *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,Number);
    return Data_;
}

const char *MLogRecordCmd::GetData(const char *Data_, const char *Limit_)
{
    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Number,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}
//---------------------------------------------------------------------------
MLogRecordDataShState::MLogRecordDataShState()
{
    SystemTime=0;
    State=0;
    User=0;
}

void MLogRecordDataShState::Copy(const MListItem *SrcItem_)
{
    const MLogRecordDataShState *record=
        dynamic_cast<const MLogRecordDataShState*>(SrcItem_);

    SystemTime=record->SystemTime;
    State=record->State;
    User=record->User;
}

unsigned MLogRecordDataShState::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(State)+
        sizeof(User);
}

char *MLogRecordDataShState::SetData(char *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,State);
    Data_=MemSet(Data_,User);
    return Data_;
}

const char *MLogRecordDataShState::GetData(const char *Data_, const char *Limit_)
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
}

void MLogRecordDataStates::Copy(const MListItem *SrcItem_)
{
    const MLogRecordDataStates *record=
        dynamic_cast<const MLogRecordDataStates*>(SrcItem_);

    SystemTime=record->SystemTime;
    States=record->States;
}

unsigned MLogRecordDataStates::GetDataSize() const
{
    unsigned Size;

    Size=
            sizeof(SystemTime)+
            sizeof(unsigned);                   // Счетчик состояний

    if ( States.Count()>0 )
    {
        Size+=(
            sizeof(States[0].Number)+
            sizeof(States[0].State)+
            sizeof(States[0].TariffID)+
            sizeof(States[0].StartWorkTime)+
            sizeof(States[0].SizeWorkTime)+
            sizeof(States[0].StartFineTime)+
            sizeof(States[0].SizeFineTime)+
            sizeof(States[0].StopTimerTime))*States.Count();
    }

    return Size;
}

char *MLogRecordDataStates::SetData(char *Data_) const
{
    unsigned Count;

    Data_=MemSet(Data_,SystemTime);
    Count=States.Count(); Data_=MemSet(Data_,Count);

    for ( unsigned i=0; i<Count; i++)
    {
        MState::LogData &ld=States[i];
        Data_=MemSet(Data_,ld.Number);
        Data_=MemSet(Data_,ld.State);
        Data_=MemSet(Data_,ld.TariffID);
        Data_=MemSet(Data_,ld.StartWorkTime);
        Data_=MemSet(Data_,ld.SizeWorkTime);
        Data_=MemSet(Data_,ld.StartFineTime);
        Data_=MemSet(Data_,ld.SizeFineTime);
        Data_=MemSet(Data_,ld.StopTimerTime);
    }

    return Data_;
}

const char *MLogRecordDataStates::GetData(const char *Data_, const char *Limit_)
{
    unsigned Count;

    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Count,Limit_))==NULL ) goto error;
    if ( Count>MAX_Comps ) goto error;
    States.Alloc(Count);

    for ( unsigned i=0; i<Count; i++ )
    {
        MState::LogData &ld=States[i];
        if ( (Data_=MemGet(Data_,&ld.Number,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&ld.State,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&ld.TariffID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&ld.StartWorkTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&ld.SizeWorkTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&ld.StartFineTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&ld.SizeFineTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&ld.StopTimerTime,Limit_))==NULL ) goto error;
    }

    return Data_;
error:
    return NULL;
}
//---------------------------------------------------------------------------
MLogRecordDataTariffs::MLogRecordDataTariffs()
{
    SystemTime=0;
}

void MLogRecordDataTariffs::Copy(const MListItem *SrcItem_)
{
    const MLogRecordDataTariffs *record=
        dynamic_cast<const MLogRecordDataTariffs*>(SrcItem_);

    SystemTime=record->SystemTime;
    Tariffs=record->Tariffs;
}

unsigned MLogRecordDataTariffs::GetDataSize() const
{
    unsigned Size;

    Size=
            sizeof(SystemTime)+
            sizeof(unsigned);                   // Счетчик тарифов

    for ( unsigned i=0, Count=Tariffs.Count(); i<Count; i++ )
    {
        MTariff::LogData &ld=Tariffs[i];
        Size+=
            sizeof(ld.ID)+
            ld.Name.length()+1;
    }

    return Size;
}

char *MLogRecordDataTariffs::SetData(char *Data_) const
{
    unsigned Count;

    Data_=MemSet(Data_,SystemTime);
    Count=Tariffs.Count(); Data_=MemSet(Data_,Count);

    for ( unsigned i=0; i<Count; i++ )
    {
        MTariff::LogData &ld=Tariffs[i];
        Data_=MemSet(Data_,ld.ID);
        Data_=MemSetCLine(Data_,ld.Name.c_str());
    }

    return Data_;
}

const char *MLogRecordDataTariffs::GetData(const char *Data_, const char *Limit_)
{
    unsigned Count;

    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Count,Limit_))==NULL ) goto error;
    if ( Count>MAX_Tariffs ) goto error;
    Tariffs.Alloc(Count);

    for ( unsigned i=0; i<Count; i++ )
    {
        MTariff::LogData &ld=Tariffs[i];
        if ( (Data_=MemGet(Data_,&ld.ID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,ld.Name,MAX_TariffNameLen,Limit_))==NULL ) goto error;
    }

    return Data_;
error:
    return NULL;
}
//---------------------------------------------------------------------------
MLogRecordDataFines::MLogRecordDataFines()
{
    SystemTime=0;
}

void MLogRecordDataFines::Copy(const MListItem *SrcItem_)
{
    const MLogRecordDataFines *record=
        dynamic_cast<const MLogRecordDataFines*>(SrcItem_);

    SystemTime=record->SystemTime;
    Fines=record->Fines;
}

unsigned MLogRecordDataFines::GetDataSize() const
{
    unsigned Size;

    Size=
            sizeof(SystemTime)+
            sizeof(unsigned);                   // Счетчик массива

    for ( unsigned i=0, cnt=Fines.Count(); i<cnt; i++ )
    {
        MFine::LogData &ld=Fines[i];
        Size+=
            sizeof(ld.ID)+
            ld.Descr.length()+1;
    }

    return Size;
}

char *MLogRecordDataFines::SetData(char *Data_) const
{
    unsigned Count;

    Data_=MemSet(Data_,SystemTime);
    Count=Fines.Count(); Data_=MemSet(Data_,Count);

    for ( unsigned i=0; i<Count; i++ )
    {
        MFine::LogData &ld=Fines[i];
        Data_=MemSet(Data_,ld.ID);
        Data_=MemSetCLine(Data_,ld.Descr.c_str());
    }

    return Data_;
}

const char *MLogRecordDataFines::GetData(const char *Data_, const char *Limit_)
{
    unsigned Count;

    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Count,Limit_))==NULL ) goto error;
    if ( Count>MAX_Fines ) goto error;
    Fines.Alloc(Count);

    for ( unsigned i=0; i<Count; i++ )
    {
        MFine::LogData &ld=Fines[i];
        if ( (Data_=MemGet(Data_,&ld.ID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,ld.Descr,MAX_FineDescrLen,Limit_))==NULL ) goto error;
    }

    return Data_;
error:
    return NULL;
}
//---------------------------------------------------------------------------
MLogRecordDataUsers::MLogRecordDataUsers()
{
    SystemTime=0;
}

void MLogRecordDataUsers::Copy(const MListItem *SrcItem_)
{
    const MLogRecordDataUsers *record=
        dynamic_cast<const MLogRecordDataUsers*>(SrcItem_);

    SystemTime=record->SystemTime;
    Users=record->Users;
}

unsigned MLogRecordDataUsers::GetDataSize() const
{
    unsigned Size;

    Size=
            sizeof(SystemTime)+
            sizeof(unsigned);                       // Счетчик пользователей

    for ( unsigned i=0, Count=Users.Count(); i<Count; i++ )
    {
        MUser::LogData &ld=Users[i];
        Size+=
            sizeof(ld.ID)+
            ld.Login.length()+1+
            ld.Name.length()+1;
    }

    return Size;
}

char *MLogRecordDataUsers::SetData(char *Data_) const
{
    unsigned Count;

    Data_=MemSet(Data_,SystemTime);
    Count=Users.Count(); Data_=MemSet(Data_,Count);
    for ( unsigned i=0; i<Count; i++ )
    {
        MUser::LogData &ld=Users[i];
        Data_=MemSet(Data_,ld.ID);
        Data_=MemSetCLine(Data_,ld.Login.c_str());
        Data_=MemSetCLine(Data_,ld.Name.c_str());
    }

    return Data_;
}

const char *MLogRecordDataUsers::GetData(const char *Data_, const char *Limit_)
{
    unsigned Count;

    if ( (Data_=MemGet(Data_,&SystemTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Count,Limit_))==NULL ) goto error;
    if ( Count>MAX_Users ) goto error;
    Users.Alloc(Count);

    for ( unsigned i=0; i<Count; i++ )
    {
        MUser::LogData &ld=Users[i];
        if ( (Data_=MemGet(Data_,&ld.ID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,ld.Login,MAX_UserLoginLen,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,ld.Name,MAX_UserNameLen,Limit_))==NULL ) goto error;
    }

    return Data_;
error:
    return NULL;
}
//---------------------------------------------------------------------------

