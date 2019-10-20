//---------------------------------------------------------------------------
#include <stddef.h>
#include <stdexcept>
#pragma hdrstop

#include "UnitList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MListItem *MList::Add(MListItem *NewItem_)
{
	// ��������, ��� ������ ��� �� �������� � ������-���� ������
	if (
		NewItem_->Prev!=nullptr ||
		NewItem_->Next!=nullptr )
	{
		throw std::runtime_error (
			"MList::Add()\n"
			"������ ��� �������� � ������."
			);
	}
	// �������������� ������
	NewItem_->Prev=Last;
	NewItem_->Next=nullptr;
	// �������� �������� ������ ������ � ����������� �������
	if ( First==nullptr ) First=NewItem_;
	else Last->Next=NewItem_;
	Last=NewItem_;
	// ������������ �������
	Count++;

	return NewItem_;
}

MListItem *MList::Add(unsigned char TypeID_)
{
	MListItem* (*NewF)()=NewForTypeDef;

	if ( NewF==nullptr )
	{
		if ( TypeID_>=NewForType.size() ) return nullptr;
		// ������� �� ������� ����� �������, ��������� ������� ������
		NewF=NewForType[TypeID_];
		if ( NewF==nullptr ) return nullptr;
	}

	// �������� ����� ������� � ������� ��� � ������
	return Add(NewF());
}

MListItem *MList::GetItem(size_t Index_) const
{
	if ( Index_>=Count )
	{
		throw std::out_of_range (
			"MList::GetItem()\n"
			"������� ����� �� ������� ������."
			);
	}

	MListItem *SearchItem=First;
	// ���� �� ������� ��������� �� ������ �������
	for ( size_t i=0; (i!=Index_)&&(SearchItem!=nullptr); i++ )
		SearchItem=SearchItem->Next;

	return SearchItem;
}

void MList::Swap(MListItem *Item1_, MListItem *Item2_)
{
	if ( Count==0 )
	{
		throw std::runtime_error (
			"MList::Swap()\n"
			"������ ����."
			);
	}

	if (
		Item1_==nullptr ||
		Item2_==nullptr )
	{
		throw std::runtime_error (
			"MList::Swap()\n"
			"������ �� ���������� (nullptr)."
			);
	}

	if ( Item1_==Item2_ ) return;

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
	if ( Count==0 )
	{
		throw std::runtime_error (
			"MList::Del()\n"
			"������ ����."
			);
	}

	if ( DelItem_==nullptr )
	{
		throw std::runtime_error (
			"MList::Del()\n"
			"������ �� ���������� (nullptr)."
			);
	}

	if (
		DelItem_->Prev==nullptr &&
		DelItem_->Next==nullptr &&
		DelItem_!=First )
	{
		throw std::runtime_error (
			"MList::Del()\n"
			"������ �� ����������� ������."
			);
	}

	// ����������� � ���������� ��������� ������ � ��� �������
	if ( DelItem_->Prev!=nullptr ) DelItem_->Prev->Next=DelItem_->Next;
	else First=DelItem_->Next;
	// ����������� �� ��������� ��������� ������ � ��� ������
	if ( DelItem_->Next!=nullptr ) DelItem_->Next->Prev=DelItem_->Prev;
	else Last=DelItem_->Prev;
	// ������������ �������
	Count--;

	// ������� ������ �� ������
	delete DelItem_;
}

void MList::Clear() noexcept
{
	MListItem *del=First, *next;

	while(del!=nullptr)
	{
		next=del->Next;
		delete del;
		del=next;
	}

	First=Last=nullptr;
	Count=0;
}

MList& MList::operator=(const MList& SrcList_)
{
	if ( (&SrcList_)==this ) return *this;

	// ������� ������-��������
	Clear();
	try
	{
		// �������� � ���� �������� ���������
		MListItem *SrcItem=SrcList_.First, *NewItem;
		while(SrcItem)
		{
//			Add(SrcItem->Clone());              /// ����� ��������� 'operator='
			NewItem=Add(SrcItem->gTypeID());
			*NewItem=*SrcItem;
			SrcItem=SrcItem->Next;
		}
	} catch(...)
	{
		// ��� ������ �� ����������, ������� ���
		Clear();
		// � ��������� ���������� ����
		throw;
	}

	return *this;
}

void MList::Move(MList *SrcList_) noexcept
{
    if ( SrcList_==this ) return;

	// ������� ������-��������
	Clear();
	// �������� ������ �� �������� � �� ����������
	First=SrcList_->First;
	Last=SrcList_->Last;
	Count=SrcList_->Count;
	// �������� ��� ���� � �������� ������
	SrcList_->First=nullptr;
	SrcList_->Last=nullptr;
	SrcList_->Count=0;
}

void MList::Splice(MList *AtchList_)
{
	if ( AtchList_==this ) return;

	// ��������� ���� �� ��� ������������
	if ( AtchList_->First==nullptr ) return;
	// ��������� ������
	if ( First==nullptr ) First=AtchList_->First;
	else
	{
		Last->Next=AtchList_->First;
		Last->Next->Prev=Last;
	}
	Last=AtchList_->Last;
	// ����������� �������
	Count+=AtchList_->Count;
	// ����������� �������� �� ��������� ������
    AtchList_->First=nullptr;
	AtchList_->Last=nullptr;
    AtchList_->Count=0;
}
//---------------------------------------------------------------------------

