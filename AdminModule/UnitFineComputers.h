//---------------------------------------------------------------------------
#ifndef UnitFineComputersH
#define UnitFineComputersH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
#include "UnitComputers.h"
//---------------------------------------------------------------------------
class MFineComputer;
class MFineComputers;
//---------------------------------------------------------------------------
class MFineComputer:public MListItem
{
public:
    MComputer *Computer;  // ���������, � �������� ����������� �����
    int FineTime;  // ����� ������ � �������
    bool EnableReturnMany;  // ���������� �� ������ ��� ������ ������ �������
    double ReturnMany;  // ����� ������������ �����

    MFineComputer &operator=(MFineComputer &FineComputer_);

    MFineComputer();
    ~MFineComputer();
};
//---------------------------------------------------------------------------
class MFineComputers:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MFineComputer); }
    void delete_(MListItem *ListItem_) { delete (MFineComputer*)ListItem_; }
public:
    MFineComputers() { constructor(); }
    ~MFineComputers() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

