//---------------------------------------------------------------------------
#ifndef UnitUsersH
#define UnitUsersH
//---------------------------------------------------------------------------
class MUser;
class MUsers;
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
#include "UnitPassword.h"
#include "UnitIDList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#define MAX_Users               20
#define MAX_UserLoginLen        16
#define MAX_UserPassLen         16
#define MAX_UserNameLen         50
//---------------------------------------------------------------------------
class MUser:public MIDListItem
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    std::string Login;
    std::string Name;
    bool Active;
    MPassword Pass;

    void Copy(const MListItem *SrcItem_);

    // Поддержка логов
    struct LogData
    {
        unsigned ID;
        std::string Login;
        std::string Name;

        LogData &operator=(const MUser &User_)
        {
            ID=User_.ItemID;
            Login=User_.Login;
            Name=User_.Name;
            return *this;
        }

    };
    friend LogData;             // Нужен доступ к "ItemID"

    MUser &operator=(const LogData &Data_)
    {
        ItemID=Data_.ID;
        Login=Data_.Login;
        Name=Data_.Name;
        return *this;
    }

    MUser();
    ~MUser();
};
//---------------------------------------------------------------------------
class MUsers:public MIDList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MUser; }
    void item_del(MListItem *Item_) const { delete (MUser*)Item_; }

public:
    unsigned ActiveCount() const;

    MUsers() {}
    ~MUsers() { Clear(); }
};
//---------------------------------------------------------------------------
#endif

