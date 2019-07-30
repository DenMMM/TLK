//---------------------------------------------------------------------------
#ifndef UnitClOptionsH
#define UnitClOptionsH
//---------------------------------------------------------------------------
class MClOptions;
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#define mcoTransp       0x00000001      // Полупрозрачный shell
#define mcoAddRoute     0x00000002      // Управлять маршрутом по-умолчанию
#define mcoAutoRun      0x00000004      // Обрабатывать раздел автозапуска при входе пользователя
//---------------------------------------------------------------------------
#define MAX_ClUNameLen   104            // Длина логина пользователя, под которым будет запускаться TLK shell
//---------------------------------------------------------------------------
class MClOptions:public MSLList
{
private:
    // Заглушки, т.к. списка на самом деле нет - только "заголовок"
    MListItem *item_new(unsigned char TypeID_) const { return NULL; }
    void item_del(MListItem *Item_) const {}

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    char ShellUser[MAX_ClUNameLen+1];   // Логин пользователя для блокировки TLK shell
    short ToEndTime;                    // За сколько минут предупреждать об окончании времени (0-no show)
    short MessageTime;                  // Сколько секунд показывать предупреждение
    short MsgEndTime;                   // Время показа сообщения об окончании времени, секунд (0-no show)
    short RebootWait;                   // Секунд до перезагрузки после окончании времени (0-no reboot)
    short AutoLockTime;                 // Минут без связи с сервером до блокировки (0-no lock)
    unsigned Flags;                     // Остальные настройки - флаги

    void SetShellUser(const char *Name_);
    bool Copy(MClOptions *ClOptions_);

    MClOptions();
    ~MClOptions();
};
//---------------------------------------------------------------------------
#endif
