//---------------------------------------------------------------------------
#include <stddef.h>
#include <stdexcept.h>
#pragma hdrstop

#include "UnitList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MListItem *MList::Add(unsigned char TypeID_)
{
    MListItem *NewItem;

    // Создаем новый объект
    NewItem=item_new(TypeID_);
    // Инициализируем объект
    NewItem->Prev=Last;
    NewItem->Next=NULL;
    // Изменяем атрибуты самого списка и предыдущего объекта
    if ( First==NULL ) First=NewItem;
    else Last->Next=NewItem;
    Last=NewItem;
    // Корректируем счетчик
    Count++;

    return NewItem;
}

MListItem *MList::Item(unsigned Number_) const
{
    if ( Number_>=Count ) return NULL;

    MListItem *SearchItem=First;
    // Ищем по цепочке указатель на нужный элемент
    for ( unsigned i=0; (i!=Number_)&&(SearchItem!=NULL); i++ )
        SearchItem=SearchItem->Next;

    return SearchItem;
}

void MList::Exch(MListItem *Item1_, MListItem *Item2_)
{
    MListItem *Item;

    if ( Item1_->Prev ) Item1_->Prev->Next=Item2_;
    if ( Item2_->Prev ) Item2_->Prev->Next=Item1_;
    Item=Item1_->Prev;
    Item1_->Prev=Item2_->Prev;
    Item2_->Prev=Item;

    if ( Item1_->Next ) Item1_->Next->Prev=Item2_;
    if ( Item2_->Next ) Item2_->Next->Prev=Item1_;
    Item=Item1_->Next;
    Item1_->Next=Item2_->Next;
    Item2_->Next=Item;

    if ( First==Item1_ ) First=Item2_;
    else if ( First==Item2_ ) First=Item1_;

    if ( Last==Item1_ ) Last=Item2_;
    else if ( Last==Item2_ ) Last=Item1_;
}

void MList::Del(MListItem *DelItem_)
{
    if ( DelItem_==NULL ) return;

    // Разбираемся с предыдущим элементом списка и его началом
    if ( DelItem_->Prev!=NULL ) DelItem_->Prev->Next=DelItem_->Next;
    else First=DelItem_->Next;
    // Разбираемся со следующим элементом списка и его концом
    if ( DelItem_->Next!=NULL ) DelItem_->Next->Prev=DelItem_->Prev;
    else Last=DelItem_->Prev;
    // Корректируем счетчик
    Count--;

    // Удаляем объект из памяти
    item_del(DelItem_);
}

void MList::Clear()
{
    MListItem *del=First, *next;

    while(del!=NULL)
    {
        next=del->Next;
        item_del(del);
        del=next;
    }

    First=Last=NULL;
    Count=0;
}

void MList::Copy(const MList *SrcList_)
{
    // Очищаем список-приемник
    Clear();
    try
    {
        // Копируем в него элементы исходного
        MListItem *SrcItem=SrcList_->First, *NewItem;
        while(SrcItem)
        {
            NewItem=Add(SrcItem->gTypeID());
            NewItem->Copy(SrcItem);
            SrcItem=SrcItem->Next;
        }
    } catch(std::bad_alloc &e)
    {
        // Раз список не заполнился, очистим его
        Clear();
        // И передадим исключение выше
        throw e;
    }
}

void MList::Move(MList *SrcList_)
{
    // Очищаем список-приемник
    Clear();
    // Копируем ссылки на элементы и их количество
    First=SrcList_->First;
    Last=SrcList_->Last;
    Count=SrcList_->Count;
    // Обнуляем эти поля в исходном списке
    SrcList_->First=NULL;
    SrcList_->Last=NULL;
    SrcList_->Count=0;
}

void MList::Attach(MList *AtchList_)
{
    // Проверяем есть ли что присоединять
    if ( AtchList_->First==NULL ) return;
    // Связываем списки
    if ( First==NULL ) First=AtchList_->First;
    else
    {
        Last->Next=AtchList_->First;
        Last->Next->Prev=Last;
    }
    Last=AtchList_->Last;
    // Увеличиваем счетчик
    Count+=AtchList_->Count;
    // Отсоединяем элементы от исходного списка
    AtchList_->First=NULL;
    AtchList_->Last=NULL;
    AtchList_->Count=0;
}
//---------------------------------------------------------------------------

