//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitUsersUpTime.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

MUserUpTime::MUserUpTime()
{
    User=0;
    BeginTime=0;
    EndTime=0;
    Gains=0.;
}

MUserUpTime::~MUserUpTime()
{
//
}

bool MUserUpTime::Copy(MListItem *SrcItem_)
{
    return false;
}

//---------------------------------------------------------------------------
