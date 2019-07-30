//---------------------------------------------------------------------------
#ifndef UnitSyncMsgsH
#define UnitSyncMsgsH
//---------------------------------------------------------------------------
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
    __int64 SystemTime;         // Системное время, используемое при всех расчетах
    char Number;                // Номер компьютера, с которым ассоциировано состояние
    unsigned State;             // Состояние компьютера (режимы работы)
    __int64 StartWorkTime;      // Время запуска в работу (абсолютное время)
    int SizeWorkTime;           // На какое время запущен (в минутах)
    __int64 StartFineTime;      // Время, когда был применен штраф (абсолютное время)
    int SizeFineTime;           // Время ожидания в режиме штрафа (в минутах)
    __int64 StopTimerTime;      // Время остановки отсчета времени (абсолютное время)
    unsigned Programs;          // Групы программ, разрешенных для запуска
    unsigned Commands;          // Дополнительные команды
};

struct MSyncPHeader
{
    unsigned char Version;      // Версия сетевого интерфейса
    unsigned char Type;         // Тип пакета
    unsigned int Seed;          // Сеансовый ID (функция от random сервера и клиента)
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
    unsigned int ZeroFirst;                     // Нули в начале пакета
    unsigned char Sync[MAC_AddrLength];         // Последовательность для синхронизации
    unsigned char Magic[16][MAC_AddrLength];    // 16 раз MAC-адрес
};

#pragma pack(pop)
//---------------------------------------------------------------------------
#endif
