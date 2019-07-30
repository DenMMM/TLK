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
MRunTime::MRunTime()
{
    TariffID=0;
    Number=0;
    StartTime=0;
    Type=mttUndefined;
    BeginTime=EndTime=SizeTime=0;
    WorkTime=MaxTime=0;
    Cost=0.;
}

MRunTime::~MRunTime()
{
//
}

void MRunTime::Copy(const MListItem *SrcItem_)
{
    const MRunTime *rtime=
        dynamic_cast<const MRunTime*>(SrcItem_);

    TariffID=rtime->TariffID;
    Number=rtime->Number;
    StartTime=rtime->StartTime;
    Type=rtime->Type;
    BeginTime=rtime->BeginTime;
    EndTime=rtime->EndTime;
    SizeTime=rtime->SizeTime;
    WorkTime=rtime->WorkTime;
    MaxTime=rtime->MaxTime;
    Cost=rtime->Cost;
}
//---------------------------------------------------------------------------
MTariffTime::MTariffTime()
{
    Type=mttHours;
    BeginTime=0;
    EndTime=24*60;
    SizeTime=0;
    Cost=0.;
}

MTariffTime::~MTariffTime()
{
//
}

void MTariffTime::Copy(const MListItem *SrcItem_)
{
    const MTariffTime *ttime=
        dynamic_cast<const MTariffTime*>(SrcItem_);

    Type=ttime->Type;
    BeginTime=ttime->BeginTime;
    EndTime=ttime->EndTime;
    SizeTime=ttime->SizeTime;
    Cost=ttime->Cost;
}

unsigned MTariffTime::GetDataSize() const
{
    return
        sizeof(Type)+
        sizeof(BeginTime)+
        sizeof(EndTime)+
        sizeof(SizeTime)+
        sizeof(Cost);
}

char *MTariffTime::SetData(char *Data_) const
{
    Data_=MemSet(Data_,Type);
    Data_=MemSet(Data_,BeginTime);
    Data_=MemSet(Data_,EndTime);
    Data_=MemSet(Data_,SizeTime);
    Data_=MemSet(Data_,Cost);
    return Data_;
}

const char *MTariffTime::GetData(const char *Data_, const char *Limit_)
{
    return
        (Data_=MemGet(Data_,&Type,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&BeginTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&EndTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&SizeTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&Cost,Limit_))!=NULL
        ? Data_: NULL;
}

int MTariffTime::MaxWorkTime(int Time_) const
{
    int WorkTime;

    WorkTime=EndTime-(Time_<BeginTime? Time_+24*60: Time_);
    if ( WorkTime<0 ) WorkTime=0;
    else if ( (Type==mttFlyPacket)&&(WorkTime>SizeTime) ) WorkTime=SizeTime;

    return WorkTime;
}
//---------------------------------------------------------------------------
MTariffInfo::MTariffInfo()
{
    ID=0;
}

MTariffInfo::~MTariffInfo()
{
//
}

void MTariffInfo::Copy(const MListItem *SrcItem_)
{
    const MTariffInfo *ti=
        dynamic_cast<const MTariffInfo*>(SrcItem_);

    ID=ti->ID;
    Name=ti->Name;
}

//---------------------------------------------------------------------------
MTariffInfo *MTariffsInfo::Search(unsigned ID_) const
{
    MTariffInfo *TariffInfo=(MTariffInfo*)gFirst();
    while(TariffInfo)
    {
        if ( TariffInfo->ID==ID_ ) return TariffInfo;
        TariffInfo=(MTariffInfo*)TariffInfo->gNext();
    }
    return NULL;
}
//---------------------------------------------------------------------------
MTariff::MTariff()
{
    Programs=0;
    Reboot=false;
    CompsCnt=0;
}

MTariff::~MTariff()
{
//
}

void MTariff::Copy(const MListItem *SrcItem_)
{
    const MTariff *trf=
        dynamic_cast<const MTariff*>(SrcItem_);

    Name=trf->Name;
    Programs=trf->Programs;
    Reboot=trf->Reboot;
    CompsCnt=trf->CompsCnt;
    for (int i=0; i<CompsCnt; i++) Comps[i]=trf->Comps[i];
    Times.Copy(&trf->Times);
    MIDListItem::Copy(SrcItem_);
}

unsigned MTariff::GetDataSize() const
{
    return
        MIDListItem::GetDataSize()+
        Name.length()+sizeof('\0')+
        sizeof(Programs)+
        sizeof(Reboot)+
        sizeof(CompsCnt)+
        sizeof(*Comps)*CompsCnt+
        Times.GetAllDataSize();
}

char *MTariff::SetData(char *Data_) const
{
    Data_=MIDListItem::SetData(Data_);
    Data_=MemSetCLine(Data_,Name.c_str());
    Data_=MemSet(Data_,Programs);
    Data_=MemSet(Data_,Reboot);
    Data_=MemSet(Data_,CompsCnt);
    for (int i=0; i<CompsCnt; i++) Data_=MemSet(Data_,Comps[i]);
    Data_=Times.SetAllData(Data_);
    return Data_;
}

const char *MTariff::GetData(const char *Data_, const char *Limit_)
{
    if (
        (Data_=MIDListItem::GetData(Data_,Limit_))==NULL ||
        (Data_=MemGetCLine(Data_,Name,MAX_TariffNameLen,Limit_))==NULL ||
        (Data_=MemGet(Data_,&Programs,Limit_))==NULL ||
        (Data_=MemGet(Data_,&Reboot,Limit_))==NULL ||
        (Data_=MemGet(Data_,&CompsCnt,Limit_))==NULL ||
        (CompsCnt<0)||(CompsCnt>sizeof(Comps))
        ) goto error;
    for ( int i=0; i<CompsCnt; i++ )
        if ( (Data_=MemGet(Data_,&Comps[i],Limit_))==NULL ) goto error;
    if ( (Data_=Times.GetAllData(Data_,Limit_))==NULL ) goto error;

    return Data_;
error:
    return NULL;
}

void MTariff::CostPacket(MRunTime *RunTime_) const
{
    int StartTime, WorkTime;
    double Cost;

    // �������� ���������� ����� � ������ �����
    StartTime=ExtractHoursMin(RunTime_->StartTime);
    //
    WorkTime=0; Cost=0.;

    for ( MTariffTime *TariffTime=(MTariffTime*)Times.gFirst();
        TariffTime; TariffTime=(MTariffTime*)TariffTime->gNext() )
    {
        if ( (TariffTime->Type!=mttPacket)||
            (TariffTime->BeginTime!=RunTime_->BeginTime)||
            (TariffTime->EndTime!=RunTime_->EndTime) ) continue;
        WorkTime=TariffTime->MaxWorkTime(StartTime);
        // ������������ ����� ������ � ������������ � �������� ������������
        if ( WorkTime>RunTime_->MaxTime ) WorkTime=RunTime_->MaxTime;
        if ( WorkTime!=0 ) Cost=TariffTime->Cost;
        break;
    }

    RunTime_->WorkTime=WorkTime;
    RunTime_->Cost=Cost;
}

void MTariff::CostFlyPacket(MRunTime *RunTime_) const
{
    int StartTime, SizeTime, WorkTime, MaxTime, NeedTime, RealTime;
    double Cost, LastCost;
    MTariffTime *TariffTime;

    // �������� ���������� ����� � ������ �����
    StartTime=ExtractHoursMin(RunTime_->StartTime);
    //
    SizeTime=RunTime_->SizeTime;
    WorkTime=0; Cost=LastCost=0.;

    while(true)
    {
        // ���� ���������� ������ ��� ������ � ��������� �������� ������
        for ( TariffTime=(MTariffTime*)Times.gFirst(); TariffTime;
            TariffTime=(MTariffTime*)TariffTime->gNext() )
        {
            if ( (TariffTime->Type!=mttFlyPacket)||
                (TariffTime->SizeTime!=SizeTime) ) continue;
            if ( (MaxTime=TariffTime->MaxWorkTime(StartTime))!=0 ) break;
        }
        // ���� ���������� ������� ������ ����, �� ����������� ���������� ������� � ���������
        if ( TariffTime==NULL )
        {
            // ����������� ��������� "����������" � �������� ����� �������
            // �� ��������� ����������� ��������� ������
            Cost+=(SizeTime-WorkTime)*(LastCost/SizeTime);
            break;
        }
        LastCost=TariffTime->Cost;
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

void MTariff::CostHours(MRunTime *RunTime_) const
{
    int StartTime, SizeTime, WorkTime, MaxTime, NeedTime, RealTime;
    double Cost;
    MTariffTime *TariffTime;

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
        for ( TariffTime=(MTariffTime*)Times.gFirst();
            TariffTime; TariffTime=(MTariffTime*)TariffTime->gNext() )
        {
            if ( (TariffTime->Type==mttHours)&&
                ((MaxTime=TariffTime->MaxWorkTime(StartTime))!=0) ) break;
        }
        // ���� ���������� ������� ������ ����, �� ����������� ���������� ������� � ���������
        if ( TariffTime==NULL ) break;
        // ��������� ������� ��� ������� ����� ��������
        NeedTime=SizeTime-WorkTime;
        // ��������� ������� ������� ������� ����� ��������
        RealTime=MaxTime<NeedTime? MaxTime: NeedTime;
        // ��������� ��� ����� � ����������� ���������
        WorkTime+=RealTime;
        Cost+=RealTime*TariffTime->Cost/60.;
        // ��������� �� ������� �� ��� ��� �����
        if ( WorkTime==SizeTime ) break;
        // ����������� ����� ������ ������� � ������, ����� ��� �� ��������� �������� �����
        StartTime+=RealTime; if ( StartTime>=(24*60) ) StartTime-=24*60;
    }

    RunTime_->WorkTime=WorkTime;
    RunTime_->Cost=Cost;
}

void MTariff::Cost(MRunTime *RunTime_, double Prec_) const
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

bool MTariff::SetComps(char *Comps_, int Count_)
{
    if ( (Count_<0)||(Count_>sizeof(Comps)) ) return false;
    CompsCnt=Count_;
    for ( int i=0; i<Count_; i++ ) Comps[i]=Comps_[i];
    return true;
}

bool MTariff::CheckForTime(__int64 &Time_) const
{
    int Time;

    // �������� ���������� ����� � ������ �����
    Time=ExtractHoursMin(Time_);

    for ( MTariffTime *TariffTime=(MTariffTime*)Times.gFirst();
        TariffTime; TariffTime=(MTariffTime*)TariffTime->gNext() )
        if ( TariffTime->MaxWorkTime(Time)>0 ) return true;

    return false;
}

bool MTariff::CheckForComp(char Num_) const
{
    for ( int i=0; i<CompsCnt; i++ )
        if ( Comps[i]==Num_ ) return true;
    return false;
}

bool MTariff::GetRunTimes(__int64 &Time_, MRunTimes *RunTimes_) const
{
    int SysTime;

    // �������� ���������� ����� � ������ �����
    SysTime=ExtractHoursMin(Time_);
    //
    RunTimes_->Clear();

    // ��������� ����������� ������� �� ��������� ������
    for ( MTariffTime *TTime=(MTariffTime*)Times.gFirst();
        TTime; TTime=(MTariffTime*)TTime->gNext() )
    {
        if ( (TTime->Type!=mttHours)||
            (TTime->MaxWorkTime(SysTime)==0) ) continue;
        MRunTime *RTime=(MRunTime*)RunTimes_->Add();
        RTime->Type=mttHours;
        break;
    }
    // ���� ���������� "���������" ������
    for ( MTariffTime *TTime=(MTariffTime*)Times.gFirst();
        TTime; TTime=(MTariffTime*)TTime->gNext() )
    {
        if ( (TTime->Type!=mttFlyPacket)||
            (TTime->MaxWorkTime(SysTime)==0) ) continue;
        MRunTime *RTime=(MRunTime*)RunTimes_->Add();
        RTime->Type=mttFlyPacket;
        RTime->SizeTime=TTime->SizeTime;
    }
    // ���� ���������� ������
    for ( MTariffTime *TTime=(MTariffTime*)Times.gFirst();
        TTime; TTime=(MTariffTime*)TTime->gNext() )
    {
        if ( (TTime->Type!=mttPacket)||
            (TTime->MaxWorkTime(SysTime)==0) ) continue;
        MRunTime *RTime=(MRunTime*)RunTimes_->Add();
        RTime->Type=mttPacket;
        RTime->BeginTime=TTime->BeginTime;
        RTime->EndTime=TTime->EndTime;
    }

    return true;
error:
    RunTimes_->Clear();
    return false;
}

bool MTariff::GetInfo(MTariffInfo *Info_) const
{
    Info_->ID=gItemID();
    Info_->Name=Name;
    return true;
}
//---------------------------------------------------------------------------
void MTariffs::GetForTime(__int64 &Time_, MTariffsInfo *TariffsInfo_) const
{
    TariffsInfo_->Clear();

    for ( MTariff *Tariff=(MTariff*)gFirst();
        Tariff; Tariff=(MTariff*)Tariff->gNext() )
    {
        if ( !Tariff->CheckForTime(Time_) ) continue;
        MTariffInfo *Info;
        Info=(MTariffInfo*)TariffsInfo_->Add();
        Tariff->GetInfo(Info);
    }
}
//---------------------------------------------------------------------------

