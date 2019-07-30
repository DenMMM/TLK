//---------------------------------------------------------------------------
#ifndef UnitFormNewPasswordH
#define UnitFormNewPasswordH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormNewPassword : public TForm
{
__published:	// IDE-managed Components
    TLabel *LabelPassword;
    TButton *ButtonNew;
    TLabel *LabelLength;
    TComboBox *ComboBoxLength;
    TCheckBox *CheckBoxCapital;
    TCheckBox *CheckBoxSmall;
    TCheckBox *CheckBoxNum;
    TBevel *Bevel1;
    TBevel *Bevel2;
    TBevel *BevelBorder;
    TButton *ButtonUse;
    TButton *ButtonCancel;
    void __fastcall ButtonNewClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
    char *Buffer;
    int MinLength;
    int MaxLength;
    void SetCoord(int Left_, int Top_, bool LeftTop_);
public:		// User declarations
    __fastcall TFormNewPassword(TComponent* Owner);
    bool Execute(char *Buffer_, int MinLength_, int MaxLength_,
        int Left_, int Top_, bool LeftTop_);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormNewPassword *FormNewPassword;
//---------------------------------------------------------------------------
#endif
