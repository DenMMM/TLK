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
    MListItem *Prev;    // Указатель на предыдущий элемент списка
    MListItem *Next;    // Указатель на следующий элемент списка

    MListItem();
    ~MListItem();
};
//---------------------------------------------------------------------------
class MList
{
private:
    // В производном классе реализует оператор 'new' для производного от MListItem класса
    virtual MListItem *operator_new(unsigned Type_)=0;
    // В производном классе реализует оператор 'delete' для производного от MListItem класса
    virtual void operator_delete(MListItem *DelItem_)=0;
protected:
    virtual bool Typed() { return false; }
public:
    // Присоединяет к списку уже созданный в памяти элемент
    void attach(MListItem *AtchItem_);
    // Вставляет в список уже созданный в памяти элемент после AfterItem_
    void insert(MListItem *InsItem_, MListItem *AfterItem_);
    // Отсоединяет от списка элемент без удаления его из памяти
    void detach(MListItem *DtchItem_);
private:
    //
//    void attach(MList *AtchList_);
protected:
    void constructor();
    void destructor();
public:
    MListItem *First;   // Указатель на первый элемент списка
    MListItem *Last;    // Указатель на последний элемент списка
    unsigned Count;     // Количество элементов в списке

    // Операции над элементами списка
    MListItem *Add(unsigned Type_=0);
    MListItem *Insert(MListItem *AfterItem_, unsigned Type_=0);
    MListItem *Item(unsigned Number_);
    MListItem *Search(unsigned Type_, MListItem *Start_, bool Forward_);
    void Exchange(MListItem *Item1_, MListItem *Item2_);
    void Delete(MListItem *DelItem_);

    // Операции над списком
    bool Add(MList *SrcList_);
    bool Copy(MList *SrcList_);
    void Move(MList *SrcList_);
    void Clear();
};
//---------------------------------------------------------------------------
#endif

