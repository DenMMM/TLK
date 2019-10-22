//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitComputers.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
unsigned MComputersItem::GetDataSize() const
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
MComputersItem *MComputers::Search(char Number_) const
{
	auto iComputer=cbegin();
	auto iEnd=cend();

	while ( iComputer!=iEnd )
	{
		if ( iComputer->Number==Number_ ) break;
		++iComputer;
	}

	return &(*iComputer);
}
//---------------------------------------------------------------------------

