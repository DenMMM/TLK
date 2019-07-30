//---------------------------------------------------------------------------
#include <mem.h>
#pragma hdrstop

#include "UnitStates.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MState::MState()
{
    ::InitializeCriticalSection(&CS_Main);
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
    ::DeleteCriticalSection(&CS_Main);
}
//---------------------------------------------------------------------------
bool MState::Copy(MListItem *SrcItem_)
{
    MState *State_=(MState*)SrcItem_;
    Lock(); State_->Lock();
    SystemTime=State_->SystemTime;
    Number=State_->Number;
    State=State_->State;
    TariffID=State_->TariffID;
    StartWorkTime=State_->StartWorkTime;
    SizeWorkTime=State_->SizeWorkTime;
    StartFineTime=State_->StartFineTime;
    SizeFineTime=State_->SizeFineTime;
    StopTimerTime=State_->StopTimerTime;
    Programs=State_->Programs;
    Commands=State_->Commands;
    CmdsToReset=State_->CmdsToReset;
    NetState=State_->NetState;
    Changes=State_->Changes;
    State_->UnLock(); UnLock();
    return true;
}
//---------------------------------------------------------------------------
unsigned MState::GetDataSize()
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
char *MState::SetData(char *Data_)
{
    Lock();
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
    UnLock();
    return Data_;
}
//---------------------------------------------------------------------------
char *MState::GetData(char *Data_, char *Limit_)
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
    Lock();
    if ( !((State==mcsFree)||(State&mcsWork)) ) { UnLock(); return false; }
    if ( Check_ ) { UnLock(); return true; }
    //
    if ( State==mcsFree )
    {
        // ������ ���������� � ������
        State=mcsWork;
        TariffID=Tariff_->ID;
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
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdFine(int FineSize_, bool Check_)
{
    Lock();
    if ( !(State&mcsWork) ) { UnLock(); return false; }
    if ( Check_ ) { UnLock(); return true; }
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
//            if ( !(State&mcsOpen) ) { Commands|=mccReboot; Changes|=mdcCommands; }
        }
    } else
    {
        // ��������������

    }
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdExchange(MState *State_, bool Check_)
{
    Lock(); State_->Lock();
    if ( !((State&mcsWork)&&(State_->State==mcsFree)) )
        { State_->UnLock(); UnLock(); return false; }
    if ( Check_ ) { State_->UnLock(); UnLock(); return true; }
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
//    if ( !(State&mcsOpen) ) { Commands|=mccReboot; Changes|=mdcCommands; }
    State_->UnLock(); UnLock();
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdLock(bool Apply_, bool Check_)
{
    Lock();
    if ( Apply_ )
    {
        // ��������� ������� �� ��������� � �� ������� �� �� ���
        if ( (State&(mcsWork|mcsLock))!=mcsWork ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        // ��������, ��� ��������� �������
        State|=mcsLock;
    } else
    {
        // ��������� ������� �� ���������
        if ( !(State&mcsLock) ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        // ������� �������, ��� ��������� �������
        State&=~mcsLock;
    }
    Changes|=mdcState;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdPause(bool Apply_, bool Check_)
{
    Lock();
    if ( Apply_ )
    {
        // ��������� ������� �� ��������� � �� ������������� �� �� ���
        if ( (State&(mcsWork|mcsPause))!=mcsWork ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        if ( !(State&mcsOpen) ) StopTimerTime=SystemTime;
        // ��������, ��� ��������� �������������
        State|=mcsPause;
    } else
    {
        // ��������� ������������� �� ���������
        if ( !(State&mcsPause) ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        // ���� ��������� �� ������ ��� ������������, ����������� ����� ��� ������
        if ( !(State&mcsOpen) )
        {
            StartWorkTime+=SystemTime-StopTimerTime;
//            StartFineTime+=SystemTime-StopTimerTime;
            StopTimerTime=0;
        }
        // ������� �������, ��� ��������� �������������
        State&=~mcsPause;
    }
    Changes|=mdcState;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdOpen(bool Apply_, bool Check_)
{
    Lock();
    if ( Apply_ )
    {
        // ��������� �������� ��� ������� ��������� � �� ������ �� �� ���
        if ( (!(State&(mcsFree|mcsWork)))||(State&mcsOpen) ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        // ���� ��������� ������� � �� �������������, ��������� ����� ��� ��������
        if ( (State&(mcsWork|mcsPause))==mcsWork ) StopTimerTime=SystemTime;
        // ��������, ��� ��������� ������
        State|=mcsOpen;
    } else
    {
        // ��������� ������ �� ���������
        if ( !(State&mcsOpen) ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        // ���� ��������� ������� � �� �������������, ����������� ����� ��� ������
        if ( (State&(mcsWork|mcsPause))==mcsWork )
        {
            StartWorkTime+=SystemTime-StopTimerTime;
//            StartFineTime+=SystemTime-StopTimerTime;
            StopTimerTime=0;
        }
        // ������� �������, ��� ��������� ������
        State&=~mcsOpen;
    }
    Changes|=mdcState;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdWtLocker(bool Apply_, bool Check_)
{
    Lock();
    if ( Apply_ )
    {
        if ( !(State&mcsFree) ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        State=mcsWtLocker;
        Changes|=mdcState|mdcWorkTime|mdcFineTime;
    } else
    {
        if ( !(State&mcsWtLocker) ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        State=mcsFree;
        Changes|=mdcState;
    }
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdPowerOn(bool Check_)
{
    Lock();
    if ( Check_ ) { UnLock(); return true; }
    Commands|=mccPowerOn; Changes|=mdcCommands;
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdReboot(bool Check_)
{
    Lock();
    if ( Check_ ) { UnLock(); return true; }
    Commands|=mccReboot; Changes|=mdcCommands;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
bool MState::CmdShutdown(bool Check_)
{
    Lock();
    if ( Check_ ) { UnLock(); return true; }
    Commands|=mccShutdown; Changes|=mdcCommands;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
void MState::RunParam(MRunTime *RunTime_)
{
    Lock();
    RunTime_->Number=Number;
    if ( State==mcsFree )
    {
        RunTime_->TariffID=0;
        RunTime_->StartTime=SystemTime;
        RunTime_->MaxTime=24*60;
//    } else if ( (State&mcsWork)&&(!(State&(mcsPause|mcsOpen))) )
    } else if ( State&mcsWork )
    {
        RunTime_->TariffID=TariffID;
//        RunTime_->StartTime=StartWorkTime+SizeWorkTime*60*10000000i64;
        RunTime_->StartTime=StartWorkTime+SizeWorkTime*60*10000000i64+
            (State&(mcsPause|mcsOpen)?SystemTime-StopTimerTime:0);
        RunTime_->MaxTime=24*60-SizeWorkTime;
    } else
    {
        RunTime_->TariffID=0;
        RunTime_->StartTime=0;
        RunTime_->MaxTime=0;
    }
    UnLock();
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
    UnLock();
}
//---------------------------------------------------------------------------
bool MState::Timer(__int64 SystemTime_)
{
    bool result;
    Lock();
    SystemTime=SystemTime_;
    result=ControlWorkTime()||ControlFineTime();
    UnLock();
    return result;
}
//---------------------------------------------------------------------------
int MState::Associated()
{
    return Number;
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
    Lock();
    // ��������� �� ���������� �� ������ � ������� ���������
    if ( NetState&mnsLock ) { UnLock(); return false; }
    // ��������� ������ � ������� ���������
    NetState|=mnsLock;
    // ���������� ���� ������������� ���������� ��������� �� ����
    NetState&=~mnsNeedSave;
    // ������ ���������� ������ ������ ��� ��������������������� ���������
//    if ( NetState&mnsSyncNeed ) NetState|=mnsSyncData;
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
bool MState::NetEnd()
{
    bool NeedSave;
    Lock();
    //
    CmdsToReset&=~(mccReboot|mccShutdown);
    // ������� ���������� ������� � ������� ���������
    NetState&=~mnsLock;
    //
    NeedSave=NetState&mnsNeedSave;
    UnLock();
    return NeedSave;
}
//---------------------------------------------------------------------------
bool MState::NetSyncNewData()
{
    bool result;
    Lock(); result=NetState&mnsSyncData; UnLock();
    return result;
}
//---------------------------------------------------------------------------
void MState::NetSyncData(MSyncData *Data_)
{
    Lock();
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
    UnLock();
}
//---------------------------------------------------------------------------
void MState::NetSyncExecuted(bool Executed_)
{
    Lock();
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
    UnLock();
}
//---------------------------------------------------------------------------
bool MState::NetPwrOnNeed()
{
    bool result;
    Lock(); result=Commands&mccPowerOn; UnLock();
    return result;
}
//---------------------------------------------------------------------------
void MState::NetPwrOnExecuted()
{
    Lock();
    // ���������� ������� ��� ����������, �.�. ��� ���������
    Commands&=~mccPowerOn;
    // �������� ��������� ��� ��������
    Changes|=mdcCommands;
    UnLock();
}
//---------------------------------------------------------------------------
void MState::GetStateData(MStateData *Data_)
{
    Lock();
    Data_->Number=Number;
    Data_->State=State;
    Data_->TariffID=TariffID;
    Data_->StartWorkTime=StartWorkTime;
    Data_->SizeWorkTime=SizeWorkTime;
    Data_->StartFineTime=StartFineTime;
    Data_->SizeFineTime=SizeFineTime;
    Data_->StopTimerTime=StopTimerTime;
    UnLock();
}
//---------------------------------------------------------------------------
void MState::SetStateData(MStateData *Data_)
{
    Lock();
    Number=Data_->Number;
    State=Data_->State;
    TariffID=Data_->TariffID;
    StartWorkTime=Data_->StartWorkTime;
    SizeWorkTime=Data_->SizeWorkTime;
    StartFineTime=Data_->StartFineTime;
    SizeFineTime=Data_->SizeFineTime;
    StopTimerTime=Data_->StopTimerTime;
    Changes=mdcAll;
    UnLock();
}
//---------------------------------------------------------------------------
bool MStates::Save()
{
    bool result;
    Lock(); result=MSLList::Save(); UnLock();
    return result;
}
//---------------------------------------------------------------------------
MState *MStates::Search(int Number_)
{
    MState *State=(MState*)First;
    while(State)
    {
        if ( State->Associated()==Number_ ) break;
        State=(MState*)State->Next;
    }
    return State;
}
//---------------------------------------------------------------------------
bool MStates::Update(MComputers *Computers_)
{
    MComputer *Computer;
    MState *State, *NextState;

    // ������� ���������, ��������������� � ���������������
    // ��� ��������������� ������������
    State=(MState*)First;
    while(State)
    {
        NextState=(MState*)State->Next;
        Computer=Computers_->Search(State->Associated());
        if ( (Computer==NULL)||(Computer->NotUsed) ) Delete(State);
        State=NextState;
    }
    // ��������� ��������� ��� ����� �����������
    for ( Computer=(MComputer*)Computers_->First;
        Computer; Computer=(MComputer*)Computer->Next )
    {
        if ( Computer->NotUsed||Search(Computer->Number) ) continue;
        State=(MState*)Add();
        State->Associate(Computer->Number);
    }
    // ������� ������ ������, ��������������� � ����� � ��� �� �����������
    State=(MState*)First;
    while(State)
    {
        NextState=(MState*)State->Next;
        if ( Search(State->Associated())!=State ) Delete(State);
        State=NextState;
    }

    return true;
}
//---------------------------------------------------------------------------
bool MStates::Timer(__int64 SystemTime_)
{
    bool result=false;
    //
    for ( MState *State=(MState*)First; State;
        State=(MState*)State->Next ) result|=State->Timer(SystemTime_);
    //
    return result;
}
//---------------------------------------------------------------------------

