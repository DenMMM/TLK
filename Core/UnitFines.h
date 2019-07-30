//---------------------------------------------------------------------------
#ifndef UnitFinesH
#define UnitFinesH
//---------------------------------------------------------------------------
struct MFineData;
class MFine;
class MFines;
//---------------------------------------------------------------------------
#include "UnitIDList.h"
//---------------------------------------------------------------------------
#define MAX_Fines           20      // ������ ���������� �������
#define MAX_FineDescrLen    50      // ���������� ����� �������� ������
//---------------------------------------------------------------------------
struct MFineData
{
    unsigned ID;                                // ID-����� ������
    char Descr[MAX_FineDescrLen+1];             // �������� ������
};
//---------------------------------------------------------------------------
class MFine:public MIDListItem
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    short Time;                             // ����� ������
    char Descr[MAX_FineDescrLen+1];         // �������� ������

    // ������ � ��������� ������
      char *sDescr(char *Descr_);

    void Copy(const MListItem *SrcItem_);

    // ��������� �����
    void GetFineData(MFineData *Data_) const;
    void SetFineData(MFineData *Data_);

    MFine();
    ~MFine();
};
//---------------------------------------------------------------------------
class MFines:public MIDList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MFine; }
    void item_del(MListItem *Item_) const { delete (MFine*)Item_; }

public:
    MFines() {}
    ~MFines() { Clear(); }
};
//---------------------------------------------------------------------------
#endif

