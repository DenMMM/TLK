//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitGames.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MGame &MGame::operator=(MGame &Game_)
{
    Name=Game_.Name;
    CommandLine=Game_.CommandLine;
    IconFile=Game_.IconFile;
    if ( Game_.SubGames )
    {
        if ( SubGames==NULL ) SubGames=new MGames;
        *SubGames=*Game_.SubGames;
    } else
    {
        delete SubGames;
        SubGames=NULL;
    }
    return *this;
}
//---------------------------------------------------------------------------
MGame::MGame()
{
    SubGames=NULL;
}
//---------------------------------------------------------------------------
MGame::~MGame()
{
    delete SubGames;
}
//---------------------------------------------------------------------------
MGames &MGames::operator=(MGames &Games_)
{
    Clear();
    for ( MGame *Game, *Game_=(MGame*)Games_.FirstItem;
        Game_; Game_=(MGame*)Game_->NextItem )
    {
        Game=(MGame*)Add();
        *Game=*Game_;
    }
    return *this;
}
//---------------------------------------------------------------------------

