//---------------------------------------------------------------------------
#ifndef UnitGamesH
#define UnitGamesH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
//---------------------------------------------------------------------------
#define MAX_PrgNameLength   32      // ����� �������� ����
#define MAX_PrgCmdLength    128     // ����� ���� � ����� ����
#define MAX_PrgIconLength   128     // ����� ���� � ����� ������
#define MAX_PrgLevel        2       // ���������� ������� � ������
//---------------------------------------------------------------------------
class MGame;
class MGames;
//---------------------------------------------------------------------------
class MGame:public MSLListItem
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);


public:
    char Name[MAX_PrgNameLength+1];
    char Command[MAX_PrgCmdLength+1];
    char Icon[MAX_PrgIconLength+1];
    MGames *SubGames;

    bool SetName(char *Name_);
    bool SetCommand(char *Command_);
    bool SetIcon(char *Icon_);
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

