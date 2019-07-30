//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MClOptions::MClOptions()
{
    ToEndTime=2;
    MessageTime=15;
    MsgEndTime=10;
    RebootWait=20;
    AutoLockTime=15;
    Flags=0;
}

MClOptions::~MClOptions()
{
//
}

bool MClOptions::Copy(MClOptions *ClOptions_)
{
    ToEndTime=ClOptions_->ToEndTime;
    MessageTime=ClOptions_->MessageTime;
    MsgEndTime=ClOptions_->MsgEndTime;
    RebootWait=ClOptions_->RebootWait;
    AutoLockTime=ClOptions_->AutoLockTime;
    Flags=ClOptions_->Flags;
    return true;
}

unsigned MClOptions::GetDataSize() const
{
    return
        sizeof(ToEndTime)+
        sizeof(MessageTime)+
        sizeof(MsgEndTime)+
        sizeof(RebootWait)+
        sizeof(AutoLockTime)+
        sizeof(Flags);
}

char *MClOptions::SetData(char *Data_) const
{
    Data_=MemSet(Data_,ToEndTime);
    Data_=MemSet(Data_,MessageTime);
    Data_=MemSet(Data_,MsgEndTime);
    Data_=MemSet(Data_,RebootWait);
    Data_=MemSet(Data_,AutoLockTime);
    Data_=MemSet(Data_,Flags);
    return Data_;
}

const char *MClOptions::GetData(const char *Data_, const char *Limit_)
{
    return
        (Data_=MemGet(Data_,&ToEndTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&MessageTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&MsgEndTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&RebootWait,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&AutoLockTime,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&Flags,Limit_))!=NULL
        ? Data_: NULL;
}
//---------------------------------------------------------------------------
