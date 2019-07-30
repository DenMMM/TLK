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
    MListItem *PrevItem, *NextItem;  // Указатели на предыдущий и следующий элементы списка
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
    int CountItems;  // Количество элементов в списке
    MListItem *FirstItem, *LastItem;  // Указатели на первый и последний элементы списка

    MListItem *Add();
    MListItem *Insert(MListItem *ListItem_);
    void Delete(MListItem *ListItem_);
    void Delete(int Number_);
    MListItem *Item(int Number_);
    void Clear();
};
//---------------------------------------------------------------------------
#endif

