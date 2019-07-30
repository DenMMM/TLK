//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitIDList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
unsigned MIDList::FirstUUID()
{
	__int64 time;
	unsigned id;

	if ( !GetLocalTimeInt64(&time) )
	{
		throw std::runtime_error (
            "MIDList::FirstUUID()"
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

unsigned MIDList::NextUUID(unsigned LastUUID_)
{
    // ���������� ID (��������� '0') ���� �� ������ ���������
	do { if ( (++LastUUID_)==0 ) continue; }
    while ( SrchUUID(LastUUID_)!=nullptr );

    return LastUUID_;
}

MIDListItem *MIDList::SrchUUID(unsigned UUID_) const
{
	MIDListItem *Item=gFirst();

	while(Item)
	{
		if ( Item->UUID==UUID_ ) break;
		Item=Item->gNext();
	}

	return Item;
}

void MIDList::SetUUIDs()
{
	unsigned uuid;

	// ���� ��� �� �������� ID, ���������� ��������� ����������
	uuid=(LastUUID!=0)? LastUUID: FirstUUID();

	// �������� ������ � ������ ID ��� �� ���
	MIDListItem *Item=gFirst();
	while(Item)
	{
		if ( Item->UUID==0 )
		{
			uuid=NextUUID(uuid);
			Item->UUID=uuid;
		}
		Item=Item->gNext();
	}

	LastUUID=uuid;
}
//---------------------------------------------------------------------------

