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

bool MRunTime::Copy(MListItem *SrcItem_)
{
    MRunTime *RunTime_=(MRunTime*)SrcItem_;
    TariffID=RunTime_->TariffID;
    Number=RunTime_->Number;
    StartTime=RunTime_->StartTime;
    Type=RunTime_->Type;
    BeginTime=RunTime_->BeginTime;
    EndTime=RunTime_->EndTime;
    SizeTime=RunTime_->SizeTime;
    WorkTime=RunTime_->WorkTime;
    MaxTime=RunTime_->MaxTime;
    Cost=RunTime_->Cost;
    return true;
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

bool MTariffTime::Copy(MListItem *SrcItem_)
{
    MTariffTime *TariffTime_=(MTariffTime*)SrcItem_;
    Type=TariffTime_->Type;
    BeginTime=TariffTime_->BeginTime;
    EndTime=TariffTime_->EndTime;
    SizeTime=TariffTime_->SizeTime;
    Cost=TariffTime_->Cost;
    return true;
}

unsigned MTariffTime::GetDataSize()
{
    return
        sizeof(Type)+
        sizeof(BeginTime)+
        sizeof(EndTime)+
        sizeof(SizeTime)+
        sizeof(Cost);
}

char *MTariffTime::SetData(char *Data_)
{
    Data_=MemSet(Data_,Type);
    Data_=MemSet(Data_,BeginTime);
    Data_=MemSet(Data_,EndTime);
    Data_=MemSet(Data_,SizeTime);
    Data_=MemSet(Data_,Cost);
    return Data_;
}

char *MTariffTime::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&Type,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&BeginTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&EndTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&SizeTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Cost,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

int MTariffTime::MaxWorkTime(int Time_)
{
    int WorkTime;

    WorkTime=EndTime-(Time_<BeginTime? Time_+24*60: Time_);
    if ( WorkTime<0 ) WorkTime=0;
    else if ( (Type==mttFlyPacket)&&(WorkTime>SizeTime) ) WorkTime=SizeTime;

    return WorkTime;
}

//---------------------------------------------------------------------------

MTariff::MTariff()
{
    ID=0;
    *Name=0;
    Programs=0;
    Reboot=false;
    ComputersNum=0;
    Computers=NULL;
}

MTariff::~MTariff()
{
    Times.Clear();
    delete[] Computers;
}

bool MTariff::Copy(MListItem *SrcItem_)
{
    MTariff *Tariff_=(MTariff*)SrcItem_;
    ID=Tariff_->ID;
    strcpy(Name,Tariff_->Name);
    Programs=Tariff_->Programs;
    Reboot=Tariff_->Reboot;
    Times.Copy(&Tariff_->Times);
    ComputersNum=Tariff_->ComputersNum;
    delete[] Computers; Computers=new int[ComputersNum];
    memcpy(Computers,Tariff_->Computers,sizeof(int)*ComputersNum);
    return true;
}

unsigned MTariff::GetDataSize()
{
    return
        sizeof(ID)+
        strlen(Name)+1+
        sizeof(Programs)+
        sizeof(Reboot)+
        Times.GetAllDataSize(true)+
        sizeof(ComputersNum)+
        sizeof(*Computers)*ComputersNum;
}

char *MTariff::SetData(char *Data_)
{
    Data_=MemSet(Data_,ID);
    Data_=MemSetCLine(Data_,Name);
    Data_=MemSet(Data_,Programs);
    Data_=MemSet(Data_,Reboot);
    Data_=Times.SetAllData(Data_,true);
    Data_=MemSet(Data_,ComputersNum);
    for ( int i=0; i<ComputersNum; i++ ) Data_=MemSet(Data_,Computers[i]);
    return Data_;
}

char *MTariff::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&ID,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetCLine(Data_,Name,MAX_TariffNameLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Programs,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Reboot,Limit_))==NULL ) goto error;
    if ( (Data_=Times.GetAllData(Data_,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&ComputersNum,Limit_))==NULL ) goto error;
    delete[] Computers; if ( (Computers=new int[ComputersNum])==NULL ) goto error;
    for ( int i=0; i<ComputersNum; i++ )
        if ( (Data_=MemGet(Data_,Computers+i,Limit_))==NULL ) return NULL;
    return Data_;
error:
    return NULL;
}

void MTariff::CostPacket(MRunTime *RunTime_)
{
    int StartTime, WorkTime;
    double Cost;

    // Выделяем количество минут с начала суток
    StartTime=ExtractHoursMin(RunTime_->StartTime);
    //
    WorkTime=0; Cost=0.;

    for ( MTariffTime *TariffTime=(MTariffTime*)Times.First;
        TariffTime; TariffTime=(MTariffTime*)TariffTime->Next )
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

void MTariff::CostFlyPacket(MRunTime *RunTime_)
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
        for ( TariffTime=(MTariffTime*)Times.First;
            TariffTime; TariffTime=(MTariffTime*)TariffTime->Next )
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

void MTariff::CostHours(MRunTime *RunTime_)
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
        for ( TariffTime=(MTariffTime*)Times.First;
            TariffTime; TariffTime=(MTariffTime*)TariffTime->Next )
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

void MTariff::Cost(MRunTime *RunTime_, double Prec_)
{
    if ( !CheckForComputer(RunTime_->Number) )
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

char *MTariff::SetName(char *Name_)
{
    return strlen(Name_)>MAX_TariffNameLength?
        NULL: strcpy(Name,Name_);
}

int *MTariff::SetComputers(int *Computers_, int Num_)
{
    delete[] Computers;
    ComputersNum=(Computers=new int[Num_])==NULL?0:Num_;
    memcpy(Computers,Computers_,sizeof(int)*Num_);
    return Computers;
}

bool MTariff::CheckForTime(__int64 &Time_)
{
    int Time;

    // Выделяем количество минут с начала суток
    Time=ExtractHoursMin(Time_);

    for ( MTariffTime *TariffTime=(MTariffTime*)Times.First;
        TariffTime; TariffTime=(MTariffTime*)TariffTime->Next )
        if ( TariffTime->MaxWorkTime(Time) ) return true;

    return false;
}

bool MTariff::CheckForComputer(int Number_)
{
    int *Current=Computers, *End=Computers+ComputersNum;
    while(Current<End) if ( (*(Current++))==Number_ ) return true;
    return false;
}

void MTariff::GetRunTimes(__int64 &Time_, MRunTimes *RunTimes_)
{
    int SysTime;
    MTariffTime *TariffTime;

    // Выделяем количество минут с начала суток
    SysTime=ExtractHoursMin(Time_);
    //
    RunTimes_->Clear();

    // Проверяем возможность запуска на почасовой основе
    for ( TariffTime=(MTariffTime*)Times.First;
        TariffTime; TariffTime=(MTariffTime*)TariffTime->Next )
    {
        if ( (TariffTime->Type!=mttHours)||
            (TariffTime->MaxWorkTime(SysTime)==0) ) continue;
        MRunTime *NewTime=(MRunTime*)RunTimes_->Add();
        NewTime->Type=mttHours;
        break;
    }
    // Ищем подходящие "плавающие" пакеты
    for ( TariffTime=(MTariffTime*)Times.First;
        TariffTime; TariffTime=(MTariffTime*)TariffTime->Next )
    {
        if ( (TariffTime->Type!=mttFlyPacket)||
            (TariffTime->MaxWorkTime(SysTime)==0) ) continue;
        MRunTime *NewTime=(MRunTime*)RunTimes_->Add();
        NewTime->Type=mttFlyPacket;
        NewTime->SizeTime=TariffTime->SizeTime;
    }
    // Ищем подходящие пакеты
    for ( TariffTime=(MTariffTime*)Times.First;
        TariffTime; TariffTime=(MTariffTime*)TariffTime->Next )
    {
        if ( (TariffTime->Type!=mttPacket)||
            (TariffTime->MaxWorkTime(SysTime)==0) ) continue;
        MRunTime *NewTime=(MRunTime*)RunTimes_->Add();
        NewTime->Type=mttPacket;
        NewTime->BeginTime=TariffTime->BeginTime;
        NewTime->EndTime=TariffTime->EndTime;
    }
}

bool MTariff::GetInfo(MTariffInfo *Info_)
{
    Info_->ID=ID;
    return Info_->SetName(Name);
}

void MTariff::GetTariffData(MTariffData *Data_)
{
    Data_->ID=ID;
    strcpy(Data_->Name,Name);
}

void MTariff::SetTariffData(MTariffData *Data_)
{
    ID=Data_->ID;
    strcpy(Name,Data_->Name);
}

//---------------------------------------------------------------------------

MTariff *MTariffs::Search(unsigned ID_)
{
    MTariff *Tariff=(MTariff*)First;
    while(Tariff)
    {
        if ( Tariff->ID==ID_ ) break;
        Tariff=(MTariff*)Tariff->Next;
    }
    return Tariff;
}

unsigned MTariffs::NextID()
{
    do { LastID+=random(10)+1; } while(!LastID);
    return LastID;
}

void MTariffs::SetIDs()
{
    for ( MTariff *Tariff=(MTariff*)First;
        Tariff; Tariff=(MTariff*)Tariff->Next )
        if ( Tariff->ID==0 ) Tariff->ID=NextID();
}

void MTariffs::GetForTime(__int64 &Time_, MTariffsInfo *TariffsInfo_)
{
    TariffsInfo_->Clear();

    for ( MTariff *Tariff=(MTariff*)First;
        Tariff; Tariff=(MTariff*)Tariff->Next )
    {
        if ( !Tariff->CheckForTime(Time_) ) continue;
        MTariffInfo *Info;
        Info=(MTariffInfo*)TariffsInfo_->Add();
        Tariff->GetInfo(Info);
    }
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

bool MTariffInfo::Copy(MListItem *SrcItem_)
{
    MTariffInfo *TariffInfo_=(MTariffInfo*)SrcItem_;
    ID=TariffInfo_->ID;
    strcpy(Name,TariffInfo_->Name);
    return true;
}

char *MTariffInfo::SetName(char *Name_)
{
    return strlen(Name_)>MAX_TariffNameLength? NULL: strcpy(Name,Name_);
}

//---------------------------------------------------------------------------

MTariffInfo *MTariffsInfo::Search(unsigned ID_)
{
    MTariffInfo *TariffInfo=(MTariffInfo*)First;
    while(TariffInfo)
    {
        if ( TariffInfo->ID==ID_ ) return TariffInfo;
        TariffInfo=(MTariffInfo*)TariffInfo->Next;
    }
    return NULL;
}

//---------------------------------------------------------------------------

