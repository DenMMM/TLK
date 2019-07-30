//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitListOfIDs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MListOfIDsID &MListOfIDsID::
    operator=(MListOfIDsID &ListOfIDsID_)
{
    ID=ListOfIDsID_.ID;
    return *this;
}
//---------------------------------------------------------------------------
MListOfIDsID::MListOfIDsID()
{
    ID=0;
}
//---------------------------------------------------------------------------
MListOfIDsID::~MListOfIDsID()
{
//
}
//---------------------------------------------------------------------------
MListOfIDs &MListOfIDs::operator=(MListOfIDs &ListOfIDs_)
{
    Clear();
    for ( MListOfIDsID *ListOfIDsID, *ListOfIDsID_=
        (MListOfIDsID*)(ListOfIDs_.FirstItem); ListOfIDsID_;
        ListOfIDsID_=(MListOfIDsID*)(ListOfIDsID_->NextItem) )
    {
        ListOfIDsID=(MListOfIDsID*)Add();
        *ListOfIDsID=*ListOfIDsID_;
    }
    return *this;
}
//---------------------------------------------------------------------------
MListOfIDsID *MListOfIDs::SearchID(unsigned int ID_)
{
    MListOfIDsID *ListOfIDsID=(MListOfIDsID*)FirstItem;
    while(ListOfIDsID)
    {
        if ( ListOfIDsID->ID==ID_ ) break;
        ListOfIDsID=(MListOfIDsID*)ListOfIDsID->NextItem;
    }
    return ListOfIDsID;
}
//---------------------------------------------------------------------------

