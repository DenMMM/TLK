//---------------------------------------------------------------------------
#ifndef UnitTariffsH
#define UnitTariffsH
//---------------------------------------------------------------------------
#include "UnitListOfIDs.h"
//---------------------------------------------------------------------------
#define mwdMo ((unsigned int)1)
#define mwdTu ((unsigned int)2)
#define mwdWe ((unsigned int)4)
#define mwdTh ((unsigned int)8)
#define mwdFr ((unsigned int)16)
#define mwdSa ((unsigned int)32)
#define mwdSu ((unsigned int)64)
#define mwdAll ((unsigned int)127)
//---------------------------------------------------------------------------
#define mgp1 ((unsigned int)1)
#define mgp2 ((unsigned int)2)
#define mgp3 ((unsigned int)4)
#define mgp4 ((unsigned int)8)
#define mgp5 ((unsigned int)16)
#define mgp6 ((unsigned int)32)
#define mgp7 ((unsigned int)64)
//---------------------------------------------------------------------------
class MTariffTime;
class MTariffTimes;
class MTariff;
class MTariffs;
//---------------------------------------------------------------------------
class MTariffTime:public MListItem
{
public:
    int BeginTime;  // ����� ������ �������� ������ � ������� � ������ �����
    int SizeTime;  // ������������ �� ������� ������ � �������
    int EndTime;  // ����� ��������� �������� ������ � ������� � ������ �����
    unsigned int WeekDays; // ��� ������, ��� ������� ������ ���������
    double Cost;  // ��������� �� ������
    bool EnableDesc;  // ����������� ������ �� ��� �����
    MTariffTime &operator=(MTariffTime &TariffTime_);

    MTariffTime();
    ~MTariffTime();
};
//---------------------------------------------------------------------------
class MTariffTimes:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MTariffTime); }
    void delete_(MListItem *ListItem_) { delete (MTariffTime*)ListItem_; }
public:
    MTariffTimes &operator=(MTariffTimes &TariffTimes_);

    MTariffTimes() { constructor(); }
    ~MTariffTimes() { destructor(); }
};
//---------------------------------------------------------------------------
class MTariff:public MListItem
{
public:
    unsigned int ID;
    AnsiString Name;  // �������� ������
    AnsiString IconFileName;  // ��� ����� � ������� ��� ������
    int FaceType;  // ��� ���������� ���������� �����
    unsigned int GamesPages;  // ������� � ����������� ��� �������
    bool Reboot;  // �������������� ��������� ����� �������
    MTariffTimes Times;  // ���� ������ �� �������
    MListOfIDs Computers;  // ����������, � ������� �������� �����
    int IconNumber;  // ����� ������, �������������� � �������

    MTariff &operator=(MTariff &Tariff_);

    MTariff();
    ~MTariff();
};
//---------------------------------------------------------------------------
class MTariffs:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MTariff); }
    void delete_(MListItem *ListItem_) { delete (MTariff*)ListItem_; }
public:
    MTariff *SearchID(unsigned int ID_);

    MTariffs &operator=(MTariffs &Tariffs_);

    MTariffs() { constructor(); }
    ~MTariffs() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

