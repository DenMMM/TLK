//---------------------------------------------------------------------------
#ifndef UnitListH
#define UnitListH
//---------------------------------------------------------------------------
class MListItem;
class MList;
//---------------------------------------------------------------------------
class MListItem
{
private:
    MListItem *Prev;    // ��������� �� ���������� ������� ������
    MListItem *Next;    // ��������� �� ��������� ������� ������
    friend class MList; // �������� MList �������� � MListItem ��������

public:
    // ������ � ��������� ������
    MListItem* gPrev() const { return Prev; }
    MListItem* gNext() const { return Next; }

    virtual unsigned char gTypeID() const { return 0; }
    virtual void Copy(const MListItem *SrcItem_)=0;

    MListItem()
    {
        Prev=Next=NULL;
    }
};
//---------------------------------------------------------------------------
class MList
{
private:
    MListItem *First;   // ��������� �� ������ ������� ������
    MListItem *Last;    // ��������� �� ��������� ������� ������
    unsigned Count;     // ���������� ��������� � ������

    // ��������� 'new' � 'delete' ��� ������������ �� MListItem ������
    virtual MListItem *item_new(unsigned char TypeID_) const=0;
    virtual void item_del(MListItem *Item_) const=0;

protected:
    // ���������� ����� �� �������� ������ ���� ������ �����
    // ��� 'true' MListItem::TypeID() ������ ���� ��������������
    virtual bool Typed() const { return false; }

public:
    // ������ � ��������� ������
    MListItem *gFirst() const { return First; }
    MListItem *gLast() const { return Last; }
    unsigned gCount() const { return Count; }

    // �������� ��� ���������� ���������� _������_ ������
    MListItem *Add(unsigned char TypeID_=0);
    MListItem *Item(unsigned Number_) const;
    void Exch(MListItem *Item1_, MListItem *Item2_);
    void Del(MListItem *DelItem_);

    // �������� ��� ������� ������� (�� ������ �������� ���������� MList)
    void Clear();                       // ������� ��� �������� ������
    void Copy(const MList *SrcList_);   // ������� ����� ��������� ��������� ������
    void Move(MList *SrcList_);         // ��������� �������� ������ ���������
    void Attach(MList *AtchList_);      // ������������ �������� ��������� ������

    MList()
    {
        First=Last=NULL;
        Count=0;
    }
//    ~MList()
//    {
// �������� � ����������� ������, ��� ���������� �-� item_del(),
// �.�. � ������� ������ ~MList() ������� � ����� ��������� ��� ���.
// ����� �������� ������ �� ����� ������� - memory leak !

//    Clear();
//    }
};
//---------------------------------------------------------------------------
#endif

