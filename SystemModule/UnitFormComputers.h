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
    TLabel *LabelAddress;
    TEdit *EditAddress;
    TLabel *LabelNumber;
    TEdit *EditNumber;
    TBevel *BevelBorder;
    TBitBtn *BitBtnMarkerRed;
    TBitBtn *BitBtnMarkerLime;
    TLabel *LabelMarkerColor;
    TBitBtn *BitBtnMarkerAqua;
    TBitBtn *BitBtnMarkerYellow;
    TShape *ShapeMarkerColor;
    TBitBtn *BitBtnSave;
    TRadioButton *RadioButtonJust;
    TRadioButton *RadioButtonPattern;
    TRadioButton *RadioButtonNet;
    TLabel *LabelPattern;
    TEdit *EditPattern;
    TBevel *BevelJust;
    TBevel *BevelPattern;
    TLabel *LabelBeginNumber;
    TEdit *EditBeginNumber;
    TLabel *LabelEndNumber;
    TEdit *EditEndNumber;
    TLabel *LabelGlobalPattern;
    TEdit *EditGlobalPattern;
    TBevel *BevelNet;
    TBitBtn *BitBtnCancel;
    TBitBtn *BitBtnHelp;
    TButton *ButtonAdd;
    TButton *ButtonDelete;
    TButton *ButtonChange;
    TBevel *Bevel1;
    TBitBtn *BitBtnMarkerNone;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall EditAddressExit(TObject *Sender);
    void __fastcall EditNumberExit(TObject *Sender);
    void __fastcall RadioButtonJustClick(TObject *Sender);
    void __fastcall BitBtnSaveClick(TObject *Sender);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDeleteClick(TObject *Sender);
    void __fastcall ButtonChangeClick(TObject *Sender);
    void __fastcall ListViewComputersDeletion(TObject *Sender,
          TListItem *Item);
    void __fastcall ListViewComputersSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall ListViewComputersInsert(TObject *Sender,
          TListItem *Item);
    void __fastcall BitBtnMarkerNoneClick(TObject *Sender);
private:	// User declarations
    MCompParam *TMPCompParam;
    void __fastcall SetListViewComputersLine(TListItem *Item_);
    void __fastcall SetGroupStateJust(bool State_);
    void __fastcall SetGroupStatePattern(bool State_);
    void __fastcall SetGroupStateNet(bool State_);
public:		// User declarations
    __fastcall TFormComputers(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormComputers *FormComputers;
//---------------------------------------------------------------------------
#endif

