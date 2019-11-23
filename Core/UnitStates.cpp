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
    // Помечаем данные для отправки по сети
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
    // Помечаем данные для отправки по сети
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
		// Запуск компьютера в работу
		State=mcsWork;
		TariffID=Tariff_.gUUID();
		StartWorkTime=SystemTime;
		SizeWorkTime=Time_.WorkTime;
		Programs=Tariff_.Programs;
		Changes|=mdcState|mdcTariff|mdcWorkTime;
		// Если компьютер после запуска нужно перезагрузить, то делаем нужные пометки
		if ( Tariff_.Reboot ) { Commands|=mccReboot; Changes|=mdcCommands; }
	} else
	{
		// Добавление времени работающему компьютеру
		SizeWorkTime+=Time_.WorkTime;
		if ( SizeWorkTime>(24*60) ) SizeWorkTime=24*60;
		Changes|=mdcWorkTime;
	}
	// Помечаем данные для отправки по сети
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
        // Установка штрафа с ожиданием
        if ( State&mcsFine )
        {
            // Увеличиваем время уже примененного штрафа
            SizeFineTime+=FineSize_;
            Changes|=mdcFineTime;
        } else
        {
            // Задаем новый штраф
            State|=mcsFine;
            StartFineTime=SystemTime;
            SizeFineTime=FineSize_;
            Changes|=mdcState|mdcFineTime;
        }
        // Проверяем не перекрывает ли время штрафа оставшееся время работы
        if ( ((SizeWorkTime-SizeFineTime)*(60*10000000i64)+
            StartWorkTime-StartFineTime+
            SystemTime-(State&(mcsPause|mcsOpen)?StopTimerTime:SystemTime))<0 ) goto full;
    } else if ( FineSize_<0 )
    {
        // Уменьшение времени работы по штрафу без ожидания
        SizeWorkTime+=FineSize_;
        Changes|=mdcWorkTime;
        if ( SizeWorkTime<0 )
        {
full:
            // Полное снятие времени работы
            State=mcsFree|(State&mcsOpen);
            TariffID=0;
            StartWorkTime=0; SizeWorkTime=0;
            StartFineTime=0; SizeFineTime=0;
            StopTimerTime=0;
            Programs=0;
            Changes|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime;
            // Перезагружаем компьютер при закрытии
///            if ( !(State&mcsOpen) ) { Commands|=mccReboot; Changes|=mdcCommands; }
        }
    } else
    {
        // Предупреждение

    }
    // Помечаем данные для отправки по сети
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MStatesItem::CmdExchange(MStatesItem &State_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::DualLock lckObj(CS_Main,State_.CS_Main);

    if ( !((State&mcsWork)&&(State_.State==mcsFree)) ) return false;
    if ( Check_ ) return true;
    // Задаем новые режимы работы
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
    // Помечаем данные для отправки по сети
	State_.NetState|=mnsSyncNeed|mnsSyncData; State_.Changes|=mdcNetState;
	// Задаем новые режимы работы
	State=mcsFree|(State&mcsOpen);
	TariffID=0;
	StartWorkTime=0; SizeWorkTime=0;
    StartFineTime=0; SizeFineTime=0;
    StopTimerTime=0;
    Programs=0;
    Changes|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime;
    // Помечаем данные для отправки по сети
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    // Перезагружаем компьютер при закрытии
///    if ( !(State&mcsOpen) ) { Commands|=mccReboot; Changes|=mdcCommands; }

    return true;
}
//---------------------------------------------------------------------------
bool MStatesItem::CmdLock(bool Apply_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Apply_ )
    {
        // Проверяем запущен ли компьютер и не прикрыт ли он уже
        if ( (State&(mcsWork|mcsLock))!=mcsWork ) return false;
        if ( Check_ ) return true; 
        // Помечаем, что компьютер прикрыт
        State|=mcsLock;
    } else
    {
        // Проверяем прикрыт ли компьютер
        if ( !(State&mcsLock) ) return false;
        if ( Check_ ) return true;
        // Снимаем пометку, что компьютер прикрыт
        State&=~mcsLock;
    }
    Changes|=mdcState;
    // Помечаем данные для отправки по сети
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MStatesItem::CmdPause(bool Apply_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Apply_ )
    {
        // Проверяем запущен ли компьютер и не приостановлен ли он уже
        if ( (State&(mcsWork|mcsPause))!=mcsWork ) return false;
        if ( Check_ ) return true;
        if ( !(State&mcsOpen) ) StopTimerTime=SystemTime;
        // Помечаем, что компьютер приостановлен
        State|=mcsPause;
    } else
    {
        // Проверяем приостановлен ли компьютер
        if ( !(State&mcsPause) ) return false;
        if ( Check_ ) return true;
        // Если компьютер не открыт для обслуживания, корретируем время его работы
        if ( !(State&mcsOpen) )
        {
            StartWorkTime+=SystemTime-StopTimerTime;
///            StartFineTime+=SystemTime-StopTimerTime;  // Приостановка штрафа
            StopTimerTime=0;
        }
        // Снимаем пометку, что компьютер приостановлен
        State&=~mcsPause;
    }
    Changes|=mdcState;
    // Помечаем данные для отправки по сети
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MStatesItem::CmdOpen(bool Apply_, bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Apply_ )
    {
        // Проверяем свободен или запущен компьютер и не открыт ли он уже
        if ( (!(State&(mcsFree|mcsWork)))||(State&mcsOpen) ) return false;
        if ( Check_ ) return true;
        // Если компьютер запущен и не приостановлен, сохраняем время его открытия
        if ( (State&(mcsWork|mcsPause))==mcsWork ) StopTimerTime=SystemTime;
        // Помечаем, что компьютер открыт
        State|=mcsOpen;
    } else
    {
        // Проверяем открыт ли компьютер
        if ( !(State&mcsOpen) ) return false;
        if ( Check_ ) return true;
        // Если компьютер запущен и не приостановлен, корретируем время его работы
        if ( (State&(mcsWork|mcsPause))==mcsWork )
        {
            StartWorkTime+=SystemTime-StopTimerTime;
///            StartFineTime+=SystemTime-StopTimerTime; // Приостановка штрафа
            StopTimerTime=0;
        }
        // Снимаем пометку, что компьютер открыт
        State&=~mcsOpen;
    }
    Changes|=mdcState;
    // Помечаем данные для отправки по сети
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
    // Помечаем данные для отправки по сети
    NetState|=mnsSyncNeed|mnsSyncData; Changes|=mdcNetState;

    return true;
}
//---------------------------------------------------------------------------
bool MStatesItem::CmdShutdown(bool Check_)
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    if ( Check_ ) return true;
    Commands|=mccShutdown; Changes|=mdcCommands;
    // Помечаем данные для отправки по сети
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

    // Проверяем не блокирован ли доступ к сетевым операциям
    if ( NetState&mnsLock ) return false;
    // Блокируем доступ к сетевым операциям
    NetState|=mnsLock;
    // Сбрасываем флаг необходимости сохранения состояния на диск
    NetState&=~mnsNeedSave;
    // Задаем постоянную выдачу данных для несинхронизированного состояния
///    if ( NetState&mnsSyncNeed ) NetState|=mnsSyncData;
    return true;
}
//---------------------------------------------------------------------------
bool MStatesItem::NetEnd()
{
    MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Main);

    CmdsToReset&=~(mccReboot|mccShutdown);
    // Снимаем блокировку доступа к сетевым операциям
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

	// Сбрасываем флаг наличия новых данных
	NetState&=~mnsSyncData;
	// Заносим новые данные о режиме работы
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
        // Если за время отправки последних запрошенных данных новых не поступило,
        // сбрасываем флаг необходимости синхронизации
        if ( (NetState&(mnsSyncNeed|mnsSyncData))==mnsSyncNeed )
        {
            NetState&=~mnsSyncNeed;
            NetState|=mnsNeedSave;
        }
        // Помечаем, что компьютер доступен по сети
        NetState|=mnsPresent;
        // Сбрасываем команды, отправленные для исполнения клиентом
        Commands&=~CmdsToReset;
        CmdsToReset=0;
        // Помечаем изменения для оболочки
        Changes|=mdcCommands;
    } else
    {
        // Сбрасываем флаг доступности компьютера по сети
        NetState&=~mnsPresent;
    }
    // Помечаем изменения для оболочки
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

	// Сбрасываем команду для компьютера, т.к. она выполнена
	Commands&=~mccPowerOn;
	// Помечаем изменения для оболочки
	Changes|=mdcCommands;
}
//---------------------------------------------------------------------------
bool MStates::Save()
{
	MWAPI::CRITICAL_SECTION::Lock lckObj(CS_File);

	// Сохраняем в файл с безопасной перезаписью и без кэширования
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

	// Убираем состояния, ассоциированные с несуществующими
	// или неиспользуемыми компьютерами
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
	// Добавляем состояния для новых компьютеров
	for ( const auto &Computer: Computers_ )
	{
		if ( Computer.NotUsed ) continue;
		auto iState=Search(Computer.Number);
		if ( iState!=end() ) continue;
		result=true;
		Add().Associate(Computer.Number);
	}
	// Убираем лишние записи, ассоциированные с одним и тем же компьютером
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

	// Проверяем отклонение системного времени и при необходимости корректируем его
	GetLocalTimeInt64(CurrentTime);
	CurrentTime=(CurrentTime-=Data_.SystemTime)<0?-CurrentTime:CurrentTime;
	if ( CurrentTime>=MAX_TimeShift*10000000i64 ) SetLocalTimeInt64(Data_.SystemTime);

    CS_Main.Enter();
    // Заносим новые данные о режиме работы и помечаем события для оболочки
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

