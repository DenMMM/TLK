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
    TListView *ListViewTimesAndCosts;
    TLabel *LabelTimeTypeText;
    TComboBox *ComboBoxTimeType;
    TBevel *Bevel1;
    TLabel *LabelDaysText;
    TCheckBox *CheckBoxDayMo;
    TCheckBox *CheckBoxDayTu;
    TCheckBox *CheckBoxDayWe;
    TCheckBox *CheckBoxDayTh;
    TCheckBox *CheckBoxDayFr;
    TCheckBox *CheckBoxDaySa;
    TCheckBox *CheckBoxDaySu;
    TLabel *LabelBeginTimeText;
    TLabel *LabelEndTimeText;
    TLabel *LabelSizeTimeText;
    TLabel *LabelCostText;
    TEdit *EditCost;
    TBitBtn *BitBtnSave;
    TBitBtn *BitBtnCancel;
    TComboBox *ComboBoxBeginTimeH;
    TComboBox *ComboBoxBeginTimeM;
    TComboBox *ComboBoxEndTimeH;
    TComboBox *ComboBoxEndTimeM;
    TComboBox *ComboBoxSizeTimeH;
    TComboBox *ComboBoxSizeTimeM;
    TBevel *Bevel2;
    TBitBtn *BitBtnHelp;
    TCheckBox *CheckBoxEnableDesc;
    TButton *ButtonAdd;
    TButton *ButtonDelete;
    TButton *ButtonChange;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ListViewTimesAndCostsChange(TObject *Sender,
          TListItem *Item, TItemChange Change);
    void __fastcall ComboBoxTimeTypeChange(TObject *Sender);
    void __fastcall CheckBoxDayMoClick(TObject *Sender);
    void __fastcall EditCostExit(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ComboBoxBeginTimeHChange(TObject *Sender);
    void __fastcall ComboBoxEndTimeHChange(TObject *Sender);
    void __fastcall ComboBoxSizeTimeHChange(TObject *Sender);
    void __fastcall CheckBoxEnableDescClick(TObject *Sender);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDeleteClick(TObject *Sender);
    void __fastcall ButtonChangeClick(TObject *Sender);
private:	// User declarations
    MTariffTimes *TMPTariffTimes;
    MTariffTime *TMPTariffTime;
    void __fastcall SetTimesAndCostsLine(TListItem *ListItem_,MTariffTime *TariffTime_);
    void __fastcall SetTimeAndCostParam();
public:		// User declarations
    void __fastcall Execute(MTariffTimes *TariffTimes_);
    __fastcall TFormTariffTimes(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTariffTimes *FormTariffTimes;
//---------------------------------------------------------------------------
#endif
