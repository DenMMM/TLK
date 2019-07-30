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
	public MSLListItem::Simple <
		MSLListItem::Proxy <MSLListItem, MGamesItem>,
		MGamesItem>
{
public:
	// Функции механизма сохранения/загрузки данных
	unsigned GetDataSize() const override;
	void *SetData(void *Data_) const override;
	const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::wstring Name;
	std::wstring Command;
	std::wstring Icon;
	MGames *SubGames;

	MGames *AddSubGames();
	void DelSubGames();
	void Copy(const MListItem *SrcItem_) override;

	MGamesItem():
		SubGames(nullptr)
	{
	}

	virtual ~MGamesItem();
};
//---------------------------------------------------------------------------
class MGames:
	public MSLList::Simple <MSLList, MGames, MGamesItem>
{
public:
	MGames() {}
	~MGames() {}
};
//---------------------------------------------------------------------------
#endif

