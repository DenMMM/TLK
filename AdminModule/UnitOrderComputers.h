//---------------------------------------------------------------------------
#ifndef UnitOrderComputersH
#define UnitOrderComputersH
//---------------------------------------------------------------------------
#include "UnitComputers.h"
#include "UnitTariffs.h"
#include "UnitListOfComputers.h"
//---------------------------------------------------------------------------
class MOrderComputer;
class MOrderComputers;
//---------------------------------------------------------------------------
class MOrderComputer:public MListItem
{
public:
    MComputer *Computer;  // Компьютер, для которого оформлен заказ
    MTariff *Tariff;  // По какому тарифу оформлен заказ
    int TimeWorkSize;  // На какое время занимается компьютер (в минутах)
    int Discount;  // Скидка при подсчете стоимости заказа (в процентах)
    double Cost;  // Просчитанная стоимость заказа

    MOrderComputer &operator=(MOrderComputer &OrderComputer_);

    MOrderComputer();
    ~MOrderComputer();
};
//---------------------------------------------------------------------------
class MOrderComputers:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MOrderComputer); }
    void delete_(MListItem *ListItem_) { delete (MOrderComputer*)ListItem_; }
public:
    MOrderComputers() { constructor(); }
    ~MOrderComputers() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

