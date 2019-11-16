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
#include <Menus.hpp>
//---------------------------------------------------------------------------
// Main Computers Filter
#define mcfAll          1       // Все компьютеры
#define mcfFree         2       // Свободные компьютеры
#define mcfService      3       // Открытые для обслуживания
//---------------------------------------------------------------------------
// Main List Events
#define mleAll          1       // Все
#define mleService      2       // Служебные
#define mleComputers    3       // Компьютеры
#define mleUsers        4       // Смены
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
    TPopupMenu *PopupMenuEventFilter;
    TMenuItem *NEventAll;
    TMenuItem *NEventUsers;
    TMenuItem *NEventComputers;
    TMenuItem *NEventService;
    TMenuItem *NEventAdminMdl;
    TMenuItem *N1;
    void __fastcall ListViewComputersInsert(TObject *Sender,
          TListItem *Item);
    void __fastcall ListViewEventsSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall CheckBoxCompAUpdClick(TObject *Sender);
    void __fastcall ButtonCompUpdClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ListViewEventsColumnClick(TObject *Sender,
          TListColumn *Column);
    void __fastcall ListViewEventsCompare(TObject *Sender,
          TListItem *Item1, TListItem *Item2, int Data, int &Compare);
    void __fastcall ListViewComputersCompare(TObject *Sender,
          TListItem *Item1, TListItem *Item2, int Data, int &Compare);
    void __fastcall NEventAllClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
    const MLogRecords *EventsLog;
	MLogRecords::const_iterator EventsBegin;
	MLogRecords::const_iterator EventsEnd;
	int EventSort;
	int StateFilter;
	bool CheckFilter(MStatesInfo *Info_, int FreeTime_);
	int GetCompNum(const MLogRecordsItem *Item_);
	void SetListViewComputersLine(TListItem *Item_, MStatesInfo *Info_, MTariffs *Tariffs_);
	void UpdateListViewComputers(bool Full_, MStates *States_, MTariffs *Tariffs_);
	void UpdateTariffs(MTariffs *Tariffs_, MLogRecords::DataTariffs *LogRecord_);
	void UpdateFines(MFines *Fines_, MLogRecords::DataFines *LogRecord_);
	void UpdateUsers(MUsers *Users_, MLogRecords::DataUsers *LogRecord_);
	bool CheckEventFilter(unsigned char TypeID_);
	void UpdateListViewEvents();
public:		// User declarations
	bool Open(
		const MLogFile &File_,
		MLogRecords::const_iterator Begin_,
		MLogRecords::const_iterator End_);
	__fastcall TFormEvents(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormEvents *FormEvents;
//---------------------------------------------------------------------------
#endif

