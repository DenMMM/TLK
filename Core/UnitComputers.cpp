﻿//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitComputers.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
std::size_t MComputersItem::GetDataSize() const
{
    return
		sizeof(Number)+
		sizeof(Color)+
		sizeofLine(Address)+
		sizeof(NotUsed);
}

void *MComputersItem::SetData(void *Data_) const
{
	Data_=MemSet(Data_,Number);
	Data_=MemSet(Data_,Color);
	Data_=MemSetLine(Data_,Address);
	Data_=MemSet(Data_,NotUsed);
	return Data_;
}

const void *MComputersItem::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&Number,Limit_)) &&
		(Data_=MemGet(Data_,&Color,Limit_)) &&
		(Data_=MemGetLine(Data_,Address,MAX_CompAddrLen,Limit_)) &&
		(Data_=MemGet(Data_,&NotUsed,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
MComputers::const_iterator MComputers::Search(std::int8_t Number_) const
{
	auto iComputer=begin();
	auto iEnd=end();

	while ( iComputer!=iEnd )
	{
		if ( iComputer->Number==Number_ ) break;
		++iComputer;
	}

	return iComputer;
}
//---------------------------------------------------------------------------

