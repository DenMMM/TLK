//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitListOfTariffs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MListOfTariffsTariff &MListOfTariffsTariff::
    operator=(MListOfTariffsTariff &ListOfTariffsTariff_)
{
    Tariff=ListOfTariffsTariff_.Tariff;
    return *this;
}
//---------------------------------------------------------------------------
MListOfTariffsTariff::MListOfTariffsTariff()
{
    Tariff=NULL;
}
//---------------------------------------------------------------------------
MListOfTariffsTariff::~MListOfTariffsTariff()
{
//
}
//---------------------------------------------------------------------------
MListOfTariffs &MListOfTariffs::operator=(MListOfTariffs &ListOfTariffs_)
{
    Clear();
    for ( MListOfTariffsTariff *ListOfTariffsTariff, *ListOfTariffsTariff_=
        (MListOfTariffsTariff*)(ListOfTariffs_.FirstItem); ListOfTariffsTariff_;
        ListOfTariffsTariff_=(MListOfTariffsTariff*)(ListOfTariffsTariff_->NextItem) )
    {
        ListOfTariffsTariff=(MListOfTariffsTariff*)Add();
        *ListOfTariffsTariff=*ListOfTariffsTariff_;
    }
    return *this;
}
//---------------------------------------------------------------------------
MListOfTariffsTariff *MListOfTariffs::SearchTariff(MTariff *Tariff_)
{
    MListOfTariffsTariff *ListOfTariffsTariff=(MListOfTariffsTariff*)FirstItem;

    while(ListOfTariffsTariff)
    {
        if ( ListOfTariffsTariff->Tariff==Tariff_ ) return ListOfTariffsTariff;
        ListOfTariffsTariff=(MListOfTariffsTariff*)ListOfTariffsTariff->NextItem;
    }

    return NULL;
}
//---------------------------------------------------------------------------

