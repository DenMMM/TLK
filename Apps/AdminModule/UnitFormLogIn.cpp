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
	for ( auto &User: *Users_ )
	{
		if ( !User.Active ) continue;
		ComboBoxLogin->Items->AddObject(
			User.Login.c_str(),
			reinterpret_cast<TObject*>(&User));
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

	// Добавим энтропии
	BasicRand.event();
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
    MUsersItem *User;

	if ( ModalResult!=mrOk ) return;
	// Определяем какой пользователь был выбран
	if ( (Index=ComboBoxLogin->ItemIndex)<0 )
		{ ActiveControl=ComboBoxLogin; goto error; }
	User=reinterpret_cast<MUsersItem*>(ComboBoxLogin->Items->Objects[Index]);
	// Проверяем пароль
	if ( !User->Pass.Check(EditPassword->Text.c_str()) )
		{ ActiveControl=EditPassword; goto error; }
	//
	ID=User->gUUID();

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
    EditPassword->Text=L""; EditPassword->ClearUndo();
}
//---------------------------------------------------------------------------

