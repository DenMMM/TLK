//---------------------------------------------------------------------------
#ifndef UnitGamesH
#define UnitGamesH
//---------------------------------------------------------------------------
#include <string>

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
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::wstring Name;
	std::wstring Command;
	std::wstring Icon;
	MGames *SubGames;               /// ������ �� �������� !!! �������� smart-pointer

	MGames *AddSubGames();
	void DelSubGames();

	MGamesItem():
		SubGames(nullptr)
	{
	}

	MGamesItem(MGamesItem&) = default;
	MGamesItem(MGamesItem&&) = default;
	MGamesItem& operator=(MGamesItem&) = default;
	MGamesItem& operator=(MGamesItem&&) noexcept = default;
	~MGamesItem();
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

