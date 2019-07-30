//---------------------------------------------------------------------------
#include <string.h>
#include <mem.h>
#pragma hdrstop

#include "UnitComputers.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

MComputer::MComputer()
{
    *Address=0;
    Number=0;
    GroupColor=mgcNone;
    NotUsed=false;
}

MComputer::~MComputer()
{
//
}

bool MComputer::Copy(MListItem *SrcItem_)
{
    MComputer *Computer_=(MComputer*)SrcItem_;
    strcpy(Address,Computer_->Address);
    Number=Computer_->Number;
    GroupColor=Computer_->GroupColor;
    NotUsed=Computer_->NotUsed;
    return true;
}

unsigned int MComputer::GetDataSize()
{
    return
        sizeof(Number)+
        strlen(Address)+1+
        sizeof(GroupColor)+
        sizeof(NotUsed);
}

char *MComputer::SetData(char *Data_)
{
    Data_=MemSet(Data_,Number);
    Data_=MemSetCLine(Data_,Address);
    Data_=MemSet(Data_,GroupColor);
    Data_=MemSet(Data_,NotUsed);
    return Data_;
}

char *MComputer::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&Number,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetCLine(Data_,Address,MAX_ComputerAddressLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&GroupColor,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&NotUsed,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

char *MComputer::SetAddress(char *Address_)
{
    return strlen(Address_)>MAX_ComputerAddressLength?
        NULL: strcpy(Address,Address_);
}

//---------------------------------------------------------------------------

MComputer *MComputers::Search(int Number_)
{
    MComputer *Computer=(MComputer*)First;
    while(Computer)
    {
        if ( Computer->Number==Number_ ) break;
        Computer=(MComputer*)Computer->Next;
    }
    return Computer;
}

void MComputers::Sort()
{
    MComputer *Computer, *NextComputer;
    bool Sorted;

    if ( First==NULL ) return;

    do
    {
        Sorted=true; Computer=(MComputer*)First;
        while((NextComputer=(MComputer*)Computer->Next)!=NULL)
        {
            if ( Computer->Number>NextComputer->Number )
            {
                Exchange(Computer,NextComputer);
                Sorted=false;
            } else Computer=NextComputer;
        }
    } while(!Sorted);
}

//---------------------------------------------------------------------------

