//---------------------------------------------------------------------------
#ifndef UnitSyncMsgsH
#define UnitSyncMsgsH
//---------------------------------------------------------------------------
#include <cstdint>

#include "UnitAuth.h"
//---------------------------------------------------------------------------
struct MSyncData;
struct MSyncPHeader;
struct MSyncPHello;
struct MSyncPData;
struct MSyncPConf;
struct MSyncPWOL;
//---------------------------------------------------------------------------
#define MAC_AddrLength      6
//---------------------------------------------------------------------------
#pragma pack(push, 1)

struct MSyncData
{
	std::int64_t SystemTime;	// Системное время, используемое при всех расчетах
	std::uint8_t Number;		// Номер компьютера, с которым ассоциировано состояние
	std::uint32_t State;		// Состояние компьютера (режимы работы)
	std::int64_t StartWorkTime;	// Время запуска в работу (абсолютное время)
	std::int32_t SizeWorkTime;	// На какое время запущен (в минутах)       	/// int16_t ?
	std::int64_t StartFineTime;	// Время, когда был применен штраф (абсолютное время)
	std::int32_t SizeFineTime;	// Время ожидания в режиме штрафа (в минутах)   /// int16_t ?
	std::int64_t StopTimerTime;	// Время остановки отсчета времени (абсолютное время)
	std::uint32_t Programs;		// Групы программ, разрешенных для запуска
	std::uint32_t Commands;		// Дополнительные команды
};

struct MSyncPHeader
{
	std::uint8_t Version;		// Версия сетевого интерфейса
	std::uint8_t Type;			// Тип пакета
	std::uint64_t SessId;		// Сеансовый ID (функция от random сервера и клиента)
};

struct MSyncPHello
{
    MSyncPHeader Header;        // Заголовок
    unsigned char MAC[MAC_Size];// MAC пакета
};

struct MSyncPData
{
    MSyncPHeader Header;        // Заголовок
    MSyncData Data;             // Данные для синхронизации с клиентом
    unsigned char MAC[MAC_Size];// MAC пакета
};

struct MSyncPConf
{
    MSyncPHeader Header;        // Заголовок
    unsigned char MAC[MAC_Size];// MAC пакета
};

struct MSyncPWOL
{
	std::uint32_t ZeroFirst;					// Нули в начале пакета
	unsigned char Sync[MAC_AddrLength];			// Последовательность для синхронизации
	unsigned char Magic[16][MAC_AddrLength];	// 16 раз MAC-адрес
};

#pragma pack(pop)
//---------------------------------------------------------------------------
#endif
