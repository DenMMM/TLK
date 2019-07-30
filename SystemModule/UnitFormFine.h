//---------------------------------------------------------------------------
#ifndef UnitFormFineH
#define UnitFormFineH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
#include "UnitFineComputers.h"
#include "UnitListOfComputers.h"
//---------------------------------------------------------------------------
class TFormFine : public TForm
{
__published:	// IDE-managed Components
    TBevel *Bevel1;
    TCheckBox *CheckBoxEnableWaiting;
    TBitBtn *BitBtnClose;
    TComboBox *ComboBoxFineTime;
    TRadioButton *RadioButtonCertainTime;
    TRadioButton *RadioButtonAllTime;
    TCheckBox *CheckBoxReturnMany;
    TBevel *Bevel3;
    TBevel *Bevel5;
    TListView *ListViewComputersForFine;
    TListView *ListViewFineComputers;
    TLabel *LabelAllReturnManyText;
    TLabel *LabelAllReturnMany;
    TBevel *Bevel6;
    TTimer *TimerDialogLock;
    TProgressBar *ProgressBarDialogUseTime;
    TBevel *BevelBaseButtonsOut;
    TBevel *BevelBaseButtonsIn;
    TButton *ButtonAdd;
    TButton *ButtonChange;
    TButton *ButtonDelete;
    TButton *ButtonFine;
    void __fastcall BitBtnCloseClick(TObject *Sender);
    void __fastcall RadioButtonCertainTimeClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ComboBoxFineTimeChange(TObject *Sender);
    void __fastcall CheckBoxReturnManyClick(TObject *Sender);
    void __fastcall CheckBoxEnableWaitingClick(TObject *Sender);
    void __fastcall ListViewComputersForFineEnter(TObject *Sender);
    void __fastcall TimerDialogLockTimer(TObject *Sender);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonChangeClick(TObject *Sender);
    void __fastcall ButtonDeleteClick(TObject *Sender);
    void __fastcall ButtonFineClick(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall ListViewFineComputersEnter(TObject *Sender);
    void __fastcall ListViewFineComputersInsert(TObject *Sender,
          TListItem *Item);
    void __fastcall ListViewFineComputersDeletion(TObject *Sender,
          TListItem *Item);
private:	// User declarations
    MFineComputer *CurrentFineComputer;
    void __fastcall SetListViewComputersForFineLine(TListItem *Item_);
    void __fastcall SetListViewFineComputersLine(TListItem *Item_);
    void __fastcall SetGroupStateCertTime(bool State_);
    void __fastcall SetGroupStateAllTime(bool State_);
public:		// User declarations
    __fastcall TFormFine(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormFine *FormFine;
//---------------------------------------------------------------------------
#endif

