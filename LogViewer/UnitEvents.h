//---------------------------------------------------------------------------
#ifndef UnitEventsH
#define UnitEventsH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
//---------------------------------------------------------------------------
class MEvent;
class MEvents;
//---------------------------------------------------------------------------
class MEvent:public MListItem
{
public:
    double Time;  // Системное время
    int ComputerNumber;  // Номер компьютера
    AnsiString TariffName;  // Название тарифа
    int TimeSize;  // На какое время
    double Cost;  // На какую сумму

    MEvent();
    ~MEvent();
};
//---------------------------------------------------------------------------
class MEvents:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MEvent); }
    void delete_(MListItem *ListItem_) { delete ((MEvent*)ListItem_); }

    char *GetData(char *Data_, char *LimitData_);
public:
    double OpenTime;  //
    double CloseTime;  //
    AnsiString Name;  // Имя работника
    AnsiString Job;  // Должность
    unsigned int Rights;  // Права

    bool Load(AnsiString FileName_);

    MEvents() { constructor(); }
    ~MEvents() { destructor(); }
};
//---------------------------------------------------------------------------
#endif
