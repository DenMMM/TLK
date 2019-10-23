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
	public MIDListItemSimple <
		MIDListItem <MFines, MFinesItem>,
		MFinesItem>
{
public:
	// ������� ��������� ����������/�������� ������
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	short Time;                   	// ����� ������
	std::wstring Descr;             // �������� ������

	// ��������� �����
	struct LogData
	{
		unsigned UUID;				// ID-����� ������
		std::wstring Descr;         // �������� ������
	};

	LogData gLogData() const
	{
		LogData ld;
		ld.UUID=gUUID();
		ld.Descr=Descr;

		return ld;
	}

	void sFromLog(const LogData &Data_)
	{
		UUID=Data_.UUID;
		Descr=Data_.Descr;
	}

	MFinesItem():
		Time(0)
	{
	}
};
//---------------------------------------------------------------------------
class MFines:
	public MIDListSimple <
		MIDList <MFines, MFinesItem>,
		MFinesItem>
{
};
//---------------------------------------------------------------------------
#endif

