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
}

unsigned MPassword::GetDataSize() const
{
    return
        sizeof(Salt)+
        sizeof(Hash);
}

void *MPassword::SetData(void *Data_) const
{
	Data_=MemSetBin(Data_,Salt,sizeof(Salt));
	Data_=MemSetBin(Data_,Hash,sizeof(Hash));

    return Data_;
}

const void *MPassword::GetData(const void *Data_, const void *Limit_)
{
    return
		(Data_=MemGetBin(Data_,Salt,sizeof(Salt),Limit_)) &&
		(Data_=MemGetBin(Data_,Hash,sizeof(Hash),Limit_))
		? Data_: nullptr;
}

void MPassword::Copy(const MPassword *SrcPass_)
{
    memcpy(Salt,SrcPass_->Salt,sizeof(Salt));
    memcpy(Hash,SrcPass_->Hash,sizeof(Hash));
}

void MPassword::Set(const std::wstring &Pass_)
{
    // Генерируем "соль"
    for ( size_t i=0; i<sizeof(Salt); i++ ) Salt[i]=random(256);

    // Вычисляем хэш введенного пароля и соли
	hmac_sha(
		PASS_Alg,
		(const unsigned char*)Pass_.c_str(),
		Pass_.length()*sizeof(wchar_t),
		Salt, sizeof(Salt),
		Hash, sizeof(Hash));
}

bool MPassword::Check(const std::wstring &Pass_) const
{
	unsigned char tmp[sizeof(Hash)];

	// Вычисляем хэш введенного пароля и соли
	hmac_sha(
		PASS_Alg,
		(const unsigned char*)Pass_.c_str(),
		Pass_.length()*sizeof(wchar_t),
		Salt, sizeof(Salt),
		tmp, sizeof(tmp));
    // Сравниваем его с сохраненным ранее хэшем
	return MemSlowCmp(tmp, Hash, sizeof(Hash));
//    return memcmp(tmp, Hash, sizeof(Hash)) == 0;
}
//---------------------------------------------------------------------------

