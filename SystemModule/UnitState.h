//---------------------------------------------------------------------------
#ifndef UnitStateH
#define UnitStateH
//---------------------------------------------------------------------------
#include "UnitUsers.h"
//---------------------------------------------------------------------------
class MState;
//---------------------------------------------------------------------------
class MState
{
    int GetStateDataSize();
    char *SetStateData(char *Data_);
    char *GetStateData(char *Data_, char *LimitData_);

public:
    AnsiString AdminPassword;
    MUser *CurrentUser;
    MUser *CurrentAdmin;
    unsigned int LogFileNumber;
//    unsigned int ComputersSecCode;
//    unsigned int UsersSecCode;
    char LogLastBytes[3];

    bool Save();
    bool Load();

    MState();
    ~MState();
};
//---------------------------------------------------------------------------
#endif

