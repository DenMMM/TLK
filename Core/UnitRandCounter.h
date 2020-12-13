//---------------------------------------------------------------------------
#ifndef UnitRandCounterH
#define UnitRandCounterH
//---------------------------------------------------------------------------
#include "fasthash.h"

#include "UnitEncode.h"
//---------------------------------------------------------------------------
class MRandCounter;
//---------------------------------------------------------------------------
class MRandCounter
{
private:
	std::uint64_t Counter;		// Последовательно возрастающий счетчик
	std::uint64_t Seed;			// Код, которым шифруется его значение

public:
	std::uint64_t operator++()
	{
		auto Res=(++Counter);
		// Не криптографический хэш вместо PRNG
		return fasthash64(&Res, sizeof(Res), Seed); /// А так можно ?
	}

	std::uint64_t operator++(int)
	{
		auto Res=(Counter++);
		return fasthash64(&Res, sizeof(Res), Seed);
	}

	MRandCounter() = delete;
	MRandCounter(
		std::uint64_t Seed_):
			Counter(0),
			Seed(Seed_)
	{
	}
};
//---------------------------------------------------------------------------
#endif
