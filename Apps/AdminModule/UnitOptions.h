//---------------------------------------------------------------------------
#ifndef UnitOptionsH
#define UnitOptionsH
//---------------------------------------------------------------------------
class MOptions;
//---------------------------------------------------------------------------
#include "UnitCommon.h"
#include "UnitSLList.h"
#include "UnitLog.h"
#include "UnitPassword.h"
//---------------------------------------------------------------------------
#define MAX_OptPassLen          16          // Длина пароля на настройки
#define MAX_DialogTime          30          // Options.CostDialogTime
#define MAX_FilterFreeTime      60          // Options.FilterFreeTime
//---------------------------------------------------------------------------
// Main Users Rights
#define murPause                1           // Разрешить админам приостанавливать компьютеры
//---------------------------------------------------------------------------
class MOptions:public MSLList
{
private:
    // Заглушки, т.к. списка на самом деле нет - только "заголовок"
    MListItem *item_new(unsigned char TypeID_) const { return NULL; }
    void item_del(MListItem *Item_) const {}

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

    MPassword Pass;             // Пароль на изменение настроек

public:
    char LogPeriod;             // Период ведения файла лога
    short FilterFreeTime;       // Время до окончания работы компьютера, когда он подпадает под фильтр свободных (в минутах)
    short CostDialogTime;       // Время использования диалогов с расчетом цен (в минутах)
    double CostPrecision;       // Точность расчета цен
    unsigned UsersRights;       // Права пользователей (администраторов)

    void SetPass(char *Pass_) { Pass.Set(Pass_); }
    bool CheckPass(char *Pass_) { return Pass.Check(Pass_); }
    bool Copy(MOptions *Opt_);

    MOptions();
    ~MOptions();
};
//---------------------------------------------------------------------------
#endif

