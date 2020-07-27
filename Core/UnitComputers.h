//---------------------------------------------------------------------------
#ifndef UnitComputersH
#define UnitComputersH
//---------------------------------------------------------------------------
#include <string>
#include <cstdint>

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
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::int8_t Number;					// ����� ����������
	std::uint8_t Color;					// ���� ������
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
	const_iterator Search(std::int8_t Number_) const;
	iterator Search(std::int8_t Number_)
	{
		const auto *const_this=this;
		return const_cast_iter(const_this->Search(Number_));
	}
};
//---------------------------------------------------------------------------
#endif

