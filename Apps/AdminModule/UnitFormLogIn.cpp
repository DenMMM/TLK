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
unsigned TFormLogIn::Execute(const MUsers &Users_)
{
    ID=0;

    // Заносим в список активные логины
	for ( const auto &User: Users_ )
	{
		if ( !User.Active ) continue;
		ComboBoxLogin->Items->AddObject(
			User.Login.c_str(),
			reinterpret_cast<TObject*>(
				const_cast<MUsersItem*>(&User)));
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
	if ( ModalResult!=mrOk ) return;

	// Определяем какой пользователь был выбран
	int Index=ComboBoxLogin->ItemIndex;
	if ( Index<0 )
	{
		ActiveControl=ComboBoxLogin;
		CanClose=false;
		return;
	}

	auto &User=*reinterpret_cast<const MUsersItem*>(
		ComboBoxLogin->Items->Objects[Index]);

	// Проверяем пароль
	if ( !User.Pass.Check(EditPassword->Text.c_str()) )
	{
		ActiveControl=EditPassword;
		CanClose=false;
		return;
	}
	//
	ID=User.gUUID();

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

