//---------------------------------------------------------------------------
#include <string.h>
#pragma hdrstop

#include "UnitAuth.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MAuth::MAuth()
{
    memset(Key,0x00,sizeof(Key));
}

MAuth::~MAuth()
{
//
}

unsigned MAuth::GetDataSize() const
{
    return
        sizeof(Key);
}

char *MAuth::SetData(char *Data_) const
{
    Data_=MemSetBLine(Data_,Key,sizeof(Key));
    return Data_;
}

const char *MAuth::GetData(const char *Data_, const char *Limit_)
{
    if ( (Data_=MemGetBLine(Data_,Key,sizeof(Key),Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

void MAuth::SetKey(const char *Key_, int Size_)
{
    if ( Size_!=sizeof(Key) )
    {
        throw std::runtime_error (
            "MAuth::SetKey"
            "Не допустимый размер ключа."
            );
    }
    memcpy(Key,Key_,sizeof(Key));
}

void MAuth::GetKey(char *Buff_, int Size_) const 
{
    if ( Size_!=sizeof(Key) )
    {
        throw std::runtime_error (
            "MAuth::GetKey"
            "Не допустимый размер буфера для ключа."
            );
    }
    memcpy(Buff_,Key,sizeof(Key));
}

void MAuth::Calc(const char *Data_, int DataSize_, char *MAC_, int MACSize_) const 
{
    hmac_sha(MAC_Alg,
        Key,sizeof(Key),
        Data_,DataSize_,
        MAC_,MACSize_);
}

bool MAuth::Check(const char *Data_, int DataSize_, const char *MAC_, int MACSize_) const
{
    unsigned char mac[MAC_Size];

    // Если длины MAC отличаются, то и сравнивать нечего
    if ( MACSize_!=sizeof(mac) ) return false;
    // Вычисляем MAC для поданных данных с известным нам ключем
    hmac_sha(MAC_Alg,
        Key,sizeof(Key),
        Data_,DataSize_,
        mac,sizeof(mac));
    // Сравниваем MAC
    return MemSlowCmp(MAC_,mac,sizeof(mac));
//    return memcmp(MAC_,mac,sizeof(mac))==0;
}
//---------------------------------------------------------------------------
