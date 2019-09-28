//---------------------------------------------------------------------------
#ifndef UnitIDListH
#define UnitIDListH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
class MIDListItem;
class MIDList;
//---------------------------------------------------------------------------
class MIDListItem:
	public MSLListItem::Proxy <MSLListItem, MIDListItem>
{
	friend MIDList;

protected:
	unsigned UUID;

public:
	// Функции механизма сохранения/загрузки данных
	virtual unsigned GetDataSize() const override
		{ return sizeof(UUID); }
	virtual void *SetData(void *Data_) const override
		{ return MemSet(Data_,UUID); }
	virtual const void *GetData(const void *Data_, const void *Limit_) override
		{ return MemGet(Data_,&UUID,Limit_); }

public:
	unsigned gUUID() const { return UUID; }

	virtual void Copy(const MListItem *SrcItem_) override
	{
		const MIDListItem *item=
			dynamic_cast<const MIDListItem*>(SrcItem_);
		UUID=item->UUID;
	}

	MIDListItem():
		UUID(0)
	{
	}

	virtual ~MIDListItem() override = default;
};
//---------------------------------------------------------------------------
class MIDList:
	public MSLList::Typed <MSLList, MIDList, MIDListItem>
{
private:
	unsigned LastUUID;

	unsigned FirstUUID();
	unsigned NextUUID(unsigned LastUUID_);

public:
	virtual unsigned GetDataSize() const override
		{ return sizeof(LastUUID); }
	virtual void *SetData(void *Data_) const override
		{ return MemSet(Data_,LastUUID); }
	virtual const void *GetData(const void *Data_, const void *Limit_) override
		{ return MemGet(Data_,&LastUUID,Limit_); }

public:
	MIDListItem *SrchUUID(unsigned UUID_) const;
	void SetUUIDs();

	MIDList():
		LastUUID(0)
	{
	}

	~MIDList() = default;


public:
	template <
		typename parent_list,
		typename list_type,
		typename base_type>
	class Typed: public MSLList::Typed <parent_list, list_type, base_type>
	{
	public:
		base_type* SrchUUID(unsigned UUID_) const {
			return static_cast<base_type*>(parent_list::SrchUUID(UUID_)); }

		Typed() = default;
		~Typed() = default;
	};

	template <
		typename parent_list,
		typename list_type,
		typename item_type>
	class Simple: public MSLList::Simple <parent_list, list_type, item_type>
	{
	public:
		item_type* SrchUUID(unsigned UUID_) const {
			return static_cast<item_type*>(parent_list::SrchUUID(UUID_)); }

		Simple() = default;
		~Simple() = default;
	};
};
//---------------------------------------------------------------------------
#endif
