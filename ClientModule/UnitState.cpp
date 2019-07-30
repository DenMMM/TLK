//---------------------------------------------------------------------------
#include <mem.h>
#pragma hdrstop

#include "UnitState.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MState::MState()
{
    constructor();
    OptKey=NULL;
    OptPath=OptValue=PrgFile=NULL;
    OptCode=PrgCode=0;
    AutoLockTime=0;
    ::InitializeCriticalSection(&CS_Main);
    SystemTime=LastSyncTime=::GetTickCount();
    Number=0;
    State=mcsFree;
    StartWorkTime=0;
    SizeWorkTime=0;
    StartFineTime=0;
    SizeFineTime=0;
    StopTimerTime=0;
    Programs=0;
    Commands=0;
    Changes=mdcNumber|mdcState;
}
//---------------------------------------------------------------------------
MState::~MState()
{
    ::DeleteCriticalSection(&CS_Main);
    delete[] OptPath;
    delete[] OptValue;
    delete[] PrgFile;
    destructor();
}
//---------------------------------------------------------------------------
unsigned MState::GetDataSize()
{
    return
        sizeof(Number)+
        sizeof(State)+
        sizeof(StartWorkTime)+
        sizeof(SizeWorkTime)+
        sizeof(StartFineTime)+
        sizeof(SizeFineTime)+
        sizeof(StopTimerTime)+
        sizeof(Programs);
}
//---------------------------------------------------------------------------
char *MState::SetData(char *Data_)
{
    Lock();
    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,State);
    Data_=MemSet(Data_,StartWorkTime);
    Data_=MemSet(Data_,SizeWorkTime);
    Data_=MemSet(Data_,StartFineTime);
    Data_=MemSet(Data_,SizeFineTime);
    Data_=MemSet(Data_,StopTimerTime);
    Data_=MemSet(Data_,Programs);
    UnLock();
    return Data_;
}
//---------------------------------------------------------------------------
char *MState::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&Number,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&State,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&StartWorkTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&SizeWorkTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&StartFineTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&SizeFineTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&StopTimerTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Programs,Limit_))==NULL ) goto error;
    Changes=mdcNumber|mdcState;
    return Data_;
error:
    return NULL;
}
//---------------------------------------------------------------------------
bool MState::ControlWorkTime()
{
    if ( (!(State&mcsWork))||(State&(mcsPause|mcsOpen))||
        (SystemTime<(StartWorkTime+SizeWorkTime*60*10000000i64)) ) return false;
    State=mcsFree|(State&mcsOpen);
    StartWorkTime=0; SizeWorkTime=0;
    StartFineTime=0; SizeFineTime=0;
    StopTimerTime=0;
    Programs=0;
    Changes|=mdcState|mdcWorkTime|mdcFineTime|mdcPrograms;
    return true;
}
//---------------------------------------------------------------------------
bool MState::ControlFineTime()
{
    if ( (!(State&mcsFine))||
        (SystemTime<(StartFineTime+SizeFineTime*60*10000000i64)) ) return false;
    StartFineTime=0; SizeFineTime=0;
    State&=~mcsFine;
    Changes|=mdcState|mdcFineTime|mdcPrograms;
    return true;
}
//---------------------------------------------------------------------------
bool MState::ControlSyncTime()
{
    if ( (AutoLockTime==0)||(::GetTickCount()<
        (LastSyncTime+AutoLockTime*60*1000)) ) return false;
    // Проверяем запущен ли компьютер и не прикрыт ли он уже
    if ( (State&(mcsWork|mcsLock))!=mcsWork ) return false;
    // Помечаем, что компьютер прикрыт
    State|=mcsLock;
/*
    // Проверяем запущен ли компьютер и не приостановлен ли он уже
    if ( (State&(mcsWork|mcsPause))!=mcsWork ) return false;
    if ( !(State&mcsOpen) ) StopTimerTime=SystemTime;
    // Помечаем, что компьютер приостановлен
    State|=mcsPause;
*/
    Changes|=mdcState|mdcPrograms;
    return true;
}
//---------------------------------------------------------------------------
void MState::StateInfo(MStateInfo *Info_)
{
    memset(Info_,0,sizeof(MStateInfo));

    Lock();
    //
    Info_->Number=Number;
    Info_->State=State;
    if ( State&mcsWork )
    {
        Info_->WorkTime=SizeWorkTime;
        Info_->ToEndWork=SizeWorkTime-((State&(mcsPause|mcsOpen)?StopTimerTime:SystemTime)-
            StartWorkTime)/(60*10000000i64);
        Info_->EndWorkTime=ExtractHoursMin(StartWorkTime+SizeWorkTime*60*10000000i64+
            (State&(mcsPause|mcsOpen)?SystemTime-StopTimerTime:0));
    }
    if ( State&mcsFine )
    {
        Info_->FineTime=SizeFineTime;
        Info_->ToEndFine=SizeFineTime-(SystemTime-StartFineTime)/(60*10000000i64);
        Info_->EndFineTime=ExtractHoursMin(StartFineTime+SizeFineTime*60*10000000i64);
    }
    Info_->Programs=Programs;
    Info_->Commands=Commands;
    Info_->Changes=Changes;
    //
    Commands=Changes=0;
    UnLock();
}
//---------------------------------------------------------------------------
bool MState::GetOptions(MClOptions *Options_)
{
    bool result;
    Lock();
    result=Options_->QueryFrom(OptKey,OptPath,OptValue,OptCode);
    if ( result ) AutoLockTime=Options_->AutoLockTime;
    UnLock();
    return result;
}
//---------------------------------------------------------------------------
bool MState::GetGames(MGames *Games_)
{
    bool result;
    Lock(); result=Games_->LoadFrom(PrgFile,PrgCode); UnLock();
    return result;
}
//---------------------------------------------------------------------------
bool MState::Timer(__int64 SystemTime_)
{
    bool result;
    Lock();
    SystemTime=SystemTime_;
    result=ControlWorkTime()||ControlFineTime();
    result=ControlSyncTime()||result;
    UnLock();
    return result;
}
//---------------------------------------------------------------------------
void MState::CmdReboot()
{
    Lock();
    Commands|=mccReboot; Changes|=mdcCommands;
    UnLock();
}
//---------------------------------------------------------------------------
bool MState::NewSyncData(MSyncData *Data_)
{
    bool NeedSave=false;
    __int64 CurrentTime;

    // Проверяем отклонение системного времени и при необходимости корректируем его
    GetLocalTimeInt64(&CurrentTime);
    CurrentTime=(CurrentTime-=Data_->SystemTime)<0?-CurrentTime:CurrentTime;
    if ( CurrentTime>=MAX_TimeShift*10000000i64 ) SetLocalTimeInt64(&Data_->SystemTime);

    Lock();
    // Заносим новые данные о режиме работы и помечаем события для оболочки
    if ( Data_->Number!=Number ) { Number=Data_->Number; Changes|=mdcNumber; NeedSave=true; }
    if ( Data_->State!=State ) { State=Data_->State; Changes|=mdcState; NeedSave=true; }
    if ( Data_->StartWorkTime!=StartWorkTime ) { StartWorkTime=Data_->StartWorkTime; Changes|=mdcWorkTime; NeedSave=true; }
    if ( Data_->SizeWorkTime!=SizeWorkTime ) { SizeWorkTime=Data_->SizeWorkTime; Changes|=mdcWorkTime; NeedSave=true; }
    if ( Data_->StartFineTime!=StartFineTime ) { StartFineTime=Data_->StartFineTime; Changes|=mdcFineTime; NeedSave=true; }
    if ( Data_->SizeFineTime!=SizeFineTime ) { SizeFineTime=Data_->SizeFineTime; Changes|=mdcFineTime; NeedSave=true; }
    if ( Data_->StopTimerTime!=StopTimerTime ) { StopTimerTime=Data_->StopTimerTime; NeedSave=true; }
    if ( Data_->Programs!=Programs ) { Programs=Data_->Programs; Changes|=mdcPrograms; NeedSave=true; }
    if ( Data_->Commands!=Commands ) { Commands=Data_->Commands; Changes|=mdcCommands; NeedSave=true; }
    //
    LastSyncTime=::GetTickCount();
    UnLock();

    return NeedSave;
}
//---------------------------------------------------------------------------
bool MState::NewGames(MGames *Games_)
{
    bool result;
    Lock();
    result=Games_->SaveTo(PrgFile,PrgCode);
    if ( result ) Changes|=mdcPrograms;
    UnLock();
    return result;
}
//---------------------------------------------------------------------------
bool MState::NewOptions(MClOptions *Options_)
{
    bool result;
    Lock();
    result=Options_->StoreTo(OptKey,OptPath,OptValue,OptCode);
    if ( result ) { AutoLockTime=Options_->AutoLockTime; Changes|=mdcOptions; }
    UnLock();
    return result;
}
//---------------------------------------------------------------------------
bool MState::SetDefault(HKEY RegKey_, char *RegPath_, char *RegValue_, unsigned RegCode_,
    HKEY OptKey_, char *OptPath_, char *OptValue_, unsigned OptCode_,
    char *PrgFile_, unsigned PrgCode_)
{
    delete[] OptPath; delete[] OptValue; delete[] PrgFile;
    OptPath=OptValue=PrgFile=NULL;
    if ( !(MSLList::SetDefaultKey(RegKey_,RegPath_,RegValue_,RegCode_)&&
        ((OptPath=new char[strlen(OptPath_)+1])!=NULL)&&
        ((OptValue=new char[strlen(OptValue_)+1])!=NULL)&&
        ((PrgFile=new char[strlen(PrgFile_)+1])!=NULL)) ) return false;
    OptKey=OptKey_;
    strcpy(OptPath,OptPath_); strcpy(OptValue,OptValue_); OptCode=OptCode_;
    strcpy(PrgFile,PrgFile_); PrgCode=PrgCode_;
    return true;
}
//---------------------------------------------------------------------------

