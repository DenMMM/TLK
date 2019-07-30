//---------------------------------------------------------------------------
#ifndef UnitClOptionsH
#define UnitClOptionsH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
class MClOptions:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return NULL; }
    void operator_delete(MListItem *DelItem_) { }
public:
    unsigned ToEndTime;
    unsigned MessageTime;
    unsigned RebootWait;
    unsigned AutoLockTime;

    bool Copy(MClOptions *ClOptions_);

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MClOptions();
    ~MClOptions();
};
//---------------------------------------------------------------------------
#endif

