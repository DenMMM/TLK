//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormLogIn.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormLogIn::TFormLogIn(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
unsigned TFormLogIn::Execute(MUsers *Users_)
{
    ID=0;

    // Заносим в список активные логины
    for ( MUser *User=(MUser*)Users_->gFirst();
        User; User=(MUser*)User->gNext() )
    {
        if ( !User->Active ) continue;
        ComboBoxLogin->Items->AddObject(User->Login.c_str(),(TObject*)User);
    }

    return ShowModal()==mrOk? ID: 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogIn::FormShow(TObject *Sender)
{
    EditPassword->MaxLength=MAX_UserPassLen;
    EditPassword->PasswordChar=PASS_Char;
    LabelPassword->Enabled=false;
    EditPassword->Enabled=false;
    EditPassword->Color=clBtnFace;
    ActiveControl=ComboBoxLogin;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogIn::ComboBoxLoginClick(TObject *Sender)
{
    if ( ComboBoxLogin->ItemIndex<0 ) return;
    LabelPassword->Enabled=true;
    EditPassword->Enabled=true;
    EditPassword->Color=clWindow;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogIn::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    int Index;
    MUser *User;

    if ( ModalResult!=mrOk ) return;
    // Определяем какой пользователь был выбран
    if ( (Index=ComboBoxLogin->ItemIndex)<0 )
        { ActiveControl=ComboBoxLogin; goto error; }
    User=(MUser*)ComboBoxLogin->Items->Objects[Index];
    // Проверяем пароль
    if ( !User->Pass.Check(EditPassword->Text.c_str()) )
        { ActiveControl=EditPassword; goto error; }
    //
    ID=User->gItemID();

    return;
error:
    CanClose=false;
    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogIn::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ComboBoxLogin->Clear();
    EditPassword->Text=""; EditPassword->ClearUndo();
}
//---------------------------------------------------------------------------

