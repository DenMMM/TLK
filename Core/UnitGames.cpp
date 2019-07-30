//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitGames.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void MGamesItem::Copy(const MListItem *SrcItem_)
{
	auto game=dynamic_cast<const MGamesItem*>(SrcItem_);

	Name=game->Name;
	Command=game->Command;
	Icon=game->Icon;
	if ( (game->SubGames!=nullptr)&&
		(game->SubGames->gCount()>0) )
	{
		if ( SubGames==nullptr ) SubGames=new MGames;
		SubGames->Copy(game->SubGames);
	} else
	{
		delete SubGames;
		SubGames=nullptr;
	}
}

unsigned MGamesItem::GetDataSize() const
{
    const MGames vGames;            // Заглушка для SubGames=nullptr

    return
		sizeofLine(Name)+
		sizeofLine(Command)+
        sizeofLine(Icon)+
		(
			(SubGames==nullptr) || (SubGames->gCount()==0)?
			vGames.GetAllDataSize():
			SubGames->GetAllDataSize()
		);
}

void *MGamesItem::SetData(void *Data_) const
{
    const MGames vGames;            // Заглушка для SubGames=nullptr

	Data_=MemSetLine(Data_,Name);
	Data_=MemSetLine(Data_,Command);
    Data_=MemSetLine(Data_,Icon);
	Data_=
		(SubGames==nullptr) || (SubGames->gCount()==0)?
        vGames.SetAllData(Data_):
		SubGames->SetAllData(Data_);

    return Data_;
}

const void *MGamesItem::GetData(const void *Data_, const void *Limit_)
{
	if ( !(
		(Data_=MemGetLine(Data_,Name,MAX_PrgNameLength,Limit_)) &&
		(Data_=MemGetLine(Data_,Command,MAX_PrgCmdLength,Limit_)) &&
		(Data_=MemGetLine(Data_,Icon,MAX_PrgIconLength,Limit_))
		) ) return nullptr;

	// Создаем SubGames и пробуем его заполнить
	if ( !(
		AddSubGames() &&
		(Data_=SubGames->GetAllData(Data_,Limit_))
		) ) return nullptr;

	// Если SubGames оказался пустым, удалим его
	if ( SubGames->gCount()==0 ) DelSubGames();

	return Data_;
}

MGames *MGamesItem::AddSubGames()
{
    if ( SubGames==nullptr ) SubGames=new MGames;
    return SubGames;
}

void MGamesItem::DelSubGames()
{
	delete SubGames;
	SubGames=nullptr;
}

MGamesItem::~MGamesItem()
{
	delete SubGames;
}
//---------------------------------------------------------------------------

