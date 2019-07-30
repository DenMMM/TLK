//---------------------------------------------------------------------------
#ifndef UnitFormLogInH
#define UnitFormLogInH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitUsers.h"
//---------------------------------------------------------------------------
class TFormLogIn : public TForm
{
__published:	// IDE-managed Components
    TBevel *Bevel1;
    TLabel *LabelLogin;
    TLabel *LabelPassword;
    TBevel *Bevel2;
    TLabel *Label5;
    TComboBox *ComboBoxLogin;
    TEdit *EditPassword;
    TButton *ButtonApply;
    TButton *ButtonCancel;
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ComboBoxLoginClick(TObject *Sender);
private:	// User declarations
    unsigned ID;
public:		// User declarations
    unsigned Execute(MUsers *Users_);
    __fastcall TFormLogIn(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

