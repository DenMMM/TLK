//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitEncode.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void BasicEncode(void *Data__, std::size_t DataSize_, std::uint32_t Code_, int Round_)
{
	unsigned char *Data_=static_cast<unsigned char*>(Data__);
	unsigned char *limit;
	std::uint32_t blk;

	if ( DataSize_<sizeof(blk) ) return;

	limit=Data_+DataSize_-sizeof(blk);
	// ������� ��������� ���� �������
	blk=*((std::uint32_t*)Data_);
	goto begin;

	do
	{
		// �������� � ����� ������� ���� �����
		Data_[0]=blk;
		// ������� ���� ������ � �������� ������� ���� �� �����
		blk>>=8;
		blk|=( (std::uint32_t)Data_[sizeof(blk)] )<<(sizeof(blk)*8-8);
		++Data_;
begin:
		// ���������� ���� �����
		for ( int i=Round_; i; i-- )
			blk=BasicEncodeRound(blk,Code_);
	} while(Data_!=limit);

	// �������� � ����� ��������� ���� �������
	*((std::uint32_t*)Data_)=blk;
}
//---------------------------------------------------------------------------
void BasicDecode(void *Data__, std::size_t DataSize_, std::uint32_t Code_, int Round_)
{
	unsigned char *Data_=static_cast<unsigned char*>(Data__);
	unsigned char *limit;
	std::uint32_t blk;

	if ( DataSize_<sizeof(blk) ) return;

	limit=Data_;
	Data_+=DataSize_-sizeof(blk);

	// ������� ��������� ���� �������
	blk=*((std::uint32_t*)Data_);
	goto begin;

	do
	{
		--Data_;
		// �������� � ����� ������� ���� �����
		Data_[sizeof(blk)]=blk>>(sizeof(blk)*8-8);
		// ������� ���� ����� � �������� ������� ���� �� �����
		blk<<=8;
		blk|=Data_[0];
begin:
		// ���������� ���� �����
		for ( int i=Round_; i; i-- )
			blk=BasicDecodeRound(blk,Code_);
	} while(Data_!=limit);

	// �������� � ����� ��������� ���� �������
	*((std::uint32_t*)Data_)=blk;
}
//---------------------------------------------------------------------------
