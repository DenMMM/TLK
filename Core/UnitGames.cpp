//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitGames.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
unsigned MGamesItem::GetDataSize() const
{
    const MGames vGames;            // Заглушка для SubGames=nullptr

    return
		sizeofLine(Name)+
		sizeofLine(Command)+
        sizeofLine(Icon)+
		(
			(!upSubGames) || (upSubGames->gCount()==0)?
			vGames.GetAllDataSize():
			upSubGames->GetAllDataSize()
		);
}

void *MGamesItem::SetData(void *Data_) const
{
    const MGames vGames;            // Заглушка для SubGames=nullptr

	Data_=MemSetLine(Data_,Name);
	Data_=MemSetLine(Data_,Command);
    Data_=MemSetLine(Data_,Icon);
	Data_=
		(!upSubGames) || (upSubGames->gCount()==0)?
		vGames.SetAllData(Data_):
		upSubGames->SetAllData(Data_);

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
	upSubGames.reset(new MGames);
	if ( !(
		(Data_=upSubGames->GetAllData(Data_,Limit_))
		) ) return nullptr;

	// Если SubGames оказался пустым, удалим его
	if ( upSubGames->gCount()==0 ) upSubGames.reset(nullptr);

	return Data_;
}

MGamesItem::MGamesItem(const MGamesItem& Src_):
	Name(Src_.Name),
	Command(Src_.Command),
	Icon(Src_.Icon),
	upSubGames(nullptr)
{
	if ( Src_.upSubGames )
	{
		upSubGames.reset(new MGames);
		*upSubGames=*Src_.upSubGames;
	}
}

MGamesItem& MGamesItem::operator=(const MGamesItem& Right_)
{
	Name=Right_.Name;
	Command=Right_.Command;
	Icon=Right_.Icon;
	if ( Right_.upSubGames )
	{
//		if ( !upSubGames ) upSubGames.reset(new MGames);
		upSubGames.reset(new MGames);
		*upSubGames=*Right_.upSubGames;
	} else
	{
		upSubGames.reset(nullptr);
	}

	return *this;
}
//---------------------------------------------------------------------------

