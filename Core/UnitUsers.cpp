//---------------------------------------------------------------------------
#include <string.h>
#pragma hdrstop

#include "UnitUsers.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
std::size_t MUsersItem::GetDataSize() const
{
	return
		sizeofLine(Login)+
		sizeofLine(Name)+
		sizeof(Active)+
        Pass.GetDataSize()+
		MIDListItem::GetDataSize();     		// ID-номер пользователя
}

void *MUsersItem::SetData(void *Data_) const
{
	Data_=MIDListItem::SetData(Data_);

	Data_=MemSetLine(Data_,Login);
	Data_=MemSetLine(Data_,Name);
    Data_=Pass.SetData(Data_);
    Data_=MemSet(Data_,Active);

	return Data_;
}

const void *MUsersItem::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MIDListItem::GetData(Data_,Limit_)) &&
		(Data_=MemGetLine(Data_,Login,MAX_UserLoginLen,Limit_)) &&
		(Data_=MemGetLine(Data_,Name,MAX_UserNameLen,Limit_)) &&
		(Data_=Pass.GetData(Data_,Limit_)) &&
		(Data_=MemGet(Data_,&Active,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
std::size_t MUsers::ActiveCount() const
{
	std::size_t count=0;

	for ( auto &User: *this )
	{
		if ( User.Active ) count++;
	}

	return count;
}
//---------------------------------------------------------------------------

