//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitLogRecords.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void MLogRecords::EventBase::Copy(const MListItem *SrcItem_)
{
	auto record=&dynamic_cast<const MLogRecords::EventBase&>(*SrcItem_);
	SystemTime=record->SystemTime;
}

unsigned MLogRecords::EventBase::GetDataSize() const
{
	return
		sizeof(SystemTime);
}

void *MLogRecords::EventBase::SetData(void *Data_) const
{
	Data_=MemSet(Data_,SystemTime);
	return Data_;
}

const void *MLogRecords::EventBase::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&SystemTime,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
void MLogRecords::AppConfigBase::Copy(const MListItem *SrcItem_)
{
	auto record=&dynamic_cast<const MLogRecords::AppConfigBase&>(*SrcItem_);

	SystemTime=record->SystemTime;
	Opened=record->Opened;
}

unsigned MLogRecords::AppConfigBase::GetDataSize() const
{
	return
		sizeof(SystemTime)+
		sizeof(Opened);
}

void *MLogRecords::AppConfigBase::SetData(void *Data_) const
{
	Data_=MemSet(Data_,SystemTime);
	Data_=MemSet(Data_,Opened);

	return Data_;
}

const void *MLogRecords::AppConfigBase::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&Opened,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
void MLogRecords::AppLogInBase::Copy(const MListItem *SrcItem_)
{
	auto record=&dynamic_cast<const MLogRecords::AppLogInBase&>(*SrcItem_);

    SystemTime=record->SystemTime;
    User=record->User;
}

unsigned MLogRecords::AppLogInBase::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(User);
}

void *MLogRecords::AppLogInBase::SetData(void *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,User);

    return Data_;
}

const void *MLogRecords::AppLogInBase::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&User,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
void MLogRecords::CompRunBase::Copy(const MListItem *SrcItem_)
{
	auto record=&dynamic_cast<const MLogRecords::CompRunBase&>(*SrcItem_);

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

unsigned MLogRecords::CompRunBase::GetDataSize() const
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

void *MLogRecords::CompRunBase::SetData(void *Data_) const
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

const void *MLogRecords::CompRunBase::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&Number,Limit_)) &&
		(Data_=MemGet(Data_,&Tariff,Limit_)) &&
		(Data_=MemGet(Data_,&StartTime,Limit_)) &&
		(Data_=MemGet(Data_,&Type,Limit_)) &&
		(Data_=MemGet(Data_,&BeginTime,Limit_)) &&
		(Data_=MemGet(Data_,&EndTime,Limit_)) &&
		(Data_=MemGet(Data_,&SizeTime,Limit_)) &&
		(Data_=MemGet(Data_,&WorkTime,Limit_)) &&
		(Data_=MemGet(Data_,&Cost,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
void MLogRecords::CompFineBase::Copy(const MListItem *SrcItem_)
{
	auto record=&dynamic_cast<const MLogRecords::CompFineBase&>(*SrcItem_);

    SystemTime=record->SystemTime;
    Number=record->Number;
    Fine=record->Fine;
    Time=record->Time;
}

unsigned MLogRecords::CompFineBase::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(Number)+
        sizeof(Fine)+
        sizeof(Time);
}

void *MLogRecords::CompFineBase::SetData(void *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,Fine);
    Data_=MemSet(Data_,Time);

    return Data_;
}

const void *MLogRecords::CompFineBase::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&Number,Limit_)) &&
		(Data_=MemGet(Data_,&Fine,Limit_)) &&
		(Data_=MemGet(Data_,&Time,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
void MLogRecords::CompExchangeBase::Copy(const MListItem *SrcItem_)
{
	auto record=&dynamic_cast<const MLogRecords::CompExchangeBase&>(*SrcItem_);

    SystemTime=record->SystemTime;
    From=record->From;
    To=record->To;
}

unsigned MLogRecords::CompExchangeBase::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(From)+
        sizeof(To);
}

void *MLogRecords::CompExchangeBase::SetData(void *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,From);
    Data_=MemSet(Data_,To);

    return Data_;
}

const void *MLogRecords::CompExchangeBase::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&From,Limit_)) &&
		(Data_=MemGet(Data_,&To,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
void MLogRecords::ModeBase::Copy(const MListItem *SrcItem_)
{
	auto record=&dynamic_cast<const MLogRecords::ModeBase&>(*SrcItem_);

    SystemTime=record->SystemTime;
    Number=record->Number;
    Apply=record->Apply;
}

unsigned MLogRecords::ModeBase::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(Number)+
        sizeof(Apply);
}

void *MLogRecords::ModeBase::SetData(void *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,Apply);

    return Data_;
}

const void *MLogRecords::ModeBase::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&Number,Limit_)) &&
		(Data_=MemGet(Data_,&Apply,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
void MLogRecords::CmdBase::Copy(const MListItem *SrcItem_)
{
	auto record=&dynamic_cast<const MLogRecords::CmdBase&>(*SrcItem_);

	SystemTime=record->SystemTime;
	Number=record->Number;
}

unsigned MLogRecords::CmdBase::GetDataSize() const
{
	return
		sizeof(SystemTime)+
		sizeof(Number);
}

void *MLogRecords::CmdBase::SetData(void *Data_) const
{
	Data_=MemSet(Data_,SystemTime);
	Data_=MemSet(Data_,Number);
	return Data_;
}

const void *MLogRecords::CmdBase::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&Number,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
void MLogRecords::DataShellBase::Copy(const MListItem *SrcItem_)
{
	auto record=&dynamic_cast<const MLogRecords::DataShellBase&>(*SrcItem_);

    SystemTime=record->SystemTime;
    State=record->State;
    User=record->User;
}

unsigned MLogRecords::DataShellBase::GetDataSize() const
{
    return
        sizeof(SystemTime)+
        sizeof(State)+
        sizeof(User);
}

void *MLogRecords::DataShellBase::SetData(void *Data_) const
{
    Data_=MemSet(Data_,SystemTime);
    Data_=MemSet(Data_,State);
    Data_=MemSet(Data_,User);
    return Data_;
}

const void *MLogRecords::DataShellBase::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&State,Limit_)) &&
		(Data_=MemGet(Data_,&User,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
void MLogRecords::DataStatesBase::Copy(const MListItem *SrcItem_)
{
	auto data=&dynamic_cast<const MLogRecords::DataStatesBase&>(*SrcItem_);

	SystemTime=data->SystemTime;
    States=data->States;
}

unsigned MLogRecords::DataStatesBase::GetDataSize() const
{
    unsigned Size;

    Size=
            sizeof(SystemTime)+
            sizeof(unsigned);                   // —четчик состо€ний

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

void *MLogRecords::DataStatesBase::SetData(void *Data_) const
{
    unsigned Count;

    Data_=MemSet(Data_,SystemTime);
    Count=States.Count(); Data_=MemSet(Data_,Count);

    for ( unsigned i=0; i<Count; i++)
    {
        MStatesItem::LogData &ld=States[i];
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

const void *MLogRecords::DataStatesBase::GetData(const void *Data_, const void *Limit_)
{
	unsigned Count;

	if ( !(
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&Count,Limit_))
		) ) return nullptr;

	if ( Count>MAX_Comps ) return nullptr;
	States.Alloc(Count);

	for ( unsigned i=0; i<Count; i++ )
	{
		MStatesItem::LogData &ld=States[i];
		if ( !(
			(Data_=MemGet(Data_,&ld.Number,Limit_)) &&
			(Data_=MemGet(Data_,&ld.State,Limit_)) &&
			(Data_=MemGet(Data_,&ld.TariffID,Limit_)) &&
			(Data_=MemGet(Data_,&ld.StartWorkTime,Limit_)) &&
			(Data_=MemGet(Data_,&ld.SizeWorkTime,Limit_)) &&
			(Data_=MemGet(Data_,&ld.StartFineTime,Limit_)) &&
			(Data_=MemGet(Data_,&ld.SizeFineTime,Limit_)) &&
			(Data_=MemGet(Data_,&ld.StopTimerTime,Limit_))
			) ) return nullptr;
	}

	return Data_;
}
//---------------------------------------------------------------------------
void MLogRecords::DataTariffsBase::Copy(const MListItem *SrcItem_)
{
	auto data=&dynamic_cast<const MLogRecords::DataTariffsBase&>(*SrcItem_);

	SystemTime=data->SystemTime;
    Items=data->Items;
}

unsigned MLogRecords::DataTariffsBase::GetDataSize() const
{
    unsigned Size;

    Size=
		sizeof(SystemTime)+
		sizeof(unsigned);                   // —четчик тарифов

	for ( unsigned i=0, Count=Items.Count(); i<Count; i++ )
    {
		MTariffsItem::LogData &ld=Items[i];
        Size+=
			sizeof(ld.UUID)+
			sizeofLine(ld.Name);
    }

    return Size;
}

void *MLogRecords::DataTariffsBase::SetData(void *Data_) const
{
    unsigned Count;

    Data_=MemSet(Data_,SystemTime);
	Count=Items.Count(); Data_=MemSet(Data_,Count);

    for ( unsigned i=0; i<Count; i++ )
    {
		MTariffsItem::LogData &ld=Items[i];
		Data_=MemSet(Data_,ld.UUID);
		Data_=MemSetLine(Data_,ld.Name);
    }

    return Data_;
}

const void *MLogRecords::DataTariffsBase::GetData(const void *Data_, const void *Limit_)
{
    unsigned Count;

	if ( !(
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&Count,Limit_))
        ) ) return nullptr;

	if ( Count>MAX_Tariffs ) return nullptr;
	Items.Alloc(Count);

	for ( unsigned i=0; i<Count; i++ )
	{
		MTariffsItem::LogData &ld=Items[i];
		if ( !(
			(Data_=MemGet(Data_,&ld.UUID,Limit_)) &&
			(Data_=MemGetLine(Data_,ld.Name,MAX_TariffNameLen,Limit_))
			) ) return nullptr;
    }

    return Data_;
}
//---------------------------------------------------------------------------
void MLogRecords::DataFinesBase::Copy(const MListItem *SrcItem_)
{
	auto data=&dynamic_cast<const MLogRecords::DataFinesBase&>(*SrcItem_);

	SystemTime=data->SystemTime;
	Items=data->Items;
}

unsigned MLogRecords::DataFinesBase::GetDataSize() const
{
    unsigned Size;

    Size=
		sizeof(SystemTime)+
		sizeof(unsigned);                   // —четчик массива

    for ( unsigned i=0, cnt=Items.Count(); i<cnt; i++ )
    {
		MFinesItem::LogData &ld=Items[i];
        Size+=
			sizeof(ld.UUID)+
			sizeofLine(ld.Descr);
    }

    return Size;
}

void *MLogRecords::DataFinesBase::SetData(void *Data_) const
{
    unsigned Count;

    Data_=MemSet(Data_,SystemTime);
    Count=Items.Count(); Data_=MemSet(Data_,Count);

    for ( unsigned i=0; i<Count; i++ )
    {
		MFinesItem::LogData &ld=Items[i];
		Data_=MemSet(Data_,ld.UUID);
        Data_=MemSetLine(Data_,ld.Descr);
    }

    return Data_;
}

const void *MLogRecords::DataFinesBase::GetData(const void *Data_, const void *Limit_)
{
    unsigned Count;

	if ( !(
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&Count,Limit_))
        ) ) return nullptr;

	if ( Count>MAX_Fines ) return nullptr;
    Items.Alloc(Count);

    for ( unsigned i=0; i<Count; i++ )
    {
		MFinesItem::LogData &ld=Items[i];
		if ( !(
			(Data_=MemGet(Data_,&ld.UUID,Limit_)) &&
			(Data_=MemGetLine(Data_,ld.Descr,MAX_FineDescrLen,Limit_))
            ) ) return nullptr;
    }

	return Data_;
}
//---------------------------------------------------------------------------
void MLogRecords::DataUsersBase::Copy(const MListItem *SrcItem_)
{
	auto data=&dynamic_cast<const MLogRecords::DataUsersBase&>(*SrcItem_);

	SystemTime=data->SystemTime;
    Items=data->Items;
}

unsigned MLogRecords::DataUsersBase::GetDataSize() const
{
    unsigned Size;

    Size=
		sizeof(SystemTime)+
		sizeof(unsigned);                       // —четчик пользователей

    for ( unsigned i=0, Count=Items.Count(); i<Count; i++ )
    {
        MUsersItem::LogData &ld=Items[i];
        Size+=
			sizeof(ld.UUID)+
			sizeofLine(ld.Login)+
            sizeofLine(ld.Name);
    }

    return Size;
}

void *MLogRecords::DataUsersBase::SetData(void *Data_) const
{
    unsigned Count;

    Data_=MemSet(Data_,SystemTime);
    Count=Items.Count(); Data_=MemSet(Data_,Count);
    for ( unsigned i=0; i<Count; i++ )
    {
		MUsersItem::LogData &ld=Items[i];
        Data_=MemSet(Data_,ld.UUID);
		Data_=MemSetLine(Data_,ld.Login);
		Data_=MemSetLine(Data_,ld.Name);
	}

    return Data_;
}

const void *MLogRecords::DataUsersBase::GetData(const void *Data_, const void *Limit_)
{
    unsigned Count;

	if ( !(
		(Data_=MemGet(Data_,&SystemTime,Limit_)) &&
		(Data_=MemGet(Data_,&Count,Limit_))
        ) ) return nullptr;

	if ( Count>MAX_Users ) return nullptr;
    Items.Alloc(Count);

    for ( unsigned i=0; i<Count; i++ )
    {
        MUsersItem::LogData &ld=Items[i];
		if ( !(
			(Data_=MemGet(Data_,&ld.UUID,Limit_)) &&
			(Data_=MemGetLine(Data_,ld.Login,MAX_UserLoginLen,Limit_)) &&
			(Data_=MemGetLine(Data_,ld.Name,MAX_UserNameLen,Limit_))
            ) ) return nullptr;
    }

    return Data_;
}
//---------------------------------------------------------------------------

