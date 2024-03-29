﻿//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormNewPass.h"
#include "UnitPassword.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormNewPass::TFormNewPass(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
bool TFormNewPass::Execute(
	std::wstring &Result_,
	int MinLength_, int MaxLength_,
	int Left_, int Top_, bool LeftTop_)
{
	MinLength=MinLength_;
	MaxLength=MaxLength_;

	for ( int i=MinLength; i<=MaxLength; i++ )
		ComboBoxLength->Items->Add(IntToStr(i));

	ComboBoxLength->ItemIndex=0;
	CheckBoxCapital->Checked=false;
	CheckBoxSmall->Checked=false;
	CheckBoxNum->Checked=true;
	ActiveControl=ButtonNew;
	SetCoord(Left_,Top_,LeftTop_);

	if ( ShowModal()!=mrOk ) return false;

	Result_=std::move(TmpPass);
	return true;
}
//---------------------------------------------------------------------------
void TFormNewPass::SetCoord(int Left_, int Top_, bool LeftTop_)
{
    if ( LeftTop_ ) { Left=Left_; Top=Top_; }
    else { Left=Left_-Width; Top=Top_-Height; }
}
//---------------------------------------------------------------------------
void __fastcall TFormNewPass::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	LabelPassword->Caption=L"";
    ComboBoxLength->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormNewPass::ButtonNewClick(TObject *Sender)
{
	TmpPass=
		MPassword::New(
			ComboBoxLength->ItemIndex+MinLength,
			CheckBoxCapital->Checked,
			CheckBoxSmall->Checked,
			CheckBoxNum->Checked);
	LabelPassword->Caption=TmpPass.c_str();
}
//---------------------------------------------------------------------------

