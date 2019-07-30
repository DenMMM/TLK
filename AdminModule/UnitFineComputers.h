//---------------------------------------------------------------------------
#ifndef UnitFineComputersH
#define UnitFineComputersH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
#include "UnitComputers.h"
//---------------------------------------------------------------------------
class MFineComputer;
class MFineComputers;
//---------------------------------------------------------------------------
class MFineComputer:public MListItem
{
public:
    MComputer *Computer;  // Компьютер, к которому применяется штраф
    int FineTime;  // Время штрафа в минутах
    bool EnableReturnMany;  // Возвращать ли деньги при полном снятии времени
    double ReturnMany;  // Сумма возвращаемых денег

    MFineComputer &operator=(MFineComputer &FineComputer_);

    MFineComputer();
    ~MFineComputer();
};
//---------------------------------------------------------------------------
class MFineComputers:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MFineComputer); }
    void delete_(MListItem *ListItem_) { delete (MFineComputer*)ListItem_; }
public:
    MFineComputers() { constructor(); }
    ~MFineComputers() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

