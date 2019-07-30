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
    MListItem *Prev;    // Указатель на предыдущий элемент списка
    MListItem *Next;    // Указатель на следующий элемент списка
    friend class MList; // Разрешим MList работать с MListItem напрямую

public:
    // Доступ к элементам класса
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
    MListItem *First;   // Указатель на первый элемент списка
    MListItem *Last;    // Указатель на последний элемент списка
    unsigned Count;     // Количество элементов в списке

    // Операторы 'new' и 'delete' для производного от MListItem класса
    virtual MListItem *item_new(unsigned char TypeID_) const=0;
    virtual void item_del(MListItem *Item_) const=0;

protected:
    // Определяет могут ли элементы списка быть разных типов
    // Для 'true' MListItem::TypeID() должна быть переопределена
    virtual bool Typed() const { return false; }

public:
    // Доступ к атрибутам списка
    MListItem *gFirst() const { return First; }
    MListItem *gLast() const { return Last; }
    unsigned gCount() const { return Count; }

    // Операции над отдельными элементами _одного_ списка
    MListItem *Add(unsigned char TypeID_=0);
    MListItem *Item(unsigned Number_) const;
    void Exch(MListItem *Item1_, MListItem *Item2_);
    void Del(MListItem *DelItem_);

    // Операции над списком целиком (не трогая атрибуты наследника MList)
    void Clear();                       // Удалить все элементы списка
    void Copy(const MList *SrcList_);   // Создать копию элементов исходного списка
    void Move(MList *SrcList_);         // Заместить элементы списка исходными
    void Attach(MList *AtchList_);      // Присоединить элементы исходного списка

    MList()
    {
        First=Last=NULL;
        Count=0;
    }
//    ~MList()
//    {
// Вызывать в деструкторе класса, где определена ф-я item_del(),
// т.к. к моменту вызова ~MList() доступа к этому оператору уже нет.
// Иначе элементы списка не будут удалены - memory leak !

//    Clear();
//    }
};
//---------------------------------------------------------------------------
#endif

