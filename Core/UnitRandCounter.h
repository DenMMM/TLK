//---------------------------------------------------------------------------
#ifndef UnitRandCounterH
#define UnitRandCounterH
//---------------------------------------------------------------------------
#include "UnitEncode.h"
//---------------------------------------------------------------------------
class MRandCounter;
//---------------------------------------------------------------------------
class MRandCounter
{
private:
	std::uint32_t Counter;		// Последовательно возрастающий счетчик
	std::uint32_t Seed;			// Код, которым шифруется его значение

public:
	std::uint32_t operator++()
	{
		std::uint32_t Res=(++Counter);
		BasicEncode(&Res, sizeof(Res), Seed, 32);
		return Res;
	}

	std::uint32_t operator++(int)
	{
		std::uint32_t Res=(Counter++);
		BasicEncode(&Res, sizeof(Res), Seed, 32);
		return Res;
	}

	MRandCounter() = delete;
	MRandCounter(std::uint32_t Seed_):
		Counter(0),
		Seed(Seed_)
	{
	}
};
//---------------------------------------------------------------------------
#endif
