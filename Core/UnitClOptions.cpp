//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MClOptions::MClOptions()
{
    ToEndTime=0;
    MessageTime=0;
    RebootWait=0;
    AutoLockTime=0;
}

MClOptions::~MClOptions()
{
//
}

bool MClOptions::Copy(MClOptions *ClOptions_)
{
    ToEndTime=ClOptions_->ToEndTime;
    MessageTime=ClOptions_->MessageTime;
    RebootWait=ClOptions_->RebootWait;
    AutoLockTime=ClOptions_->AutoLockTime;
    return true;
}

unsigned MClOptions::GetDataSize() const
{
    return
        sizeof(ToEndTime)+
        sizeof(MessageTime)+
        sizeof(RebootWait)+
        sizeof(AutoLockTime);
}

char *MClOptions::SetData(char *Data_) const
{
    Data_=MemSet(Data_,ToEndTime);
    Data_=MemSet(Data_,MessageTime);
    Data_=MemSet(Data_,RebootWait);
    Data_=MemSet(Data_,AutoLockTime);
    return Data_;
}

const char *MClOptions::GetData(const char *Data_, const char *Limit_)
{
    return
        (Data_=MemGet(Data_,&ToEndTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&MessageTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&RebootWait,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&AutoLockTime,Limit_))!=NULL
        ? Data_: NULL;
}
//---------------------------------------------------------------------------
