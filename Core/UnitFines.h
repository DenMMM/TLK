//---------------------------------------------------------------------------
#ifndef UnitFinesH
#define UnitFinesH
//---------------------------------------------------------------------------
class MFine;
class MFines;
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
#include "UnitIDList.h"
//---------------------------------------------------------------------------
#define MAX_Fines           20      // ������ ���������� �������
#define MAX_FineDescrLen    50      // ���������� ����� �������� ������
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
    std::string Descr;                      // �������� ������

    void Copy(const MListItem *SrcItem_);

    // ��������� �����
    struct LogData
    {
        unsigned ID;                    // ID-����� ������
        std::string Descr;              // �������� ������

        LogData &operator=(const MFine &Fine_)
        {
            ID=Fine_.ItemID;
            Descr=Fine_.Descr;
            return *this;
        }
    };
    friend LogData;                     // ����� ������ � "ItemID"

    MFine &operator=(const LogData &Data_)
    {
        ItemID=Data_.ID;
        Descr=Data_.Descr;
        return *this;
    }

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

