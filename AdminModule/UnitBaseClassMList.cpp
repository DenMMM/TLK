//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitBaseClassMList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MListItem *MList::Add()
{
    MListItem *NewItem;

    // ������� ����� ������ �������� ������
    if ( (NewItem=new_())==NULL ) return NULL;
    // �������������� ������
    NewItem->PrevItem=LastItem;
    NewItem->NextItem=NULL;
    // �������� ��������� ������ ������ � ����������� ������� � ������
    if ( FirstItem==NULL ) FirstItem=NewItem;
    else LastItem->NextItem=NewItem;
    LastItem=NewItem;
    CountItems++;

    return NewItem;
}
//---------------------------------------------------------------------------
MListItem *MList::Insert(MListItem *ListItem_)
{
    MListItem *NewItem;

    // ������� ����� ������ �������� ������
    if ( (NewItem=new_())==NULL ) return NULL;
    // �������������� ����� ������, �������� � ��� � ����� ��� ������ ���������
    NewItem->PrevItem=ListItem_;
    if ( ListItem_==NULL )
    {
        NewItem->NextItem=FirstItem;
        if ( FirstItem==NULL ) LastItem=NewItem;
        else FirstItem->PrevItem=NewItem;
        FirstItem=NewItem;
    } else
    {
        NewItem->NextItem=ListItem_->NextItem;
        if ( ListItem_->NextItem!=NULL ) ListItem_->NextItem->PrevItem=NewItem;
        else LastItem=NewItem;
        ListItem_->NextItem=NewItem;
    }
    CountItems++;

    return NewItem;
}
//---------------------------------------------------------------------------
void MList::Exchange(MListItem *ListItem1_, MListItem *ListItem2_)
{
    MListItem *ListItem;

    if ( ListItem1_->PrevItem ) ListItem1_->PrevItem->NextItem=ListItem2_;
    if ( ListItem2_->PrevItem ) ListItem2_->PrevItem->NextItem=ListItem1_;
    ListItem=ListItem1_->PrevItem;
    ListItem1_->PrevItem=ListItem2_->PrevItem;
    ListItem2_->PrevItem=ListItem;
    if ( ListItem1_->NextItem ) ListItem1_->NextItem->PrevItem=ListItem2_;
    if ( ListItem2_->NextItem ) ListItem2_->NextItem->PrevItem=ListItem1_;
    ListItem=ListItem1_->NextItem;
    ListItem1_->NextItem=ListItem2_->NextItem;
    ListItem2_->NextItem=ListItem;
    //
    if ( FirstItem==ListItem1_ ) FirstItem=ListItem2_;
    else if ( FirstItem==ListItem2_ ) FirstItem=ListItem1_;
    //
    if ( LastItem==ListItem1_ ) LastItem=ListItem2_;
    else if ( LastItem==ListItem2_ ) LastItem=ListItem1_;
}
//---------------------------------------------------------------------------
void MList::Delete(MListItem *ListItem_)
{
    if ( ListItem_==NULL ) return;
    // ����������� � ���������� ��������� ������ � ��� �������
    if ( ListItem_->PrevItem!=NULL ) ListItem_->PrevItem->NextItem=ListItem_->NextItem;
    else FirstItem=ListItem_->NextItem;
    // ����������� �� ��������� ��������� ������ � ��� ������
    if ( ListItem_->NextItem!=NULL ) ListItem_->NextItem->PrevItem=ListItem_->PrevItem;
    else LastItem=ListItem_->PrevItem;

    // ������ ������ � ������������ �������
    delete_(ListItem_);
    CountItems--;
}
//---------------------------------------------------------------------------
void MList::Delete(int Number_)
{
    Delete(Item(Number_));
}
//---------------------------------------------------------------------------
MListItem* MList::Item(int Number_)
{
    if ( Number_<0 ) return NULL;

    MListItem *SearchItem=FirstItem;
    // ���� �� ������� ��������� �� ������ �������
    for ( int i=0; (i!=Number_)&&(SearchItem!=NULL); i++ ) SearchItem=SearchItem->NextItem;

    return SearchItem;
}
//---------------------------------------------------------------------------
void MList::Clear()
{
    // ������� ���� ������
    while (LastItem)
    {
        FirstItem=LastItem->PrevItem;
        delete_(LastItem);
        LastItem=FirstItem;
    }
    // �������� ������� � ���������
    CountItems=0;
    FirstItem=LastItem=NULL;
}
//---------------------------------------------------------------------------
void MList::constructor()
{
    CountItems=0;
    FirstItem=LastItem=NULL;
}
//---------------------------------------------------------------------------
void MList::destructor()
{
    Clear();
}
//---------------------------------------------------------------------------

