//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitGames.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MGame::MGame()
{
    SubGames=NULL;
}

MGame::~MGame()
{
    delete SubGames;
}

void MGame::Copy(const MListItem *SrcItem_)
{
    const MGame *game=
        dynamic_cast<const MGame*>(SrcItem_);

    Name=game->Name;
    Command=game->Command;
    Icon=game->Icon;
    if ( (game->SubGames!=NULL)&&
        (game->SubGames->gCount()>0) )
    {
        if ( SubGames==NULL ) SubGames=new MGames;
        SubGames->Copy(game->SubGames);
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
        Name.length()+sizeof('\0')+
        Command.length()+sizeof('\0')+
        Icon.length()+sizeof('\0')+
        ((SubGames==NULL)||(SubGames->gCount()==0)?
            vGames.GetAllDataSize():
            SubGames->GetAllDataSize());
}

char *MGame::SetData(char *Data_) const
{
    const MGames vGames;            // Заглушка для SubGames=NULL

    Data_=MemSetCLine(Data_,Name.c_str());
    Data_=MemSetCLine(Data_,Command.c_str());
    Data_=MemSetCLine(Data_,Icon.c_str());
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

