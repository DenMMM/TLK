//---------------------------------------------------------------------------
#ifndef UnitClOptionsH
#define UnitClOptionsH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
class MClOptions;
//---------------------------------------------------------------------------
#define mcoTransp       0x00000001      // �������������� shell
#define mcoAddRoute     0x00000002      // ��������� ��������� ��-���������
#define mcoAutoRun      0x00000004      // ������������ ������ ����������� ��� ����� ������������
//---------------------------------------------------------------------------
#define MAX_ClUNameLen   104            // ����� ������ ������������, ��� ������� ����� ����������� TLK shell
//---------------------------------------------------------------------------
class MClOptionsStub:
	public MSLListItem <MClOptions, MClOptionsStub> {};

class MClOptions:
	public MSLList <MClOptions, MClOptionsStub> 	/// private ?
{
public:
    // ������� ��������� ����������/�������� ������
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::wstring ShellUser;   			// ����� ������������ ��� ���������� TLK shell
	short ToEndTime;                    // �� ������� ����� ������������� �� ��������� ������� (0-no show)
	short MessageTime;                  // ������� ������ ���������� ��������������
	short MsgEndTime;                   // ����� ������ ��������� �� ��������� �������, ������ (0-no show)
	short RebootWait;                   // ������ �� ������������ ����� ��������� ������� (0-no reboot)
	short AutoLockTime;                 // ����� ��� ����� � �������� �� ���������� (0-no lock)
	unsigned Flags;                     // ��������� ��������� - �����

	void SetShellUser(const std::wstring &Name_);

	MClOptions():
		ToEndTime(2),
		MessageTime(15),
		MsgEndTime(10),
		RebootWait(20),
		AutoLockTime(15),
		Flags(0)
	{
	}
};
//---------------------------------------------------------------------------
#endif
