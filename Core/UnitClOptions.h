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
    // «аглушки, т.к. списка на самом деле нет - только "заголовок"
    MListItem *item_new(unsigned char TypeID_) const { return NULL; }
    void item_del(MListItem *Item_) const {}

    // ‘ункции механизма сохранени€/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    short ToEndTime;                    // «а сколько минут предупреждать об окончании времени (0-no show)
    short MessageTime;                  // —колько секунд показывать предупреждение
    short RebootWait;                   // —екунд до перезагрузки после этого (0-no reboot)
    short AutoLockTime;                 // ћинут без св€зи с сервером до блокировки (0-no lock)

    bool Copy(MClOptions *ClOptions_);

    MClOptions();
    ~MClOptions();
};
//---------------------------------------------------------------------------
#endif
