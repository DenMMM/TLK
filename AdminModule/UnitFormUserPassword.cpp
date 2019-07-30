//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormUserPassword.h"
#include "UnitFormNewPassword.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormUserPassword *FormUserPassword;
//---------------------------------------------------------------------------
__fastcall TFormUserPassword::TFormUserPassword(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
bool TFormUserPassword::Execute(MUser *User_, int Left_, int Top_, bool LeftTop_)
{
    Users=false;
    ComboBoxLogin->Enabled=false;
    SetEdit(false,true);
    ComboBoxLogin->Items->AddObject(User_->Login,(TObject*)User_);
    ComboBoxLogin->ItemIndex=0;
    ActiveControl=EditNew;
    SetCoord(Left_,Top_,LeftTop_);
    return ShowModal()==mrOk;
}
//---------------------------------------------------------------------------
bool TFormUserPassword::Execute(MUsers *Users_, int Left_, int Top_, bool LeftTop_)
{
    Users=true;
    ComboBoxLogin->Enabled=true;
    SetEdit(false,false);
    for ( MUser *User=(MUser*)Users_->First; User; User=(MUser*)User->Next )
    {
        if ( !User->Active ) continue;
        ComboBoxLogin->Items->AddObject(User->Login,(TObject*)User);
    }
    ActiveControl=ComboBoxLogin;
    SetCoord(Left_,Top_,LeftTop_);
    return ShowModal()==mrOk;
}
//---------------------------------------------------------------------------
void TFormUserPassword::SetCoord(int Left_, int Top_, bool LeftTop_)
{
    if ( LeftTop_ ) { Left=Left_; Top=Top_; }
    else { Left=Left_-Width; Top=Top_-Height; }
}
//---------------------------------------------------------------------------
void TFormUserPassword::SetEdit(bool Current_, bool New_)
{
    TColor color;

    color=Current_? clWindow: clBtnFace;
    LabelPassword->Enabled=Current_;
    EditPassword->Enabled=Current_;
    EditPassword->Color=color;

    color=New_? clWindow: clBtnFace;
    LabelNew->Enabled=New_;
    EditNew->Enabled=New_;
    EditNew->Color=color;
    ButtonGenerate->Enabled=New_;
    LabelConfirm->Enabled=New_;
    EditConfirm->Enabled=New_;
    EditConfirm->Color=color;
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPassword::FormShow(TObject *Sender)
{
    EditPassword->MaxLength=MAX_UserPasswordLength;
    EditPassword->PasswordChar='*';
    EditNew->MaxLength=MAX_UserPasswordLength;
    EditNew->PasswordChar='*';
    EditConfirm->MaxLength=MAX_UserPasswordLength;
    EditConfirm->PasswordChar='*';
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPassword::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
    int Index;
    MUser *User;

    if ( ModalResult!=mrOk ) return;
    // Определяем какой пользователь был выбран
    if ( (Index=ComboBoxLogin->ItemIndex)<0 )
        { ActiveControl=ComboBoxLogin; goto error; }
    User=(MUser*)ComboBoxLogin->Items->Objects[Index];
    // Проверяем текущий пароль
    if ( Users&&(!User->CheckPassword(EditPassword->Text.c_str())) )
        { ActiveControl=EditPassword; goto error; }
    // Проверяем новый пароль
    if ( EditNew->Text!=EditConfirm->Text )
        { ActiveControl=EditNew; goto error; }
    //
    User->SetPassword(EditNew->Text.c_str());

    return;
error:
    CanClose=false;
    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPassword::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ComboBoxLogin->Clear();
    EditPassword->Text=""; EditPassword->ClearUndo();
    EditNew->Text=""; EditNew->ClearUndo();
    EditConfirm->Text=""; EditConfirm->ClearUndo();
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPassword::ButtonGenerateClick(TObject *Sender)
{
    char buffer[MAX_UserPasswordLength+1];

    if ( FormNewPassword->Execute(buffer,5,MAX_UserPasswordLength,
        Left+20,Top+30,true) )
    {
        EditNew->Text=buffer;
        EditConfirm->Text=""; EditConfirm->ClearUndo();
        ActiveControl=EditConfirm;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPassword::ComboBoxLoginClick(TObject *Sender)
{
    SetEdit(true,true);
}
//---------------------------------------------------------------------------

