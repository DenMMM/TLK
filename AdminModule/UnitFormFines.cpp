//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormFines.h"
#include "UnitFormMain.h"
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
    // Формируем список штрафов
    for ( MFine *Fine=(MFine*)Fines->First; Fine; Fine=(MFine*)Fine->Next )
    {
        TListItem *NewItem;
        NewItem=ListViewFines->Items->Add();
        ((MFine*)NewItem->Data)->Copy(Fine);
        SetListViewFinesLine(NewItem);
    }
    ListViewFines->AlphaSort();

    // Формируем список для выбора времени штрафа
    ComboBoxTime->Items->Add("Все время");
    for ( int i=1; i<=60; i++ ) ComboBoxTime->Items->Add(IntToStr(i));

    EditDescription->MaxLength=MAX_FineDescLength;
    SetEdit(false);
    ActiveControl=ListViewFines;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // Чистим интерфейсные элементы
    ListViewFines->Items->Clear();
    EditDescription->Text="";
    ComboBoxTime->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ListViewFinesInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;  // Устранение ошибки VCL
    Item->Data=new MFine;
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ListViewFinesDeletion(TObject *Sender,
      TListItem *Item)
{
    delete (MFine*)Item->Data;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ListViewFinesSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( ((TListView*)Sender)->SelCount!=1 ) { SetEdit(false); return; }
    else SetEdit(true);

    MFine *SelFine=(MFine*)ListViewFines->Selected->Data;
    EditDescription->Text=SelFine->Description;
    ComboBoxTime->ItemIndex=SelFine->Time==(24*60)? 0: SelFine->Time;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::EditDescriptionExit(TObject *Sender)
{
    if ( ListViewFines->Selected==NULL ) return;

    MFine *Fine=(MFine*)ListViewFines->Selected->Data;
    Fine->SetDescription(((TEdit*)Sender)->Text.Trim().c_str());
    ((TEdit*)Sender)->Text=Fine->Description;
    SetListViewFinesLine(ListViewFines->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ComboBoxTimeClick(TObject *Sender)
{
    if ( ListViewFines->Selected==NULL ) return;

    ((MFine*)ListViewFines->Selected->Data)->Time=
        ((TComboBox*)Sender)->ItemIndex==0? 24*60: ((TComboBox*)Sender)->ItemIndex;
    SetListViewFinesLine(ListViewFines->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ButtonAddClick(TObject *Sender)
{
    TListItem *NewItem=ListViewFines->Items->Add();
    MFine *NewFine=(MFine*)NewItem->Data;
    NewFine->SetDescription("Новый штраф");
    NewFine->Time=1;
    SetListViewFinesLine(NewItem);
    ListViewFines->AlphaSort();
    //
    ListViewFines->ItemFocused=NewItem;
    ListViewFines->Selected=NULL;
    ListViewFines->Selected=ListViewFines->ItemFocused;
    ActiveControl=EditDescription;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ButtonDelClick(TObject *Sender)
{
    // Удаляем штрафы из списка
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewFines->Selected, *NewItem; Item; )
    {
        NewItem=ListViewFines->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
    //
    ListViewFines->Selected=ListViewFines->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ButtonSaveClick(TObject *Sender)
{
    Fines->Clear();

    TListItems *Items=ListViewFines->Items;
    for ( int i=0; i<Items->Count; i++ )
    {
        MFine *Fine;
        Fine=(MFine*)Fines->Add();
        Fine->Copy((MFine*)Items->Item[i]->Data);
    }
    // Запись в логах
    Log->AddFines(Fines);
    // Сохраняем штрафы
    Fines->Save();
}
//---------------------------------------------------------------------------
void TFormFines::SetEdit(bool Edit_)
{
    TColor Color=Edit_? clWindow: clBtnFace;

    LabelDescription->Enabled=Edit_;
    EditDescription->Enabled=Edit_;
    EditDescription->Color=Color;
//    if ( !Edit_ ) EditDescription->Text="";
    LabelTime->Enabled=Edit_;
    ComboBoxTime->Enabled=Edit_;
    ComboBoxTime->Color=Color;
//    if ( !Edit_ ) ComboBoxTime->ItemIndex=-1;
}
//---------------------------------------------------------------------------
void TFormFines::SetListViewFinesLine(TListItem *Item_)
{
    MFine *Fine=(MFine*)Item_->Data;
    Item_->Caption=Fine->Description;
    if ( Fine->Time==(24*60) ) Item_->SubItems->Strings[0]="Все время";
    else Item_->SubItems->Strings[0]=IntToStr(Fine->Time)+" мин.";
}
//---------------------------------------------------------------------------

