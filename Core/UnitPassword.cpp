//---------------------------------------------------------------------------
#include <array>
#include <random>
//#include <windows.h>
#include <winsock2.h>
#pragma hdrstop

#include "UnitPassword.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
std::size_t MPassword::GetDataSize() const
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

void MPassword::Set(const std::wstring &Pass_)
{
	// Сгенерируем "соль"
	if ( !CngRand(Salt,sizeof(Salt)) ) TimeRand(Salt,sizeof(Salt));

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

std::wstring MPassword::New(std::size_t Len_, bool Cap_, bool Low_, bool Num_)
{
	const std::wstring cap(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	const std::wstring low(L"abcdefghijklmnopqrstuvwxyz");
	const std::wstring dig(L"0123456789");

	// Заполняем словарь
	std::wstring dict;
	dict.reserve(cap.length()+low.length()+dig.length());
	if ( Cap_ ) dict+=cap;
	if ( Low_ ) dict+=low;
	if ( Num_ ) dict+=dig;
	if ( dict.empty() ) return L"";

	// Подготовим распределитель значений по словарю
	std::uniform_int_distribution <std::size_t> rnd_distr(0, dict.length()-1);

	// Заполним последовательность инициализации
	/// 96 бит достаточно для 16-символьных паролей из 52 знаков ?
	std::array <uint_fast32_t, 3> seeds;
	try
	{
		// Попытаемся использовать аппаратный ГСЧ...
		std::random_device rnd_dev;

		if ( rnd_dev.entropy()==0 )
			throw std::runtime_error(
				"MPassword::New()\n"
				"Аппаратный ГСЧ не поддерживается.");

		seeds[0]=rnd_dev();
		seeds[1]=rnd_dev();
		seeds[2]=rnd_dev();
	}
	catch(std::runtime_error &e)
	{
		// ... или воспользуемся таймерами
		seeds[0]=BasicRand();
		seeds[1]=BasicRand();
		seeds[2]=BasicRand();
	}

	// Инициализируем ГПСЧ
	std::seed_seq rnd_seed(seeds.begin(),seeds.end());
	std::mt19937 rnd_gen(rnd_seed);

	// Сгенерируем пароль
	std::wstring pass;
	pass.reserve(Len_);

	while ( (Len_--)>0 ) pass.push_back(
		dict[
			rnd_distr(rnd_gen)
			]
		);

	return pass;
}
//---------------------------------------------------------------------------

