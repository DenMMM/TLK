//---------------------------------------------------------------------------
#ifndef UnitLogsH
#define UnitLogsH
//---------------------------------------------------------------------------
#include "UnitComputers.h"
#include "UnitTariffs.h"
#include "UnitUsers.h"
//---------------------------------------------------------------------------
extern unsigned int LogFileNumber;
//---------------------------------------------------------------------------
int GetLogHeaderDataSize(MUser *User_);
char *SetLogHeaderData(char *Data_, MUser *User_);
bool LogNewFile(MUser *User_);
bool LogAddRecord(char *Data_, int DataSize_);
//---------------------------------------------------------------------------
int GetLogRunDataSize(MTariff *Tariff_);
char *SetLogRunData(char *Data_, MComputer *Computer_, MTariff *Tariff_, int Time_, double Cost_);
bool LogCompRun(MComputer *Computer_, MTariff *Tariff_, int Time_, double Cost_);
//---------------------------------------------------------------------------
int GetLogCloseDataSize();
char *SetLogCloseData(char *Data_);
bool LogClose();
//---------------------------------------------------------------------------
#endif

