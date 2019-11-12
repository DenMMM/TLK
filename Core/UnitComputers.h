//---------------------------------------------------------------------------
#ifndef UnitComputersH
#define UnitComputersH
//---------------------------------------------------------------------------
#include <string>

#include "UnitSLList.h"
//---------------------------------------------------------------------------
class MComputersItem;
class MComputers;
//---------------------------------------------------------------------------
#define MAX_Comps                   50      // ������� ����������� ��������� ����������� TLK
#define MAX_CompAddrLen             15      // ����� IP-������
//---------------------------------------------------------------------------
#define mgcNone                     0       // ��� �������� �����
#define mgcAqua                     1       // �������
#define mgcGreen                    2       // �������
#define mgcRed                      3       // �������
#define mgcYellow                   4       // ������
//---------------------------------------------------------------------------
class MComputersItem:
	public MSLListItemSimple <
		MSLListItem <MComputers, MComputersItem>,
		MComputersItem>
{
public:
	// ������� ��������� ����������/�������� ������
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	char Number;						// ����� ����������
	char Color;							// ���� ������
	std::wstring Address;				// IP-����� ����������
	bool NotUsed;						// ������������ ���������

	MComputersItem():
		Number(0),
		Color(mgcNone),
		NotUsed(false)
	{
	}
};
//---------------------------------------------------------------------------
class MComputers:
	public MSLListSimple <
		MSLList <MComputers, MComputersItem>,
		MComputersItem>
{
public:
	const_iterator Search(char Number_) const;
	iterator Search(char Number_)
	{
		return const_cast_iter(
			const_cast<const MComputers*>(this)->Search(Number_)
			);
	}
};
//---------------------------------------------------------------------------
#endif

