//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFineComputers.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MFineComputer &MFineComputer::operator=(MFineComputer &FineComputer_)
{
    Computer=FineComputer_.Computer;
    FineTime=FineComputer_.FineTime;
    EnableReturnMany=FineComputer_.EnableReturnMany;
    ReturnMany=FineComputer_.ReturnMany;
    return *this;
}
//---------------------------------------------------------------------------
MFineComputer::MFineComputer()
{
    Computer=NULL;
    FineTime=0;

    EnableReturnMany=false;
    ReturnMany=0.;
}
//---------------------------------------------------------------------------
MFineComputer::~MFineComputer()
{
//
}
//---------------------------------------------------------------------------

