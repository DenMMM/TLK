//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitPassword.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MPassword::MPassword()
{
    memset(Salt,0,sizeof(Salt));
    memset(Hash,0,sizeof(Hash));
}

MPassword::~MPassword()
{
//
}

unsigned MPassword::GetDataSize() const
{
    return
        sizeof(Salt)+
        sizeof(Hash);
}

char *MPassword::SetData(char *Data_) const
{
    Data_=MemSetBLine(Data_,Salt,sizeof(Salt));
    Data_=MemSetBLine(Data_,Hash,sizeof(Hash));
    return Data_;
}

const char *MPassword::GetData(const char *Data_, const char *Limit_)
{
    return
        (Data_=MemGetBLine(Data_,Salt,sizeof(Salt),Limit_))!=NULL &&
        (Data_=MemGetBLine(Data_,Hash,sizeof(Hash),Limit_))!=NULL
        ? Data_: NULL;
}

void MPassword::Copy(MPassword *SrcPass_)
{
    memcpy(Salt,SrcPass_->Salt,sizeof(Salt));
    memcpy(Hash,SrcPass_->Hash,sizeof(Hash));
}

void MPassword::Set(char *Pass_)
{
    unsigned plen;

    // Генерируем "соль"
    for ( int i=0; i<sizeof(Salt); i++ ) Salt[i]=random(256);

    plen=strlen(Pass_);
    // Вычисляем хэш введенного пароля и соли
    hmac_sha(PASS_Alg,
        Pass_,plen,
        Salt,sizeof(Salt),
        Hash,sizeof(Hash));
}

bool MPassword::Check(const char *Pass_) const
{
    unsigned plen;
    unsigned char tmp[sizeof(Hash)];

    plen=strlen(Pass_);
    // Вычисляем хэш введенного пароля и соли
    hmac_sha(PASS_Alg,
        Pass_,plen,
        Salt,sizeof(Salt),
        tmp,sizeof(tmp));
    // Сравниваем его с сохраненным ранее хэшем
    return MemSlowCmp(tmp,Hash,sizeof(Hash));
//    return memcmp(tmp,Hash,sizeof(Hash))==0;
}
//---------------------------------------------------------------------------
