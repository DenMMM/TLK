//---------------------------------------------------------------------------
#ifndef UnitFormLogInH
#define UnitFormLogInH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TFormLogIn : public TForm
{
__published:	// IDE-managed Components
    TLabel *LabelInfo;
    TLabel *Label1;
    TComboBox *ComboBoxLogin;
    TLabel *Label2;
    TEdit *EditPassword;
    TBevel *Bevel1;
    TBitBtn *BitBtnOk;
    TBitBtn *BitBtnCancel;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall BitBtnOkClick(TObject *Sender);
    void __fastcall BitBtnCancelClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
public:		// User declarations
    __fastcall TFormLogIn(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormLogIn *FormLogIn;
//---------------------------------------------------------------------------
#endif

