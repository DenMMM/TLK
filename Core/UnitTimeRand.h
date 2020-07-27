//---------------------------------------------------------------------------
#ifndef UnitTimeRandH
#define UnitTimeRandH
//---------------------------------------------------------------------------
#include <random>
#include <chrono>
#include <array>
#include <mutex>
//#include <windows.h>
#include <winsock2.h>

#include "UnitEncode.h"
//---------------------------------------------------------------------------
class MTimeRand;
//---------------------------------------------------------------------------
class MTimeRand
{
public:
	typedef uint_fast32_t result_type;

	static constexpr result_type min() {
		return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() {
		return std::numeric_limits<result_type>::max(); }

protected:
	class MEntropy
	{
	private:
		std::array <result_type, 32> Ent;
		mutable std::size_t EntSel;

	public:
		template <typename ent_cont>
		void Update(ent_cont Ent_) noexcept
		{
			// Зашифруем буфер энтропий каждым новым значением
			for ( auto E: Ent_ )
			{
				BasicEncode(
					Ent.data(), Ent.size()*sizeof(result_type),
					E, 8);
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
			result_type Num=1; for ( auto &E: Ent ) E=(Num++);
		}

		MEntropy(std::seed_seq &Seed_):
			EntSel(0)       			/// хорошо бы rand
		{
			Seed_.generate(Ent.begin(),Ent.end());
		}

		MEntropy(const MEntropy&) = delete;
		MEntropy(MEntropy&&) = delete;
		MEntropy& operator=(const MEntropy&) = delete;
		MEntropy& operator=(MEntropy&&) = delete;

		~MEntropy()
		{
			Ent.fill(0);    			/// что лучше нуля ?
		}
	};

	MEntropy Entropy;
	std::mutex mtxEntropy;

	auto GetTimers() noexcept
	{
		std::array <result_type, 2> Timers;

		// Определим системное время...
		Timers[0]=std::chrono::system_clock::
			now().time_since_epoch().count();
		// ... и значение "непрерывного" счетчика
		Timers[1]=[]() {                /// std::chrono::steady_clock ?
			LARGE_INTEGER cntr;
			return
				::QueryPerformanceCounter(&cntr)?
				cntr.LowPart:
				::GetTickCount();       /// снижает надежность генератора
		}();

		// Используем ID потока, чтобы исключить совпадения
		DWORD ThreadId=::GetCurrentThreadId();  /// std::this_thread::get_id();
		for ( auto &T: Timers ) T^=ThreadId;

		return Timers;
	}

	template <typename timers_cont>
	void UpdateEntropy(timers_cont Timers_) noexcept
	{
		std::lock_guard <std::mutex> lckObj(mtxEntropy);

		// Перестроим буфер энтропий
		Entropy.Update(Timers_);
	}

	template <typename timers_cont>
	auto GetEntropy(timers_cont Timers_)
	{
		std::lock_guard <std::mutex> lckObj(mtxEntropy);

		// Перестроим буфер энтропий
		Entropy.Update(Timers_);        /// медленно, зато безопасно (?)
		// Возьмем одну как результат...
		auto Res=Entropy.Get();
		// ... и перешифруем отдельно теми же таймерами
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
