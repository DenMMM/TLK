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
    // ������� ��������� ����������/�������� ������
	unsigned GetDataSize() const override;
	void *SetData(void *Data_) const override;
	const void *GetData(const void *Data_, const void *Limit_) override;

public:
    char LogPeriod;             // ������ ������� ����� ����
    short FilterFreeTime;       // ����� �� ��������� ������ ����������, ����� �� ��������� ��� ������ ��������� (� �������)
    short CostDialogTime;       // ����� ������������� �������� � �������� ��� (� �������)
    double CostPrecision;       // �������� ������� ���
    unsigned UsersRights;       // ����� ������������� (���������������)
    MPassword Pass;             // ������ �� ��������� ��������

    bool Copy(MOptions *Opt_);

	MOptions():
		LogPeriod(mlpDay),
		FilterFreeTime(2),
		CostDialogTime(15),
		CostPrecision(0.50),
		UsersRights(murPause)
	{
	}

	~MOptions()
	{
	}
};
//---------------------------------------------------------------------------
#endif

