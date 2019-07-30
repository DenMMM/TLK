//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

MClOptions::MClOptions()
{
    constructor();
    ToEndTime=0;
    MessageTime=0;
    RebootWait=0;
    AutoLockTime=0;
}

MClOptions::~MClOptions()
{
    destructor();
}

bool MClOptions::Copy(MClOptions *ClOptions_)
{
    ToEndTime=ClOptions_->ToEndTime;
    MessageTime=ClOptions_->MessageTime;
    RebootWait=ClOptions_->RebootWait;
    AutoLockTime=ClOptions_->AutoLockTime;
    return true;
}

unsigned MClOptions::GetDataSize()
{
    return
        sizeof(ToEndTime)+
        sizeof(MessageTime)+
        sizeof(RebootWait)+
        sizeof(AutoLockTime);
}

char *MClOptions::SetData(char *Data_)
{
    Data_=MemSet(Data_,ToEndTime);
    Data_=MemSet(Data_,MessageTime);
    Data_=MemSet(Data_,RebootWait);
    Data_=MemSet(Data_,AutoLockTime);
    return Data_;
}

char *MClOptions::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&ToEndTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&MessageTime,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&RebootWait,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&AutoLockTime,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

//---------------------------------------------------------------------------
