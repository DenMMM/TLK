﻿//---------------------------------------------------------------------------
#ifndef UnitIDListH
#define UnitIDListH
//---------------------------------------------------------------------------
#include <cstdint>

#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
class MIDListItem;

template <
	typename parent_item, typename base_type,
	typename item_type, std::uint8_t type_id>
using MIDListItemTyped = MSLListItemTyped <parent_item, base_type, item_type, type_id>;

template <typename parent_item, typename item_type>
using MIDListItemSimple = MSLListItemSimple <parent_item, item_type>;

template <typename list_type, typename base_type>
class MIDList;

template <typename parent_list, typename item_type>
using MIDListSimple = MSLListSimple <parent_list, item_type>;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <
	typename list_type,			// Имя будущего класса-списка
	typename base_type>			// Базовый (родительский) тип его элементов
class MIDListItem:
	public MSLListItem <list_type, base_type>
{
	friend MIDList <list_type, base_type>;

protected:
	std::uint32_t UUID;

public:
	// Функции механизма сохранения/загрузки данных
	virtual std::size_t GetDataSize() const override
		{ return sizeof(UUID); }
	virtual void *SetData(void *Data_) const override
		{ return MemSet(Data_,UUID); }
	virtual const void *GetData(const void *Data_, const void *Limit_) override
		{ return MemGet(Data_,&UUID,Limit_); }

public:
	std::uint32_t gUUID() const noexcept { return UUID; }

	MIDListItem():
		UUID(0)
	{
	}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <
	typename list_type,			// Создаваемый класс списка (имя)
	typename base_type>			// Базовый тип элемента списка
class MIDList:
	public MSLList <list_type, base_type>
{
private:
	std::uint32_t LastUUID;

	static std::uint32_t FirstUUID();
	std::uint32_t NextUUID(std::uint32_t LastUUID_) const;

public:
	// Функции механизма сохранения/загрузки данных
	virtual std::size_t GetDataSize() const override
		{ return sizeof(LastUUID); }
	virtual void *SetData(void *Data_) const override
		{ return MemSet(Data_,LastUUID); }
	virtual const void *GetData(const void *Data_, const void *Limit_) override
		{ return MemGet(Data_,&LastUUID,Limit_); }

protected:
	using MSLList <list_type, base_type> ::const_cast_iter;

public:
	using MSLList <list_type, base_type> ::begin;
	using MSLList <list_type, base_type> ::end;
	typedef typename MSLList <list_type, base_type> ::iterator iterator;
	typedef typename MSLList <list_type, base_type> ::const_iterator const_iterator;

	const_iterator SrchUUID(std::uint32_t UUID_) const;
	iterator SrchUUID(std::uint32_t UUID_)
	{
		const auto *const_this=this;
		return const_cast_iter(const_this->SrchUUID(UUID_));
	}

	void SetUUIDs();

	MIDList():
		LastUUID(0)
	{
	}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
std::uint32_t MIDList<list_type,base_type>::FirstUUID()
{
	std::int64_t time;
	std::uint32_t id;

	if ( !GetLocalTimeInt64(time) )
	{
		throw std::runtime_error (
			"MIDList::FirstUUID()"
			"Не удалось запросить системное время. Генерация уникальных ID не возможна.");
	}
//    id=::GetTickCount();                // чуток случайности
//    id<<=16;
	id=time/(60*10000000i64);           // 16 млн минут хватит на >30 лет
	id<<=8;                             // 256 уникальных ID в минуту достаточно
	id++;                               // '0' не положено

	return id;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
std::uint32_t MIDList<list_type,base_type>::NextUUID(std::uint32_t LastUUID_) const
{
	auto iEnd=end();
	// Перебираем ID (пропуская '0') пока не найдем свободный
	do { if ( (++LastUUID_)==0 ) continue; }
	while ( SrchUUID(LastUUID_)!=iEnd );

	return LastUUID_;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
typename MIDList<list_type,base_type>::const_iterator
	MIDList<list_type,base_type>::SrchUUID(std::uint32_t UUID_) const
{
	auto iItem=begin();
	auto iEnd=end();

	while ( iItem!=iEnd )
	{
		if ( iItem->UUID==UUID_ ) break;
		++iItem;
	}

	return iItem;
}
//---------------------------------------------------------------------------
/*
template <typename list_type, typename base_type>
typename MIDList<list_type,base_type>::iterator
	MIDList<list_type,base_type>::SrchUUID(std::uint32_t UUID_)
{
	auto iConst=const_cast<const MIDList*>(this)->SrchUUID(UUID_);
	const auto *pConst=static_cast<const MIDListItem<list_type,base_type>*>(&*iConst);
	return iterator(const_cast<MIDListItem<list_type,base_type>*>(pConst), this);
}

template <typename list_type, typename base_type>
auto MIDList<list_type,base_type>::SrchUUID(std::uint32_t UUID_)
{
	const MIDList* ConstThis=this;
	const auto ConstRes=ConstThis->SrchUUID(UUID_);
	return iterator(
		reinterpret_cast<base_type*>(&*ConstRes)		/// хак ?
		);
}
*/
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
void MIDList<list_type,base_type>::SetUUIDs()
{
	std::uint32_t uuid;

	// Если еще не задавали ID, генерируем начальное значенение
	uuid=(LastUUID!=0)? LastUUID: FirstUUID();

	// Проходим список и задаем ID где их нет
	auto iItem=begin();
	auto iEnd=end();

	while ( iItem!=iEnd )
	{
		if ( iItem->UUID==0 )
		{
			uuid=NextUUID(uuid);
			iItem->UUID=uuid;
		}
		++iItem;
	}

	LastUUID=uuid;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
