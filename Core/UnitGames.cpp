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
    SubGames=NULL;
}

MGame::~MGame()
{
    delete SubGames;
}

void MGame::Copy(const MListItem *SrcItem_)
{
    MGame *Game_=(MGame*)SrcItem_;

    strcpy(Name,Game_->Name);
    strcpy(Command,Game_->Command);
    strcpy(Icon,Game_->Icon);
    if ( (Game_->SubGames!=NULL)&&
        (Game_->SubGames->gCount()>0) )
    {
        if ( SubGames==NULL ) SubGames=new MGames;
        SubGames->Copy(Game_->SubGames);
    } else
    {
        delete SubGames;
        SubGames=NULL;
    }
}

unsigned MGame::GetDataSize() const
{
    const MGames vGames;            // Заглушка для SubGames=NULL

    return
        strlen(Name)+1+
        strlen(Command)+1+
        strlen(Icon)+1+
        ((SubGames==NULL)||(SubGames->gCount()==0)?
            vGames.GetAllDataSize():
            SubGames->GetAllDataSize());
}

char *MGame::SetData(char *Data_) const
{
    const MGames vGames;            // Заглушка для SubGames=NULL

    Data_=MemSetCLine(Data_,Name);
    Data_=MemSetCLine(Data_,Command);
    Data_=MemSetCLine(Data_,Icon);
    Data_=(SubGames==NULL)||(SubGames->gCount()==0)?
        vGames.SetAllData(Data_):
        SubGames->SetAllData(Data_);
    return Data_;
}

const char *MGame::GetData(const char *Data_, const char *Limit_)
{
    if ( (Data_=MemGetCLine(Data_,Name,MAX_PrgNameLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetCLine(Data_,Command,MAX_PrgCmdLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetCLine(Data_,Icon,MAX_PrgIconLength,Limit_))==NULL ) goto error;
    // Создаем SubGames и пробуем его заполнить
    if ( AddSubGames()==NULL ) goto error;
    if ( (Data_=SubGames->GetAllData(Data_,Limit_))==NULL ) goto error;
    // Если SubGames оказался пустым, удалим его
    if ( SubGames->gCount()==0 ) DelSubGames();
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

MGames *MGame::AddSubGames()
{
    if ( SubGames==NULL ) SubGames=new MGames;
    return SubGames;
}

void MGame::DelSubGames()
{
    delete SubGames;
    SubGames=NULL;
}
//---------------------------------------------------------------------------

