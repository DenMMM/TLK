//---------------------------------------------------------------------------
#ifndef UnitOptionsH
#define UnitOptionsH
//---------------------------------------------------------------------------
#include "UnitCommon.h"
#include "UnitSLList.h"
#include "UnitLog.h"
#include "UnitPassword.h"
//---------------------------------------------------------------------------
class MOptions;
//---------------------------------------------------------------------------
#define MAX_OptPassLen          16          // ����� ������ �� ���������
#define MAX_DialogTime          30          // Options.CostDialogTime
#define MAX_FilterFreeTime      60          // Options.FilterFreeTime
//---------------------------------------------------------------------------
// Main Users Rights
#define murPause                1           // ��������� ������� ���������������� ����������
//---------------------------------------------------------------------------
class MOptionsStub:
	public MSLListItem <MOptions, MOptionsStub> {};

class MOptions:
	public MSLList <MOptions, MOptionsStub>       /// private ?
{
public:
	std::uint8_t LogPeriod;			// ������ ������� ����� ����
	std::int16_t FilterFreeTime;	// ����� �� ��������� ������ ����������, ����� �� ��������� ��� ������ ��������� (� �������)
	std::int16_t CostDialogTime;	// ����� ������������� �������� � �������� ��� (� �������)
	double CostPrecision;			// �������� ������� ���
	std::uint32_t UsersRights;		// ����� ������������� (���������������)
	MPassword Pass;					// ������ �� ��������� ��������

	// ������� ��������� ����������/�������� ������
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

	MOptions():
		LogPeriod(mlpDay),
		FilterFreeTime(2),
		CostDialogTime(15),
		CostPrecision(0.50),
		UsersRights(murPause)
	{
	}
};
//---------------------------------------------------------------------------
#endif

