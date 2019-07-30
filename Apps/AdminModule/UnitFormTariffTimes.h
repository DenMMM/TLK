//---------------------------------------------------------------------------
#ifndef UnitFormTariffTimesH
#define UnitFormTariffTimesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <CheckLst.hpp>
//---------------------------------------------------------------------------
#include "UnitTariffs.h"
//---------------------------------------------------------------------------
class TFormTariffTimes : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewTimes;
    TLabel *LabelTimeTypeText;
    TComboBox *ComboBoxType;
    TLabel *LabelBeginText;
    TLabel *LabelEndText;
    TLabel *LabelSizeText;
    TLabel *LabelCostText;
    TEdit *EditCost;
    TComboBox *ComboBoxBeginH;
    TComboBox *ComboBoxBeginM;
    TComboBox *ComboBoxEndH;
    TComboBox *ComboBoxEndM;
    TComboBox *ComboBoxSizeH;
    TComboBox *ComboBoxSizeM;
    TBevel *Bevel2;
    TButton *ButtonSave;
    TButton *ButtonCancel;
    TButton *ButtonHelp;
    TButton *ButtonAdd;
    TButton *ButtonDel;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall EditCostExit(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDelClick(TObject *Sender);
    void __fastcall ListViewTimesInsert(TObject *Sender, TListItem *Item);
    void __fastcall ListViewTimesSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall ComboBoxSizeHClick(TObject *Sender);
    void __fastcall ComboBoxBeginHClick(TObject *Sender);
    void __fastcall ComboBoxEndHClick(TObject *Sender);
    void __fastcall ComboBoxTypeClick(TObject *Sender);
    void __fastcall ListViewTimesCompare(TObject *Sender, TListItem *Item1,
          TListItem *Item2, int Data, int &Compare);
private:	// User declarations
    MTariffTimes TmpTimes;              // Буфер для списка пакетов тарифа
    void SetEdit(bool Edit_);
    void SetListViewTimesLine(TListItem *Item_);
public:		// User declarations
    bool Execute(MTariffTimes *Times_, char *Name_, int Left_, int Top_);
    __fastcall TFormTariffTimes(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

