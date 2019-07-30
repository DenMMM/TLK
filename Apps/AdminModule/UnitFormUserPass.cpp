//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormUserPass.h"
#include "UnitFormNewPass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormUserPass::TFormUserPass(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
bool TFormUserPass::Execute(MUser *User_, int Left_, int Top_, bool LeftTop_)
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
bool TFormUserPass::Execute(MUsers *Users_, int Left_, int Top_, bool LeftTop_)
{
    Users=true;
    ComboBoxLogin->Enabled=true;
    SetEdit(false,false);
    for ( MUser *User=(MUser*)Users_->gFirst(); User;
        User=(MUser*)User->gNext() )
    {
        if ( !User->Active ) continue;
        ComboBoxLogin->Items->AddObject(User->Login,(TObject*)User);
    }
    ActiveControl=ComboBoxLogin;
    SetCoord(Left_,Top_,LeftTop_);
    return ShowModal()==mrOk;
}
//---------------------------------------------------------------------------
void TFormUserPass::SetCoord(int Left_, int Top_, bool LeftTop_)
{
    if ( LeftTop_ ) { Left=Left_; Top=Top_; }
    else { Left=Left_-Width; Top=Top_-Height; }
}
//---------------------------------------------------------------------------
void TFormUserPass::SetEdit(bool Current_, bool New_)
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
void __fastcall TFormUserPass::FormShow(TObject *Sender)
{
    EditPassword->MaxLength=MAX_UserPassLen;
    EditPassword->PasswordChar=PASS_Char;
    EditNew->MaxLength=MAX_UserPassLen;
    EditNew->PasswordChar=PASS_Char;
    EditConfirm->MaxLength=MAX_UserPassLen;
    EditConfirm->PasswordChar=PASS_Char;
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPass::FormCloseQuery(TObject *Sender,
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
    if ( Users&&(!User->CheckPass(EditPassword->Text.c_str())) )
        { ActiveControl=EditPassword; goto error; }
    // Проверяем новый пароль
    if ( EditNew->Text!=EditConfirm->Text )
        { ActiveControl=EditNew; goto error; }
    //
    User->SetPass(EditNew->Text.c_str());

    return;
error:
    CanClose=false;
    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPass::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ComboBoxLogin->Clear();
    EditPassword->Text=""; EditPassword->ClearUndo();
    EditNew->Text=""; EditNew->ClearUndo();
    EditConfirm->Text=""; EditConfirm->ClearUndo();
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPass::ButtonGenerateClick(TObject *Sender)
{
    Mptr <TFormNewPass> form;
    char buffer[MAX_UserPassLen+1];

    try
    {
        form(new TFormNewPass(0));
        if ( !form->Execute(buffer,5,sizeof(buffer),
            Left+20,Top+30,true) ) return;
        EditNew->Text=buffer;
        EditConfirm->Text=""; EditConfirm->ClearUndo();
        ActiveControl=EditConfirm;
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPass::ComboBoxLoginClick(TObject *Sender)
{
    SetEdit(true,true);
}
//---------------------------------------------------------------------------

