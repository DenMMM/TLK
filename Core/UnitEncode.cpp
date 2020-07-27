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
	// Считаем начальный блок целиком
	blk=*((std::uint32_t*)Data_);
	goto begin;

	do
	{
		// Сохраним в выход младший байт блока
		Data_[0]=blk;
		// Сдвинем блок вправо и загрузим старший байт из входа
		blk>>=8;
		blk|=( (std::uint32_t)Data_[sizeof(blk)] )<<(sizeof(blk)*8-8);
		++Data_;
begin:
		// Перемешаем биты блока
		for ( int i=Round_; i; i-- )
			blk=BasicEncodeRound(blk,Code_);
	} while(Data_!=limit);

	// Сохраним в выход последний блок целиком
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

	// Считаем начальный блок целиком
	blk=*((std::uint32_t*)Data_);
	goto begin;

	do
	{
		--Data_;
		// Сохраним в выход старший байт блока
		Data_[sizeof(blk)]=blk>>(sizeof(blk)*8-8);
		// Сдвинем блок влево и загрузим младший байт из входа
		blk<<=8;
		blk|=Data_[0];
begin:
		// Перемешаем биты блока
		for ( int i=Round_; i; i-- )
			blk=BasicDecodeRound(blk,Code_);
	} while(Data_!=limit);

	// Сохраним в выход последний блок целиком
	*((std::uint32_t*)Data_)=blk;
}
//---------------------------------------------------------------------------
