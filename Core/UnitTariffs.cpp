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
    MRunTime *RTime_=(MRunTime*)SrcItem_;

    TariffID=RTime_->TariffID;
    Number=RTime_->Number;
    StartTime=RTime_->StartTime;
    Type=RTime_->Type;
    BeginTime=RTime_->BeginTime;
    EndTime=RTime_->EndTime;
    SizeTime=RTime_->SizeTime;
    WorkTime=RTime_->WorkTime;
    MaxTime=RTime_->MaxTime;
    Cost=RTime_->Cost;
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
    MTariffTime *TTime_=(MTariffTime*)SrcItem_;

    Type=TTime_->Type;
    BeginTime=TTime_->BeginTime;
    EndTime=TTime_->EndTime;
    SizeTime=TTime_->SizeTime;
    Cost=TTime_->Cost;
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
    *Name=0;
}

MTariffInfo::~MTariffInfo()
{
//
}

void MTariffInfo::Copy(const MListItem *SrcItem_)
{
    MTariffInfo *TariffInfo_=(MTariffInfo*)SrcItem_;
    ID=TariffInfo_->ID;
    strcpy(Name,TariffInfo_->Name);
}

char *MTariffInfo::SetName(const char *Name_)
{
    return strlen(Name_)>MAX_TariffNameLen?
        NULL: strcpy(Name,Name_);
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
    *Name=0;
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
    MTariff *Tariff_=(MTariff*)SrcItem_;

    strcpy(Name,Tariff_->Name);
    Programs=Tariff_->Programs;
    Reboot=Tariff_->Reboot;
    CompsCnt=Tariff_->CompsCnt;
    for (int i=0; i<CompsCnt; i++) Comps[i]=Tariff_->Comps[i];
    Times.Copy(&Tariff_->Times);
    MIDListItem::Copy(SrcItem_);
}

unsigned MTariff::GetDataSize() const
{
    return
        MIDListItem::GetDataSize()+
        strlen(Name)+1+
        sizeof(Programs)+
        sizeof(Reboot)+
        sizeof(CompsCnt)+
        sizeof(*Comps)*CompsCnt+
        Times.GetAllDataSize();
}

char *MTariff::SetData(char *Data_) const
{
    Data_=MIDListItem::SetData(Data_);
    Data_=MemSetCLine(Data_,Name);
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
        (Data_=MemGetCLine(Data_,Name,sizeof(Name)-1,Limit_))==NULL ||
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

    // Выделяем количество минут с начала суток
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
        // Корректируем время работы в соответствии с заданным ограничением
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

    // Выделяем количество минут с начала суток
    StartTime=ExtractHoursMin(RunTime_->StartTime);
    //
    SizeTime=RunTime_->SizeTime;
    WorkTime=0; Cost=LastCost=0.;

    while(true)
    {
        // Ищем подходящую запись для пакета с ненулевым временем работы
        for ( TariffTime=(MTariffTime*)Times.gFirst(); TariffTime;
            TariffTime=(MTariffTime*)TariffTime->gNext() )
        {
            if ( (TariffTime->Type!=mttFlyPacket)||
                (TariffTime->SizeTime!=SizeTime) ) continue;
            if ( (MaxTime=TariffTime->MaxWorkTime(StartTime))!=0 ) break;
        }
        // Если подходящих записей больше нету, то заканчиваем накопление времени и стоимости
        if ( TariffTime==NULL )
        {
            // Досчитываем стоимость "невлезшего" в тарифную сетку времени
            // по последней встреченной стоимости пакета
            Cost+=(SizeTime-WorkTime)*(LastCost/SizeTime);
            break;
        }
        LastCost=TariffTime->Cost;
        // Вычисляем сколько еще времени нужно для формирования полного пакета
        NeedTime=SizeTime-WorkTime;
        // Проверяем сколько реально времени можно добавить к пакету
        RealTime=MaxTime<NeedTime? MaxTime: NeedTime;
        // Добавляем это время к пакету и увеличиваем стоимость пакета
        WorkTime+=RealTime; Cost+=RealTime*(LastCost/SizeTime);
        // Проверяем не полное ли время пакета уже
        if ( WorkTime==SizeTime ) break;
        // Увеличиваем время начала отсчета и следим, чтобы оно не превысило суточное время
        StartTime+=RealTime; if ( StartTime>=(24*60) ) StartTime-=24*60;
    }
    // Корректируем время работы в соответствии с заданным ограничением
    if ( WorkTime>RunTime_->MaxTime ) WorkTime=RunTime_->MaxTime;

    RunTime_->WorkTime=WorkTime;
    RunTime_->Cost=Cost;
}

void MTariff::CostHours(MRunTime *RunTime_) const
{
    int StartTime, SizeTime, WorkTime, MaxTime, NeedTime, RealTime;
    double Cost;
    MTariffTime *TariffTime;

    // Выделяем количество минут с начала суток
    StartTime=ExtractHoursMin(RunTime_->StartTime);
    // Корректируем желаемое время работы в соответствии с заданным ограничением
    SizeTime=RunTime_->SizeTime;
    if ( SizeTime>RunTime_->MaxTime ) SizeTime=RunTime_->MaxTime;
    //
    WorkTime=0; Cost=0.;

    while(true)
    {
        // Ищем подходящую запись с ненулевым временем работы
        for ( TariffTime=(MTariffTime*)Times.gFirst();
            TariffTime; TariffTime=(MTariffTime*)TariffTime->gNext() )
        {
            if ( (TariffTime->Type==mttHours)&&
                ((MaxTime=TariffTime->MaxWorkTime(StartTime))!=0) ) break;
        }
        // Если подходящих записей больше нету, то заканчиваем накопление времени и стоимости
        if ( TariffTime==NULL ) break;
        // Вычисляем сколько еще времени нужно добавить
        NeedTime=SizeTime-WorkTime;
        // Проверяем сколько реально времени можно добавить
        RealTime=MaxTime<NeedTime? MaxTime: NeedTime;
        // Добавляем это время и увеличиваем стоимость
        WorkTime+=RealTime;
        Cost+=RealTime*TariffTime->Cost/60.;
        // Проверяем не набрано ли уже все время
        if ( WorkTime==SizeTime ) break;
        // Увеличиваем время начала отсчета и следим, чтобы оно не превысило суточное время
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
    // Округляем стоимость до желаемой точности
    RunTime_->Cost=ceil(RunTime_->Cost/Prec_)*Prec_;
}

char *MTariff::SetName(const char *Name_)
{
    return strlen(Name_)>MAX_TariffNameLen?
        NULL: strcpy(Name,Name_);
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

    // Выделяем количество минут с начала суток
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

    // Выделяем количество минут с начала суток
    SysTime=ExtractHoursMin(Time_);
    //
    RunTimes_->Clear();

    // Проверяем возможность запуска на почасовой основе
    for ( MTariffTime *TTime=(MTariffTime*)Times.gFirst();
        TTime; TTime=(MTariffTime*)TTime->gNext() )
    {
        if ( (TTime->Type!=mttHours)||
            (TTime->MaxWorkTime(SysTime)==0) ) continue;
        MRunTime *RTime=(MRunTime*)RunTimes_->Add();
        RTime->Type=mttHours;
        break;
    }
    // Ищем подходящие "плавающие" пакеты
    for ( MTariffTime *TTime=(MTariffTime*)Times.gFirst();
        TTime; TTime=(MTariffTime*)TTime->gNext() )
    {
        if ( (TTime->Type!=mttFlyPacket)||
            (TTime->MaxWorkTime(SysTime)==0) ) continue;
        MRunTime *RTime=(MRunTime*)RunTimes_->Add();
        RTime->Type=mttFlyPacket;
        RTime->SizeTime=TTime->SizeTime;
    }
    // Ищем подходящие пакеты
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
    return Info_->SetName(Name);
}

void MTariff::GetTariffData(MTariffData *Data_) const 
{
    Data_->ID=ItemID;
    strcpy(Data_->Name,Name);
}

void MTariff::SetTariffData(MTariffData *Data_)
{
    ItemID=Data_->ID;
    strcpy(Name,Data_->Name);
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

