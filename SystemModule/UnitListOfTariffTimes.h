//---------------------------------------------------------------------------
#ifndef UnitListOfTariffTimesH
#define UnitListOfTariffTimesH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
#include "UnitTariffs.h"
//---------------------------------------------------------------------------
class MListOfTariffTimesTariffTime;
class MListOfTariffTimes;
//---------------------------------------------------------------------------
class MListOfTariffTimesTariffTime:public MListItem
{
public:
    MTariffTime *TariffTime;

    MListOfTariffTimesTariffTime &operator=(MListOfTariffTimesTariffTime &ListOfTariffTimesTariffTime_);

    MListOfTariffTimesTariffTime();
    ~MListOfTariffTimesTariffTime();
};
//---------------------------------------------------------------------------
class MListOfTariffTimes:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MListOfTariffTimesTariffTime); }
    void delete_(MListItem *ListItem_) { delete (MListOfTariffTimesTariffTime*)ListItem_; }
public:
    MListOfTariffTimes &operator=(MListOfTariffTimes &ListOfTariffTimes_);

    MListOfTariffTimesTariffTime *SearchTariffTime(MTariffTime *TariffTime_);

    MListOfTariffTimes() { constructor(); }
    ~MListOfTariffTimes() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

