//---------------------------------------------------------------------------
#ifndef UnitTariffsH
#define UnitTariffsH
//---------------------------------------------------------------------------
struct MTariffData;
class MRunTime;
class MRunTimes;
class MTariffTime;
class MTariffTimes;
class MTariff;
class MTariffs;
class MTariffInfo;          /// только для TFormRun
class MTariffsInfo;         /// больше нигде не используются
//---------------------------------------------------------------------------
#include "UnitComputers.h"
#include "UnitIDList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#define MAX_Tariffs             30
#define MAX_TariffTimes         20
#define MAX_TariffNameLen       50
#define MAX_TariffTimeCost      1000000.
//---------------------------------------------------------------------------
#define mgpNone     0
#define mgp1        1
#define mgp2        2
#define mgp3        4
#define mgp4        8
#define mgp5        16
#define mgp6        32
#define mgp7        64
#define mgp8        128
#define mgpRoute    256
#define mgpDesktop  512
#define mgpAll (mgp1|mgp2|mgp3|mgp4|mgp5|mgp6|mgp7|mgp8|mgpRoute)
//---------------------------------------------------------------------------
#define mttUndefined    0
#define mttHours        1       // Почасовой
#define mttFlyPacket    2       // Пакет N часов
#define mttPacket       3       // Пакет с A до B часов (ночь, например)
//---------------------------------------------------------------------------
//#define Cost_Precision  0.50
//---------------------------------------------------------------------------
struct MTariffData
{
    unsigned ID;                        // ID-номер тарифа
    char Name[MAX_TariffNameLen+1];     // Название тарифа
};
//---------------------------------------------------------------------------
class MRunTime:public MListItem
{
public:
    unsigned TariffID;      // ID-номер тарифа
    char Number;            // Номер компьютера
    __int64 StartTime;      // Время начала работы
    char Type;              // =
    short BeginTime;        // =    Информация о пакете
    short EndTime;          // =    и время почасовой работы
    short SizeTime;         // =
    short WorkTime;         // Сколько времени реально возможно работать по тарифу и его пакету
    short MaxTime;          // Ограничение на максимальное время работы (задается перед расчетом)
    double Cost;            // Стоимость работы на компьютере в течение 'WorkTime'

    void Copy(const MListItem *SrcItem_);

    MRunTime();
    ~MRunTime();
};
//---------------------------------------------------------------------------
class MRunTimes:public MList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MRunTime; }
    void item_del(MListItem *Item_) const { delete (MRunTime*)Item_; }

public:
    MRunTimes() {}
    ~MRunTimes() { Clear(); }
};
//---------------------------------------------------------------------------
class MTariffTime:public MSLListItem
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    char Type;              // Тип записи: почасовой, пакет, "плавающий" пакет
    short BeginTime;        // Время начала действия тарифа в минутах с начала суток
    short EndTime;          // Время окончания действия тарифа в минутах с начала суток
    short SizeTime;         // Длительность по времени тарифа в минутах
    double Cost;            // Стоимость

    int MaxWorkTime(int Time_) const;
    void Copy(const MListItem *SrcItem_);

    MTariffTime();
    ~MTariffTime();
};
//---------------------------------------------------------------------------
class MTariffTimes:public MSLList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MTariffTime; }
    void item_del(MListItem *Item_) const { delete (MTariffTime*)Item_; }

public:
    MTariffTimes() {}
    ~MTariffTimes() { Clear(); }
};
//---------------------------------------------------------------------------
class MTariffInfo:public MListItem
{
public:
    unsigned ID;
    char Name[MAX_TariffNameLen+1];

    char *SetName(const char *Name_);
    void Copy(const MListItem *SrcItem_);

    MTariffInfo();
    ~MTariffInfo();
};
//---------------------------------------------------------------------------
class MTariffsInfo:public MList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MTariffInfo; }
    void item_del(MListItem *Item_) const { delete (MTariffInfo*)Item_; }

public:
    bool Copy(MListItem *SrcItem_);
    MTariffInfo *Search(unsigned ID_) const;

    MTariffsInfo() {}
    ~MTariffsInfo() { Clear(); }
};
//---------------------------------------------------------------------------
class MTariff:public MIDListItem
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

    void CostPacket(MRunTime *RunTime_) const;
    void CostFlyPacket(MRunTime *RunTime_) const;
    void CostHours(MRunTime *RunTime_) const;

    char CompsCnt;                      // Число компьютеров, к которым применим тариф
    char Comps[MAX_Comps];              // Их номера

public:
    char Name[MAX_TariffNameLen+1];     // Название тарифа
    unsigned Programs;                  // Группы программ для запуска
    bool Reboot;                        // Пререзагружать компьютер после запуска
    MTariffTimes Times;                 // Типы тарифа по времени

    char *SetName(const char *Name_);
    bool SetComps(char *Comps_, int Count_);
    // Проверяет есть ли для заданного времени пакеты по тарифу
    bool CheckForTime(__int64 &Time_) const;
    // Проверяет применим ли тариф к компьютеру
    bool CheckForComp(char Num_) const;
    //
    bool GetRunTimes(__int64 &Time_, MRunTimes *RunTimes_) const;
    void Cost(MRunTime *RunTime_, double Prec_) const;
    bool GetInfo(MTariffInfo *Info_) const;

    void Copy(const MListItem *SrcItem_);

    // Поддержка логов
    void GetTariffData(MTariffData *Data_) const;
    void SetTariffData(MTariffData *Data_);

    MTariff();
    ~MTariff();
};
//---------------------------------------------------------------------------
class MTariffs:public MIDList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MTariff; }
    void item_del(MListItem *Item_) const { delete (MTariff*)Item_; }

public:
    void GetForTime(__int64 &Time_, MTariffsInfo *TariffsInfo_) const;

    MTariffs() {}
    ~MTariffs() { Clear(); }
};
//---------------------------------------------------------------------------
#endif

