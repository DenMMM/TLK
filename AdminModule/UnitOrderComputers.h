//---------------------------------------------------------------------------
#ifndef UnitOrderComputersH
#define UnitOrderComputersH
//---------------------------------------------------------------------------
#include "UnitComputers.h"
#include "UnitTariffs.h"
#include "UnitListOfComputers.h"
//---------------------------------------------------------------------------
class MOrderComputer;
class MOrderComputers;
//---------------------------------------------------------------------------
class MOrderComputer:public MListItem
{
public:
    MComputer *Computer;  // ���������, ��� �������� �������� �����
    MTariff *Tariff;  // �� ������ ������ �������� �����
    int TimeWorkSize;  // �� ����� ����� ���������� ��������� (� �������)
    int Discount;  // ������ ��� �������� ��������� ������ (� ���������)
    double Cost;  // ������������ ��������� ������

    MOrderComputer &operator=(MOrderComputer &OrderComputer_);

    MOrderComputer();
    ~MOrderComputer();
};
//---------------------------------------------------------------------------
class MOrderComputers:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MOrderComputer); }
    void delete_(MListItem *ListItem_) { delete (MOrderComputer*)ListItem_; }
public:
    MOrderComputers() { constructor(); }
    ~MOrderComputers() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

