//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitOrderComputers.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MOrderComputer &MOrderComputer::operator=(MOrderComputer &OrderComputer_)
{
    Computer=OrderComputer_.Computer;
    Tariff=OrderComputer_.Tariff;
    TimeWorkSize=OrderComputer_.TimeWorkSize;
    Discount=OrderComputer_.Discount;
    Cost=OrderComputer_.Cost;
    return *this;
}
//---------------------------------------------------------------------------
MOrderComputer::MOrderComputer()
{
    Computer=NULL;
    Tariff=NULL;
    TimeWorkSize=0;
    Discount=0;
    Cost=0.;
}
//---------------------------------------------------------------------------
MOrderComputer::~MOrderComputer()
{
//
}
//---------------------------------------------------------------------------

