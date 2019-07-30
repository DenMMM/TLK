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
    // Помечаем данные для отправки по сети
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
    // Помечаем данные для отправки по сети
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
        // Запуск компьютера в работу
        State=mcsWork;
        TariffID=Tariff_->ID;
        StartWorkTime=SystemTime;
        SizeWorkTime=Time_->WorkTime;
        Programs=Tariff_->Programs;
        Changes|=mdcState|mdcTariff|mdcWorkTime;
        // Если компьютер после запуска нужно перезагрузить, то делаем нужные пометки
        if ( Tariff_->Reboot ) { Commands|=mccReboot; Changes|=mdcCommands; }
    } else
    {
        // Добавление времени работающему компьютеру
        SizeWorkTime+=Time_->WorkTime;
        if ( SizeWorkTime>(24*60) ) SizeWorkTime=24*60;
        Changes|=mdcWorkTime;
    }
    // Помечаем данные для отправки по сети
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
//            if ( !(State&mcsOpen) ) { Commands|=mccReboot; Changes|=mdcCommands; }
        }
    } else
    {
        // Предупреждение

    }
    // Помечаем данные для отправки по сети
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
    // Задаем новые режимы работы
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
    // Помечаем данные для отправки по сети
    State_->NetState|=mnsSyncNeed|mnsSyncData; State_->Changes|=mdcNetState;
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
        // Проверяем запущен ли компьютер и не прикрыт ли он уже
        if ( (State&(mcsWork|mcsLock))!=mcsWork ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        // Помечаем, что компьютер прикрыт
        State|=mcsLock;
    } else
    {
        // Проверяем прикрыт ли компьютер
        if ( !(State&mcsLock) ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        // Снимаем пометку, что компьютер прикрыт
        State&=~mcsLock;
    }
    Changes|=mdcState;
    // Помечаем данные для отправки по сети
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
        // Проверяем запущен ли компьютер и не приостановлен ли он уже
        if ( (State&(mcsWork|mcsPause))!=mcsWork ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        if ( !(State&mcsOpen) ) StopTimerTime=SystemTime;
        // Помечаем, что компьютер приостановлен
        State|=mcsPause;
    } else
    {
        // Проверяем приостановлен ли компьютер
        if ( !(State&mcsPause) ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        // Если компьютер не открыт для обслуживания, корретируем время его работы
        if ( !(State&mcsOpen) )
        {
            StartWorkTime+=SystemTime-StopTimerTime;
//            StartFineTime+=SystemTime-StopTimerTime;
            StopTimerTime=0;
        }
        // Снимаем пометку, что компьютер приостановлен
        State&=~mcsPause;
    }
    Changes|=mdcState;
    // Помечаем данные для отправки по сети
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
        // Проверяем свободен или запущен компьютер и не открыт ли он уже
        if ( (!(State&(mcsFree|mcsWork)))||(State&mcsOpen) ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        // Если компьютер запущен и не приостановлен, сохраняем время его открытия
        if ( (State&(mcsWork|mcsPause))==mcsWork ) StopTimerTime=SystemTime;
        // Помечаем, что компьютер открыт
        State|=mcsOpen;
    } else
    {
        // Проверяем открыт ли компьютер
        if ( !(State&mcsOpen) ) { UnLock(); return false; }
        if ( Check_ ) { UnLock(); return true; }
        // Если компьютер запущен и не приостановлен, корретируем время его работы
        if ( (State&(mcsWork|mcsPause))==mcsWork )
        {
            StartWorkTime+=SystemTime-StopTimerTime;
//            StartFineTime+=SystemTime-StopTimerTime;
            StopTimerTime=0;
        }
        // Снимаем пометку, что компьютер открыт
        State&=~mcsOpen;
    }
    Changes|=mdcState;
    // Помечаем данные для отправки по сети
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
    // Помечаем данные для отправки по сети
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
    // Помечаем данные для отправки по сети
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
    // Помечаем данные для отправки по сети
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
    // Проверяем не блокирован ли доступ к сетевым операциям
    if ( NetState&mnsLock ) { UnLock(); return false; }
    // Блокируем доступ к сетевым операциям
    NetState|=mnsLock;
    // Сбрасываем флаг необходимости сохранения состояния на диск
    NetState&=~mnsNeedSave;
    // Задаем постоянную выдачу данных для несинхронизированного состояния
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
    // Снимаем блокировку доступа к сетевым операциям
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
    // Сбрасываем флаг наличия новых данных
    NetState&=~mnsSyncData;
    // Заносим новые данные о режиме работы
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
    // Сбрасываем команду для компьютера, т.к. она выполнена
    Commands&=~mccPowerOn;
    // Помечаем изменения для оболочки
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

    // Убираем состояния, ассоциированные с несуществующими
    // или неиспользуемыми компьютерами
    State=(MState*)First;
    while(State)
    {
        NextState=(MState*)State->Next;
        Computer=Computers_->Search(State->Associated());
        if ( (Computer==NULL)||(Computer->NotUsed) ) Delete(State);
        State=NextState;
    }
    // Добавляем состояния для новых компьютеров
    for ( Computer=(MComputer*)Computers_->First;
        Computer; Computer=(MComputer*)Computer->Next )
    {
        if ( Computer->NotUsed||Search(Computer->Number) ) continue;
        State=(MState*)Add();
        State->Associate(Computer->Number);
    }
    // Убираем лишние записи, ассоциированные с одним и тем же компьютером
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

