//---------------------------------------------------------------------------
#ifndef UnitClOptionsH
#define UnitClOptionsH
//---------------------------------------------------------------------------
class MClOptions;
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
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
    short RebootWait;                   // ������ �� ������������ ����� ����� (0-no reboot)
    short AutoLockTime;                 // ����� ��� ����� � �������� �� ���������� (0-no lock)

    bool Copy(MClOptions *ClOptions_);

    MClOptions();
    ~MClOptions();
};
//---------------------------------------------------------------------------
#endif
