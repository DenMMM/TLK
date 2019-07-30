//---------------------------------------------------------------------------
#ifndef UnitGamesH
#define UnitGamesH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
//---------------------------------------------------------------------------
#define MAX_PrgNameLength   32
#define MAX_PrgCmdLength    128
#define MAX_PrgIconLength   128
//---------------------------------------------------------------------------
#define mgpNone     0
#define mgp1        1
#define mgp2        2
#define mgp3        4
#define mgp4        8
#define mgp5        16
#define mgp6        32
#define mgp7        64
#define mgp8        128
#define mgpRoute    256
#define mgpAll (mgp1|mgp2|mgp3|mgp4|mgp5|mgp6|mgp7|mgp8|mgpRoute)
//---------------------------------------------------------------------------
class MGame;
class MGames;
//---------------------------------------------------------------------------
class MGame:public MSLListItem
{
public:
    bool Copy(MListItem *SrcItem_);
public:
    char Name[MAX_PrgNameLength+1];
    char Command[MAX_PrgCmdLength+1];
    char Icon[MAX_PrgIconLength+1];
    MGames *SubGames;

    bool SetName(char *Name_);
    bool SetCommand(char *Command_);
    bool SetIcon(char *Icon_);

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MGame();
    ~MGame();
};
//---------------------------------------------------------------------------
class MGames:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MGame; }
    void operator_delete(MListItem *DelItem_) { delete (MGame*)DelItem_; }
public:
    MGames() { constructor(); }
    ~MGames() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

