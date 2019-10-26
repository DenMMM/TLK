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
bool ProcessComputersState(
	MLogRecords::const_iterator Position_,
	MStates *States_, MTariffs *Tariffs_);
//---------------------------------------------------------------------------
bool ProcessUsersUpTime(
	MLogRecords::const_iterator Begin_,
	MLogRecords::const_iterator End_,
	MUsers *Users_, MUsersUpTime *UpTimes_);
//---------------------------------------------------------------------------
#endif

