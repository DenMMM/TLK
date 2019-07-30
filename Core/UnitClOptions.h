//---------------------------------------------------------------------------
#ifndef UnitClOptionsH
#define UnitClOptionsH
//---------------------------------------------------------------------------
class MClOptions;
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#define mcoTransp       0x00000001      // �������������� shell
//---------------------------------------------------------------------------
class MClOptions:public MSLList
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
    short ToEndTime;                    // �� ������� ����� ������������� �� ��������� ������� (0-no show)
    short MessageTime;                  // ������� ������ ���������� ��������������
    short MsgEndTime;                   // ����� ������ ��������� �� ��������� �������, ������ (0-no show)
    short RebootWait;                   // ������ �� ������������ ����� ��������� ������� (0-no reboot)
    short AutoLockTime;                 // ����� ��� ����� � �������� �� ���������� (0-no lock)
    unsigned Flags;                     // ��������� ��������� - �����

    bool Copy(MClOptions *ClOptions_);

    MClOptions();
    ~MClOptions();
};
//---------------------------------------------------------------------------
#endif
