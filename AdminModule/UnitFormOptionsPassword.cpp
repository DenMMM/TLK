//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormOptionsPassword.h"
#include "UnitFormNewPassword.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormOptionsPassword *FormOptionsPassword;
//---------------------------------------------------------------------------
__fastcall TFormOptionsPassword::TFormOptionsPassword(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
bool TFormOptionsPassword::Execute(MOptions *Options_, int Left_, int Top_, bool LeftTop_)
{
    Options=Options_;
    ActiveControl=EditPassword;
    SetCoord(Left_,Top_,LeftTop_);
    return ShowModal()==mrOk;
}
//---------------------------------------------------------------------------
void TFormOptionsPassword::SetCoord(int Left_, int Top_, bool LeftTop_)
{
    if ( LeftTop_ ) { Left=Left_; Top=Top_; }
    else { Left=Left_-Width; Top=Top_-Height; }
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPassword::FormShow(TObject *Sender)
{
    EditPassword->MaxLength=MAX_UserPasswordLength;
    EditPassword->PasswordChar='*';
    EditNew->MaxLength=MAX_UserPasswordLength;
    EditNew->PasswordChar='*';
    EditConfirm->MaxLength=MAX_UserPasswordLength;
    EditConfirm->PasswordChar='*';
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPassword::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
    if ( ModalResult!=mrOk ) return;
    // Проверяем текущий пароль
    if ( !Options->CheckPassword(EditPassword->Text.c_str()) )
        { ActiveControl=EditPassword; goto error; }
    // Проверяем новый пароль
    if ( EditNew->Text!=EditConfirm->Text )
        { ActiveControl=EditNew; goto error; }
    //
    Options->SetPassword(EditNew->Text.c_str());

    return;
error:
    CanClose=false;
    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPassword::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    EditPassword->Text=""; EditPassword->ClearUndo();
    EditNew->Text=""; EditNew->ClearUndo();
    EditConfirm->Text=""; EditConfirm->ClearUndo();
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPassword::ButtonGenerateClick(TObject *Sender)
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

