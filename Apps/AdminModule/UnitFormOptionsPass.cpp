//---------------------------------------------------------------------------
#include <vcl.h>
#include <memory>
#pragma hdrstop

#include "UnitFormOptionsPass.h"
#include "UnitFormNewPass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormOptionsPass::TFormOptionsPass(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
bool TFormOptionsPass::Execute(MOptions *Options_, int Left_, int Top_, bool LeftTop_)
{
    TmpOptions=Options_;
    ActiveControl=EditPassword;
    SetCoord(Left_,Top_,LeftTop_);
    return ShowModal()==mrOk;
}
//---------------------------------------------------------------------------
void TFormOptionsPass::SetCoord(int Left_, int Top_, bool LeftTop_)
{
    if ( LeftTop_ ) { Left=Left_; Top=Top_; }
    else { Left=Left_-Width; Top=Top_-Height; }
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPass::FormShow(TObject *Sender)
{
    EditPassword->MaxLength=MAX_OptPassLen;
    EditPassword->PasswordChar=PASS_Char;
    EditNew->MaxLength=MAX_OptPassLen;
    EditNew->PasswordChar=PASS_Char;
    EditConfirm->MaxLength=MAX_OptPassLen;
	EditConfirm->PasswordChar=PASS_Char;

	// Добавим энтропии
	BasicRand.event();
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPass::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
    if ( ModalResult!=mrOk ) return;
    // Проверяем текущий пароль
    if ( !TmpOptions->Pass.Check(EditPassword->Text.c_str()) )
        { ActiveControl=EditPassword; goto error; }
    // Проверяем новый пароль
    if ( EditNew->Text!=EditConfirm->Text )
        { ActiveControl=EditNew; goto error; }
    //
    TmpOptions->Pass.Set(EditNew->Text.c_str());

    return;
error:
    CanClose=false;
    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPass::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	EditPassword->Text=L""; EditPassword->ClearUndo();
	EditNew->Text=L""; EditNew->ClearUndo();
    EditConfirm->Text=L""; EditConfirm->ClearUndo();
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPass::ButtonGenerateClick(TObject *Sender)
{
	std::unique_ptr <TFormNewPass> form;
	std::wstring buffer;

	try
	{
		form.reset(new TFormNewPass(0));
		if ( !form->Execute(buffer,5,MAX_OptPassLen,
			Left+20,Top+30,true) ) return;
		EditNew->Text=buffer.c_str();
		EditConfirm->Text=L""; EditConfirm->ClearUndo();
		ActiveControl=EditConfirm;
	}
	catch (Exception &ex)
	{
		Application->ShowException(&ex);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPass::EditPasswordKeyPress(TObject *Sender, System::WideChar &Key)
{
	// Добавим энтропии
	BasicRand.event();
}
//---------------------------------------------------------------------------

