//---------------------------------------------------------------------------
#include <string.h>
#include <mem.h>
#pragma hdrstop

#include "UnitFines.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

MFine::MFine()
{
    Time=0;
}

MFine::~MFine()
{
//
}

void MFine::Copy(const MListItem *SrcItem_)
{
    const MFine *fine=
        dynamic_cast<const MFine*>(SrcItem_);

    Descr=fine->Descr;
    Time=fine->Time;
    MIDListItem::Copy(SrcItem_);
}

unsigned MFine::GetDataSize() const
{
    return
        MIDListItem::GetDataSize()+     // ID-номер штрафа
        Descr.length()+sizeof('\0')+    // Описание штрафа
        sizeof(Time);                   // Время штрафа
}

char *MFine::SetData(char *Data_) const
{
    Data_=MIDListItem::SetData(Data_);
    Data_=MemSetCLine(Data_,Descr.c_str());
    Data_=MemSet(Data_,Time);
    return Data_;
}

const char *MFine::GetData(const char *Data_, const char *Limit_)
{
    return
        (Data_=MIDListItem::GetData(Data_,Limit_))!=NULL &&
        (Data_=MemGetCLine(Data_,Descr,MAX_FineDescrLen,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&Time,Limit_))!=NULL
        ? Data_: NULL;
}
//---------------------------------------------------------------------------

