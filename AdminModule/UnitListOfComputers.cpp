//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitListOfComputers.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MListOfComputersComputer &MListOfComputersComputer::
    operator=(MListOfComputersComputer &ListOfComputersComputer_)
{
    Computer=ListOfComputersComputer_.Computer;
    return *this;
}
//---------------------------------------------------------------------------
MListOfComputersComputer::MListOfComputersComputer()
{
    Computer=NULL;
}
//---------------------------------------------------------------------------
MListOfComputersComputer::~MListOfComputersComputer()
{
//
}
//---------------------------------------------------------------------------
MListOfComputers &MListOfComputers::operator=(MListOfComputers &ListOfComputers_)
{
    Clear();
    for ( MListOfComputersComputer *ListOfComputersComputer, *ListOfComputersComputer_=
        (MListOfComputersComputer*)(ListOfComputers_.FirstItem); ListOfComputersComputer_;
        ListOfComputersComputer_=(MListOfComputersComputer*)(ListOfComputersComputer_->NextItem) )
    {
        ListOfComputersComputer=(MListOfComputersComputer*)Add();
        *ListOfComputersComputer=*ListOfComputersComputer_;
    }
    return *this;
}
//---------------------------------------------------------------------------
MListOfComputersComputer *MListOfComputers::SearchComputer(MComputer *Computer_)
{
    MListOfComputersComputer *ListOfComputersComputer=(MListOfComputersComputer*)FirstItem;

    while(ListOfComputersComputer)
    {
        if ( ListOfComputersComputer->Computer==Computer_ ) return ListOfComputersComputer;
        ListOfComputersComputer=(MListOfComputersComputer*)ListOfComputersComputer->NextItem;
    }

    return NULL;
}
//---------------------------------------------------------------------------

