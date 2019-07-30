//---------------------------------------------------------------------------
#ifndef UnitFinesH
#define UnitFinesH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
//---------------------------------------------------------------------------
#define MAX_Fines           20      //
#define MAX_FineDescLength  30      //
//---------------------------------------------------------------------------
class MFine;
class MFines;
struct MFineData;
//---------------------------------------------------------------------------
class MFine:public MSLListItem
{
public:
    bool Copy(MListItem *SrcItem_);
public:
    unsigned ID;                                // ID-����� ������
    char Description[MAX_FineDescLength+1];     // �������� ������
    int Time;                                   // ����� ������

    char *SetDescription(char *Description_);

    // ��������� �����
    void GetFineData(MFineData *Data_);
    void SetFineData(MFineData *Data_);

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MFine();
    ~MFine();
};
//---------------------------------------------------------------------------
class MFines:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MFine; }
    void operator_delete(MListItem *DelItem_) { delete (MFine*)DelItem_; }
public:
    MFine *Search(unsigned ID_);

    MFines() { constructor(); }
    ~MFines() { destructor(); }
};
//---------------------------------------------------------------------------
struct MFineData
{
    unsigned ID;                                // ID-����� ������
    char Description[MAX_FineDescLength+1];     // �������� ������
};
//---------------------------------------------------------------------------
#endif

