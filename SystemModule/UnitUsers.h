//---------------------------------------------------------------------------
#ifndef UnitUsersH
#define UnitUsersH
//---------------------------------------------------------------------------
#include "UnitBaseClassMList.h"
//---------------------------------------------------------------------------
#define murClientsServices ((unsigned int)1)
#define murComputersServices ((unsigned int)2)
#define murWorkWithLogs ((unsigned int)4)
//---------------------------------------------------------------------------
class MUser;
class MUsers;
//---------------------------------------------------------------------------
class MUser:public MListItem
{
public:
    unsigned int ID;
    AnsiString Login;
    AnsiString Password;
    AnsiString JobTitle;
    AnsiString FullName;
    unsigned int Rights;
    bool Active;

    MUser &operator=(MUser &User_);

    MUser();
    ~MUser();
};
//---------------------------------------------------------------------------
class MUsers:public MList
{
private:
    MListItem *new_() { return (MListItem*)new MUser; }
    void delete_(MListItem *ListItem_) { delete (MUser*)ListItem_; }
public:
    MUser *SearchID(unsigned int ID_);

    MUsers &operator=(MUsers &Users_);
    MUsers() { constructor(); }
    ~MUsers() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

