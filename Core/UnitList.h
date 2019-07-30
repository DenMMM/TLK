//---------------------------------------------------------------------------
#ifndef UnitListH
#define UnitListH
//---------------------------------------------------------------------------
class MListItem;
class MList;
//---------------------------------------------------------------------------
class MListItem
{
public:
    virtual unsigned TypeID() { return 0; }
    virtual bool Copy(MListItem *SrcItem_)=0;
public:
    MListItem *Prev;    // ��������� �� ���������� ������� ������
    MListItem *Next;    // ��������� �� ��������� ������� ������

    MListItem();
    ~MListItem();
};
//---------------------------------------------------------------------------
class MList
{
private:
    // � ����������� ������ ��������� �������� 'new' ��� ������������ �� MListItem ������
    virtual MListItem *operator_new(unsigned Type_)=0;
    // � ����������� ������ ��������� �������� 'delete' ��� ������������ �� MListItem ������
    virtual void operator_delete(MListItem *DelItem_)=0;
protected:
    virtual bool Typed() { return false; }
public:
    // ������������ � ������ ��� ��������� � ������ �������
    void attach(MListItem *AtchItem_);
    // ��������� � ������ ��� ��������� � ������ ������� ����� AfterItem_
    void insert(MListItem *InsItem_, MListItem *AfterItem_);
    // ����������� �� ������ ������� ��� �������� ��� �� ������
    void detach(MListItem *DtchItem_);
private:
    //
//    void attach(MList *AtchList_);
protected:
    void constructor();
    void destructor();
public:
    MListItem *First;   // ��������� �� ������ ������� ������
    MListItem *Last;    // ��������� �� ��������� ������� ������
    unsigned Count;     // ���������� ��������� � ������

    // �������� ��� ���������� ������
    MListItem *Add(unsigned Type_=0);
    MListItem *Insert(MListItem *AfterItem_, unsigned Type_=0);
    MListItem *Item(unsigned Number_);
    MListItem *Search(unsigned Type_, MListItem *Start_, bool Forward_);
    void Exchange(MListItem *Item1_, MListItem *Item2_);
    void Delete(MListItem *DelItem_);

    // �������� ��� �������
    bool Add(MList *SrcList_);
    bool Copy(MList *SrcList_);
    void Move(MList *SrcList_);
    void Clear();
};
//---------------------------------------------------------------------------
#endif

