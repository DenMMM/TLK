//---------------------------------------------------------------------------
#ifndef UnitCommonH
#define UnitCommonH
//---------------------------------------------------------------------------
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitComputers.h"
#include "UnitTariffs.h"
//---------------------------------------------------------------------------
extern AnsiString PrgDir;
//---------------------------------------------------------------------------
struct MComputerSL;
//---------------------------------------------------------------------------
void BasicCode(char *Data_, int DataSize_, unsigned int Code_);
void BasicEncode(char *Data_, int DataSize_, unsigned int Code_);
//---------------------------------------------------------------------------
bool CorrectComputersData(MComputers *Computers_,MTariffs *Tariffs_);
bool CorrectTariffsData(MTariffs *Tariffs_, MComputers *Computers_);
//---------------------------------------------------------------------------
bool ComputerSave(MComputer *Computer_, char *FileName_);
bool ComputersSave(MComputers *Computers_, char *FileName_);
bool ComputersLoad(MComputers *Computers_, char *FileName_);
//---------------------------------------------------------------------------
int GetGamesDataSize(MGames *Games_);
char *SetGamesData(char *Data_, MGames *Games_);
char *GetGamesData(char *Data_, char *LimitData_, MGames *Games_);
bool GamesSave(MGames *Games_, char *FileName_);
bool GamesLoad(MGames *Games_, char *FileName_);
//---------------------------------------------------------------------------
struct MComputerSL
{
    unsigned int ID;
    char Address[15+1];
    int Number;
    int GroupColor;
    unsigned int State;
    double StartWorkTime;
    int SizeWorkTime;
    double StartFineTime;
    int SizeFineTime;
    double StopTimerTime;
    int FaceType;
    unsigned int GamesPages;
    unsigned int TariffID;
    unsigned int StateVer;
    unsigned int NetSync;

    void FromComputer(MComputer *Computer_);
    void ToComputer(MComputer *Computer_);
};
//---------------------------------------------------------------------------
#endif

