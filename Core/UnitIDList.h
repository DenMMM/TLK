//---------------------------------------------------------------------------
#ifndef UnitIDListH
#define UnitIDListH
//---------------------------------------------------------------------------
class MIDListItem;
class MIDList;
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
class MIDListItem:public MSLListItem
{
protected:
    unsigned ItemID;

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const
        { return sizeof(ItemID); }
    char *SetData(char *Data_) const
        { return MemSet(Data_,ItemID); }
    const char *GetData(const char *Data_, const char *Limit_)
        { return MemGet(Data_,&ItemID,Limit_); }

    friend MIDList;

public:
    void Copy(const MListItem *SrcItem_)
    {
        MIDListItem *Item_=(MIDListItem*)SrcItem_;
        ItemID=Item_->ItemID;
    }

    unsigned gItemID() const { return ItemID; }

    MIDListItem() { ItemID=0; }
};

class MIDList:public MSLList
{
private:
    unsigned LastID;

    unsigned FirstID();
    unsigned NextID(unsigned LastID_);

protected:
    unsigned GetDataSize() const
        { return sizeof(LastID); }
    char *SetData(char *Data_) const
        { return MemSet(Data_,LastID); }
    const char *GetData(const char *Data_, const char *Limit_)
        { return MemGet(Data_,&LastID,Limit_); }

public:
    MIDListItem *SrchID(unsigned ID_) const;
    void SetIDs();

    MIDList::MIDList() { LastID=0; }
    MIDList::~MIDList() { Clear(); }
};
//---------------------------------------------------------------------------
#endif
