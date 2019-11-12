//---------------------------------------------------------------------------
#ifndef UnitTimeRandH
#define UnitTimeRandH
//---------------------------------------------------------------------------
#include <random>
#include <chrono>
#include <array>
//#include <windows.h>
#include <winsock2.h>

#include "UnitEncode.h"
#include "UnitWinAPI.h"
//---------------------------------------------------------------------------
class MTimeRand;
//---------------------------------------------------------------------------
class MTimeRand
{
public:
	typedef uint_fast32_t result_type;

	static result_type min() {
		return std::numeric_limits<result_type>::min(); }
	static result_type max() {
		return std::numeric_limits<result_type>::max(); }

protected:
	class MEntropy
	{
	private:
		std::array <result_type, 32> Ent;
		mutable size_t EntSel;

	public:
		template <typename ent_cont>
		void Update(ent_cont Ent_) noexcept
		{
			// ��������� ����� �������� ������ ����� ���������
			for ( auto E: Ent_ )
			{
				BasicEncode(
					Ent.data(), Ent.size()*sizeof(result_type),
					E, 32);
			}
		}

		result_type Get() const noexcept
		{
			return Ent[(EntSel++)%Ent.size()];
		}

		MEntropy():
			EntSel(0)
		{
//			Ent.fill(0);
			result_type Num=0; for ( auto &E: Ent ) E=(++Num);
		}

		MEntropy(std::seed_seq &Seed_):
			EntSel(0)       			/// ������ �� rand
		{
			Seed_.generate(Ent.begin(),Ent.end());
		}

		MEntropy(const MEntropy&) = delete;
		MEntropy(MEntropy&&) = delete;
		MEntropy& operator=(const MEntropy&) = delete;
		MEntropy& operator=(MEntropy&&) = delete;

		~MEntropy()
		{
			Ent.fill(0);    			/// ��� ����� ���� ?
		}
	};

	MEntropy Entropy;
	MWAPI::CRITICAL_SECTION CS_Entropy;

	auto GetTimers() noexcept
	{
		std::array <result_type, 2> Timers;

		// ��������� ��������� �����...
		Timers[0]=std::chrono::system_clock::
			now().time_since_epoch().count();
		// ... � �������� "������������" ��������
		Timers[1]=[]() {
			LARGE_INTEGER cntr;
			return
				::QueryPerformanceCounter(&cntr)?
				cntr.LowPart:
				::GetTickCount();       /// ������� ���������� ����������
		}();

		// ���������� ID ������, ����� ��������� ����������
		DWORD ThreadId=::GetCurrentThreadId();
		Timers[0]^=ThreadId;
		Timers[1]^=ThreadId;

		return Timers;
	}

	template <typename timers_cont>
	void UpdateEntropy(timers_cont Timers_) noexcept
	{
		MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Entropy);

		// ���������� ����� ��������
		Entropy.Update(Timers_);
	}

	template <typename timers_cont>
	auto GetEntropy(timers_cont Timers_)
	{
		MWAPI::CRITICAL_SECTION::Lock lckObj(CS_Entropy);

		// ���������� ����� ��������
		Entropy.Update(Timers_);        /// ��������, ���� ��������� (?)
		// ������� ���� ��� ���������...
		auto Res=Entropy.Get();
		// ... � ����������� �������� ���� �� ���������
		for ( auto T: Timers_ ) BasicDecode(&Res, sizeof(Res), T, 32);

		return Res;
	}

public:
	void event() noexcept
	{
		UpdateEntropy(GetTimers());
	}

	auto operator()()
	{
		return GetEntropy(GetTimers());
	}

	MTimeRand() = default;
	MTimeRand(std::seed_seq &Seed_): Entropy(Seed_) {}
};
//---------------------------------------------------------------------------
#endif
