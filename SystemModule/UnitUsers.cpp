//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "UnitUsers.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MUser &MUser::operator=(MUser &User_)
{
    ID=User_.ID;
    Login=User_.Login;
    Password=User_.Password;
    JobTitle=User_.JobTitle;
    FullName=User_.FullName;
    Rights=User_.Rights;
    Active=User_.Active;
    return *this;
}
//---------------------------------------------------------------------------
MUser::MUser()
{
    ID=0;
    Rights=0;
    Active=false;
}
//---------------------------------------------------------------------------
MUser::~MUser()
{
//
}
//---------------------------------------------------------------------------
MUser *MUsers::SearchID(unsigned int ID_)
{
    MUser *User=(MUser*)FirstItem;
    for ( ; User; User=(MUser*)User->NextItem ) if ( User->ID==ID_ ) break;
    return User;
}
//---------------------------------------------------------------------------
MUsers &MUsers::operator=(MUsers &Users_)
{
    Clear();
    for ( MUser *User, *User_=(MUser*)Users_.FirstItem;
        User_; User_=(MUser*)User_->NextItem )
    {
        User=(MUser*)Add();
        *User=*User_;
    }
    return *this;
}
//---------------------------------------------------------------------------

