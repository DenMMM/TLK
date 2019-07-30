//---------------------------------------------------------------------------
#ifndef UnitOptionsLoadSaveH
#define UnitOptionsLoadSaveH
//---------------------------------------------------------------------------
#include "UnitTariffs.h"
#include "UnitFines.h"
#include "UnitUsers.h"
//---------------------------------------------------------------------------
int GetTariffsDataSize(MTariffs *Tariffs_);
char *SetTariffsData(char *Data_, MTariffs *Tariffs_);
char *GetTariffsData(char *Data_, char *LimitData_, MTariffs *Tariffs_);
bool TariffsSave(MTariffs *Tariffs_);
bool TariffsLoad(MTariffs *Tariffs_, TImageList *ImageList_);
//---------------------------------------------------------------------------
int GetFinesDataSize(MFines *Fines_);
char *SetFinesData(char *Data_, MFines *Fines_);
char *GetFinesData(char *Data_, char *LimitData_, MFines *Fines_);
bool FinesSave(MFines *Fines_);
bool FinesLoad(MFines *Fines_);
//---------------------------------------------------------------------------
int GetUsersDataSize(MUsers *Users_);
char *SetUsersData(char *Data_, MUsers *Users_);
char *GetUsersData(char *Data_, char *LimitData_, MUsers *Users_);
bool UsersSave(MUsers *Users_);
bool UsersLoad(MUsers *Users_);
//---------------------------------------------------------------------------
#endif

