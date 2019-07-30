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
    *Descr=0;
    Time=0;
}

MFine::~MFine()
{
//
}

void MFine::Copy(const MListItem *SrcItem_)
{
    MFine *Fine_=(MFine*)SrcItem_;
    strcpy(Descr,Fine_->Descr);
    Time=Fine_->Time;
    MIDListItem::Copy(SrcItem_);
}

unsigned MFine::GetDataSize() const
{
    return
        MIDListItem::GetDataSize()+     // ID-номер штрафа
        strlen(Descr)+1+                // Описание штрафа
        sizeof(Time);                   // Время штрафа
}

char *MFine::SetData(char *Data_) const
{
    Data_=MIDListItem::SetData(Data_);
    Data_=MemSetCLine(Data_,Descr);
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

char *MFine::sDescr(char *Descr_)
{
    return strlen(Descr_)>MAX_FineDescrLen?
        NULL: strcpy(Descr,Descr_);
}

void MFine::GetFineData(MFineData *Data_) const
{
    Data_->ID=ItemID;
    strcpy(Data_->Descr,Descr);
}

void MFine::SetFineData(MFineData *Data_)
{
    ItemID=Data_->ID;
    strcpy(Descr,Data_->Descr);
}

//---------------------------------------------------------------------------

