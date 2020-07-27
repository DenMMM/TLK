//---------------------------------------------------------------------------
#include <string.h>
#pragma hdrstop

#include "UnitAuth.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
std::size_t MAuth::GetDataSize() const
{
    return
        sizeof(Key);
}

void *MAuth::SetData(void *Data_) const
{
    Data_=MemSetBin(Data_,Key,sizeof(Key));
    return Data_;
}

const void *MAuth::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGetBin(Data_,Key,sizeof(Key),Limit_))
		? Data_: nullptr;
}

void MAuth::SetKey(const void *Key_, std::size_t Size_)
{
	if ( Size_!=sizeof(Key) )
	{
		throw std::runtime_error (
			"MAuth::SetKey()"
			"Не допустимый размер ключа.");
	}
	memcpy(Key,Key_,sizeof(Key));
}

void MAuth::GetKey(void *Buff_, std::size_t Size_) const
{
    if ( Size_!=sizeof(Key) )
    {
        throw std::runtime_error (
            "MAuth::GetKey()"
            "Не допустимый размер буфера для ключа.");
    }
    memcpy(Buff_,Key,sizeof(Key));
}

void MAuth::Calc(const void *Data_, std::size_t DataSize_, void *MAC_, std::size_t MACSize_) const
{
	hmac_sha(
		MAC_Alg,
		Key, sizeof(Key),
		static_cast<const unsigned char*>(Data_), DataSize_,
		static_cast<unsigned char*>(MAC_), MACSize_);
}

bool MAuth::Check(const void *Data_, std::size_t DataSize_, const void *MAC_, std::size_t MACSize_) const
{
    unsigned char mac[MAC_Size];

    // Если длины MAC отличаются, то и сравнивать нечего
    if ( MACSize_!=sizeof(mac) ) return false;
	// Вычисляем MAC для данных с известным нам ключем
	hmac_sha(
		MAC_Alg,
		Key, sizeof(Key),
		static_cast<const unsigned char*>(Data_), DataSize_,
		static_cast<unsigned char*>(mac), sizeof(mac));
    // Сравниваем MAC
	return MemSlowCmp(MAC_, mac, sizeof(mac));
//    return memcmp(MAC_, mac, sizeof(mac)) == 0;
}
//---------------------------------------------------------------------------
