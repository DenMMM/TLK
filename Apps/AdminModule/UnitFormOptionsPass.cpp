//---------------------------------------------------------------------------
#include <vcl.h>
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
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPass::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
    if ( ModalResult!=mrOk ) return;
    // Проверяем текущий пароль
    if ( !TmpOptions->CheckPass(EditPassword->Text.c_str()) )
        { ActiveControl=EditPassword; goto error; }
    // Проверяем новый пароль
    if ( EditNew->Text!=EditConfirm->Text )
        { ActiveControl=EditNew; goto error; }
    //
    TmpOptions->SetPass(EditNew->Text.c_str());

    return;
error:
    CanClose=false;
    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPass::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    EditPassword->Text=""; EditPassword->ClearUndo();
    EditNew->Text=""; EditNew->ClearUndo();
    EditConfirm->Text=""; EditConfirm->ClearUndo();
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionsPass::ButtonGenerateClick(TObject *Sender)
{
    Munique_ptr <TFormNewPass> form;
    char buffer[MAX_OptPassLen+1];

    try
    {
        form(new TFormNewPass(0));
        if ( !form.get()->Execute(buffer,5,sizeof(buffer),
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

