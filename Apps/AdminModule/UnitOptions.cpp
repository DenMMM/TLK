//---------------------------------------------------------------------------
#include <string.h>
#pragma hdrstop

#include "UnitOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MOptions::MOptions()
{
    LogPeriod=mlpDay;
    FilterFreeTime=2;
    CostDialogTime=15;
    CostPrecision=0.50;
    UsersRights=murPause;
}

MOptions::~MOptions()
{
//
}

bool MOptions::Copy(MOptions *Opt_)
{
    LogPeriod=Opt_->LogPeriod;
    FilterFreeTime=Opt_->FilterFreeTime;
    CostDialogTime=Opt_->CostDialogTime;
    CostPrecision=Opt_->CostPrecision;
    UsersRights=Opt_->UsersRights;
    Pass.Copy(&Opt_->Pass);
    return true;
}

unsigned MOptions::GetDataSize() const
{
    return
        sizeof(LogPeriod)+
        sizeof(FilterFreeTime)+
        sizeof(CostDialogTime)+
        sizeof(CostPrecision)+
        sizeof(UsersRights)+
        Pass.GetDataSize();
}

char *MOptions::SetData(char *Data_) const
{
    Data_=MemSet(Data_,LogPeriod);
    Data_=MemSet(Data_,FilterFreeTime);
    Data_=MemSet(Data_,CostDialogTime);
    Data_=MemSet(Data_,CostPrecision);
    Data_=MemSet(Data_,UsersRights);
    Data_=Pass.SetData(Data_);
    return Data_;
}

const char *MOptions::GetData(const char *Data_, const char *Limit_)
{
    return
        (Data_=MemGet(Data_,&LogPeriod,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&FilterFreeTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&CostDialogTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&CostPrecision,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&UsersRights,Limit_))!=NULL &&
        (Data_=Pass.GetData(Data_,Limit_))!=NULL
        ? Data_: NULL;
}
//---------------------------------------------------------------------------

