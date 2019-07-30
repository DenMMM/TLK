//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitListOfTariffTimes.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MListOfTariffTimesTariffTime &MListOfTariffTimesTariffTime::
    operator=(MListOfTariffTimesTariffTime &ListOfTariffTimesTariffTime_)
{
    TariffTime=ListOfTariffTimesTariffTime_.TariffTime;
    return *this;
}
//---------------------------------------------------------------------------
MListOfTariffTimesTariffTime::MListOfTariffTimesTariffTime()
{
    TariffTime=NULL;
}
//---------------------------------------------------------------------------
MListOfTariffTimesTariffTime::~MListOfTariffTimesTariffTime()
{
//
}
//---------------------------------------------------------------------------
MListOfTariffTimes &MListOfTariffTimes::operator=(MListOfTariffTimes &ListOfTariffTimes_)
{
    Clear();
    for ( MListOfTariffTimesTariffTime *ListOfTariffTimesTariffTime, *ListOfTariffTimesTariffTime_=
        (MListOfTariffTimesTariffTime*)(ListOfTariffTimes_.FirstItem); ListOfTariffTimesTariffTime_;
        ListOfTariffTimesTariffTime_=(MListOfTariffTimesTariffTime*)(ListOfTariffTimesTariffTime_->NextItem) )
    {
        ListOfTariffTimesTariffTime=(MListOfTariffTimesTariffTime*)Add();
        *ListOfTariffTimesTariffTime=*ListOfTariffTimesTariffTime_;
    }
    return *this;
}
//---------------------------------------------------------------------------
MListOfTariffTimesTariffTime *MListOfTariffTimes::SearchTariffTime(MTariffTime *TariffTime_)
{
    MListOfTariffTimesTariffTime *ListOfTariffTimesTariffTime=(MListOfTariffTimesTariffTime*)FirstItem;

    while(ListOfTariffTimesTariffTime)
    {
        if ( ListOfTariffTimesTariffTime->TariffTime==TariffTime_ ) return ListOfTariffTimesTariffTime;
        ListOfTariffTimesTariffTime=(MListOfTariffTimesTariffTime*)ListOfTariffTimesTariffTime->NextItem;
    }

    return NULL;
}
//---------------------------------------------------------------------------

