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
#define mcfAll          1       // ��� ����������
#define mcfFree         2       // ��������� ����������
#define mcfService      3       // �������� ��� ������������
//---------------------------------------------------------------------------
// Main List Events
#define mleAll          1       // ���
#define mleService      2       // ���������
#define mleComputers    3       // ����������
#define mleUsers        4       // �����
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
    MLogRecord *EventsBegin;
    MLogRecord *EventsEnd;
    int EventSort;
    int StateFilter;
    bool CheckFilter(MStateInfo *Info_, int FreeTime_);
    void SetListViewComputersLine(TListItem *Item_, MStateInfo *Info_, MTariffs *Tariffs_);
    void UpdateListViewComputers(bool Full_, MStates *States_, MTariffs *Tariffs_);
    void UpdateTariffs(MTariffs *Tariffs_, MLogRecordDataTariffs *LogRecord_);
    void UpdateFines(MFines *Fines_, MLogRecordDataFines *LogRecord_);
    void UpdateUsers(MUsers *Users_, MLogRecordDataUsers *LogRecord_);
    bool CheckEventFilter(unsigned char TypeID_);
    void UpdateListViewEvents();
public:		// User declarations
    bool Open(MLogFile *File_, MLogRecord *Begin_, MLogRecord *End_);
    __fastcall TFormEvents(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormEvents *FormEvents;
//---------------------------------------------------------------------------
#endif

