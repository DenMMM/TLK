//---------------------------------------------------------------------------
#ifndef UnitFormUsersUpTimeH
#define UnitFormUsersUpTimeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitLogRecords.h"
#include "UnitUsersUpTime.h"
//---------------------------------------------------------------------------
#include "UnitFormMain.h"
//---------------------------------------------------------------------------
class TFormUsersUpTime : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewUpTimes;
    TLabel *Label1;
    TLabel *LabelSelTime;
    TLabel *LabelSelMoney;
    TLabel *Label2;
    TBevel *Bevel1;
    TBevel *Bevel2;
    void __fastcall ListViewUpTimesSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ListViewUpTimesColumnClick(TObject *Sender,
          TListColumn *Column);
    void __fastcall ListViewUpTimesCompare(TObject *Sender,
          TListItem *Item1, TListItem *Item2, int Data, int &Compare);
private:	// User declarations
    int SortMode;
    MUsers Users;
    MUsersUpTime Times;
public:		// User declarations
	bool Open(
		const MLogFile &File_,
		MLogRecords::const_iterator Begin_,
		MLogRecords::const_iterator End_);
    __fastcall TFormUsersUpTime(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormUsersUpTime *FormUsersUpTime;
//---------------------------------------------------------------------------
#endif

