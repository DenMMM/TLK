//---------------------------------------------------------------------------
#ifndef UnitGamesH
#define UnitGamesH
//---------------------------------------------------------------------------
#include <string>
#include "UnitSLList.h"
//---------------------------------------------------------------------------
#define MAX_PrgNameLength   32      // Длина названия игры
#define MAX_PrgCmdLength    128     // Длина пути к файлу игры
#define MAX_PrgIconLength   128     // Длина пути к файлу иконки
#define MAX_PrgLevel        2       // Количество уровней в списке
//---------------------------------------------------------------------------
class MGame;
class MGames;
//---------------------------------------------------------------------------
class MGame:public MSLListItem
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    std::string Name;
    std::string Command;
    std::string Icon;
    MGames *SubGames;

    MGames *AddSubGames();
    void DelSubGames();
    void Copy(const MListItem *SrcItem_);

    MGame();
    ~MGame();
};
//---------------------------------------------------------------------------
class MGames:public MSLList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MGame; }
    void item_del(MListItem *Item_) const { delete (MGame*)Item_; }

public:
    MGames() {}
    ~MGames() { Clear(); }
};
//---------------------------------------------------------------------------
#endif

