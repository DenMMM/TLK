//---------------------------------------------------------------------------
#include <string.h>
#pragma hdrstop

#include "UnitOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

MOptions::MOptions()
{
    constructor();
    *Password=0;
    LogPeriod=mlpMonth;
    CostDialogTime=15;
    CostPrecision=0.50;
    FilterFreeTime=2;
    UsersRights=murPause;
}

MOptions::~MOptions()
{
    destructor();
}

bool MOptions::Copy(MOptions *Options_)
{
    strcpy(Password,Options_->Password);
    LogPeriod=Options_->LogPeriod;
    CostDialogTime=Options_->CostDialogTime;
    CostPrecision=Options_->CostPrecision;
    FilterFreeTime=Options_->FilterFreeTime;
    UsersRights=Options_->UsersRights;
    return true;
}

unsigned MOptions::GetDataSize()
{
    return
        strlen(Password)+1+
        sizeof(LogPeriod)+
        sizeof(CostDialogTime)+
        sizeof(CostPrecision)+
        sizeof(FilterFreeTime)+
        sizeof(UsersRights);
}

char *MOptions::SetData(char *Data_)
{
    Data_=MemSetCLine(Data_,Password);
    Data_=MemSet(Data_,LogPeriod);
    Data_=MemSet(Data_,CostDialogTime);
    Data_=MemSet(Data_,CostPrecision);
    Data_=MemSet(Data_,FilterFreeTime);
    Data_=MemSet(Data_,UsersRights);
    return Data_;
}

char *MOptions::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGetCLine(Data_,Password,MAX_OptionsPasswordLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&LogPeriod,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&CostDialogTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&CostPrecision,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&FilterFreeTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&UsersRights,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

bool MOptions::SetPassword(char *Password_)
{
    return strlen(Password_)>MAX_OptionsPasswordLength?
        false: (bool)strcpy(Password,Password_);
}

bool MOptions::CheckPassword(char *Password_)
{
    return strcmp(Password,Password_)==0;
}

//---------------------------------------------------------------------------

