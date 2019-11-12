//---------------------------------------------------------------------------
#ifndef UnitIDListH
#define UnitIDListH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
class MIDListItem;

template <
	typename parent_item, typename base_type,
	typename item_type, unsigned char type_id>
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
	typename list_type,			// ��� �������� ������-������
	typename base_type>			// ������� (������������) ��� ��� ���������
class MIDListItem:
	public MSLListItem <list_type, base_type>
{
	friend MIDList <list_type, base_type>;

protected:
	unsigned UUID;

public:
	// ������� ��������� ����������/�������� ������
	virtual unsigned GetDataSize() const override
		{ return sizeof(UUID); }
	virtual void *SetData(void *Data_) const override
		{ return MemSet(Data_,UUID); }
	virtual const void *GetData(const void *Data_, const void *Limit_) override
		{ return MemGet(Data_,&UUID,Limit_); }

public:
	unsigned gUUID() const noexcept { return UUID; }

	MIDListItem():
		UUID(0)
	{
	}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <
	typename list_type,			// ����������� ����� ������ (���)
	typename base_type>			// ������� ��� �������� ������
class MIDList:
	public MSLList <list_type, base_type>
{
private:
	unsigned LastUUID;

	static unsigned FirstUUID();
	unsigned NextUUID(unsigned LastUUID_) const;

public:
	// ������� ��������� ����������/�������� ������
	virtual unsigned GetDataSize() const override
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

	const_iterator SrchUUID(unsigned UUID_) const;
	iterator SrchUUID(unsigned UUID_)
	{
		return const_cast_iter(
			const_cast<const MIDList*>(this)->SrchUUID(UUID_)
			);
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
unsigned MIDList<list_type,base_type>::FirstUUID()
{
	__int64 time;
	unsigned id;

	if ( !GetLocalTimeInt64(&time) )
	{
		throw std::runtime_error (
			"MIDList::FirstUUID()"
			"�� ������� ��������� ��������� �����. ��������� ���������� ID �� ��������.");
	}
//    id=::GetTickCount();                // ����� �����������
//    id<<=16;
	id=time/(60*10000000i64);           // 16 ��� ����� ������ �� >30 ���
	id<<=8;                             // 256 ���������� ID � ������ ����������
	id++;                               // '0' �� ��������

	return id;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
unsigned MIDList<list_type,base_type>::NextUUID(unsigned LastUUID_) const
{
	auto iEnd=end();
	// ���������� ID (��������� '0') ���� �� ������ ���������
	do { if ( (++LastUUID_)==0 ) continue; }
	while ( SrchUUID(LastUUID_)!=iEnd );

	return LastUUID_;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
typename MIDList<list_type,base_type>::const_iterator
	MIDList<list_type,base_type>::SrchUUID(unsigned UUID_) const
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
	MIDList<list_type,base_type>::SrchUUID(unsigned UUID_)
{
	auto iConst=const_cast<const MIDList*>(this)->SrchUUID(UUID_);
	const auto *pConst=static_cast<const MIDListItem<list_type,base_type>*>(&*iConst);
	return iterator(const_cast<MIDListItem<list_type,base_type>*>(pConst), this);
}

template <typename list_type, typename base_type>
auto MIDList<list_type,base_type>::SrchUUID(unsigned UUID_)
{
	const MIDList* ConstThis=this;
	const auto ConstRes=ConstThis->SrchUUID(UUID_);
	return iterator(
		reinterpret_cast<base_type*>(&*ConstRes)		/// ��� ?
		);
}
*/
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
void MIDList<list_type,base_type>::SetUUIDs()
{
	unsigned uuid;

	// ���� ��� �� �������� ID, ���������� ��������� ����������
	uuid=(LastUUID!=0)? LastUUID: FirstUUID();

	// �������� ������ � ������ ID ��� �� ���
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
