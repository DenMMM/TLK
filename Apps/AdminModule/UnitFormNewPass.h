//---------------------------------------------------------------------------
#ifndef UnitFormNewPassH
#define UnitFormNewPassH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormNewPass : public TForm
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
	std::wstring TmpPass;
	int MinLength;
    int MaxLength;
    void SetCoord(int Left_, int Top_, bool LeftTop_);
public:		// User declarations
    __fastcall TFormNewPass(TComponent* Owner);
	bool Execute(
		std::wstring &Result_,
		int MinLength_, int MaxLength_,
        int Left_, int Top_, bool LeftTop_);
};
//---------------------------------------------------------------------------
#endif
