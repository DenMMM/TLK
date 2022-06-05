//---------------------------------------------------------------------------
#include <string.h>
#pragma hdrstop

#include "UnitOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
std::size_t MOptions::GetDataSize() const
{
	return
		sizeof(LogPeriod)+
		sizeof(FilterFreeTime)+
		sizeof(CostDialogTime)+
		sizeof(CostPrecision)+
		sizeof(UsersRights)+
		Pass.GetDataSize();
}

void *MOptions::SetData(void *Data_) const
{
    Data_=MemSet(Data_,LogPeriod);
    Data_=MemSet(Data_,FilterFreeTime);
    Data_=MemSet(Data_,CostDialogTime);
    Data_=MemSet(Data_,CostPrecision);
    Data_=MemSet(Data_,UsersRights);
    Data_=Pass.SetData(Data_);
    return Data_;
}

const void *MOptions::GetData(const void *Data_, const void *Limit_)
{
    return
        (Data_=MemGet(Data_,&LogPeriod,Limit_)) &&
        (Data_=MemGet(Data_,&FilterFreeTime,Limit_)) &&
        (Data_=MemGet(Data_,&CostDialogTime,Limit_)) &&
        (Data_=MemGet(Data_,&CostPrecision,Limit_)) &&
        (Data_=MemGet(Data_,&UsersRights,Limit_)) &&
		(Data_=Pass.GetData(Data_,Limit_))
        ? Data_: nullptr;
}
//---------------------------------------------------------------------------

