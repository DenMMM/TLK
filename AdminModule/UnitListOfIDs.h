//---------------------------------------------------------------------------
#ifndef UnitListOfIDsH
#define UnitListOfIDsH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
//---------------------------------------------------------------------------
class MListOfIDsID;
class MListOfIDs;
//---------------------------------------------------------------------------
class MListOfIDsID:public MListItem
{
public:
    unsigned int ID;

    MListOfIDsID &operator=(MListOfIDsID &ListOfIDsID_);

    MListOfIDsID();
    ~MListOfIDsID();
};
//---------------------------------------------------------------------------
class MListOfIDs:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MListOfIDsID); }
    void delete_(MListItem *ListItem_) { delete (MListOfIDsID*)ListItem_; }
public:
    MListOfIDs &operator=(MListOfIDs &ListOfIDs_);

    MListOfIDsID *SearchID(unsigned int ID_);

    MListOfIDs() { constructor(); }
    ~MListOfIDs() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

