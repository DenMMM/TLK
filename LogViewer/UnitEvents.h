//---------------------------------------------------------------------------
#ifndef UnitEventsH
#define UnitEventsH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
//---------------------------------------------------------------------------
class MEvent;
class MEvents;
//---------------------------------------------------------------------------
class MEvent:public MListItem
{
public:
    double Time;  // ��������� �����
    int ComputerNumber;  // ����� ����������
    AnsiString TariffName;  // �������� ������
    int TimeSize;  // �� ����� �����
    double Cost;  // �� ����� �����

    MEvent();
    ~MEvent();
};
//---------------------------------------------------------------------------
class MEvents:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MEvent); }
    void delete_(MListItem *ListItem_) { delete ((MEvent*)ListItem_); }

    char *GetData(char *Data_, char *LimitData_);
public:
    double OpenTime;  //
    double CloseTime;  //
    AnsiString Name;  // ��� ���������
    AnsiString Job;  // ���������
    unsigned int Rights;  // �����

    bool Load(AnsiString FileName_);

    MEvents() { constructor(); }
    ~MEvents() { destructor(); }
};
//---------------------------------------------------------------------------
#endif
