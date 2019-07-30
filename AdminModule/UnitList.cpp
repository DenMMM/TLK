//---------------------------------------------------------------------------
#include <stddef.h>
#pragma hdrstop

#include "UnitList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

MListItem::MListItem()
{
    Prev=Next=NULL;
}

MListItem::~MListItem()
{
    Prev=Next=NULL;
}

//---------------------------------------------------------------------------

void MList::constructor()
{
    First=Last=NULL; Count=0;
}

void MList::destructor()
{
    Clear();
}

void MList::attach(MListItem *AtchItem_)
{
    // Инициализируем объект
    AtchItem_->Prev=Last;
    AtchItem_->Next=NULL;
    // Изменяем параметры самого списка и предыдущего объекта в списке
    if ( First==NULL ) First=AtchItem_;
    else Last->Next=AtchItem_;
    Last=AtchItem_;
    // Корректируем счетчик
    Count++;
}

void MList::insert(MListItem *InsItem_, MListItem *AfterItem_)
{
    // Инициализируем новый объект, соседние с ним и общие для списка указатели
    InsItem_->Prev=AfterItem_;
    if ( AfterItem_==NULL )
    {
        InsItem_->Next=First;
        if ( First==NULL ) Last=InsItem_;
        else First->Prev=InsItem_;
        First=InsItem_;
    } else
    {
        InsItem_->Next=AfterItem_->Next;
        if ( AfterItem_->Next!=NULL ) AfterItem_->Next->Prev=InsItem_;
        else Last=InsItem_;
        AfterItem_->Next=InsItem_;
    }
    // Корректируем счетчик
    Count++;
}

void MList::detach(MListItem *DtchItem_)
{
    // Разбираемся с предыдущим элементом списка и его началом
    if ( DtchItem_->Prev!=NULL ) DtchItem_->Prev->Next=DtchItem_->Next;
    else First=DtchItem_->Next;
    // Разбираемся со следующим элементом списка и его концом
    if ( DtchItem_->Next!=NULL ) DtchItem_->Next->Prev=DtchItem_->Prev;
    else Last=DtchItem_->Prev;
    // Корректируем счетчик
    Count--;
}

/*void MList::attach(MList *AtchList_)
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
    // Корректируем счетчик
    Count+=SrcList_->Count;
}*/

MListItem *MList::Add(unsigned Type_)
{
    MListItem *NewItem;

    // Создаем новый объект элемента списка
    if ( (NewItem=operator_new(Type_))==NULL ) return NULL;
    // Присоединяем его к списку
    attach(NewItem);

    return NewItem;
}

MListItem *MList::Insert(MListItem *AfterItem_, unsigned Type_)
{
    MListItem *NewItem;

    // Создаем новый объект элемента списка
    if ( (NewItem=operator_new(Type_))==NULL ) return NULL;
    //
    insert(NewItem,AfterItem_);

    return NewItem;
}

MListItem *MList::Item(unsigned Number_)
{
    if ( Number_>=Count ) return NULL;

    MListItem *SearchItem=First;
    // Ищем по цепочке указатель на нужный элемент
    for ( unsigned i=0; (i!=Number_)&&(SearchItem!=NULL); i++ )
        SearchItem=SearchItem->Next;

    return SearchItem;
}

MListItem *MList::Search(unsigned Type_, MListItem *Start_, bool Forward_)
{
    // Проверяем направление просмотра списка
    if ( Forward_ )
        while(Start_&&(Start_->TypeID()!=Type_)) Start_=Start_->Next;
    else
        while(Start_&&(Start_->TypeID()!=Type_)) Start_=Start_->Prev;

    return NULL;
}

void MList::Exchange(MListItem *Item1_, MListItem *Item2_)
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
    //
    if ( First==Item1_ ) First=Item2_;
    else if ( First==Item2_ ) First=Item1_;
    //
    if ( Last==Item1_ ) Last=Item2_;
    else if ( Last==Item2_ ) Last=Item1_;
}

void MList::Delete(MListItem *DelItem_)
{
    if ( DelItem_==NULL ) return;
    // Отсоединяем элемент от списка
    detach(DelItem_);
    // Удаляем его из памяти
    operator_delete(DelItem_);
}

bool MList::Add(MList *SrcList_)
{
    MListItem *SrcItem=SrcList_->First, *NewItem;
    while(SrcItem)
    {
        NewItem=Add(SrcItem->TypeID());
        NewItem->Copy(SrcItem);
        SrcItem=SrcItem->Next;
    }
    return true;
}

bool MList::Copy(MList *SrcList_)
{
/*
    MListItem *SrcItem=SrcList_->First, *DstItem=First;

    // Изменяем существующие элементы списка
    while(SrcItem&&DstItem)
    {
        DstItem->Copy(SrcItem);
        SrcItem=SrcItem->Next;
        DstItem=DstItem->Next;
    }
    // Проверяем добавить новые или удалить лишние элементы списка
    if ( DstItem==NULL )
    {
        // Добавляем новые элементы списка
        while(SrcItem)
        {
            DstItem=Add();
            DstItem->Copy(SrcItem);
            SrcItem=SrcItem->Next;
        }
    } else
    {
        // Удаляем лишние элементы списка
        while(DstItem)
        {
            SrcItem=DstItem->Next;
            operator_delete(DstItem);
            DstItem=SrcItem;
        }
    }

    return true;
*/
    Clear(); return Add(SrcList_);
}

void MList::Move(MList *SrcList_)
{
    MListItem *SrcItem=SrcList_->First;
    while(SrcItem)
    {
        attach(SrcItem);
        SrcItem=SrcItem->Next;
    }
    SrcList_->First=SrcList_->Last=NULL;
    SrcList_->Count=0;
}

void MList::Clear()
{
    while(Last)
    {
        First=Last->Prev;
        operator_delete(Last);
        Last=First;
    }
    First=Last=NULL; Count=0;
}

//---------------------------------------------------------------------------

