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
    // Функции механизма сохранения/загрузки данных
	unsigned GetDataSize() const override;
	void *SetData(void *Data_) const override;
	const void *GetData(const void *Data_, const void *Limit_) override;

public:
    char LogPeriod;             // Период ведения файла лога
    short FilterFreeTime;       // Время до окончания работы компьютера, когда он подпадает под фильтр свободных (в минутах)
    short CostDialogTime;       // Время использования диалогов с расчетом цен (в минутах)
    double CostPrecision;       // Точность расчета цен
    unsigned UsersRights;       // Права пользователей (администраторов)
    MPassword Pass;             // Пароль на изменение настроек

    bool Copy(MOptions *Opt_);

	MOptions():
		LogPeriod(mlpDay),
		FilterFreeTime(2),
		CostDialogTime(15),
		CostPrecision(0.50),
		UsersRights(murPause)
	{
	}

	~MOptions()
	{
	}
};
//---------------------------------------------------------------------------
#endif

