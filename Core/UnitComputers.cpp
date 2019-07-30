//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitComputers.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void MComputersItem::Copy(const MListItem *SrcItem_)
{
	auto comp=dynamic_cast<const MComputersItem*>(SrcItem_);

    Number=comp->Number;
    Color=comp->Color;
    Address=comp->Address;
    NotUsed=comp->NotUsed;
}

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
	MComputersItem *Computer=gFirst();
	while(Computer)
	{
		if ( Computer->Number==Number_ ) break;
		Computer=Computer->gNext();
	}
	return Computer;
}

void MComputers::Sort()
{
    MComputersItem *Computer, *NextComputer;
    bool Sorted;

    if ( gFirst()==nullptr ) return;

    do
    {
		Sorted=true; Computer=gFirst();
		while((NextComputer=Computer->gNext())!=nullptr)
        {
            if ( Computer->Number>NextComputer->Number )
            {
                Swap(Computer,NextComputer);
                Sorted=false;
            } else Computer=NextComputer;
        }
    } while(!Sorted);
}
//---------------------------------------------------------------------------

