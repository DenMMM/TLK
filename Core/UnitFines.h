//---------------------------------------------------------------------------
#ifndef UnitFinesH
#define UnitFinesH
//---------------------------------------------------------------------------
#include <string>

#include "UnitIDList.h"
//---------------------------------------------------------------------------
class MFinesItem;
class MFines;
//---------------------------------------------------------------------------
#define MAX_Fines           20      // ������ ���������� �������
#define MAX_FineDescrLen    50      // ���������� ����� �������� ������
//---------------------------------------------------------------------------
class MFinesItem:
	public MIDListItem::Simple <
		MIDListItem::Proxy <MIDListItem, MFinesItem>,
		MFinesItem>
{
public:
	// ������� ��������� ����������/�������� ������
	unsigned GetDataSize() const override;
	void *SetData(void *Data_) const override;
	const void *GetData(const void *Data_, const void *Limit_) override;

public:
	short Time;                   	// ����� ������
	std::wstring Descr;             // �������� ������

	void Copy(const MListItem *SrcItem_) override;

	// ��������� �����
	struct LogData
	{
		unsigned UUID;				// ID-����� ������
		std::wstring Descr;         // �������� ������

		LogData &operator=(const MFinesItem &Fine_)
		{
			UUID=Fine_.gUUID();
			Descr=Fine_.Descr;
			return *this;
		}
	};

	MFinesItem &operator=(const LogData &Data_)
	{
		UUID=Data_.UUID;
		Descr=Data_.Descr;
		return *this;
	}

	MFinesItem():
		Time(0)
	{
	}

	virtual ~MFinesItem()
	{
	}
};
//---------------------------------------------------------------------------
class MFines:
	public MIDList::Simple <MIDList, MFines, MFinesItem>
{
public:
	MFines() {}
	~MFines() {}
};
//---------------------------------------------------------------------------
#endif

