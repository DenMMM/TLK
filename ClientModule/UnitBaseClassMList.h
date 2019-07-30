//---------------------------------------------------------------------------
#ifndef UnitBaseClassMListH
#define UnitBaseClassMListH
//---------------------------------------------------------------------------
class MListItem;
class MList;
//---------------------------------------------------------------------------
class MListItem
{
public:
    MListItem *PrevItem, *NextItem;  // ��������� �� ���������� � ��������� �������� ������
};
//---------------------------------------------------------------------------
class MList
{
private:
    virtual MListItem *new_()=0;
    virtual void delete_(MListItem *ListItem_)=0;
protected:
    void constructor();
    void destructor();
public:
    int CountItems;  // ���������� ��������� � ������
    MListItem *FirstItem, *LastItem;  // ��������� �� ������ � ��������� �������� ������

    MListItem *Add();
    MListItem *Insert(MListItem *ListItem_);
    void Delete(MListItem *ListItem_);
    void Delete(int Number_);
    MListItem *Item(int Number_);
    void Clear();
};
//---------------------------------------------------------------------------
#endif

