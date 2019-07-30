//---------------------------------------------------------------------------
#ifndef UnitFormEventsH
#define UnitFormEventsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitLogRecords.h"
#include "UnitStates.h"
#include "UnitTariffs.h"
#include "UnitFines.h"
#include "UnitUsers.h"
//---------------------------------------------------------------------------
#include "UnitFormMain.h"
//---------------------------------------------------------------------------
// Main Events Sort
#define mesNone 0
#define mesTime 1
#define mesNumber 2
#define mesDesc 3
#define mesSize 4
#define mesMoney 5
//---------------------------------------------------------------------------
// Main Computers Filter
#define mcfAll ((int)1)     // Все компьютеры
#define mcfFree ((int)2)    // Свободные компьютеры
#define mcfService ((int)3) // Открытые для обслуживания
//---------------------------------------------------------------------------
class TFormEvents : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel;
    TPanel *PanelState;
    TListView *ListViewComputers;
    TCheckBox *CheckBoxCompAUpd;
    TButton *ButtonCompUpd;
    TListView *ListViewEvents;
    TSplitter *Splitter;
    TBevel *Bevel2;
    TLabel *Label1;
    TLabel *LabelSelTime;
    TLabel *LabelSelMoney;
    TLabel *Label2;
    void __fastcall ListViewComputersInsert(TObject *Sender,
          TListItem *Item);
    void __fastcall ListViewEventsSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall CheckBoxCompAUpdClick(TObject *Sender);
    void __fastcall ButtonCompUpdClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ListViewEventsColumnClick(TObject *Sender,
          TListColumn *Column);
    void __fastcall ListViewEventsCompare(TObject *Sender,
          TListItem *Item1, TListItem *Item2, int Data, int &Compare);
private:	// User declarations
    int Filter;
    int SortMode;
    bool CheckFilter(MStateInfo *Info_, int FreeTime_);
    void SetListViewComputersLine(TListItem *Item_, MStateInfo *Info_, MTariffs *Tariffs_);
    void UpdateListViewComputers(bool Full_, MStates *States_, MTariffs *Tariffs_);
public:		// User declarations
    bool Open(MLogFile *File_, MLogRecord *Begin_, MLogRecord *End_);
    __fastcall TFormEvents(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormEvents *FormEvents;
//---------------------------------------------------------------------------
#endif

