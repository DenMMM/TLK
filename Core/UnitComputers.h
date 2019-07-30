//---------------------------------------------------------------------------
#ifndef UnitComputersH
#define UnitComputersH
//---------------------------------------------------------------------------
class MComputer;
class MComputers;
//---------------------------------------------------------------------------
#include "UnitSLList.h"
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
class MComputer:public MSLListItem
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    char Number;                                // ����� ����������
    char Color;                                 // ���� ������
    char Address[MAX_CompAddrLen+1];            // IP-����� ����������
    bool NotUsed;                               // ������������ ���������

    char *SetAddress(char *Address_);
    void Copy(const MListItem *SrcItem_);

    MComputer();
    ~MComputer();
};
//---------------------------------------------------------------------------
class MComputers:public MSLList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MComputer; }
    void item_del(MListItem *Item_) const { delete (MComputer*)Item_; }

public:
    MComputer *Search(char Number_) const;
    void Sort();

    MComputers() {}
    ~MComputers() { Clear(); }
};
//---------------------------------------------------------------------------
#endif

