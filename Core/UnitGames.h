//---------------------------------------------------------------------------
#ifndef UnitGamesH
#define UnitGamesH
//---------------------------------------------------------------------------
#include <string>

#include "UnitSLList.h"
//---------------------------------------------------------------------------
class MGamesItem;
class MGames;
//---------------------------------------------------------------------------
#define MAX_PrgNameLength   32      // Длина названия игры
#define MAX_PrgCmdLength    128     // Длина пути к файлу игры
#define MAX_PrgIconLength   128     // Длина пути к файлу иконки
#define MAX_PrgLevel        2       // Количество уровней в списке
//---------------------------------------------------------------------------
class MGamesItem:
	public MSLListItemSimple <
		MSLListItem <MGames, MGamesItem>,
		MGamesItem>
{
public:
	// Функции механизма сохранения/загрузки данных
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::wstring Name;
	std::wstring Command;
	std::wstring Icon;
	MGames *SubGames;               /// Теперь не работает !!! Заменить smart-pointer

	MGames *AddSubGames();
	void DelSubGames();

	MGamesItem():
		SubGames(nullptr)
	{
	}

	MGamesItem(MGamesItem&) = default;
	MGamesItem(MGamesItem&&) = default;
	MGamesItem& operator=(MGamesItem&) = default;
	MGamesItem& operator=(MGamesItem&&) noexcept = default;
	~MGamesItem();
};
//---------------------------------------------------------------------------
class MGames:
	public MSLListSimple <
		MSLList <MGames, MGamesItem>,
		MGamesItem>
{
};
//---------------------------------------------------------------------------
#endif

