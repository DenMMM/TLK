//---------------------------------------------------------------------------
#ifndef UnitOptionsH
#define UnitOptionsH
//---------------------------------------------------------------------------
class MOptions;
//---------------------------------------------------------------------------
#include "UnitCommon.h"
#include "UnitSLList.h"
#include "UnitLog.h"
#include "UnitPassword.h"
//---------------------------------------------------------------------------
#define MAX_OptPassLen          16          // ����� ������ �� ���������
#define MAX_DialogTime          30          // Options.CostDialogTime
#define MAX_FilterFreeTime      60          // Options.FilterFreeTime
//---------------------------------------------------------------------------
// Main Users Rights
#define murPause                1           // ��������� ������� ���������������� ����������
//---------------------------------------------------------------------------
class MOptions:public MSLList
{
private:
    // ��������, �.�. ������ �� ����� ���� ��� - ������ "���������"
    MListItem *item_new(unsigned char TypeID_) const { return NULL; }
    void item_del(MListItem *Item_) const {}

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    char LogPeriod;             // ������ ������� ����� ����
    short FilterFreeTime;       // ����� �� ��������� ������ ����������, ����� �� ��������� ��� ������ ��������� (� �������)
    short CostDialogTime;       // ����� ������������� �������� � �������� ��� (� �������)
    double CostPrecision;       // �������� ������� ���
    unsigned UsersRights;       // ����� ������������� (���������������)
    MPassword Pass;             // ������ �� ��������� ��������

    bool Copy(MOptions *Opt_);

    MOptions();
    ~MOptions();
};
//---------------------------------------------------------------------------
#endif

