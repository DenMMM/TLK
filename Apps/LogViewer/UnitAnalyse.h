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
bool ProcessComputersState(MLogRecordsItem *Position_,
    MStates *States_, MTariffs *Tariffs_);
//---------------------------------------------------------------------------
bool ProcessUsersUpTime(MLogRecordsItem *Begin_, MLogRecordsItem *End_,
	MUsers *Users_, MUsersUpTime *UpTimes_);
//---------------------------------------------------------------------------
#endif

