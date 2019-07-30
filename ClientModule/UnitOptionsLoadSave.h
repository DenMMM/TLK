//---------------------------------------------------------------------------
#ifndef UnitOptionsLoadSaveH
#define UnitOptionsLoadSaveH
//---------------------------------------------------------------------------
#include "UnitState.h"
#include "UnitGames.h"
//---------------------------------------------------------------------------
struct MStateSL;
//---------------------------------------------------------------------------
int GetStateDataSize(MState *State_);
char *SetStateData(char *Data_, MState *State_);
char *GetStateData(char *Data_, char *LimitData_, MState *State_);
bool StateSave(MState *State_);
bool StateLoad(MState *State_);
//---------------------------------------------------------------------------
char *GetGamesData(char *Data_, char *LimitData_, MGames *Games_);
bool GamesLoad(MGames *Games_);
//---------------------------------------------------------------------------
struct MStateSL
{
    int Number;
    unsigned int State;
    double StartWorkTime;
    int SizeWorkTime;
    double StartFineTime;
    int SizeFineTime;
    double StopTimerTime;
    unsigned int AppPages;

    void FromState(MState *State_);
    void ToState(MState *State_);
};
//---------------------------------------------------------------------------
#endif

