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
#define MAX_Fines           20      // Предел количества штрафов
#define MAX_FineDescrLen    50      // Допустимая длина описания штрафа
//---------------------------------------------------------------------------
class MFine:public MIDListItem
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    short Time;                             // Время штрафа
    std::string Descr;                      // Описание штрафа

    void Copy(const MListItem *SrcItem_);

    // Поддержка логов
    struct LogData
    {
        unsigned ID;                    // ID-номер штрафа
        std::string Descr;              // Описание штрафа

        LogData &operator=(const MFine &Fine_)
        {
            ID=Fine_.ItemID;
            Descr=Fine_.Descr;
            return *this;
        }
    };
    friend LogData;                     // Нужен дсотуп к "ItemID"

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

