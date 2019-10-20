//---------------------------------------------------------------------------
#include <vcl.h>
#include <memory>
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
bool TFormUserPass::Execute(MUsersItem *User_, int Left_, int Top_, bool LeftTop_)
{
    Users=false;
    ComboBoxLogin->Enabled=false;
    SetEdit(false,true);
	ComboBoxLogin->Items->AddObject(
		User_->Login.c_str(),
		reinterpret_cast<TObject*>(User_));
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
	for ( auto &User: *Users_ )
	{
		if ( !User.Active ) continue;
		ComboBoxLogin->Items->AddObject(
			User.Login.c_str(),
			reinterpret_cast<TObject*>(&User));
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
    MUsersItem *User;

	if ( ModalResult!=mrOk ) return;
	// Определяем какой пользователь был выбран
	if ( (Index=ComboBoxLogin->ItemIndex)<0 )
		{ ActiveControl=ComboBoxLogin; goto error; }
	User=reinterpret_cast<MUsersItem*>(
		ComboBoxLogin->Items->Objects[Index]);
	// Проверяем текущий пароль
	if ( Users&&(!User->Pass.Check(EditPassword->Text.c_str())) )
		{ ActiveControl=EditPassword; goto error; }
	// Проверяем новый пароль
	if ( EditNew->Text!=EditConfirm->Text )
		{ ActiveControl=EditNew; goto error; }
	//
	User->Pass.Set(EditNew->Text.c_str());

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
	EditPassword->Text=L""; 	EditPassword->ClearUndo();
	EditNew->Text=L""; 			EditNew->ClearUndo();
	EditConfirm->Text=L""; 		EditConfirm->ClearUndo();
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPass::ButtonGenerateClick(TObject *Sender)
{
	std::unique_ptr <TFormNewPass> form;
	char buffer[MAX_UserPassLen+1];

	try
	{
		form.reset(new TFormNewPass(0));
		if ( !form->Execute(buffer,5,MAX_UserPassLen,
			Left+20,Top+30,true) ) return;
		EditNew->Text=buffer;
		EditConfirm->Text=L""; EditConfirm->ClearUndo();
        ActiveControl=EditConfirm;
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUserPass::ComboBoxLoginClick(TObject *Sender)
{
    SetEdit(true,true);
}
//---------------------------------------------------------------------------

