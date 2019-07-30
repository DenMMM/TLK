//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitIDList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
unsigned MIDList::FirstID()
{
    __int64 time;
    unsigned id;

    if ( !GetLocalTimeInt64(&time) )
    {
        throw std::runtime_error (
            "MIDList::FirstID()"
            "Не удалось запросить системное время. Генерация уникальных ID не возможна."
            );
    }
//    id=::GetTickCount();                // чуток случайности
//    id<<=16;
    id=time/(60*10000000i64);           // 16 млн минут хватит на >30 лет
    id<<=8;                             // 256 уникальных ID в минуту достаточно
    id++;                               // '0' не положено

    return id;
}

unsigned MIDList::NextID(unsigned LastID_)
{
    // Перебираем ID (пропуская '0') пока не найдем свободный
    do { if ( (++LastID_)==0 ) continue; }
    while ( SrchID(LastID_)!=NULL );

    return LastID_;
}

MIDListItem *MIDList::SrchID(unsigned ID_) const
{
    MIDListItem *Item=(MIDListItem*)gFirst();

    while(Item)
    {
        if ( Item->ItemID==ID_ ) break;
        Item=(MIDListItem*)Item->gNext();
    }

    return Item;
}

void MIDList::SetIDs()
{
    unsigned id;

    // Если еще не задавали ID, генерируем начальное значенение
    id=(LastID!=0)? LastID: FirstID();

    // Проходим список и задаем ID где их нет
    MIDListItem *Item=(MIDListItem*)gFirst();
    while(Item)
    {
        if ( Item->ItemID==0 )
        {
            id=NextID(id);
            Item->ItemID=id;
        }
        Item=(MIDListItem*)Item->gNext();
    }

    LastID=id;
}
//---------------------------------------------------------------------------

