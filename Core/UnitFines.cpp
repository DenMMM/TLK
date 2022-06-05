//---------------------------------------------------------------------------
#include <string.h>
//#include <mem.h>
#pragma hdrstop

#include "UnitFines.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
std::size_t MFinesItem::GetDataSize() const
{
	return
		MIDListItem::GetDataSize()+				// ID-номер штрафа
		sizeofLine(Descr)+						// Описание штрафа
		sizeof(Time);                   		// Время штрафа
}

void *MFinesItem::SetData(void *Data_) const
{
    Data_=MIDListItem::SetData(Data_);
	Data_=MemSetLine(Data_,Descr);
	Data_=MemSet(Data_,Time);

    return Data_;
}

const void *MFinesItem::GetData(const void *Data_, const void *Limit_)
{
    return
		(Data_=MIDListItem::GetData(Data_,Limit_)) &&
		(Data_=MemGetLine(Data_,Descr,MAX_FineDescrLen,Limit_)) &&
		(Data_=MemGet(Data_,&Time,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------

