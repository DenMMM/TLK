//---------------------------------------------------------------------------
#ifndef UnitUsersH
#define UnitUsersH
//---------------------------------------------------------------------------
#include <string>
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
	public MIDListItem::Simple <
		MIDListItem::Proxy <MIDListItem, MUsersItem>,
		MUsersItem>
{
public:
	// Функции механизма сохранения/загрузки данных
	unsigned GetDataSize() const override;
	void *SetData(void *Data_) const override;
	const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::wstring Login;
	std::wstring Name;
	bool Active;
	MPassword Pass;

	void Copy(const MListItem *SrcItem_) override;

	// Поддержка логов
	struct LogData
	{
		unsigned UUID;
		std::wstring Login;
		std::wstring Name;

		LogData &operator=(const MUsersItem &User_)
		{
			UUID=User_.gUUID();
			Login=User_.Login;
			Name=User_.Name;
			return *this;
		}
	};

	MUsersItem &operator=(const LogData &Data_)
	{
		UUID=Data_.UUID;
		Login=Data_.Login;
		Name=Data_.Name;
		return *this;
	}

	MUsersItem():
		Active(true)
	{
	}

	virtual ~MUsersItem()
	{
	}
};
//---------------------------------------------------------------------------
class MUsers:
	public MIDList::Simple <MIDList, MUsers, MUsersItem>
{
public:
	size_t ActiveCount() const;

	MUsers() {}
	~MUsers() {}
};
//---------------------------------------------------------------------------
#endif

