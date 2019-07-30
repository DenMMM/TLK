//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormOpenConfig.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormOpenConfig::TFormOpenConfig(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
bool TFormOpenConfig::Execute(MOptions *Options_, int Left_, int Top_, bool LeftTop_)
{
    Options=Options_;
    ActiveControl=EditPassword;
    SetCoord(Left_,Top_,LeftTop_);
    return ShowModal()==mrOk;
}
//---------------------------------------------------------------------------
void TFormOpenConfig::SetCoord(int Left_, int Top_, bool LeftTop_)
{
    if ( LeftTop_ ) { Left=Left_; Top=Top_; }
    else { Left=Left_-Width; Top=Top_-Height; }
}
//---------------------------------------------------------------------------
void __fastcall TFormOpenConfig::FormShow(TObject *Sender)
{
    EditPassword->MaxLength=MAX_OptPassLen;
    EditPassword->PasswordChar=PASS_Char;
}
//---------------------------------------------------------------------------
void __fastcall TFormOpenConfig::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
    if ( ModalResult!=mrOk ) return;
    // Проверяем пароль
    if ( !Options->Pass.Check(EditPassword->Text.c_str()) )
        { ActiveControl=EditPassword; CanClose=false; }
    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormOpenConfig::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    EditPassword->Text=""; EditPassword->ClearUndo();
}
//---------------------------------------------------------------------------

