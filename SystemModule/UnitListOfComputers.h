//---------------------------------------------------------------------------
#ifndef UnitListOfComputersH
#define UnitListOfComputersH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
#include "UnitComputers.h"
//---------------------------------------------------------------------------
class MListOfComputersComputer;
class MListOfComputers;
//---------------------------------------------------------------------------
class MListOfComputersComputer:public MListItem
{
public:
    MComputer *Computer;

    MListOfComputersComputer &operator=(MListOfComputersComputer &ListOfComputersComputer_);

    MListOfComputersComputer();
    ~MListOfComputersComputer();
};
//---------------------------------------------------------------------------
class MListOfComputers:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MListOfComputersComputer); }
    void delete_(MListItem *ListItem_) { delete (MListOfComputersComputer*)ListItem_; }
public:
    MListOfComputers &operator=(MListOfComputers &ListOfComputers_);

    MListOfComputersComputer *SearchComputer(MComputer *Computer_);

    MListOfComputers() { constructor(); }
    ~MListOfComputers() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

