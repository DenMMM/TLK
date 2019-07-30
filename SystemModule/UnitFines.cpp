//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFines.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MFine &MFine::operator=(MFine &Fine_)
{
    Time=Fine_.Time;
    Comment=Fine_.Comment;
    return *this;
}
//---------------------------------------------------------------------------
MFine::MFine()
{
    Time=0;
    Comment="";
}
//---------------------------------------------------------------------------
MFine::~MFine()
{
//
}
//---------------------------------------------------------------------------
MFines &MFines::operator=(MFines &Fines_)
{
    Clear();
    for ( MFine *Fine, *Fine_=(MFine*)Fines_.FirstItem;
        Fine_; Fine_=(MFine*)Fine_->NextItem )
    {
        Fine=(MFine*)Add();
        *Fine=*Fine_;
    }
    return *this;
}
//---------------------------------------------------------------------------

