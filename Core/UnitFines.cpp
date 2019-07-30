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
    ID=0;
    *Description=0;
    Time=0;
}

MFine::~MFine()
{
//
}

bool MFine::Copy(MListItem *SrcItem_)
{
    MFine *Fine_=(MFine*)SrcItem_;
    ID=Fine_->ID;
    strcpy(Description,Fine_->Description);
    Time=Fine_->Time;
    return true;
}

unsigned int MFine::GetDataSize()
{
    return
        sizeof(ID)+             // ID-номер штрафа
        strlen(Description)+1+  // Описание штрафа
        sizeof(Time);           // Время штрафа
}

char *MFine::SetData(char *Data_)
{
    Data_=MemSet(Data_,ID);
    Data_=MemSetCLine(Data_,Description);
    Data_=MemSet(Data_,Time);
    return Data_;
}

char *MFine::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&ID,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetCLine(Data_,Description,MAX_FineDescLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Time,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

char *MFine::SetDescription(char *Description_)
{
    return strlen(Description_)>MAX_FineDescLength?
        NULL: strcpy(Description,Description_);
}

void MFine::GetFineData(MFineData *Data_)
{
    Data_->ID=ID;
    strcpy(Data_->Description,Description);
}

void MFine::SetFineData(MFineData *Data_)
{
    ID=Data_->ID;
    strcpy(Description,Data_->Description);
}

//---------------------------------------------------------------------------

MFine *MFines::Search(unsigned ID_)
{
    MFine *Fine=(MFine*)First;
    while(Fine)
    {
        if ( Fine->ID==ID_ ) break;
        Fine=(MFine*)Fine->Next;
    }
    return Fine;
}

//---------------------------------------------------------------------------

