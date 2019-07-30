//---------------------------------------------------------------------------
#ifndef UnitUsersH
#define UnitUsersH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#define MAX_Users               20
#define MAX_UserLoginLength     16
#define MAX_UserPasswordLength  8
#define MAX_UserNameLength      40
//---------------------------------------------------------------------------
class MUser;
class MUsers;
struct MUserData;
//---------------------------------------------------------------------------
class MUser:public MSLListItem
{
public:
    bool Copy(MListItem *SrcItem_);
private:
    char Password[MAX_UserPasswordLength+1];
public:
    unsigned ID;
    char Login[MAX_UserLoginLength+1];
    char Name[MAX_UserNameLength+1];
    bool Active;

    bool SetLogin(char *Login_);
    bool SetPassword(char *Password_);
    bool SetName(char *Name_);
    bool CheckPassword(char *Password_);

    // Поддержка логов
    void GetUserData(MUserData *Data_);
    void SetUserData(MUserData *Data_);

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MUser();
    ~MUser();
};
//---------------------------------------------------------------------------
class MUsers:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MUser; }
    void operator_delete(MListItem *DelItem_) { delete (MUser*)DelItem_; }
public:
    bool Copy(MList *SrcList_)
        { LastID=((MUsers*)SrcList_)->LastID; return MList::Copy(SrcList_); }
private:
    unsigned LastID;
    unsigned NextID();
public:
    void SetIDs();
    MUser *Search(unsigned ID_);

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() { return sizeof(LastID); }
    char *SetData(char *Data_) { return MemSet(Data_,LastID); }
    char *GetData(char *Data_, char *Limit_) { return MemGet(Data_,&LastID,Limit_); }

    MUsers() { constructor(); LastID=0; }
    ~MUsers() { destructor(); }
};
//---------------------------------------------------------------------------
struct MUserData
{
    unsigned ID;
    char Login[MAX_UserLoginLength+1];
    char Name[MAX_UserNameLength+1];
};
//---------------------------------------------------------------------------
#endif

