//---------------------------------------------------------------------------
#ifndef UnitTariffsH
#define UnitTariffsH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#define MAX_Tariffs             30
#define MAX_TariffTimes         50
#define MAX_TariffNameLength    40
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
#define mgpAll (mgp1|mgp2|mgp3|mgp4|mgp5|mgp6|mgp7|mgp8|mgpRoute)
//---------------------------------------------------------------------------
#define mttUndefined    0
#define mttHours        1
#define mttFlyPacket    2
#define mttPacket       3
//---------------------------------------------------------------------------
//#define Cost_Precision  0.50
//---------------------------------------------------------------------------
class MRunTime;
class MRunTimes;
class MTariffTime;
class MTariffTimes;
class MTariff;
class MTariffs;
class MTariffInfo;
class MTariffsInfo;
struct MTariffData;
//---------------------------------------------------------------------------
class MRunTime:public MListItem
{
public:
    bool Copy(MListItem *SrcItem_);
public:
    unsigned TariffID;      // ID-����� ������
    int Number;             // ����� ����������
    __int64 StartTime;      // ����� ������ ������
    int Type;               // =
    int BeginTime;          // =    ���������� � ������
    int EndTime;            // =    � ����� ��������� ������
    int SizeTime;           // =
    int WorkTime;           // ������� ������� ������� �������� �������� �� ������ � ��� ������
    int MaxTime;            // ����������� �� ������������ ����� ������ (�������� ����� ��������)
    double Cost;            // ��������� ������ �� ���������� � ������� 'WorkTime'

    MRunTime();
    ~MRunTime();
};
//---------------------------------------------------------------------------
class MRunTimes:public MList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MRunTime; }
    void operator_delete(MListItem *DelItem_) { delete (MRunTime*)DelItem_; }
public:
    MRunTimes() { constructor(); }
    ~MRunTimes() { destructor(); }
};
//---------------------------------------------------------------------------
class MTariffTime:public MSLListItem
{
public:
    bool Copy(MListItem *SrcItem_);
public:
    int Type;       // ��� ������: ���������, �����, "���������" �����
    int BeginTime;  // ����� ������ �������� ������ � ������� � ������ �����
    int EndTime;    // ����� ��������� �������� ������ � ������� � ������ �����
    int SizeTime;   // ������������ �� ������� ������ � �������
    double Cost;    // ���������

    int MaxWorkTime(int Time_);

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MTariffTime();
    ~MTariffTime();
};
//---------------------------------------------------------------------------
class MTariffTimes:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MTariffTime; }
    void operator_delete(MListItem *DelItem_) { delete (MTariffTime*)DelItem_; }
public:
    MTariffTimes() { constructor(); }
    ~MTariffTimes() { destructor(); }
};
//---------------------------------------------------------------------------
class MTariff:public MSLListItem
{
public:
    bool Copy(MListItem *SrcItem_);
private:
    void CostPacket(MRunTime *RunTime_);
    void CostFlyPacket(MRunTime *RunTime_);
    void CostHours(MRunTime *RunTime_);
public:
    unsigned ID;                        // ID-����� ������
    char Name[MAX_TariffNameLength+1];  // �������� ������
    unsigned Programs;                  // ������ �������� ��� �������
    bool Reboot;                        // �������������� ��������� ����� �������
    MTariffTimes Times;                 // ���� ������ �� �������
    int ComputersNum;                   // ����� ������� � �������
    int *Computers;                     // ������ ������� �����������, � ������� �������� �����

    char *SetName(char *Name_);
    int *SetComputers(int *Computers_, int Num_);
    bool CheckForTime(__int64 &Time_);
    bool CheckForComputer(int Number_);
    void GetRunTimes(__int64 &Time_, MRunTimes *RunTimes_);
    void Cost(MRunTime *RunTime_, double Prec_);
    bool GetInfo(MTariffInfo *Info_);

    // ��������� �����
    void GetTariffData(MTariffData *Data_);
    void SetTariffData(MTariffData *Data_);

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MTariff();
    ~MTariff();
};
//---------------------------------------------------------------------------
class MTariffs:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MTariff; }
    void operator_delete(MListItem *DelItem_) { delete (MTariff*)DelItem_; }
public:
    bool Copy(MList *SrcList_)
        { LastID=((MTariffs*)SrcList_)->LastID; return MList::Copy(SrcList_); }
private:
    unsigned LastID;
    unsigned NextID();
public:
    MTariff *Search(unsigned ID_);
    void SetIDs();
    void GetForTime(__int64 &Time_, MTariffsInfo *TariffsInfo_);

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() { return sizeof(LastID); }
    char *SetData(char *Data_) { return MemSet(Data_,LastID); }
    char *GetData(char *Data_, char *Limit_) { return MemGet(Data_,&LastID,Limit_); }

    MTariffs() { constructor(); LastID=0; }
    ~MTariffs() { destructor(); }
};
//---------------------------------------------------------------------------
class MTariffInfo:public MListItem
{
public:
    bool Copy(MListItem *SrcItem_);
public:
    unsigned ID;
    char Name[MAX_TariffNameLength+1];

    char *SetName(char *Name_);

    MTariffInfo();
    ~MTariffInfo();
};
//---------------------------------------------------------------------------
class MTariffsInfo:public MList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MTariffInfo; }
    void operator_delete(MListItem *DelItem_) { delete (MTariffInfo*)DelItem_; }
public:
    MTariffInfo *Search(unsigned ID_);

    MTariffsInfo() { constructor(); }
    ~MTariffsInfo() { destructor(); }
};
//---------------------------------------------------------------------------
struct MTariffData
{
    unsigned ID;                        // ID-����� ������
    char Name[MAX_TariffNameLength+1];  // �������� ������
};
//---------------------------------------------------------------------------
#endif

