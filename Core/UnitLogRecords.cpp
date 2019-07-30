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
        // ���� ����
        case mlrBegin: Item=new MLogRecordBegin; break;
        case mlrEnd: Item=new MLogRecordEnd; break;
        // ��������� ������
        case mlrStart: Item=new MLogRecordStart; break;
        case mlrStop: Item=new MLogRecordStop; break;
        // ��������� ���������� ������
        case mlrConfig: Item=new MLogRecordConfig; break;
        case mlrComputers: Item=new MLogRecordComputers; break;
        case mlrTariffs: Item=new MLogRecordTariffs; break;
        case mlrFines: Item=new MLogRecordFines; break;
        case mlrUsers: Item=new MLogRecordUsers; break;
        case mlrOptions: Item=new MLogRecordOptions; break;
        // ������������
        case mlrLogIn: Item=new MLogRecordLogIn; break;
        case mlrLogOut: Item=new MLogRecordLogOut; break;
        // ������� �����������
        case mlrRun: Item=new MLogRecordRun; break;
        case mlrFine: Item=new MLogRecordFine; break;
        case mlrExchange: Item=new MLogRecordExchange; break;
        case mlrLock: Item=new MLogRecordLock; break;
        case mlrPause: Item=new MLogRecordPause; break;
        case mlrOpen: Item=new MLogRecordOpen; break;
        case mlrPowerOn: Item=new MLogRecordPowerOn; break;
        case mlrReboot: Item=new MLogRecordReboot; break;
        case mlrShutdown: Item=new MLogRecordShutdown; break;
        // ��������� ������
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
        // ���� ����
        case mlrBegin: delete (MLogRecordBegin*)Item_; break;
        case mlrEnd: delete (MLogRecordEnd*)Item_; break;
        // ��������� ������
        case mlrStart: delete (MLogRecordStart*)Item_; break;
        case mlrStop: delete (MLogRecordStop*)Item_; break;
        // ��������� ���������� ������
        case mlrConfig: delete (MLogRecordConfig*)Item_; break;
        case mlrComputers: delete (MLogRecordComputers*)Item_; break;
        case mlrTariffs: delete(MLogRecordTariffs*)Item_; break;
        case mlrFines: delete (MLogRecordFines*)Item_; break;
        case mlrUsers: delete (MLogRecordUsers*)Item_; break;
        case mlrOptions: delete (MLogRecordOptions*)Item_; break;
        // ������������
        case mlrLogIn: delete (MLogRecordLogIn*)Item_; break;
        case mlrLogOut: delete (MLogRecordLogOut*)Item_; break;
        // ������� �����������
        case mlrRun: delete (MLogRecordRun*)Item_; break;
        case mlrFine: delete (MLogRecordFine*)Item_; break;
        case mlrExchange: delete (MLogRecordExchange*)Item_; break;
        case mlrLock: delete (MLogRecordLock*)Item_; break;
        case mlrPause: delete (MLogRecordPause*)Item_; break;
        case mlrOpen: delete (MLogRecordOpen*)Item_; break;
        case mlrPowerOn: delete (MLogRecordPowerOn*)Item_; break;
        case mlrReboot: delete (MLogRecordReboot*)Item_; break;
        case mlrShutdown: delete (MLogRecordShutdown*)Item_; break;
        // ��������� ������
        case mlrDataShState: delete (MLogRecordDataShState*)Item_; break;
        case mlrDataStates: delete (MLogRecordDataStates*)Item_; break;
        case mlrDataTariffs: delete (MLogRecordDataTariffs*)Item_; break;
        case mlrDataFines: delete (MLogRecordDataFines*)Item_; break;
        case mlrDataUsers: delete (MLogRecordDataUsers*)Item_; break;
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
//
}

void MLogRecordEvent::Copy(const MListItem *SrcItem_)
{
    MLogRecordEvent *record=(MLogRecordEvent*)SrcItem_;
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

MLogRecordConfig::~MLogRecordConfig()
{
//
}

void MLogRecordConfig::Copy(const MListItem *SrcItem_)
{
    MLogRecordConfig *record=(MLogRecordConfig*)SrcItem_;

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

MLogRecordLogIn::~MLogRecordLogIn()
{
//
}

void MLogRecordLogIn::Copy(const MListItem *SrcItem_)
{
    MLogRecordLogIn *record=(MLogRecordLogIn*)SrcItem_;

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

MLogRecordRun::~MLogRecordRun()
{
//
}

void MLogRecordRun::Copy(const MListItem *SrcItem_)
{
    MLogRecordRun *record=(MLogRecordRun*)SrcItem_;

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

MLogRecordFine::~MLogRecordFine()
{
//
}

void MLogRecordFine::Copy(const MListItem *SrcItem_)
{
    MLogRecordFine *record=(MLogRecordFine*)SrcItem_;

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

MLogRecordExchange::~MLogRecordExchange()
{
}

void MLogRecordExchange::Copy(const MListItem *SrcItem_)
{
    MLogRecordExchange *record=(MLogRecordExchange*)SrcItem_;

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

MLogRecordMode::~MLogRecordMode()
{
}

void MLogRecordMode::Copy(const MListItem *SrcItem_)
{
    MLogRecordMode *record=(MLogRecordMode*)SrcItem_;

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

MLogRecordCmd::~MLogRecordCmd()
{
}

void MLogRecordCmd::Copy(const MListItem *SrcItem_)
{
    MLogRecordCmd *record=(MLogRecordCmd*)SrcItem_;

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

MLogRecordDataShState::~MLogRecordDataShState()
{
}

void MLogRecordDataShState::Copy(const MListItem *SrcItem_)
{
    MLogRecordDataShState *record=(MLogRecordDataShState*)SrcItem_;

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

MLogRecordDataStates::~MLogRecordDataStates()
{
//
}

void MLogRecordDataStates::Copy(const MListItem *SrcItem_)
{
    MLogRecordDataStates *record=(MLogRecordDataStates*)SrcItem_;

    SystemTime=record->SystemTime;
    States=record->States;
}

unsigned MLogRecordDataStates::GetDataSize() const
{
    unsigned Size;

    Size=
            sizeof(SystemTime)+
            sizeof(unsigned);                   // ������� ���������

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
        MStateData &state=States[i];
        Data_=MemSet(Data_,state.Number);
        Data_=MemSet(Data_,state.State);
        Data_=MemSet(Data_,state.TariffID);
        Data_=MemSet(Data_,state.StartWorkTime);
        Data_=MemSet(Data_,state.SizeWorkTime);
        Data_=MemSet(Data_,state.StartFineTime);
        Data_=MemSet(Data_,state.SizeFineTime);
        Data_=MemSet(Data_,state.StopTimerTime);
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
        MStateData &state=States[i];
        if ( (Data_=MemGet(Data_,&state.Number,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&state.State,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&state.TariffID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&state.StartWorkTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&state.SizeWorkTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&state.StartFineTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&state.SizeFineTime,Limit_))==NULL ) goto error;
        if ( (Data_=MemGet(Data_,&state.StopTimerTime,Limit_))==NULL ) goto error;
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

MLogRecordDataTariffs::~MLogRecordDataTariffs()
{
//
}

void MLogRecordDataTariffs::Copy(const MListItem *SrcItem_)
{
    MLogRecordDataTariffs *record=(MLogRecordDataTariffs*)SrcItem_;

    SystemTime=record->SystemTime;
    Tariffs=record->Tariffs;
}

unsigned MLogRecordDataTariffs::GetDataSize() const
{
    unsigned Size;

    Size=
            sizeof(SystemTime)+
            sizeof(unsigned);                   // ������� �������

    for ( unsigned i=0, Count=Tariffs.Count(); i<Count; i++ )
    {
        MTariffData &tariff=Tariffs[i];
        Size+=
            sizeof(tariff.ID)+
            strlen(tariff.Name)+1;
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
        MTariffData &tariff=Tariffs[i];
        Data_=MemSet(Data_,tariff.ID);
        Data_=MemSetCLine(Data_,tariff.Name);
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
        MTariffData &tariff=Tariffs[i];
        if ( (Data_=MemGet(Data_,&tariff.ID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,tariff.Name,MAX_TariffNameLen,Limit_))==NULL ) goto error;
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

MLogRecordDataFines::~MLogRecordDataFines()
{
}

void MLogRecordDataFines::Copy(const MListItem *SrcItem_)
{
    MLogRecordDataFines *record=(MLogRecordDataFines*)SrcItem_;

    SystemTime=record->SystemTime;
    Fines=record->Fines;
}

unsigned MLogRecordDataFines::GetDataSize() const
{
    unsigned Size;

    Size=
            sizeof(SystemTime)+
            sizeof(unsigned);                   // ������� �������

    for ( unsigned i=0, cnt=Fines.Count(); i<cnt; i++ )
    {
        MFineData &fine=Fines[i];
        Size+=
            sizeof(fine.ID)+
            strlen(fine.Descr)+1;
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
        MFineData &fine=Fines[i];
        Data_=MemSet(Data_,fine.ID);
        Data_=MemSetCLine(Data_,fine.Descr);
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
        MFineData &fine=Fines[i];
        if ( (Data_=MemGet(Data_,&fine.ID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,fine.Descr,MAX_FineDescrLen,Limit_))==NULL ) goto error;
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

MLogRecordDataUsers::~MLogRecordDataUsers()
{
//
}

void MLogRecordDataUsers::Copy(const MListItem *SrcItem_)
{
    MLogRecordDataUsers *record=(MLogRecordDataUsers*)SrcItem_;

    SystemTime=record->SystemTime;
    Users=record->Users;
}

unsigned MLogRecordDataUsers::GetDataSize() const
{
    unsigned Size;

    Size=
            sizeof(SystemTime)+
            sizeof(unsigned);                       // ������� �������������

    for ( unsigned i=0, Count=Users.Count(); i<Count; i++ )
    {
        MUserData &user=Users[i];
        Size+=
            sizeof(user.ID)+
            strlen(user.Login)+1+
            strlen(user.Name)+1;
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
        MUserData &user=Users[i];
        Data_=MemSet(Data_,user.ID);
        Data_=MemSetCLine(Data_,user.Login);
        Data_=MemSetCLine(Data_,user.Name);
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
        MUserData &user=Users[i];
        if ( (Data_=MemGet(Data_,&user.ID,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,user.Login,MAX_UserLoginLen,Limit_))==NULL ) goto error;
        if ( (Data_=MemGetCLine(Data_,user.Name,MAX_UserNameLen,Limit_))==NULL ) goto error;
    }

    return Data_;
error:
    return NULL;
}
//---------------------------------------------------------------------------

