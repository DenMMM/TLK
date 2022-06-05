//---------------------------------------------------------------------------
#ifndef UnitUsersH
#define UnitUsersH
//---------------------------------------------------------------------------
#include <string>
#include <cstdint>
//---------------------------------------------------------------------------
#include "UnitPassword.h"
#include "UnitIDList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
class MUsersItem;
class MUsers;
//---------------------------------------------------------------------------
#define MAX_Users               20
#define MAX_UserLoginLen        16
#define MAX_UserPassLen         16
#define MAX_UserNameLen         50
//---------------------------------------------------------------------------
class MUsersItem:
	public MIDListItemSimple <
		MIDListItem <MUsers, MUsersItem>,
		MUsersItem>
{
public:
	// Функции механизма сохранения/загрузки данных
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::wstring Login;
	std::wstring Name;
	bool Active;
	MPassword Pass;

	// Поддержка логов
	struct LogData
	{
		std::uint32_t UUID;
		std::wstring Login;
		std::wstring Name;
	};

	LogData gLogData() const
	{
		LogData ld;
		ld.UUID=gUUID();
		ld.Login=Login;
		ld.Name=Name;

		return ld;
	}

	void sFromLog(const LogData &Data_)
	{
		UUID=Data_.UUID;
		Login=Data_.Login;
		Name=Data_.Name;
	}

	MUsersItem():
		Active(true)
	{
	}
};
//---------------------------------------------------------------------------
class MUsers:
	public MIDListSimple <
		MIDList <MUsers, MUsersItem>,
		MUsersItem>
{
public:
	std::size_t ActiveCount() const;
};
//---------------------------------------------------------------------------
#endif

