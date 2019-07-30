//---------------------------------------------------------------------------
#ifndef UnitFormUserPasswordH
#define UnitFormUserPasswordH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitUsers.h"
//---------------------------------------------------------------------------
class TFormUserPassword : public TForm
{
__published:	// IDE-managed Components
    TLabel *LabelLogin;
    TComboBox *ComboBoxLogin;
    TLabel *LabelPassword;
    TEdit *EditPassword;
    TLabel *LabelNew;
    TEdit *EditNew;
    TLabel *LabelConfirm;
    TEdit *EditConfirm;
    TButton *ButtonGenerate;
    TButton *ButtonChange;
    TButton *ButtonCancel;
    TBevel *Bevel1;
    TBevel *Bevel2;
    TLabel *Label5;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall ButtonGenerateClick(TObject *Sender);
    void __fastcall ComboBoxLoginClick(TObject *Sender);
private:	// User declarations
    bool Users;
    void SetCoord(int Left_, int Top_, bool LeftTop_);
    void SetEdit(bool Current_, bool New_);
public:		// User declarations
    __fastcall TFormUserPassword(TComponent* Owner);
    bool Execute(MUser *User_, int Left_, int Top_, bool LeftTop_);
    bool Execute(MUsers *Users_, int Left_, int Top_, bool LeftTop_);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormUserPassword *FormUserPassword;
//---------------------------------------------------------------------------
#endif

