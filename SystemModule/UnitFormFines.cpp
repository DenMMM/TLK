//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormFines.h"
#include "UnitFormMain.h"
#include "UnitOptionsLoadSave.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormFines *FormFines;
//---------------------------------------------------------------------------
__fastcall TFormFines::TFormFines(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::FormShow(TObject *Sender)
{
    TMPFines=new MFines;
    TMPFine=new MFine;

    *TMPFines=*Fines;
    //
    ListViewFines->Items->Clear();
    int i=0;
    for ( MFine *Fine=(MFine*)TMPFines->FirstItem; Fine; Fine=(MFine*)Fine->NextItem )
    {
        TListItem *NewItem;
        NewItem=ListViewFines->Items->Add();
        NewItem->SubItems->Add("");
        NewItem->SubItems->Add("");
        SetListViewFinesLine(i);
        i++;
    }

    ComboBoxTime->Items->Clear();
    for ( int i=1; i<=60; i++ ) ComboBoxTime->Items->Add(IntToStr(i));
    ComboBoxTime->ItemIndex=10-1;
    ComboBoxTimeChange(ComboBoxTime);
    EditComment->Text="Обычный штраф";
    EditCommentExit(EditComment);

    ActiveControl=ListViewFines;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ListViewFines->Items->Clear();
    ComboBoxTime->Items->Clear();
    EditComment->Text="";

    delete TMPFines;
    delete TMPFine;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ListViewFinesChange(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
    if ( (Change!=ctState)||(!Item->Focused) ) return;
    //
    *TMPFine=*((MFine*)TMPFines->Item(Item->Index));
    //
    ComboBoxTime->ItemIndex=TMPFine->Time-1;
    EditComment->Text=TMPFine->Comment;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ComboBoxTimeChange(TObject *Sender)
{
    int a;
    if ( (a=ComboBoxTime->ItemIndex)<0 ) return;
    TMPFine->Time=a+1;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::EditCommentExit(TObject *Sender)
{
    TMPFine->Comment=EditComment->Text.Trim();
    EditComment->Text=TMPFine->Comment;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::BitBtnAddClick(TObject *Sender)
{
    MFine *NewFine;
    NewFine=(MFine*)TMPFines->Add();
    *NewFine=*TMPFine;

    TListItem *NewItem;
    NewItem=ListViewFines->Items->Add();
    NewItem->SubItems->Add("");
    NewItem->SubItems->Add("");
    SetListViewFinesLine(NewItem->Index);
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::BitBtnDeleteClick(TObject *Sender)
{
    TListItems *Items=ListViewFines->Items;
    TListItem *Item;
    for ( int i=Items->Count-1; i>=0; i-- )
    {
        if ( !(Item=Items->Item[i])->Selected ) continue;
        Item->Delete();
        TMPFines->Delete(i);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::BitBtnChangeClick(TObject *Sender)
{
    TListItem *Item;
    if ( ((Item=ListViewFines->ItemFocused)==NULL)||
        (ListViewFines->SelCount>=2) ) return;
    *((MFine*)TMPFines->Item(Item->Index))=*TMPFine;
    SetListViewFinesLine(Item->Index);
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::BitBtnSaveClick(TObject *Sender)
{
    *Fines=*TMPFines;
    FinesSave(Fines);
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::SetListViewFinesLine(int Number_)
{
    MFine *Fine=(MFine*)TMPFines->Item(Number_);
    TStrings *SubItems=ListViewFines->Items->Item[Number_]->SubItems;

    SubItems->Strings[0]=IntToStr(Fine->Time)+" мин.";
    SubItems->Strings[1]=Fine->Comment;
}
//---------------------------------------------------------------------------

