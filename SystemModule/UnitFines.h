//---------------------------------------------------------------------------
#ifndef UnitFinesH
#define UnitFinesH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
//---------------------------------------------------------------------------
class MFine;
class MFines;
//---------------------------------------------------------------------------
class MFine:public MListItem
{
public:
    int Time;
    AnsiString Comment;

    MFine &operator=(MFine &Fine_);

    MFine();
    ~MFine();
};
//---------------------------------------------------------------------------
class MFines:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MFine); }
    void delete_(MListItem *ListItem_) { delete ((MFine*)ListItem_); }
public:
    MFines &operator=(MFines &Fines_);

    MFines() { constructor(); }
    ~MFines() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

