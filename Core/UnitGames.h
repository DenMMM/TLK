//---------------------------------------------------------------------------
#ifndef UnitGamesH
#define UnitGamesH
//---------------------------------------------------------------------------
#include <string>
#include <memory>

#include "UnitSLList.h"
//---------------------------------------------------------------------------
class MGamesItem;
class MGames;
//---------------------------------------------------------------------------
#define MAX_PrgNameLength   32      // ����� �������� ����
#define MAX_PrgCmdLength    128     // ����� ���� � ����� ����
#define MAX_PrgIconLength   128     // ����� ���� � ����� ������
#define MAX_PrgLevel        2       // ���������� ������� � ������
//---------------------------------------------------------------------------
class MGamesItem:
	public MSLListItemSimple <
		MSLListItem <MGames, MGamesItem>,
		MGamesItem>
{
public:
	// ������� ��������� ����������/�������� ������
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::wstring Name;
	std::wstring Command;
	std::wstring Icon;
	std::unique_ptr <MGames> upSubGames;

	MGamesItem() = default;
	MGamesItem(const MGamesItem& Src_);

	MGamesItem(MGamesItem&& Src_):
		Name(std::move(Src_.Name)),
		Command(std::move(Src_.Command)),
		Icon(std::move(Src_.Icon)),
		upSubGames(std::move(Src_.upSubGames))
	{
	}

	MGamesItem& operator=(const MGamesItem& Right_);
	MGamesItem& operator=(MGamesItem&& Right_)
	{
		Name=std::move(Right_.Name);
		Command=std::move(Right_.Command);
		Icon=std::move(Right_.Icon);
		upSubGames=std::move(Right_.upSubGames);

        return *this;
	}

	virtual ~MGamesItem() override = default;
};
//---------------------------------------------------------------------------
class MGames:
	public MSLListSimple <
		MSLList <MGames, MGamesItem>,
		MGamesItem>
{
};
//---------------------------------------------------------------------------
#endif

