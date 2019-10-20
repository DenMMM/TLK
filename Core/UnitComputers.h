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
	public MSLListItem::Simple <
		MSLListItem::Proxy <MSLListItem, MComputersItem>,
		MComputersItem>
{
public:
	// ������� ��������� ����������/�������� ������
	unsigned GetDataSize() const override;
	void *SetData(void *Data_) const override;
	const void *GetData(const void *Data_, const void *Limit_) override;

public:
	char Number;						// ����� ����������
	char Color;							// ���� ������
	std::wstring Address;				// IP-����� ����������
	bool NotUsed;						// ������������ ���������

	void Copy(const MListItem *SrcItem_) override;

	MComputersItem():
		Number(0),
		Color(mgcNone),
		NotUsed(false)
	{
	}

	virtual ~MComputersItem()
	{
	}
};
//---------------------------------------------------------------------------
class MComputers:
	public MSLList::Simple <MSLList, MComputers, MComputersItem>
{
public:
	MComputersItem *Search(char Number_) const;
	void Sort();

	MComputers() {}
	~MComputers() {}
};
//---------------------------------------------------------------------------
#endif

