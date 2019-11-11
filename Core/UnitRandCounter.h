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
	unsigned Counter;       // Последовательно возрастающий счетчик
	unsigned Seed;          // Код, которым шифруется его значение

public:
	unsigned operator++()
	{
		unsigned Res=(++Counter);
		BasicEncode(&Res, sizeof(Res), Seed, 32);
		return Res;
	}

	unsigned operator++(int)
	{
		unsigned Res=(Counter++);
		BasicEncode(&Res, sizeof(Res), Seed, 32);
		return Res;
	}

	MRandCounter() = delete;
	MRandCounter(unsigned Seed_):
		Counter(0),
		Seed(Seed_)
	{
	}
};
//---------------------------------------------------------------------------
#endif
