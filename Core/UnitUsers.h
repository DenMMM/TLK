//---------------------------------------------------------------------------
#ifndef UnitUsersH
#define UnitUsersH
//---------------------------------------------------------------------------
struct MUserData;

class MUser;
class MUsers;
//---------------------------------------------------------------------------
#include "UnitPassword.h"
#include "UnitIDList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#define MAX_Users               20
#define MAX_UserLoginLen        16
#define MAX_UserPassLen         8
#define MAX_UserNameLen         50
//---------------------------------------------------------------------------
struct MUserData
{
    unsigned ID;
    char Login[MAX_UserLoginLen+1];
    char Name[MAX_UserNameLen+1];
};
//---------------------------------------------------------------------------
class MUser:public MIDListItem
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

    MPassword Pass;

public:
    char Login[MAX_UserLoginLen+1];
    char Name[MAX_UserNameLen+1];
    bool Active;

    bool SetLogin(char *Login_);
    bool SetName(char *Name_);
    void SetPass(char *Pass_) { Pass.Set(Pass_); }
    bool CheckPass(char *Pass_) const { return Pass.Check(Pass_); }
    void Copy(const MListItem *SrcItem_);

    // Поддержка логов
    void GetUserData(MUserData *Data_) const;
    void SetUserData(MUserData *Data_);

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

