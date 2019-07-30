//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitState.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MState::MState()
{
    Number=0;
    State=mcsFree;
    StartWorkTime=0.; SizeWorkTime=0;
    StartFineTime=0.; SizeFineTime=0;
    StopTimerTime=0.;
    GamesPages=mgpNone;
    StateVer=-1;
    CS_Main=CS_Games=NULL;
    NotPingTime=0;
    NeedUpdate=false;
}
//---------------------------------------------------------------------------
MState::~MState()
{
    DelCriticalSection();
}
//---------------------------------------------------------------------------
bool MState::ControlWorkTime()
{
    double SystemTime=(double)(Date()+Time());
    if ( (!(State&mcsWork))||(State&(mcsPause|mcsAuto))||
        (SystemTime<(StartWorkTime-5/(24.*60*60)+SizeWorkTime/(24.*60))) ) return false;
    StartWorkTime=0.; SizeWorkTime=0;
    StartFineTime=0.; SizeFineTime=0;
    State=mcsFree|(State&mcsAuto);
    GamesPages=mgpNone;
    return true;
}
//---------------------------------------------------------------------------
bool MState::ControlFineTime()
{
    double SystemTime=(double)(Date()+Time());
    if ( (!(State&mcsFine))||
        (SystemTime<(StartFineTime+SizeFineTime/(24.*60))) ) return false;
    StartFineTime=0.; SizeFineTime=0;
    State&=~mcsFine;
    return true;
}
//---------------------------------------------------------------------------
bool MState::ControlPingTime()
{
    if ( (++NotPingTime)<MaxPingTime ) return false;
    NotPingTime=0;
    if ( !(State&mcsWork) ) return false;
    State|=mcsLock; StateVer=-1;
    return true;
}
//---------------------------------------------------------------------------
void MState::InitCriticalSection()
{
    if ( CS_Main==NULL ) CS_Main=new CRITICAL_SECTION;
    ::InitializeCriticalSection(CS_Main);
    if ( CS_Games==NULL ) CS_Games=new CRITICAL_SECTION;
    ::InitializeCriticalSection(CS_Games);
}
//---------------------------------------------------------------------------
void MState::DelCriticalSection()
{
    if ( CS_Main!=NULL )
    {
        ::DeleteCriticalSection(CS_Main);
        delete CS_Main; CS_Main=NULL;
    }
    if ( CS_Games!=NULL )
    {
        ::DeleteCriticalSection(CS_Games);
        delete CS_Games; CS_Games=NULL;
    }
}
//---------------------------------------------------------------------------

