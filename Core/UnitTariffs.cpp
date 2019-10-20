//---------------------------------------------------------------------------
#include <string.h>
#include <mem.h>
#include <stdlib.h>
#include <math.h>
#pragma hdrstop

#include "UnitTariffs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
unsigned MTariffTimesItem::GetDataSize() const
{
	return
		sizeof(Type)+
		sizeof(BeginTime)+
		sizeof(EndTime)+
		sizeof(SizeTime)+
		sizeof(Cost);
}

void *MTariffTimesItem::SetData(void *Data_) const
{
	Data_=MemSet(Data_,Type);
	Data_=MemSet(Data_,BeginTime);
	Data_=MemSet(Data_,EndTime);
	Data_=MemSet(Data_,SizeTime);
	Data_=MemSet(Data_,Cost);
	return Data_;
}

const void *MTariffTimesItem::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&Type,Limit_)) &&
		(Data_=MemGet(Data_,&BeginTime,Limit_)) &&
		(Data_=MemGet(Data_,&EndTime,Limit_)) &&
		(Data_=MemGet(Data_,&SizeTime,Limit_)) &&
		(Data_=MemGet(Data_,&Cost,Limit_))
		? Data_: nullptr;
}

int MTariffTimesItem::MaxWorkTime(int Time_) const
{
	int WorkTime;

	WorkTime=EndTime - (Time_<BeginTime? Time_+24*60: Time_);
	if ( WorkTime<0 ) WorkTime=0;
	else if ( (Type==mttFlyPacket)&&(WorkTime>SizeTime) ) WorkTime=SizeTime;

	return WorkTime;
}
//---------------------------------------------------------------------------
MTariffsInfoItem *MTariffsInfo::Search(unsigned ID_) const
{
	MTariffsInfoItem* ti=gFirst();

	while(ti)
	{
		if ( ti->ID==ID_ ) return ti;
		ti=ti->gNext();
	}

    return nullptr;
}
//---------------------------------------------------------------------------
unsigned MTariffsItem::GetDataSize() const
{
    return
        MIDListItem::GetDataSize()+
        sizeofLine(Name)+
        sizeof(Programs)+
        sizeof(Reboot)+
        sizeof(CompsCnt)+
        sizeof(*Comps)*CompsCnt+
        Times.GetAllDataSize();
}

void *MTariffsItem::SetData(void *Data_) const
{
    Data_=MIDListItem::SetData(Data_);
	Data_=MemSetLine(Data_,Name);
    Data_=MemSet(Data_,Programs);
    Data_=MemSet(Data_,Reboot);
    Data_=MemSet(Data_,CompsCnt);
    for (int i=0; i<CompsCnt; i++) Data_=MemSet(Data_,Comps[i]);
    Data_=Times.SetAllData(Data_);
    return Data_;
}

const void *MTariffsItem::GetData(const void *Data_, const void *Limit_)
{
    if ( !(
		(Data_=MIDListItem::GetData(Data_,Limit_)) &&
		(Data_=MemGetLine(Data_,Name,MAX_TariffNameLen,Limit_)) &&
        (Data_=MemGet(Data_,&Programs,Limit_)) &&
        (Data_=MemGet(Data_,&Reboot,Limit_)) &&
		(Data_=MemGet(Data_,&CompsCnt,Limit_))
		) ) return nullptr;

	if ( (CompsCnt<0) || (CompsCnt>sizeof(Comps)) ) return nullptr;

    for ( int i=0; i<CompsCnt; i++ )
        if ( !(Data_=MemGet(Data_,&Comps[i],Limit_)) ) return nullptr;

	if ( !(Data_=Times.GetAllData(Data_,Limit_)) ) return nullptr;

	return Data_;
}

void MTariffsItem::CostPacket(MTariffRunTimesItem *RunTime_) const
{
    int StartTime, WorkTime;
    double Cost;

    // �������� ���������� ����� � ������ �����
    StartTime=ExtractHoursMin(RunTime_->StartTime);
    //
    WorkTime=0; Cost=0.;

	for ( MTariffTimesItem* tt=Times.gFirst(); tt; tt=tt->gNext() )
	{
		if ( (tt->Type!=mttPacket)||
			(tt->BeginTime!=RunTime_->BeginTime)||
			(tt->EndTime!=RunTime_->EndTime) ) continue;
		WorkTime=tt->MaxWorkTime(StartTime);
		// ������������ ����� ������ � ������������ � �������� ������������
		if ( WorkTime>RunTime_->MaxTime ) WorkTime=RunTime_->MaxTime;
		if ( WorkTime!=0 ) Cost=tt->Cost;
		break;
	}

	RunTime_->WorkTime=WorkTime;
	RunTime_->Cost=Cost;
}

void MTariffsItem::CostFlyPacket(MTariffRunTimesItem *RunTime_) const
{
	int StartTime, SizeTime, WorkTime, MaxTime, NeedTime, RealTime;
	double Cost, LastCost;

	// �������� ���������� ����� � ������ �����
	StartTime=ExtractHoursMin(RunTime_->StartTime);
	//
	SizeTime=RunTime_->SizeTime;
	WorkTime=0; Cost=LastCost=0.;

	while(true)
	{
		// ���� ���������� ������ ��� ������ � ��������� �������� ������
		MTariffTimesItem* tt=Times.gFirst();
		for ( ; tt; tt=tt->gNext() )
		{
			if ( (tt->Type!=mttFlyPacket)||
				(tt->SizeTime!=SizeTime) ) continue;
			if ( (MaxTime=tt->MaxWorkTime(StartTime))!=0 ) break;
		}
		// ���� ���������� ������� ������ ����, �� ����������� ���������� ������� � ���������
		if ( tt==nullptr )
		{
			// ����������� ��������� "����������" � �������� ����� �������
			// �� ��������� ����������� ��������� ������
			Cost+=(SizeTime-WorkTime)*(LastCost/SizeTime);
			break;
		}
		LastCost=tt->Cost;
		// ��������� ������� ��� ������� ����� ��� ������������ ������� ������
		NeedTime=SizeTime-WorkTime;
		// ��������� ������� ������� ������� ����� �������� � ������
		RealTime=MaxTime<NeedTime? MaxTime: NeedTime;
		// ��������� ��� ����� � ������ � ����������� ��������� ������
		WorkTime+=RealTime; Cost+=RealTime*(LastCost/SizeTime);
		// ��������� �� ������ �� ����� ������ ���
		if ( WorkTime==SizeTime ) break;
		// ����������� ����� ������ ������� � ������, ����� ��� �� ��������� �������� �����
		StartTime+=RealTime; if ( StartTime>=(24*60) ) StartTime-=24*60;
	}
	// ������������ ����� ������ � ������������ � �������� ������������
	if ( WorkTime>RunTime_->MaxTime ) WorkTime=RunTime_->MaxTime;

	RunTime_->WorkTime=WorkTime;
	RunTime_->Cost=Cost;
}

void MTariffsItem::CostHours(MTariffRunTimesItem *RunTime_) const
{
	int StartTime, SizeTime, WorkTime, MaxTime, NeedTime, RealTime;
	double Cost;

	// �������� ���������� ����� � ������ �����
	StartTime=ExtractHoursMin(RunTime_->StartTime);
	// ������������ �������� ����� ������ � ������������ � �������� ������������
	SizeTime=RunTime_->SizeTime;
	if ( SizeTime>RunTime_->MaxTime ) SizeTime=RunTime_->MaxTime;
	//
	WorkTime=0; Cost=0.;

	while(true)
	{
		// ���� ���������� ������ � ��������� �������� ������
		MTariffTimesItem* tt=Times.gFirst();
		for ( ; tt; tt=tt->gNext() )
        {
			if ( (tt->Type==mttHours)&&
				((MaxTime=tt->MaxWorkTime(StartTime))!=0) ) break;
		}
		// ���� ���������� ������� ������ ����, �� ����������� ���������� ������� � ���������
		if ( tt==nullptr ) break;
		// ��������� ������� ��� ������� ����� ��������
        NeedTime=SizeTime-WorkTime;
        // ��������� ������� ������� ������� ����� ��������
        RealTime=MaxTime<NeedTime? MaxTime: NeedTime;
        // ��������� ��� ����� � ����������� ���������
        WorkTime+=RealTime;
        Cost+=RealTime*tt->Cost/60.;
        // ��������� �� ������� �� ��� ��� �����
        if ( WorkTime==SizeTime ) break;
        // ����������� ����� ������ ������� � ������, ����� ��� �� ��������� �������� �����
        StartTime+=RealTime; if ( StartTime>=(24*60) ) StartTime-=24*60;
    }

    RunTime_->WorkTime=WorkTime;
    RunTime_->Cost=Cost;
}

void MTariffsItem::Cost(MTariffRunTimesItem *RunTime_, double Prec_) const
{
    if ( !CheckForComp(RunTime_->Number) )
    {
        RunTime_->WorkTime=0; RunTime_->Cost=0.;
        return;
    }

    switch(RunTime_->Type)
    {
        case mttHours: CostHours(RunTime_); break;
        case mttFlyPacket: CostFlyPacket(RunTime_); break;
        case mttPacket: CostPacket(RunTime_); break;
        default: RunTime_->WorkTime=0; RunTime_->Cost=0.; break;
    }
    // ��������� ��������� �� �������� ��������
    RunTime_->Cost=ceil(RunTime_->Cost/Prec_)*Prec_;
}

bool MTariffsItem::SetComps(char *Comps_, int Count_)
{
    if ( (Count_<0)||(Count_>sizeof(Comps)) ) return false;
    CompsCnt=Count_;
    for ( int i=0; i<Count_; i++ ) Comps[i]=Comps_[i];
    return true;
}

bool MTariffsItem::CheckForTime(__int64 &Time_) const
{
    int Time;

    // �������� ���������� ����� � ������ �����
    Time=ExtractHoursMin(Time_);

	for ( MTariffTimesItem* tt=Times.gFirst();
		tt; tt=tt->gNext() )
		if ( tt->MaxWorkTime(Time)>0 ) return true;

	return false;
}

bool MTariffsItem::CheckForComp(char Num_) const
{
	for ( int i=0; i<CompsCnt; i++ )
		if ( Comps[i]==Num_ ) return true;
	return false;
}

void MTariffsItem::GetInfo(MTariffsInfoItem *Info_) const
{
	Info_->ID=gUUID();
	Info_->Name=Name;
}

void MTariffsItem::GetRunTimes(__int64 &Time_, MTariffRunTimes *RunTimes_) const
{
	int SysTime;

	// �������� ���������� ����� � ������ �����
	SysTime=ExtractHoursMin(Time_);
	//
	RunTimes_->Clear();

	// ��������� ����������� ������� �� ��������� ������
	for ( MTariffTimesItem* tt=Times.gFirst(); tt; tt=tt->gNext() )
	{
		if ( (tt->Type!=mttHours)||
			(tt->MaxWorkTime(SysTime)==0) ) continue;

		MTariffRunTimesItem* rt=RunTimes_->Add();
		rt->Type=mttHours;
		break;
	}
	// ���� ���������� "���������" ������
	for ( MTariffTimesItem* tt=Times.gFirst(); tt; tt=tt->gNext() )
	{
		if ( (tt->Type!=mttFlyPacket)||
			(tt->MaxWorkTime(SysTime)==0) ) continue;

		MTariffRunTimesItem* rt=RunTimes_->Add();
		rt->Type=mttFlyPacket;
		rt->SizeTime=tt->SizeTime;
	}
	// ���� ���������� ������
	for ( MTariffTimesItem* tt=Times.gFirst(); tt; tt=tt->gNext() )
	{
		if ( (tt->Type!=mttPacket)||
			(tt->MaxWorkTime(SysTime)==0) ) continue;

		MTariffRunTimesItem* rt=RunTimes_->Add();
		rt->Type=mttPacket;
		rt->BeginTime=tt->BeginTime;
		rt->EndTime=tt->EndTime;
	}
}
//---------------------------------------------------------------------------
void MTariffs::GetForTime(__int64 &Time_, MTariffsInfo *TariffsInfo_) const
{
	TariffsInfo_->Clear();

	for ( MTariffsItem* trf=gFirst(); trf; trf=trf->gNext() )
	{
		if ( !trf->CheckForTime(Time_) ) continue;

		MTariffsInfoItem* ti=TariffsInfo_->Add();
		trf->GetInfo(ti);
	}
}
//---------------------------------------------------------------------------

