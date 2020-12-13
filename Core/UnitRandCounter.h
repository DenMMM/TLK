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
	std::uint64_t Counter;		// ��������������� ������������ �������
	std::uint64_t Seed;			// ���, ������� ��������� ��� ��������

public:
	std::uint64_t operator++()
	{
		auto Res=(++Counter);
		// �� ����������������� ��� ������ PRNG
		return fasthash64(&Res, sizeof(Res), Seed); /// � ��� ����� ?
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
