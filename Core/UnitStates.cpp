//---------------------------------------------------------------------------
#include <mem.h>
#include <stdexcept.h>
#pragma hdrstop

#include "UnitStates.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MState::MState()
{
    SystemTime=0;
    Number=0;
    State=0;
    TariffID=0;
    StartWorkTime=0;
    SizeWorkTime=0;
    StartFineTime=0;
    SizeFineTime=0;
    StopTimerTime=0;
    Programs=0;
    Commands=0;
    CmdsToReset=0;
    NetState=0;
    Changes=0;
}
//---------------------------------------------------------------------------
MState::~MState()
{
}
//---------------------------------------------------------------------------
unsigned MState::GetDataSize() const
{
    return
        sizeof(Number)+
        sizeof(State)+
        sizeof(TariffID)+
        sizeof(StartWorkTime)+
        sizeof(SizeWorkTime)+
        sizeof(StartFineTime)+
        sizeof(SizeFineTime)+
        sizeof(StopTimerTime)+
        sizeof(Programs)+
        sizeof(NetState);
}
//---------------------------------------------------------------------------
char *MState::SetData(char *Data_) const
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,State);
    Data_=MemSet(Data_,TariffID);
    Data_=MemSet(Data_,StartWorkTime);
    Data_=MemSet(Data_,SizeWorkTime);
    Data_=MemSet(Data_,StartFineTime);
    Data_=MemSet(Data_,SizeFineTime);
    Data_=MemSet(Data_,StopTimerTime);
    Data_=MemSet(Data_,Programs);
    Data_=MemSet(Data_,NetState&mnsSyncNeed);

    return Data_;
}
//---------------------------------------------------------------------------
const char *MState::GetData(const char *Data_, const char *Limit_)
{
    if ( (Data_=MemGet(Data_,&Number,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&State,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&TariffID,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&StartWorkTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&SizeWorkTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&StartFineTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&SizeFineTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&StopTimerTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Programs,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&NetState,Limit_))==NULL ) goto error;
    if ( NetState&mnsSyncNeed ) NetState|=mnsSyncData;
    Changes=mdcAll;
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
    TariffID=0;
    StartWorkTime=0; SizeWorkTime=0;
    StartFineTime=0; SizeFineTime=0;
    StopTimerTime=0;
    Programs=0;
    Changes|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MState::ControlFineTime()
{
    if ( (!(State&mcsFine))||
        (SystemTime<(StartFineTime+SizeFineTime*60*10000000i64)) ) return false;
    StartFineTime=0; SizeFineTime=0;
    State&=~mcsFine;
    Changes|=mdcState|mdcFineTime;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdRun(MTariff *Tariff_, MRunTime *Time_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( !((State==mcsFree)||(State&mcsWork)) ) return false;
    if ( Check_ ) return true;
    //
    if ( State==mcsFree )
    {
        // ������ ���������� � ������
        State=mcsWork;
        TariffID=Tariff_->gItemID();
        StartWorkTime=SystemTime;
        SizeWorkTime=Time_->WorkTime;
        Programs=Tariff_->Programs;
        Changes|=mdcState|mdcTariff|mdcWorkTime;
        // ���� ��������� ����� ������� ����� �������������, �� ������ ������ �������
        if ( Tariff_->Reboot ) { Commands|=mccReboot; Changes|=mdcCommands; }
    } else
    {
        // ���������� ������� ����������� ����������
        SizeWorkTime+=Time_->WorkTime;
        if ( SizeWorkTime>(24*60) ) SizeWorkTime=24*60;
        Changes|=mdcWorkTime;
    }
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdFine(short FineSize_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( !(State&mcsWork) ) return false;
    if ( Check_ ) return true;
    //
    if ( FineSize_>0 )
    {
        // ��������� ������ � ���������
        if ( State&mcsFine )
        {
            // ����������� ����� ��� ������������ ������
            SizeFineTime+=FineSize_;
            Changes|=mdcFineTime;
        } else
        {
            // ������ ����� �����
            State|=mcsFine;
            StartFineTime=SystemTime;
            SizeFineTime=FineSize_;
            Changes|=mdcState|mdcFineTime;
        }
        // ��������� �� ����������� �� ����� ������ ���������� ����� ������
        if ( ((SizeWorkTime-SizeFineTime)*(60*10000000i64)+
            StartWorkTime-StartFineTime+
            SystemTime-(State&(mcsPause|mcsOpen)?StopTimerTime:SystemTime))<0 ) goto full;
    } else if ( FineSize_<0 )
    {
        // ���������� ������� ������ �� ������ ��� ��������
        SizeWorkTime+=FineSize_;
        Changes|=mdcWorkTime;
        if ( SizeWorkTime<0 )
        {
full:
            // ������ ������ ������� ������
            State=mcsFree|(State&mcsOpen);
            TariffID=0;
            StartWorkTime=0; SizeWorkTime=0;
            StartFineTime=0; SizeFineTime=0;
            StopTimerTime=0;
            Programs=0;
            Changes|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime;
            // ������������� ��������� ��� ��������
///            if ( !(State&mcsOpen) ) { Commands|=mccReboot; Changes|=mdcCommands; }
        }
    } else
    {
        // ��������������

    }
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdExchange(MState *State_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::DualLock lckObj(CS_Main,State_->CS_Main);

    if ( !((State&mcsWork)&&(State_->State==mcsFree)) ) return false;
    if ( Check_ ) return true;
    // ������ ����� ������ ������
    State_->State=State&(mcsWork|mcsFine|mcsLock|mcsPause);
    State_->TariffID=TariffID;
    State_->StartWorkTime=StartWorkTime;
    State_->SizeWorkTime=SizeWorkTime;
    State_->StartFineTime=StartFineTime;
    State_->SizeFineTime=SizeFineTime;
    State_->StopTimerTime=StopTimerTime;
    State_->Programs=Programs;
    if ( (State&(mcsPause|mcsOpen))==mcsOpen )
    {
        State_->StartWorkTime+=SystemTime-StopTimerTime;
        State_->StopTimerTime=0;
    }
    State_->Changes|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime;
    // �������� ������ ��� �������� �� ����
    State_->NetState|=mnsSyncNeed|mnsSyncData; State_->Changes|=mdcNetState;
    // ������ ����� ������ ������
    State=mcsFree|(State&mcsOpen);
    TariffID=0;
    StartWorkTime=0; SizeWorkTime=0;
    StartFineTime=0; SizeFineTime=0;
    StopTimerTime=0;
    Programs=0;
    Changes|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    // ������������� ��������� ��� ��������
///    if ( !(State&mcsOpen) ) { Commands|=mccReboot; Changes|=mdcCommands; }

    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdLock(bool Apply_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Apply_ )
    {
        // ��������� ������� �� ��������� � �� ������� �� �� ���
        if ( (State&(mcsWork|mcsLock))!=mcsWork ) return false;
        if ( Check_ ) return true; 
        // ��������, ��� ��������� �������
        State|=mcsLock;
    } else
    {
        // ��������� ������� �� ���������
        if ( !(State&mcsLock) ) return false;
        if ( Check_ ) return true;
        // ������� �������, ��� ��������� �������
        State&=~mcsLock;
    }
    Changes|=mdcState;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdPause(bool Apply_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Apply_ )
    {
        // ��������� ������� �� ��������� � �� ������������� �� �� ���
        if ( (State&(mcsWork|mcsPause))!=mcsWork ) return false;
        if ( Check_ ) return true;
        if ( !(State&mcsOpen) ) StopTimerTime=SystemTime;
        // ��������, ��� ��������� �������������
        State|=mcsPause;
    } else
    {
        // ��������� ������������� �� ���������
        if ( !(State&mcsPause) ) return false;
        if ( Check_ ) return true;
        // ���� ��������� �� ������ ��� ������������, ����������� ����� ��� ������
        if ( !(State&mcsOpen) )
        {
            StartWorkTime+=SystemTime-StopTimerTime;
///            StartFineTime+=SystemTime-StopTimerTime;  // ������������ ������
            StopTimerTime=0;
        }
        // ������� �������, ��� ��������� �������������
        State&=~mcsPause;
    }
    Changes|=mdcState;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdOpen(bool Apply_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Apply_ )
    {
        // ��������� �������� ��� ������� ��������� � �� ������ �� �� ���
        if ( (!(State&(mcsFree|mcsWork)))||(State&mcsOpen) ) return false;
        if ( Check_ ) return true;
        // ���� ��������� ������� � �� �������������, ��������� ����� ��� ��������
        if ( (State&(mcsWork|mcsPause))==mcsWork ) StopTimerTime=SystemTime;
        // ��������, ��� ��������� ������
        State|=mcsOpen;
    } else
    {
        // ��������� ������ �� ���������
        if ( !(State&mcsOpen) ) return false;
        if ( Check_ ) return true;
        // ���� ��������� ������� � �� �������������, ����������� ����� ��� ������
        if ( (State&(mcsWork|mcsPause))==mcsWork )
        {
            StartWorkTime+=SystemTime-StopTimerTime;
///            StartFineTime+=SystemTime-StopTimerTime; // ������������ ������
            StopTimerTime=0;
        }
        // ������� �������, ��� ��������� ������
        State&=~mcsOpen;
    }
    Changes|=mdcState;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdPowerOn(bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Check_ ) return true;
    Commands|=mccPowerOn; Changes|=mdcCommands;

    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdReboot(bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Check_ ) return true;
    Commands|=mccReboot; Changes|=mdcCommands;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;

    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdShutdown(bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Check_ ) return true;
    Commands|=mccShutdown; Changes|=mdcCommands;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;

    return true;
}
//---------------------------------------------------------------------------
void MState::RunParam(MRunTime *RunTime_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    RunTime_->Number=Number;
    if ( State==mcsFree )
    {
        RunTime_->TariffID=0;
        RunTime_->StartTime=SystemTime;
        RunTime_->MaxTime=24*60;
///    } else if ( (State&mcsWork)&&(!(State&(mcsPause|mcsOpen))) )
    } else if ( State&mcsWork )
    {
        RunTime_->TariffID=TariffID;
///        RunTime_->StartTime=StartWorkTime+SizeWorkTime*60*10000000i64;
        RunTime_->StartTime=StartWorkTime+SizeWorkTime*60*10000000i64+
            (State&(mcsPause|mcsOpen)?SystemTime-StopTimerTime:0);
        RunTime_->MaxTime=24*60-SizeWorkTime;
    } else
    {
        RunTime_->TariffID=0;
        RunTime_->StartTime=0;
        RunTime_->MaxTime=0;
    }
}
//---------------------------------------------------------------------------
void MState::StateInfo(MStateInfo *Info_)
{
    memset(Info_,0,sizeof(MStateInfo));

    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);
    //
    Info_->Number=Number;
    Info_->State=State;
    if ( State&mcsWork )
    {
        Info_->TariffID=TariffID;
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
    Info_->Commands=Commands;
    Info_->NetState=NetState&(mnsPresent|mnsSyncNeed);
    Info_->Changes=Changes;
    //
    Changes=0;
}
//---------------------------------------------------------------------------
bool MState::Timer(__int64 SystemTime_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    SystemTime=SystemTime_;
    return ControlWorkTime()||ControlFineTime();
}
//---------------------------------------------------------------------------
void MState::Associate(int Number_)
{
    Number=Number_;
    State=mcsFree;
    TariffID=0;
    StartWorkTime=0;
    SizeWorkTime=0;
    StartFineTime=0;
    SizeFineTime=0;
    StopTimerTime=0;
    Programs=0;
    Commands=0;
    CmdsToReset=0;
    NetState=mnsSyncNeed|mnsSyncData;
    Changes=mdcAll;
}
//---------------------------------------------------------------------------
bool MState::NetBegin()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    // ��������� �� ���������� �� ������ � ������� ���������
    if ( NetState&mnsLock ) return false;
    // ��������� ������ � ������� ���������
    NetState|=mnsLock;
    // ���������� ���� ������������� ���������� ��������� �� ����
    NetState&=~mnsNeedSave;
    // ������ ���������� ������ ������ ��� ��������������������� ���������
///    if ( NetState&mnsSyncNeed ) NetState|=mnsSyncData;
    return true;
}
//---------------------------------------------------------------------------
bool MState::NetEnd()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    CmdsToReset&=~(mccReboot|mccShutdown);
    // ������� ���������� ������� � ������� ���������
    NetState&=~mnsLock;
    return NetState&mnsNeedSave;
}
//---------------------------------------------------------------------------
bool MState::NetSyncNewData()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    return NetState&mnsSyncData;
}
//---------------------------------------------------------------------------
void MState::NetSyncData(MSyncData *Data_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    // ���������� ���� ������� ����� ������
    NetState&=~mnsSyncData;
    // ������� ����� ������ � ������ ������
    Data_->SystemTime=SystemTime;
    Data_->Number=Number;
    Data_->State=State;
    Data_->StartWorkTime=StartWorkTime;
    Data_->SizeWorkTime=SizeWorkTime;
    Data_->StartFineTime=StartFineTime;
    Data_->SizeFineTime=SizeFineTime;
    Data_->StopTimerTime=StopTimerTime;
    Data_->Programs=Programs;
    Data_->Commands=Commands;
    CmdsToReset|=Data_->Commands;
}
//---------------------------------------------------------------------------
void MState::NetSyncExecuted(bool Executed_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Executed_ )
    {
        // ���� �� ����� �������� ��������� ����������� ������ ����� �� ���������,
        // ���������� ���� ������������� �������������
        if ( (NetState&(mnsSyncNeed|mnsSyncData))==mnsSyncNeed )
        {
            NetState&=~mnsSyncNeed;
            NetState|=mnsNeedSave;
        }
        // ��������, ��� ��������� �������� �� ����
        NetState|=mnsPresent;
        // ���������� �������, ������������ ��� ���������� ��������
        Commands&=~CmdsToReset;
        CmdsToReset=0;
        // �������� ��������� ��� ��������
        Changes|=mdcCommands;
    } else
    {
        // ���������� ���� ����������� ���������� �� ����
        NetState&=~mnsPresent;
    }
    // �������� ��������� ��� ��������
    Changes|=mdcNetState;
}
//---------------------------------------------------------------------------
bool MState::NetPwrOnNeed()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    return Commands&mccPowerOn;
}
//---------------------------------------------------------------------------
void MState::NetPwrOnExecuted()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    // ���������� ������� ��� ����������, �.�. ��� ���������
    Commands&=~mccPowerOn;
    // �������� ��������� ��� ��������
    Changes|=mdcCommands;
}
//---------------------------------------------------------------------------
bool MStates::Save()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_File);

    // ��������� � ���� � ���������� ����������� � ��� �����������
    return MSLList::Save(true,true);
}
//---------------------------------------------------------------------------
MState *MStates::Search(int Number_)
{
    MState *State=(MState*)gFirst();
    while(State)
    {
        if ( State->Associated()==Number_ ) break;
        State=(MState*)State->gNext();
    }
    return State;
}
//---------------------------------------------------------------------------
bool MStates::Update(MComputers *Computers_)
{
    bool result=false;
    MComputer *Computer;
    MState *State, *NextState;

    // ������� ���������, ��������������� � ���������������
    // ��� ��������������� ������������
    State=(MState*)gFirst();
    while(State)
    {
        NextState=(MState*)State->gNext();
        Computer=Computers_->Search(State->Associated());
        if ( (Computer==NULL)||(Computer->NotUsed) )
        {
            result=true;
            Del(State);
        }
        State=NextState;
    }
    // ��������� ��������� ��� ����� �����������
    for ( Computer=(MComputer*)Computers_->gFirst();
        Computer; Computer=(MComputer*)Computer->gNext() )
    {
        if ( Computer->NotUsed||Search(Computer->Number) ) continue;
        result=true;
        State=(MState*)Add();
        State->Associate(Computer->Number);
    }
    // ������� ������ ������, ��������������� � ����� � ��� �� �����������
    State=(MState*)gFirst();
    while(State)
    {
        NextState=(MState*)State->gNext();
        if ( Search(State->Associated())!=State )
        {
            result=true;
            Del(State);
        }
        State=NextState;
    }

    return result;
}
//---------------------------------------------------------------------------
bool MStates::Timer(__int64 SystemTime_)
{
    bool result=false;
    //
    for ( MState *State=(MState*)gFirst(); State;
        State=(MState*)State->gNext() ) result|=State->Timer(SystemTime_);
    //
    return result;
}
//---------------------------------------------------------------------------
MStateCl::MStateCl()
{
    OptKey=NULL;
//    OptCode=PrgCode=0;
    AutoLockTime=0;
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
MStateCl::~MStateCl()
{
//
}
//---------------------------------------------------------------------------
unsigned MStateCl::GetDataSize() const
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
char *MStateCl::SetData(char *Data_) const 
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,State);
    Data_=MemSet(Data_,StartWorkTime);
    Data_=MemSet(Data_,SizeWorkTime);
    Data_=MemSet(Data_,StartFineTime);
    Data_=MemSet(Data_,SizeFineTime);
    Data_=MemSet(Data_,StopTimerTime);
    Data_=MemSet(Data_,Programs);

    return Data_;
}
//---------------------------------------------------------------------------
const char *MStateCl::GetData(const char *Data_, const char *Limit_)
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
bool MStateCl::ControlWorkTime()
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
bool MStateCl::ControlFineTime()
{
    if ( (!(State&mcsFine))||
        (SystemTime<(StartFineTime+SizeFineTime*60*10000000i64)) ) return false;
    StartFineTime=0; SizeFineTime=0;
    State&=~mcsFine;
    Changes|=mdcState|mdcFineTime|mdcPrograms;
    return true;
}
//---------------------------------------------------------------------------
bool MStateCl::ControlSyncTime()
{
    if ( (AutoLockTime==0)||(::GetTickCount()<
        (LastSyncTime+AutoLockTime*60*1000)) ) return false;
    // ��������� ������� �� ��������� � �� ������� �� �� ���
    if ( (State&(mcsWork|mcsLock))!=mcsWork ) return false;
    // ��������, ��� ��������� �������
    State|=mcsLock;
/*
    // ��������� ������� �� ��������� � �� ������������� �� �� ���
    if ( (State&(mcsWork|mcsPause))!=mcsWork ) return false;
    if ( !(State&mcsOpen) ) StopTimerTime=SystemTime;
    // ��������, ��� ��������� �������������
    State|=mcsPause;
*/
    Changes|=mdcState|mdcPrograms;
    return true;
}
//---------------------------------------------------------------------------
void MStateCl::StateInfo(MStateInfo *Info_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    memset(Info_,0,sizeof(MStateInfo));
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
}
//---------------------------------------------------------------------------
bool MStateCl::GetOptions(MClOptions *Options_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);
    bool result;

    result=Options_->QueryFrom(OptKey,OptPath,OptValue,DefaultCode);
    if ( result ) AutoLockTime=Options_->AutoLockTime;

    return result;
}
//---------------------------------------------------------------------------
bool MStateCl::GetGames(MGames *Games_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    return Games_->LoadFrom(PrgFile,DefaultCode);
}
//---------------------------------------------------------------------------
bool MStateCl::Timer(__int64 SystemTime_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);
    bool result;

    SystemTime=SystemTime_;
    result=ControlWorkTime()||ControlFineTime();
    result=ControlSyncTime()||result;

    return result;
}
//---------------------------------------------------------------------------
void MStateCl::CmdReboot()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    Commands|=mccReboot; Changes|=mdcCommands;
}
//---------------------------------------------------------------------------
void MStateCl::CmdShutdown()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    Commands|=mccShutdown; Changes|=mdcCommands;
}
//---------------------------------------------------------------------------
bool MStateCl::NewSyncData(MSyncData *Data_)
{
    bool NeedSave=false;
    __int64 CurrentTime;

    // ��������� ���������� ���������� ������� � ��� ������������� ������������ ���
    GetLocalTimeInt64(&CurrentTime);
    CurrentTime=(CurrentTime-=Data_->SystemTime)<0?-CurrentTime:CurrentTime;
    if ( CurrentTime>=MAX_TimeShift*10000000i64 ) SetLocalTimeInt64(&Data_->SystemTime);

    CS_Main.Enter();
    // ������� ����� ������ � ������ ������ � �������� ������� ��� ��������
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
    CS_Main.Leafe();

    return NeedSave;
}
//---------------------------------------------------------------------------
bool MStateCl::NewGames(MGames *Games_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);
    bool result=false;

    try { result=Games_->SaveTo(PrgFile,DefaultCode); }
    catch (std::bad_alloc &e) {}
    if ( result ) Changes|=mdcPrograms;

    return result;
}
//---------------------------------------------------------------------------
bool MStateCl::NewOptions(MClOptions *Options_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);
    bool result=false;

    try { result=Options_->StoreTo(OptKey,OptPath,OptValue,DefaultCode); }
    catch (std::bad_alloc &e) {}
    if ( result ) { AutoLockTime=Options_->AutoLockTime; Changes|=mdcOptions; }

    return result;
}
//---------------------------------------------------------------------------
void MStateCl::SetDefault(
        HKEY RegKey_,
        const std::string &RegPath_,
        const std::string &RegValue_,
        HKEY OptKey_,
        const std::string &OptPath_,
        const std::string &OptValue_,
        const std::string &PrgFile_,
        unsigned RegCode_)
{
    MSLList::SetDefaultKey(RegKey_,RegPath_,RegValue_,RegCode_);

    OptKey=OptKey_;
    OptPath=OptPath_;
    OptValue=OptValue_;
    PrgFile=PrgFile_;
}
//---------------------------------------------------------------------------

