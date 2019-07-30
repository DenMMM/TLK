//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitTariffs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MTariffTime &MTariffTime::operator=(MTariffTime &TariffTime_)
{
    BeginTime=TariffTime_.BeginTime;
    SizeTime=TariffTime_.SizeTime;
    EndTime=TariffTime_.EndTime;
    WeekDays=TariffTime_.WeekDays;
    Cost=TariffTime_.Cost;
    EnableDesc=TariffTime_.EnableDesc;
    return *this;
}
//---------------------------------------------------------------------------
MTariffTime::MTariffTime()
{
    BeginTime=0;
    SizeTime=0;
    EndTime=0;
    WeekDays=0;
    Cost=0.;
    EnableDesc=false;
}
//---------------------------------------------------------------------------
MTariffTime::~MTariffTime()
{
//
}
//---------------------------------------------------------------------------
MTariffTimes &MTariffTimes::operator=(MTariffTimes &TariffTimes_)
{
    Clear();
    for ( MTariffTime *TariffTime, *TariffTime_=(MTariffTime*)TariffTimes_.FirstItem;
        TariffTime_; TariffTime_=(MTariffTime*)TariffTime_->NextItem )
    {
        TariffTime=(MTariffTime*)Add();
        *TariffTime=*TariffTime_;
    }
    return *this;
}
//---------------------------------------------------------------------------
MTariff &MTariff::operator=(MTariff &Tariff_)
{
    ID=Tariff_.ID;
    Name=Tariff_.Name;
    IconFileName=Tariff_.IconFileName;
    IconNumber=Tariff_.IconNumber;
    FaceType=Tariff_.FaceType;
    GamesPages=Tariff_.GamesPages;
    Reboot=Tariff_.Reboot;
    Times=Tariff_.Times;
    Computers=Tariff_.Computers;
    return *this;
}
//---------------------------------------------------------------------------
MTariff::MTariff()
{
    ID=0;
    FaceType=0;
    GamesPages=0;
    Reboot=false;
    IconNumber=-1;
}
//---------------------------------------------------------------------------
MTariff::~MTariff()
{
    Times.Clear();
    Computers.Clear();
}
//---------------------------------------------------------------------------
MTariff *MTariffs::SearchID(unsigned int ID_)
{
    MTariff *Tariff=(MTariff*)FirstItem;
    while(Tariff)
    {
        if ( Tariff->ID==ID_ ) break;
        Tariff=(MTariff*)Tariff->NextItem;
    }
    return Tariff;
}
//---------------------------------------------------------------------------
MTariffs &MTariffs::operator=(MTariffs &Tariffs_)
{
    Clear();
    for ( MTariff *Tariff, *Tariff_=(MTariff*)Tariffs_.FirstItem;
        Tariff_; Tariff_=(MTariff*)Tariff_->NextItem )
    {
        Tariff=(MTariff*)Add();
        *Tariff=*Tariff_;
    }
    return *this;
}
//---------------------------------------------------------------------------

