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
	const MLogRecords &Log_,
	const MLogRecords::const_iterator iPoint_,
	MStates &States_, MTariffs &Tariffs_);
//---------------------------------------------------------------------------
bool ProcessUsersUpTime(
//	const MLogRecords &Log_,
	MLogRecords::const_iterator iBegin_,
	const MLogRecords::const_iterator iEnd_,
	MUsers &Users_, MUsersUpTime &UpTimes_);
//---------------------------------------------------------------------------
#endif

