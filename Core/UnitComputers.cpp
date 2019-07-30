//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitComputers.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MComputer::MComputer()
{
    Number=0;
    Color=mgcNone;
    NotUsed=false;
}

MComputer::~MComputer()
{
}

void MComputer::Copy(const MListItem *SrcItem_)
{
    const MComputer *comp=
        dynamic_cast<const MComputer*>(SrcItem_);

    Number=comp->Number;
    Color=comp->Color;
    Address=comp->Address;
    NotUsed=comp->NotUsed;
}

unsigned MComputer::GetDataSize() const
{
    return
        sizeof(Number)+
        sizeof(Color)+
        Address.length()+sizeof('\0')+
        sizeof(NotUsed);
}

char *MComputer::SetData(char *Data_) const
{
    Data_=MemSet(Data_,Number);
    Data_=MemSet(Data_,Color);
    Data_=MemSetCLine(Data_,Address.c_str());
    Data_=MemSet(Data_,NotUsed);
    return Data_;
}

const char *MComputer::GetData(const char *Data_, const char *Limit_)
{
    return
        (Data_=MemGet(Data_,&Number,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&Color,Limit_))!=NULL &&
        (Data_=MemGetCLine(Data_,Address,MAX_CompAddrLen,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&NotUsed,Limit_))!=NULL
        ? Data_: NULL;
}
//---------------------------------------------------------------------------
MComputer *MComputers::Search(char Number_) const
{
    MComputer *Computer=(MComputer*)gFirst();
    while(Computer)
    {
        if ( Computer->Number==Number_ ) break;
        Computer=(MComputer*)Computer->gNext();
    }
    return Computer;
}

void MComputers::Sort()
{
    MComputer *Computer, *NextComputer;
    bool Sorted;

    if ( gFirst()==NULL ) return;

    do
    {
        Sorted=true; Computer=(MComputer*)gFirst();
        while((NextComputer=(MComputer*)Computer->gNext())!=NULL)
        {
            if ( Computer->Number>NextComputer->Number )
            {
                Exch(Computer,NextComputer);
                Sorted=false;
            } else Computer=NextComputer;
        }
    } while(!Sorted);
}
//---------------------------------------------------------------------------

