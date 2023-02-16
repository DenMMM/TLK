#ifndef UNITGLOBAL_H
#define UNITGLOBAL_H
//---------------------------------------------------------------------------
#include <memory>

#include "..\..\Core\UnitComputers.h"
#include "..\..\Core\UnitTariffs.h"
#include "..\..\Core\UnitFines.h"
#include "..\..\Core\UnitUsers.h"
#include "..\..\Core\UnitStates.h"
#include "..\..\Core\UnitSync.h"
#include "..\..\Core\UnitOptions.h"
#include "..\..\Core\UnitLog.h"
//---------------------------------------------------------------------------
extern std::unique_ptr <MShellState> ShellState;
extern std::unique_ptr <MOptions> Options;
extern std::unique_ptr <MComputers> Computers;
extern std::unique_ptr <MTariffs> Tariffs;
extern std::unique_ptr <MFines> Fines;
extern std::unique_ptr <MUsers> Users;
extern std::unique_ptr <MStates> States;
extern std::unique_ptr <MSync> Sync;
extern std::unique_ptr <MAuth> Auth;
extern std::unique_ptr <MLog> Log;
//---------------------------------------------------------------------------
#endif // UNITGLOBAL_H
