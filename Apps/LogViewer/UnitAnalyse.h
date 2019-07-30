//---------------------------------------------------------------------------
#ifndef UnitAnalyseH
#define UnitAnalyseH
//---------------------------------------------------------------------------
#include "UnitLogRecords.h"
#include "UnitStates.h"
#include "UnitTariffs.h"
#include "UnitUsers.h"
#include "UnitUsersUpTime.h"
//---------------------------------------------------------------------------
bool ProcessComputersState(MLogRecord *Position_,
    MStates *States_, MTariffs *Tariffs_);
//---------------------------------------------------------------------------
bool ProcessUsersUpTime(MLogRecord *Begin_, MLogRecord *End_,
    MUsers *Users_, MUsersUpTime *UpTimes_);
//---------------------------------------------------------------------------
#endif

