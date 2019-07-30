//---------------------------------------------------------------------------
#ifndef UnitFormTariffsH
#define UnitFormTariffsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include <CheckLst.hpp>
//---------------------------------------------------------------------------
#include "UnitTariffs.h"
//---------------------------------------------------------------------------
class TFormTariffs : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewNames;
    TBitBtn *BitBtnSetSelComp;
    TBitBtn *BitBtnSetAllComp;
    TBitBtn *BitBtnResetSelComp;
    TBitBtn *BitBtnResetAllComp;
    TBevel *BevelBorder;
    TEdit *EditName;
    TCheckBox *CheckBoxPage1;
    TCheckBox *CheckBoxPage2;
    TCheckBox *CheckBoxPage3;
    TLabel *LabelPages;
    TBitBtn *BitBtnClose;
    TImage *ImageIcon;
    TBitBtn *BitBtnSelectIcon;
    TBevel *BevelBorderIcon;
    TOpenPictureDialog *OpenPictureDialog;
    TLabel *Label1;
    TCheckBox *CheckBoxPage4;
    TCheckBox *CheckBoxPage5;
    TCheckBox *CheckBoxPage6;
    TCheckBox *CheckBoxPage7;
    TBevel *Bevel1;
    TBitBtn *BitBtnSave;
    TListView *ListViewComputers;
    TBitBtn *BitBtnHelp;
    TBevel *Bevel2;
    TButton *ButtonAdd;
    TButton *ButtonDelete;
    TButton *ButtonChange;
    TButton *ButtonPorts;
    TButton *ButtonTimes;
    TCheckBox *CheckBoxReboot;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ListViewNamesChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
    void __fastcall EditNameExit(TObject *Sender);
    void __fastcall BitBtnSetSelCompClick(TObject *Sender);
    void __fastcall BitBtnSetAllCompClick(TObject *Sender);
    void __fastcall BitBtnSelectIconClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ListViewComputersKeyPress(TObject *Sender, char &Key);
    void __fastcall BitBtnSaveClick(TObject *Sender);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDeleteClick(TObject *Sender);
    void __fastcall ButtonChangeClick(TObject *Sender);
    void __fastcall ButtonTimesClick(TObject *Sender);
    void __fastcall CheckBoxPage1Click(TObject *Sender);
    void __fastcall CheckBoxRebootClick(TObject *Sender);
private:	// User declarations
    MTariffs *TMPTariffs;
    MTariff *TMPTariff;
    unsigned int NewID;
    void SetListViewNamesLine(int Number_);
public:		// User declarations
    __fastcall TFormTariffs(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTariffs *FormTariffs;
//---------------------------------------------------------------------------
#endif

