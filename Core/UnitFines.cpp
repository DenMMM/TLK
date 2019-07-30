//---------------------------------------------------------------------------
#include <string.h>
#include <mem.h>
#pragma hdrstop

#include "UnitFines.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void MFinesItem::Copy(const MListItem *SrcItem_)
{
	auto fine=dynamic_cast<const MFinesItem*>(SrcItem_);

	Descr=fine->Descr;
	Time=fine->Time;

	MIDListItem::Copy(SrcItem_);
}

unsigned MFinesItem::GetDataSize() const
{
	return
		MIDListItem::GetDataSize()+				// ID-����� ������
		sizeofLine(Descr)+						// �������� ������
		sizeof(Time);                   		// ����� ������
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

