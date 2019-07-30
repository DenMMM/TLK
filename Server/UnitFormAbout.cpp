//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormAbout.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAbout *FormAbout;
//---------------------------------------------------------------------------
__fastcall TFormAbout::TFormAbout(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormAbout::Panel1Click(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

