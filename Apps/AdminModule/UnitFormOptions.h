//---------------------------------------------------------------------------
#ifndef UnitFormOptionsH
#define UnitFormOptionsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitOptions.h"
//---------------------------------------------------------------------------
class TFormOptions : public TForm
{
__published:	// IDE-managed Components
    TBevel *BevelBorder;
    TButton *ButtonSave;
    TButton *ButtonCancel;
    TButton *ButtonHelp;
    TLabel *Label1;
    TEdit *EditCostDialogTime;
    TLabel *Label2;
    TEdit *EditCostPrecision;
    TLabel *Label3;
    TEdit *EditFilterFreeTime;
    TCheckBox *CheckBoxRightPause;
    TBevel *Bevel1;
    TBevel *Bevel2;
    TLabel *Label4;
    TComboBox *ComboBoxLogPeriod;
    TBevel *Bevel3;
    void __fastcall EditCostDialogTimeExit(TObject *Sender);
    void __fastcall EditCostPrecisionExit(TObject *Sender);
    void __fastcall EditFilterFreeTimeExit(TObject *Sender);
    void __fastcall CheckBoxRightPauseExit(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ComboBoxLogPeriodExit(TObject *Sender);
    void __fastcall ButtonSaveClick(TObject *Sender);
private:	// User declarations
    MOptions TmpOptions;
public:		// User declarations
    __fastcall TFormOptions(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

