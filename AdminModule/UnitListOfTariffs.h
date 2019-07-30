//---------------------------------------------------------------------------
#ifndef UnitListOfTariffsH
#define UnitListOfTariffsH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
#include "UnitTariffs.h"
//---------------------------------------------------------------------------
class MListOfTariffsTariff;
class MListOfTariffs;
//---------------------------------------------------------------------------
class MListOfTariffsTariff:public MListItem
{
public:
    MTariff *Tariff;

    MListOfTariffsTariff &operator=(MListOfTariffsTariff &ListOfTariffsTariff_);

    MListOfTariffsTariff();
    ~MListOfTariffsTariff();
};
//---------------------------------------------------------------------------
class MListOfTariffs:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MListOfTariffsTariff); }
    void delete_(MListItem *ListItem_) { delete (MListOfTariffsTariff*)ListItem_; }
public:
    MListOfTariffs &operator=(MListOfTariffs &ListOfTariffs_);

    MListOfTariffsTariff *SearchTariff(MTariff *Tariff_);

    MListOfTariffs() { constructor(); }
    ~MListOfTariffs() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

