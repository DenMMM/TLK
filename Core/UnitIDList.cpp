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

unsigned MIDList::NextUUID(unsigned LastUUID_)
{
    // Перебираем ID (пропуская '0') пока не найдем свободный
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

	// Если еще не задавали ID, генерируем начальное значенение
	uuid=(LastUUID!=0)? LastUUID: FirstUUID();

	// Проходим список и задаем ID где их нет
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

