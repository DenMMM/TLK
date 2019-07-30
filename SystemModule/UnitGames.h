//---------------------------------------------------------------------------
#ifndef UnitGamesH
#define UnitGamesH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
//---------------------------------------------------------------------------
class MGame;
class MGames;
//---------------------------------------------------------------------------
class MGame:public MListItem
{
public:
    AnsiString Name;
    AnsiString CommandLine;
    AnsiString IconFile;
    MGames *SubGames;

    MGame &operator=(MGame &Game_);

    MGame();
    ~MGame();
};
//---------------------------------------------------------------------------
class MGames:public MList
{
private:
    MListItem *new_() { return (MListItem*)(new MGame); }
    void delete_(MListItem *ListItem_) { delete ((MGame*)ListItem_); }
public:
    MGames &operator=(MGames &Games_);

    MGames() { constructor(); }
    ~MGames() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

