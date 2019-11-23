//---------------------------------------------------------------------------
#include <mem.h>
#include <stdexcept>
#pragma hdrstop

#include "UnitStates.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
unsigned MStatesItem::GetDataSize() const
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
void* MStatesItem::SetData(void *Data_) const
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
const void* MStatesItem::GetData(const void *Data_, const void *Limit_)
{
	if ( !(
		(Data_=MemGet(Data_,&Number,Limit_)) &&
		(Data_=MemGet(Data_,&State,Limit_)) &&
		(Data_=MemGet(Data_,&TariffID,Limit_)) &&
		(Data_=MemGet(Data_,&StartWorkTime,Limit_)) &&
		(Data_=MemGet(Data_,&SizeWorkTime,Limit_)) &&
		(Data_=MemGet(Data_,&StartFineTime,Limit_)) &&
		(Data_=MemGet(Data_,&SizeFineTime,Limit_)) &&
		(Data_=MemGet(Data_,&StopTimerTime,Limit_)) &&
		(Data_=MemGet(Data_,&Programs,Limit_)) &&
		(Data_=MemGet(Data_,&NetState,Limit_))
		) ) return nullptr;

	if ( NetState&mnsSyncNeed ) NetState|=mnsSyncData;
	Changes=mdcAll;

	return Data_;
}
//---------------------------------------------------------------------------
bool MStatesItem::ControlWorkTime()
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
bool MStatesItem::ControlFineTime()
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
bool MStatesItem::CmdRun(
	const MTariffsItem &Tariff_,
	const MTariffRunTimesItem &Time_,
	bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( !((State==mcsFree)||(State&mcsWork)) ) return false;
    if ( Check_ ) return true;
	//
	if ( State==mcsFree )
	{
		// ������ ���������� � ������
		State=mcsWork;
		TariffID=Tariff_.gUUID();
		StartWorkTime=SystemTime;
		SizeWorkTime=Time_.WorkTime;
		Programs=Tariff_.Programs;
		Changes|=mdcState|mdcTariff|mdcWorkTime;
		// ���� ��������� ����� ������� ����� �������������, �� ������ ������ �������
		if ( Tariff_.Reboot ) { Commands|=mccReboot; Changes|=mdcCommands; }
	} else
	{
		// ���������� ������� ����������� ����������
		SizeWorkTime+=Time_.WorkTime;
		if ( SizeWorkTime>(24*60) ) SizeWorkTime=24*60;
		Changes|=mdcWorkTime;
	}
	// �������� ������ ��� �������� �� ����
	NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
	return true;
}
//---------------------------------------------------------------------------
bool MStatesItem::CmdFine(short FineSize_, bool Check_)
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
bool MStatesItem::CmdExchange(MStatesItem &State_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::DualLock lckObj(CS_Main,State_.CS_Main);

    if ( !((State&mcsWork)&&(State_.State==mcsFree)) ) return false;
    if ( Check_ ) return true;
    // ������ ����� ������ ������
    State_.State=State&(mcsWork|mcsFine|mcsLock|mcsPause);
    State_.TariffID=TariffID;
    State_.StartWorkTime=StartWorkTime;
    State_.SizeWorkTime=SizeWorkTime;
    State_.StartFineTime=StartFineTime;
    State_.SizeFineTime=SizeFineTime;
    State_.StopTimerTime=StopTimerTime;
    State_.Programs=Programs;
    if ( (State&(mcsPause|mcsOpen))==mcsOpen )
    {
        State_.StartWorkTime+=SystemTime-StopTimerTime;
        State_.StopTimerTime=0;
    }
    State_.Changes|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime;
    // �������� ������ ��� �������� �� ����
	State_.NetState|=mnsSyncNeed|mnsSyncData; State_.Changes|=mdcNetState;
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
bool MStatesItem::CmdLock(bool Apply_, bool Check_)
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
bool MStatesItem::CmdPause(bool Apply_, bool Check_)
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
bool MStatesItem::CmdOpen(bool Apply_, bool Check_)
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
bool MStatesItem::CmdPowerOn(bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Check_ ) return true;
    Commands|=mccPowerOn; Changes|=mdcCommands;

    return true;
}
//---------------------------------------------------------------------------
bool MStatesItem::CmdReboot(bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Check_ ) return true;
    Commands|=mccReboot; Changes|=mdcCommands;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;

    return true;
}
//---------------------------------------------------------------------------
bool MStatesItem::CmdShutdown(bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Check_ ) return true;
    Commands|=mccShutdown; Changes|=mdcCommands;
    // �������� ������ ��� �������� �� ����
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;

    return true;
}
//---------------------------------------------------------------------------
MTariffRunTimesItem MStatesItem::GetRunParam() const
{
	MTariffRunTimesItem RunTime;

	MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

	RunTime.Number=Number;
	if ( State==mcsFree )
	{
		RunTime.TariffID=0;
		RunTime.StartTime=SystemTime;
		RunTime.MaxTime=24*60;
///    } else if ( (State&mcsWork)&&(!(State&(mcsPause|mcsOpen))) )
	} else if ( State&mcsWork )
	{
		RunTime.TariffID=TariffID;
///        RunTime.StartTime=StartWorkTime+SizeWorkTime*60*10000000i64;
		RunTime.StartTime=StartWorkTime+SizeWorkTime*60*10000000i64+
			(State&(mcsPause|mcsOpen)?SystemTime-StopTimerTime:0);
		RunTime.MaxTime=24*60-SizeWorkTime;
	} else
	{
		RunTime.TariffID=0;
		RunTime.StartTime=0;
		RunTime.MaxTime=0;
	}

	return RunTime;
}
//---------------------------------------------------------------------------
MStatesInfo MStatesItem::GetInfo()
{
	MStatesInfo Info;
	memset(&Info,0,sizeof(Info));

    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);
    //
    Info.Number=Number;
    Info.State=State;
    if ( State&mcsWork )
    {
        Info.TariffID=TariffID;
        Info.WorkTime=SizeWorkTime;
        Info.ToEndWork=SizeWorkTime-((State&(mcsPause|mcsOpen)?StopTimerTime:SystemTime)-
            StartWorkTime)/(60*10000000i64);
        Info.EndWorkTime=ExtractHoursMin(StartWorkTime+SizeWorkTime*60*10000000i64+
            (State&(mcsPause|mcsOpen)?SystemTime-StopTimerTime:0));
    }
    if ( State&mcsFine )
    {
        Info.FineTime=SizeFineTime;
        Info.ToEndFine=SizeFineTime-(SystemTime-StartFineTime)/(60*10000000i64);
        Info.EndFineTime=ExtractHoursMin(StartFineTime+SizeFineTime*60*10000000i64);
    }
    Info.Commands=Commands;
    Info.NetState=NetState&(mnsPresent|mnsSyncNeed);
    Info.Changes=Changes;
    //
	Changes=0;

    return Info;
}
//---------------------------------------------------------------------------
bool MStatesItem::Timer(__int64 SystemTime_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    SystemTime=SystemTime_;
    return ControlWorkTime()||ControlFineTime();
}
//---------------------------------------------------------------------------
void MStatesItem::Associate(int Number_)
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
bool MStatesItem::NetBegin()
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
bool MStatesItem::NetEnd()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    CmdsToReset&=~(mccReboot|mccShutdown);
    // ������� ���������� ������� � ������� ���������
    NetState&=~mnsLock;
    return NetState&mnsNeedSave;
}
//---------------------------------------------------------------------------
bool MStatesItem::NetSyncNewData()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    return NetState&mnsSyncData;
}
//---------------------------------------------------------------------------
MSyncData MStatesItem::NetSyncData()
{
	MSyncData ResData;

	MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

	// ���������� ���� ������� ����� ������
	NetState&=~mnsSyncData;
	// ������� ����� ������ � ������ ������
	ResData.SystemTime=SystemTime;
	ResData.Number=Number;
	ResData.State=State;
	ResData.StartWorkTime=StartWorkTime;
	ResData.SizeWorkTime=SizeWorkTime;
	ResData.StartFineTime=StartFineTime;
	ResData.SizeFineTime=SizeFineTime;
	ResData.StopTimerTime=StopTimerTime;
	ResData.Programs=Programs;
	ResData.Commands=Commands;
	CmdsToReset|=ResData.Commands;

	return ResData;
}
//---------------------------------------------------------------------------
void MStatesItem::NetSyncExecuted(bool Executed_)
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
bool MStatesItem::NetPwrOnNeed()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    return Commands&mccPowerOn;
}
//---------------------------------------------------------------------------
void MStatesItem::NetPwrOnExecuted()
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
MStates::const_iterator MStates::Search(int Number_) const
{
	auto iState=begin();
	auto iEnd=end();

	while ( iState!=iEnd )
	{
		if ( iState->Associated()==Number_ ) break;
		++iState;
	}

	return iState;
}
//---------------------------------------------------------------------------
bool MStates::Update(const MComputers &Computers_)
{
	bool result=false;

	// ������� ���������, ��������������� � ���������������
	// ��� ��������������� ������������
	for ( auto iState=cbegin(), iEnd=cend(); iState!=iEnd; )
	{
		auto iComputer=Computers_.Search(iState->Associated());

		if (
			(iComputer==Computers_.end())||
			(iComputer->NotUsed) )
		{
			result=true;
			iState=Del(iState);
		} else
		{
			++iState;
		}
	}
	// ��������� ��������� ��� ����� �����������
	for ( const auto &Computer: Computers_ )
	{
		if ( Computer.NotUsed ) continue;
		auto iState=Search(Computer.Number);
		if ( iState!=end() ) continue;
		result=true;
		Add().Associate(Computer.Number);
	}
	// ������� ������ ������, ��������������� � ����� � ��� �� �����������
	for ( auto iState=cbegin(), iEnd=cend(); iState!=iEnd; )
	{
		if ( Search(iState->Associated())!=iState )
		{
			result=true;
			iState=Del(iState);
		} else
		{
			++iState;
		}
	}

	return result;
}
//---------------------------------------------------------------------------
bool MStates::Timer(__int64 SystemTime_)
{
	bool result=false;
	//
	for ( auto &State: *this ) result|=State.Timer(SystemTime_);
    //
    return result;
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
void *MStateCl::SetData(void *Data_) const
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
const void *MStateCl::GetData(const void *Data_, const void *Limit_)
{
	if ( !(
		(Data_=MemGet(Data_,&Number,Limit_)) &&
		(Data_=MemGet(Data_,&State,Limit_)) &&
		(Data_=MemGet(Data_,&StartWorkTime,Limit_)) &&
		(Data_=MemGet(Data_,&SizeWorkTime,Limit_)) &&
		(Data_=MemGet(Data_,&StartFineTime,Limit_)) &&
		(Data_=MemGet(Data_,&SizeFineTime,Limit_)) &&
		(Data_=MemGet(Data_,&StopTimerTime,Limit_)) &&
		(Data_=MemGet(Data_,&Programs,Limit_))
		) ) return nullptr;

	Changes=mdcNumber|mdcState;

	return Data_;
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
MStatesInfo MStateCl::GetInfo()
{
	MStatesInfo ResInfo;
	memset(&ResInfo,0,sizeof(ResInfo));

	MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

	//
    ResInfo.Number=Number;
    ResInfo.State=State;
    if ( State&mcsWork )
    {
        ResInfo.WorkTime=SizeWorkTime;
        ResInfo.ToEndWork=SizeWorkTime-((State&(mcsPause|mcsOpen)?StopTimerTime:SystemTime)-
            StartWorkTime)/(60*10000000i64);
        ResInfo.EndWorkTime=ExtractHoursMin(StartWorkTime+SizeWorkTime*60*10000000i64+
			(State&(mcsPause|mcsOpen)?SystemTime-StopTimerTime:0));
    }
    if ( State&mcsFine )
	{
		ResInfo.FineTime=SizeFineTime;
		ResInfo.ToEndFine=SizeFineTime-(SystemTime-StartFineTime)/(60*10000000i64);
		ResInfo.EndFineTime=ExtractHoursMin(StartFineTime+SizeFineTime*60*10000000i64);
	}
	ResInfo.Programs=Programs;
	ResInfo.Commands=Commands;
	ResInfo.Changes=Changes;
    //
	Commands=Changes=0;

    return ResInfo;
}
//---------------------------------------------------------------------------
bool MStateCl::GetOptions(MClOptions &Options_)
{
	MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);
	bool result;

	result=Options_.QueryFrom(OptKey,OptPath,OptValue,DefaultCode);
    if ( result ) AutoLockTime=Options_.AutoLockTime;

    return result;
}
//---------------------------------------------------------------------------
bool MStateCl::GetGames(MGames &Games_)
{
	MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    return Games_.LoadFrom(PrgFile,DefaultCode);
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
bool MStateCl::NewSyncData(MSyncData &Data_)
{
	bool NeedSave=false;
	__int64 CurrentTime;

	// ��������� ���������� ���������� ������� � ��� ������������� ������������ ���
	GetLocalTimeInt64(CurrentTime);
	CurrentTime=(CurrentTime-=Data_.SystemTime)<0?-CurrentTime:CurrentTime;
	if ( CurrentTime>=MAX_TimeShift*10000000i64 ) SetLocalTimeInt64(Data_.SystemTime);

    CS_Main.Enter();
    // ������� ����� ������ � ������ ������ � �������� ������� ��� ��������
    if ( Data_.Number!=Number ) { Number=Data_.Number; Changes|=mdcNumber; NeedSave=true; }
    if ( Data_.State!=State ) { State=Data_.State; Changes|=mdcState; NeedSave=true; }
    if ( Data_.StartWorkTime!=StartWorkTime ) { StartWorkTime=Data_.StartWorkTime; Changes|=mdcWorkTime; NeedSave=true; }
    if ( Data_.SizeWorkTime!=SizeWorkTime ) { SizeWorkTime=Data_.SizeWorkTime; Changes|=mdcWorkTime; NeedSave=true; }
    if ( Data_.StartFineTime!=StartFineTime ) { StartFineTime=Data_.StartFineTime; Changes|=mdcFineTime; NeedSave=true; }
    if ( Data_.SizeFineTime!=SizeFineTime ) { SizeFineTime=Data_.SizeFineTime; Changes|=mdcFineTime; NeedSave=true; }
    if ( Data_.StopTimerTime!=StopTimerTime ) { StopTimerTime=Data_.StopTimerTime; NeedSave=true; }
    if ( Data_.Programs!=Programs ) { Programs=Data_.Programs; Changes|=mdcPrograms; NeedSave=true; }
	if ( Data_.Commands!=Commands ) { Commands=Data_.Commands; Changes|=mdcCommands; NeedSave=true; }
	//
    LastSyncTime=::GetTickCount();
    CS_Main.Leafe();

    return NeedSave;
}
//---------------------------------------------------------------------------
bool MStateCl::NewGames(const MGames &Games_)
{
	MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);
	bool result=false;

	try { result=Games_.SaveTo(PrgFile,DefaultCode); }
	catch (std::bad_alloc &e) {}
	if ( result ) Changes|=mdcPrograms;

    return result;
}
//---------------------------------------------------------------------------
bool MStateCl::NewOptions(const MClOptions &Options_)
{
	MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);
	bool result=false;

	try { result=Options_.StoreTo(OptKey,OptPath,OptValue,DefaultCode); }
	catch (std::bad_alloc &e) {}
	if ( result ) { AutoLockTime=Options_.AutoLockTime; Changes|=mdcOptions; }

	return result;
}
//---------------------------------------------------------------------------
void MStateCl::SetDefault(
        HKEY RegKey_,
        const std::wstring &RegPath_,
        const std::wstring &RegValue_,
        HKEY OptKey_,
        const std::wstring &OptPath_,
        const std::wstring &OptValue_,
        const std::wstring &PrgFile_,
        unsigned RegCode_)
{
    MSLList::SetDefaultKey(RegKey_,RegPath_,RegValue_,RegCode_);

    OptKey=OptKey_;
    OptPath=OptPath_;
    OptValue=OptValue_;
    PrgFile=PrgFile_;
}
//---------------------------------------------------------------------------

