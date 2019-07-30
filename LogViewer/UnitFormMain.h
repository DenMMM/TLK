//---------------------------------------------------------------------------
#ifndef UnitFormMainH
#define UnitFormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
#include "UnitEvents.h"
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewEvents;
    TLabel *Label1;
    TEdit *EditName;
    TLabel *Label2;
    TEdit *EditJob;
    TLabel *Label3;
    TLabel *LabelOpenTime;
    TLabel *Label5;
    TBevel *Bevel1;
    TLabel *LabelCloseTime;
    TLabel *LabelDurationTime;
    TLabel *LabelLengthTime;
    TBevel *Bevel2;
    TLabel *Label9;
    TLabel *LabelPayTime;
    TLabel *Label11;
    TLabel *LabelPayMany;
    TButton *ButtonSelect;
    TOpenDialog *OpenDialog;
    TBitBtn *BitBtnClose;
    TMemo *MemoReport;
    TButton *ButtonSaveReport;
    TSaveDialog *SaveDialog;
    TLabel *Label4;
    TLabel *LabelSelPayMany;
    void __fastcall ButtonSelectClick(TObject *Sender);
    void __fastcall ListViewEventsInsert(TObject *Sender, TListItem *Item);
    void __fastcall BitBtnCloseClick(TObject *Sender);
    void __fastcall ListViewEventsColumnClick(TObject *Sender,
          TListColumn *Column);
    void __fastcall ButtonSaveReportClick(TObject *Sender);
    void __fastcall ListViewEventsDeletion(TObject *Sender,
          TListItem *Item);
    void __fastcall ListViewEventsSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
private:	// User declarations
    void __fastcall SetListViewEventsLine(TListItem *Item_, MEvent *Event_);
public:		// User declarations
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif

