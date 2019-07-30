//---------------------------------------------------------------------------
#ifndef UnitFormComputersH
#define UnitFormComputersH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitComputers.h"
//---------------------------------------------------------------------------
class TFormComputers : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewComputers;
    TBevel *BevelBorder;
    TBevel *Bevel3;
    TLabel *LabelNumber;
    TEdit *EditNumber;
    TEdit *EditAddress;
    TLabel *LabelAddress;
    TBevel *Bevel2;
    TLabel *LabelComputerMarker;
    TBitBtn *BitBtnNone;
    TBitBtn *BitBtnRed;
    TBitBtn *BitBtnLime;
    TBitBtn *BitBtnAqua;
    TBitBtn *BitBtnYellow;
    TBevel *Bevel11;
    TButton *ButtonAdd;
    TButton *ButtonDel;
    TButton *ButtonSave;
    TButton *ButtonCancel;
    TButton *ButtonHelp;
    TBitBtn *BitBtnUsed;
    TBitBtn *BitBtnNotUsed;
    TLabel *Label1;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ListViewComputersDeletion(TObject *Sender,
          TListItem *Item);
    void __fastcall ListViewComputersInsert(TObject *Sender,
          TListItem *Item);
    void __fastcall EditNumberExit(TObject *Sender);
    void __fastcall EditAddressExit(TObject *Sender);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDelClick(TObject *Sender);
    void __fastcall ButtonSaveClick(TObject *Sender);
    void __fastcall BitBtnNoneClick(TObject *Sender);
    void __fastcall BitBtnUsedClick(TObject *Sender);
    void __fastcall ListViewComputersSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ListViewComputersCompare(TObject *Sender,
          TListItem *Item1, TListItem *Item2, int Data, int &Compare);
private:	// User declarations
    void SetEdit(bool Edit_);
    void SetListViewComputersLine(TListItem *Item_);
public:		// User declarations
    __fastcall TFormComputers(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormComputers *FormComputers;
//---------------------------------------------------------------------------
#endif

