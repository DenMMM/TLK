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
            "�� ������� ��������� ��������� �����. ��������� ���������� ID �� ��������."
            );
    }
//    id=::GetTickCount();                // ����� �����������
//    id<<=16;
    id=time/(60*10000000i64);           // 16 ��� ����� ������ �� >30 ���
    id<<=8;                             // 256 ���������� ID � ������ ����������
    id++;                               // '0' �� ��������

    return id;
}

unsigned MIDList::NextID(unsigned LastID_)
{
    // ���������� ID (��������� '0') ���� �� ������ ���������
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

    // ���� ��� �� �������� ID, ���������� ��������� ����������
    id=(LastID!=0)? LastID: FirstID();

    // �������� ������ � ������ ID ��� �� ���
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

