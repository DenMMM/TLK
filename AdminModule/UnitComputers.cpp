//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include "UnitComputers.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
CRITICAL_SECTION CS_ComputersStateFile;
//---------------------------------------------------------------------------
MCompParam::MCompParam()
{
    ID=0;
    Address="000.000.000.000";
    Number=0;
    GroupColor=mgcNone;
}
//---------------------------------------------------------------------------
MCompParam::~MCompParam()
{
//
}
//---------------------------------------------------------------------------
MCompParam &MCompParam::operator=(MCompParam &CompParam_)
{
    ID=CompParam_.ID;
    Address=CompParam_.Address;
    Number=CompParam_.Number;
    GroupColor=CompParam_.GroupColor;
    return *this;
}
//---------------------------------------------------------------------------
bool MComputer::Save()
{
    char file_name[MAX_PATH];
    bool result;

    strcpy(file_name,PrgDir.c_str());
    strcat(file_name,"COMPUTERS\\PARAM.DAT");
    ::EnterCriticalSection(&CS_ComputersStateFile);
    result=ComputerSave(this,file_name);
    ::LeaveCriticalSection(&CS_ComputersStateFile);

    return result;
}
//---------------------------------------------------------------------------
bool MComputer::CmdRun(unsigned int TariffID_, unsigned int GamesPages_, int WorkTime_)
{
    Lock();
    if ( !(State&(mcsFree|mcsWork)) ) { UnLock(); return false; }
    double SystemTime=(double)(Date()+Time());
    if ( State&mcsFree )
    {
        // ������ ���������� � ������ (������� ����������� ������ �����������)
        StartWorkTime=SystemTime;
        SizeWorkTime=WorkTime_;
        GamesPages=GamesPages_;
        TariffID=TariffID_;
        State=mcsWork|(State&mcsAuto);
        DataChanges|=mdcState|mdcTariff|mdcWorkTime|mdcNetState;
    } else
    {
        // ���������� ������� ����������� ���������� (��� ������ �����������)
        SizeWorkTime+=WorkTime_;
        if ( SizeWorkTime>(24*60) ) SizeWorkTime=24*60;
        DataChanges|=mdcWorkTime;
    }
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    UnLock();
    return true;
}
//---------------------------------------------------------------------------
void MComputer::CmdFine(int FineSize_)
{
    Lock();
    if ( !(State&mcsWork) ) { UnLock(); return; }
    double SystemTime=(double)(Date()+Time());
    if ( FineSize_>0 )
    {
        // ��������� ������ � ���������
        if ( State&mcsFine )
        {
            // ����������� ����� ��� ���������� ������
            SizeFineTime+=FineSize_;
            if ( SizeFineTime>(24*60) ) SizeFineTime=24*60;
            DataChanges|=mdcFineTime|mdcNetState;
        } else
        {
            // ������ ����� �����
            StartFineTime=SystemTime;
            SizeFineTime=FineSize_;
            State|=mcsFine;
            DataChanges|=mdcState|mdcFineTime|mdcNetState;
        }
    } else if ( FineSize_<0 )
    {
        // ���������� ������� ������ �� ������ ��� ��������
        SizeWorkTime+=FineSize_;
        if ( SizeWorkTime<0 ) goto full;
        DataChanges|=mdcWorkTime|mdcNetState;
    } else
    {
full:
        // ������ ������ ������� ������
        TariffID=0; GamesPages=mgpNone;
        StartWorkTime=0.; SizeWorkTime=0;
        StartFineTime=0.; SizeFineTime=0;
        State=mcsFree|(State&mcsAuto);
        DataChanges|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime|mdcNetState;
    }
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    UnLock();
}
//---------------------------------------------------------------------------
void MComputer::CmdExchange(MComputer *Computer_)
{
    Lock(); Computer_->Lock();
    if ( (!((State&mcsWork)&&(Computer_->State&mcsFree)||
        (State&mcsFree)&&(Computer_->State&mcsWork)))||
         ((State|Computer_->State)&(mcsWtLocker|mcsNotUse)))
         { Computer_->UnLock(); UnLock(); return; }

    int a; double b;
    a=State; State=Computer_->State; Computer_->State=a;
    b=StartWorkTime; StartWorkTime=Computer_->StartWorkTime; Computer_->StartWorkTime=b;
    a=SizeWorkTime; SizeWorkTime=Computer_->SizeWorkTime; Computer_->SizeWorkTime=a;
    b=StartFineTime; StartFineTime=Computer_->StartFineTime; Computer_->StartFineTime=b;
    a=SizeFineTime; SizeFineTime=Computer_->SizeFineTime; Computer_->SizeFineTime=a;
    b=StopTimerTime; StopTimerTime=Computer_->StopTimerTime; Computer_->StopTimerTime=b;
    a=FaceType; FaceType=Computer_->FaceType; Computer_->FaceType=a;
    a=GamesPages; GamesPages=Computer_->GamesPages; Computer_->GamesPages=a;
    a=TariffID; TariffID=Computer_->TariffID; Computer_->TariffID=a;

    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    DataChanges|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime|mdcNetState;
    Computer_->NextStateVer(); Computer_->NetSync|=mnsState; Computer_->Save();
    Computer_->NeedConnect=true; Computer_->NetTimeOut=0;
    Computer_->DataChanges|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime|mdcNetState;
    Computer_->UnLock(); UnLock();
}
//---------------------------------------------------------------------------
void MComputer::CmdLock(bool Apply_)
{
    Lock();
    if ( Apply_ )
    {
        if ( !(State&mcsWork) ) { UnLock(); return; }
        State|=mcsLock;
    } else
    {
        if ( !(State&mcsLock) ) { UnLock(); return; }
        State&=~mcsLock;
    }
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    DataChanges|=mdcState|mdcNetState;
    UnLock();
}
//---------------------------------------------------------------------------
void MComputer::CmdPause(bool Apply_)
{
    double SystemTime=(double)(Date()+Time());
    Lock();
    if ( Apply_ )
    {
        if ( (!(State&mcsWork))||(State&mcsPause) ) { UnLock(); return; }
        if ( !(State&mcsAuto) ) StopTimerTime=SystemTime;
        State|=mcsPause;
    } else
    {
        if ( !(State&mcsPause) ) { UnLock(); return; }
        if ( !(State&mcsAuto) )
        {
            // ���� ��������� �� ���������, �� ��������������� �����
            StartWorkTime+=SystemTime-StopTimerTime;
//            StartFineTime+=SystemTime-StopTimerTime;
            StopTimerTime=0.;
        }
        State&=~mcsPause;
    }
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    DataChanges|=mdcState|mdcNetState;
    UnLock();
}
//---------------------------------------------------------------------------
void MComputer::CmdAuto(bool Apply_)
{
    Lock();
    double SystemTime=(double)(Date()+Time());
    if ( Apply_ )
    {
        if ( !(State&(mcsFree|mcsWork)) ) { UnLock(); return; }
        if ( !(State&mcsPause) )StopTimerTime=SystemTime;
        State|=mcsAuto;
    } else
    {
        if ( !(State&mcsAuto) ) { UnLock(); return; }
        if ( !(State&mcsPause) )
        {
            // ���� ��������� �� �������������, �� ��������������� �����
            StartWorkTime+=SystemTime-StopTimerTime;
//            StartFineTime+=SystemTime-StopTimerTime;
            StopTimerTime=0.;
        }
        State&=~mcsAuto;
    }
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    DataChanges|=mdcState|mdcNetState;
    UnLock();
}
//---------------------------------------------------------------------------
void MComputer::CmdWtLocker(bool Apply_)
{
    Lock();
    if ( Apply_ )
    {
        if ( !(State&mcsFree) ) { UnLock(); return; }
        State=mcsWtLocker;
        DataChanges|=mdcState|mdcWorkTime|mdcFineTime|mdcNetState;
    } else
    {
        if ( !(State&mcsWtLocker) ) { UnLock(); return; }
        State=mcsFree;
        DataChanges|=mdcState|mdcNetState;
    }
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    UnLock();
}
//---------------------------------------------------------------------------
void MComputer::CmdNotUse(bool Apply_)
{
    Lock();
    if ( Apply_ )
    {
        TariffID=0; GamesPages=mgpNone;
        StartWorkTime=0.; SizeWorkTime=0;
        StartFineTime=0.; SizeFineTime=0;
        StopTimerTime=0.;
        State=mcsNotUse;
        DataChanges|=mdcState|mdcWorkTime|mdcFineTime|mdcNetState;
    } else
    {
        if ( !(State&mcsNotUse) ) { UnLock(); return; }
        State=mcsFree;
        DataChanges|=mdcState|mdcNetState;
    }
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    UnLock();
}
//---------------------------------------------------------------------------
void MComputer::CmdReboot()
{
    Lock();
    State|=mcsReboot;
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    DataChanges|=mdcNetState;
    UnLock();
}
//---------------------------------------------------------------------------
void MComputer::CmdShutdown()
{
    Lock();
    State|=mcsShutdown;
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    DataChanges|=mdcNetState;
    UnLock();
}
//---------------------------------------------------------------------------
bool MComputer::ControlWorkTime()
{
    double SystemTime=(double)(Date()+Time());
    if ( (!(State&mcsWork))||(State&(mcsPause|mcsAuto))||
        (SystemTime<(StartWorkTime+SizeWorkTime/(24.*60))) ) return false;
    TariffID=0; GamesPages=mgpNone;
    StartWorkTime=0.; SizeWorkTime=0;
    StartFineTime=0.; SizeFineTime=0;
    State=mcsFree|(State&mcsAuto);
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    DataChanges|=mdcState|mdcTariff|mdcWorkTime|mdcFineTime|mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
bool MComputer::ControlFineTime()
{
    double SystemTime=(double)(Date()+Time());
    if ( (!(State&mcsFine))||
        (SystemTime<(StartFineTime+SizeFineTime/(24.*60))) ) return false;
    StartFineTime=0.; SizeFineTime=0;
    State&=~mcsFine;
    // �������� ����� ��������� ���������
    NextStateVer(); NetSync|=mnsState; Save();
    NeedConnect=true; NetTimeOut=0;
    DataChanges|=mdcState|mdcFineTime|mdcNetState;
    return true;
}
//---------------------------------------------------------------------------
void MComputer::NextStateVer()
{
    do StateVer+=random(100)+1;
    while((StateVer==0)||(StateVer==-1));
}
//---------------------------------------------------------------------------
bool MComputer::SetGames(MGames *Games_)
{
    char file_name[MAX_PATH];
    bool result;

    strcpy(file_name,PrgDir.c_str());
    sprintf(file_name+strlen(file_name),"%s%08X%s","COMPUTERS\\",ID,".GMS");
    LockGames(); result=GamesSave(Games_,file_name); UnLockGames();
    if ( result )
    {
        Lock(); NetSync|=mnsGames; Save();
        NeedConnect=true; NetTimeOut=0;
        DataChanges|=mdcNetState; UnLock();
    }

    return result;
}
//---------------------------------------------------------------------------
bool MComputer::GetGames(MGames *Games_)
{
    char file_name[MAX_PATH];
    bool result;

    strcpy(file_name,PrgDir.c_str());
    sprintf(file_name+strlen(file_name),"%s%08X%s","COMPUTERS\\",ID,".GMS");
    LockGames(); result=GamesLoad(Games_,file_name); UnLockGames();

    return result;
}
//---------------------------------------------------------------------------
void MComputer::Timer()
{
    Lock();
    ControlWorkTime()||ControlFineTime();
    if ( (++NetTimeOut)>=MAX_NetTimeOut ) { NetTimeOut=0; NeedConnect=true; }
    UnLock();
}
//---------------------------------------------------------------------------
MComputer::MComputer()
{
    ::InitializeCriticalSection(&CS_Main);
    ::InitializeCriticalSection(&CS_Games);
    ID=0;
    Address="000.000.000.000";
    Number=0;
    GroupColor=mgcNone;
    State=mcsFree;
    StartWorkTime=0.; SizeWorkTime=0;
    StartFineTime=0.; SizeFineTime=0;
    StopTimerTime=0.;
    FaceType=0;
    GamesPages=mgpNone;
    TariffID=0;
    StateVer=0;
    NetSync=NotSync=mnsState;
    NetTimeOut=0;
    NetPresent=SyncProcess=false; NeedConnect=true;
    DataChanges=0;
}
//---------------------------------------------------------------------------
MComputer::~MComputer()
{
    ::DeleteCriticalSection(&CS_Main);
    ::DeleteCriticalSection(&CS_Games);
}
//---------------------------------------------------------------------------
MComputer *MComputers::SearchID(unsigned int ID_)
{
    MComputer *Computer=(MComputer*)FirstItem;
    while(Computer)
    {
        if ( Computer->ID==ID_ ) break;
        Computer=(MComputer*)Computer->NextItem;
    }
    return Computer;
}
//---------------------------------------------------------------------------
MComputer *MComputers::SearchIP(AnsiString &IP_)
{
    MComputer *Computer=(MComputer*)FirstItem;
    while(Computer)
    {
        if ( Computer->Address==IP_ ) break;
        Computer=(MComputer*)Computer->NextItem;
    }
    return Computer;
}
//---------------------------------------------------------------------------
bool MComputers::Save()
{
    char file_name[MAX_PATH];
    bool result;

    // ������������� ������� ��������
    for ( int i=0; i<NUM_SyncThreads; i++ ) ThreadNetSync[i]->Pause(true);
    // ��������� ������ � �����������
    strcpy(file_name,PrgDir.c_str());
    strcat(file_name,"COMPUTERS\\PARAM.DAT");
    ::EnterCriticalSection(&CS_ComputersStateFile);
    result=ComputersSave(this,file_name);
    ::LeaveCriticalSection(&CS_ComputersStateFile);
    // ��������������� ������� ��������
    for ( int i=0; i<NUM_SyncThreads; i++ ) ThreadNetSync[i]->Pause(false);

    return result;
}
//---------------------------------------------------------------------------
bool MComputers::Load()
{
    char file_name[MAX_PATH];
    bool result;

    // ������������� ������� ��������
    for ( int i=0; i<NUM_SyncThreads; i++ ) ThreadNetSync[i]->Pause(true);
    // ��������� ������ � �����������
    strcpy(file_name,PrgDir.c_str());
    strcat(file_name,"COMPUTERS\\PARAM.DAT");
    ::EnterCriticalSection(&CS_ComputersStateFile);
    result=ComputersLoad(this,file_name);
    ::LeaveCriticalSection(&CS_ComputersStateFile);
    // ��������������� ������� ��������
    for ( int i=0; i<NUM_SyncThreads; i++ )
    {
        ThreadNetSync[i]->Init(this); ThreadNetSync[i]->Pause(false);
    }

    return result;
}
//---------------------------------------------------------------------------
void MComputers::Get(MCompParams *CompParams_)
{
    CompParams_->Clear();
    for ( MComputer *Computer=(MComputer*)FirstItem; Computer;
        Computer=(MComputer*)Computer->NextItem )
    {
        MCompParam *NewCompParam;
        NewCompParam=(MCompParam*)CompParams_->Add();
        NewCompParam->ID=Computer->ID;
        NewCompParam->Address=Computer->Address;
        NewCompParam->Number=Computer->Number;
        NewCompParam->GroupColor=Computer->GroupColor;
    }
}
//---------------------------------------------------------------------------
void MComputers::Set(MCompParams *CompParams_)
{
    unsigned int NewID=0;
    MComputer *CurrentComputer, *NewComputer;

    // ������������� ������� ��������
    for ( int i=0; i<NUM_SyncThreads; i++ ) ThreadNetSync[i]->Pause(true);
    //
    for ( MComputer *Computer=(MComputer*)FirstItem; Computer;
        Computer=(MComputer*)Computer->NextItem ) if ( NewID<Computer->ID ) NewID=Computer->ID;
    NewID++;
    // ��������� ����� ��������� ����������� � �������� ������������
    CurrentComputer=(MComputer*)FirstItem;
    for ( MCompParam *CompParam=(MCompParam*)CompParams_->FirstItem; CompParam;
        CompParam=(MCompParam*)CompParam->NextItem )
    {
        // ���� ���������� � ����� ID-������� ��� ���, �� ��������� ���
        if ( (NewComputer=SearchID(CompParam->ID))==NULL )
        {
            NewComputer=(MComputer*)Add();
            NewComputer->ID=NewID; NewID++;
        }
        // ��������� ������������ �������� ���������� �����������
        if ( (NewComputer!=CurrentComputer)&&(CurrentComputer!=NULL) )
            Exchange(NewComputer,CurrentComputer);
        CurrentComputer=(MComputer*)NewComputer->NextItem;
        NewComputer->Address=CompParam->Address;
        NewComputer->Number=CompParam->Number;
        NewComputer->GroupColor=CompParam->GroupColor;
    }
    // ������� ������ ��������� �����������
    while(CurrentComputer)
    {
        NewComputer=(MComputer*)CurrentComputer->NextItem;
        Delete(CurrentComputer); CurrentComputer=NewComputer;
    }
    //
    Save();
    // ��������������� ������� ��������
    for ( int i=0; i<NUM_SyncThreads; i++ )
    {
        ThreadNetSync[i]->Init(this); ThreadNetSync[i]->Pause(false);
    }
}
//---------------------------------------------------------------------------
void MComputers::Timer()
{
    for ( MComputer *Computer=(MComputer*)FirstItem; Computer;
        Computer=(MComputer*)Computer->NextItem ) Computer->Timer();
    for ( int i=0; i<NUM_SyncThreads; i++ ) ThreadNetSync[i]->Timer();
}
//---------------------------------------------------------------------------
int MComputers::GetPosition()
{
    int position=0;
    for ( int i=0; i<NUM_SyncThreads; i++ ) position+=ThreadNetSync[i]->GetPosition();
    return position/NUM_SyncThreads;
}
//---------------------------------------------------------------------------
MComputers::MComputers()
{
    constructor();
    for ( int i=0; i<NUM_SyncThreads; i++ )
    {
        ThreadNetSync[i]=new MThreadNetSync(true);
        ThreadNetSync[i]->Priority=tpHigher;
        ThreadNetSync[i]->FreeOnTerminate=true;
        ThreadNetSync[i]->Init(this);
        ThreadNetSync[i]->Pause(false);
        ThreadNetSync[i]->Resume();
    }
}
//---------------------------------------------------------------------------
MComputers::~MComputers()
{
    for ( int i=0; i<NUM_SyncThreads; i++ ) ThreadNetSync[i]->Exit();
    destructor();
}
//---------------------------------------------------------------------------

