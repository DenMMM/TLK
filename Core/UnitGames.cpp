//---------------------------------------------------------------------------
#include <string.h>
#pragma hdrstop

#include "UnitGames.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MGame::MGame()
{
    *Name=*Command=*Icon=0;
    SubGames=new MGames;
}

MGame::~MGame()
{
    delete SubGames;
}

bool MGame::Copy(MListItem *SrcItem_)
{
    MGame *Game_=(MGame*)SrcItem_;
    strcpy(Name,Game_->Name);
    strcpy(Command,Game_->Command);
    strcpy(Icon,Game_->Icon);
    return SubGames->Copy(Game_->SubGames);
}

unsigned MGame::GetDataSize()
{
    return
        strlen(Name)+1+
        strlen(Command)+1+
        strlen(Icon)+1+
        SubGames->GetAllDataSize(true);
}

char *MGame::SetData(char *Data_)
{
    Data_=MemSetCLine(Data_,Name);
    Data_=MemSetCLine(Data_,Command);
    Data_=MemSetCLine(Data_,Icon);
    Data_=SubGames->SetAllData(Data_,true);
    return Data_;
}

char *MGame::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGetCLine(Data_,Name,MAX_PrgNameLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetCLine(Data_,Command,MAX_PrgCmdLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetCLine(Data_,Icon,MAX_PrgIconLength,Limit_))==NULL ) goto error;
    if ( (Data_=SubGames->GetAllData(Data_,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

bool MGame::SetName(char *Name_)
{
    return strlen(Name_)>MAX_PrgNameLength?
        false: (bool)strcpy(Name,Name_);
}

bool MGame::SetCommand(char *Command_)
{
    return strlen(Command_)>MAX_PrgCmdLength?
        false: (bool)strcpy(Command,Command_);
}

bool MGame::SetIcon(char *Icon_)
{
    return strlen(Icon_)>MAX_PrgIconLength?
        false: (bool)strcpy(Icon,Icon_);
}
//---------------------------------------------------------------------------

