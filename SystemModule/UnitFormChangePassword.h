//---------------------------------------------------------------------------
#ifndef UnitFormChangePasswordH
#define UnitFormChangePasswordH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitUsers.h"
//---------------------------------------------------------------------------
class TFormChangePassword : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TComboBox *ComboBoxLogin;
    TEdit *EditPassword;
    TLabel *Label2;
    TBitBtn *BitBtnOk;
    TBitBtn *BitBtnCancel;
    TBevel *Bevel1;
    TLabel *Label4;
    TEdit *EditNewPassword;
    TLabel *Label5;
    TEdit *EditConfirmPassword;
    TBevel *Bevel2;
        TComboBox *ComboBoxPasswordLength;
    TButton *Button1;
    TLabel *LabelInfo;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
    void __fastcall BitBtnCancelClick(TObject *Sender);
    void __fastcall BitBtnOkClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
public:		// User declarations
    __fastcall TFormChangePassword(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormChangePassword *FormChangePassword;
//---------------------------------------------------------------------------
#endif

