//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "UnitFormComputers.h"
#include "UnitFormMain.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormComputers *FormComputers;
//---------------------------------------------------------------------------
__fastcall TFormComputers::TFormComputers(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::FormShow(TObject *Sender)
{
    for ( MComputer *Computer=(MComputer*)Computers->First; Computer;
        Computer=(MComputer*)Computer->Next )
    {
        TListItem *NewItem;
        NewItem=ListViewComputers->Items->Add();
        ((MComputer*)NewItem->Data)->Copy(Computer);
        SetListViewComputersLine(NewItem);
    }

    ListViewComputers->AlphaSort();

    EditNumber->MaxLength=4;
    EditAddress->MaxLength=MAX_ComputerAddressLength;

    SetEdit(false);
    ActiveControl=ListViewComputers;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // Чистим интерфейсные элементы
    ListViewComputers->Items->Clear();
    EditNumber->Text="";
    EditAddress->Text="";
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ListViewComputersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
    Item->Data=new MComputer;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ListViewComputersDeletion(TObject *Sender,
      TListItem *Item)
{
    delete (MComputer*)Item->Data;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ListViewComputersSelectItem(
      TObject *Sender, TListItem *Item, bool Selected)
{
    if ( ((TListView*)Sender)->SelCount!=1 ) { SetEdit(false); return; }
    else SetEdit(true);

    MComputer *SelParam=(MComputer*)ListViewComputers->Selected->Data;
    EditNumber->Text=IntToStr(SelParam->Number);
    EditAddress->Text=SelParam->Address;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ListViewComputersCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
    int Number1=((MComputer*)Item1->Data)->Number,
        Number2=((MComputer*)Item2->Data)->Number;
    if ( Number1<Number2 ) Compare=-1;
    else if ( Number1>Number2 ) Compare=1;
    else Compare=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::EditNumberExit(TObject *Sender)
{
    if ( ListViewComputers->Selected==NULL ) return;

    MComputer *SelParam=(MComputer*)ListViewComputers->Selected->Data;

    int Number;
    try { Number=StrToInt(((TEdit*)Sender)->Text); }
    catch ( EConvertError *Error ) { goto error; }
    if ( (Number<=0)||(Number>MAX_Computers) ) goto error;
    SelParam->Number=Number;
error:
    ((TEdit*)Sender)->Text=IntToStr(SelParam->Number);
    SetListViewComputersLine(ListViewComputers->Selected);
    ListViewComputers->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::EditAddressExit(TObject *Sender)
{
    if ( ListViewComputers->Selected==NULL ) return;

    MComputer *Computer=(MComputer*)ListViewComputers->Selected->Data;
    Computer->SetAddress(((TEdit*)Sender)->Text.Trim().c_str());
    ((TEdit*)Sender)->Text=Computer->Address;
    SetListViewComputersLine(ListViewComputers->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::BitBtnNoneClick(TObject *Sender)
{
    int GroupColor;
    if ( Sender==BitBtnNone )GroupColor=mgcNone;
    else if ( Sender==BitBtnRed ) GroupColor=mgcRed;
    else if ( Sender==BitBtnLime ) GroupColor=mgcLime;
    else if ( Sender==BitBtnAqua ) GroupColor=mgcAqua;
    else if ( Sender==BitBtnYellow ) GroupColor=mgcYellow;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        ((MComputer*)Item->Data)->GroupColor=GroupColor;
        SetListViewComputersLine(Item);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::BitBtnUsedClick(TObject *Sender)
{
    bool NotUsed=Sender==BitBtnUsed? false: true;
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        ((MComputer*)Item->Data)->NotUsed=NotUsed;
        SetListViewComputersLine(Item);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ButtonAddClick(TObject *Sender)
{
    if ( ListViewComputers->Items->Count>=MAX_Computers )
    {
        ::MessageBox(Handle,"Большее количество компьютеров не поддерживается.",
            "Сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
        return;
    }

    TListItem *Item;
    MComputer *Computer, *SelComputer;
    char Address[15+1];

    SelComputer=ListViewComputers->Selected==NULL?NULL:
        (MComputer*)ListViewComputers->Selected->Data;
    Item=ListViewComputers->Items->Add();
    Computer=(MComputer*)Item->Data;
    Computer->Number=SelComputer?SelComputer->Number+1:1;
    sprintf(Address,"192.168.1.%i",Computer->Number);
    Computer->SetAddress(Address);

    SetListViewComputersLine(Item);
    ListViewComputers->AlphaSort();
    //
    ListViewComputers->ItemFocused=Item;
    ListViewComputers->Selected=NULL;
    ListViewComputers->Selected=ListViewComputers->ItemFocused;
    ActiveControl=EditNumber;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ButtonDelClick(TObject *Sender)
{
    // Удаляем компьютеры из списка
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected, *NewItem; Item; )
    {
        NewItem=ListViewComputers->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
    //
    ListViewComputers->Selected=ListViewComputers->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ButtonSaveClick(TObject *Sender)
{
    Computers->Clear();
    // Сохраняем новый список компьютеров
    TListItems *Items=ListViewComputers->Items;
    for ( int i=0, j=Items->Count; i<j; i++ )
    {
        MComputer *NewComputer;
        NewComputer=(MComputer*)Computers->Add();
        NewComputer->Copy((MComputer*)Items->Item[i]->Data);
    }
    Computers->Save();
    //
    Sync->Stop();
    if ( States->Update(Computers) )
    {
        // Запись в логах
        Log->AddComputers(States);
        //
        States->Save();
    }
    Sync->Associate(States,Computers);
    FormMain->ProgressBarNetProcess->Max=Sync->GetCounterMax();
    Sync->Start();
    //
    FormMain->UpdateListViewComputers(true);
}
//---------------------------------------------------------------------------
void TFormComputers::SetEdit(bool Edit_)
{
    TColor Color=Edit_? clWindow: clBtnFace;

    LabelNumber->Enabled=Edit_;
    EditNumber->Enabled=Edit_;
    EditNumber->Color=Color;
//    if ( !Edit_ ) EditNumber->Text="";
    LabelAddress->Enabled=Edit_;
    EditAddress->Enabled=Edit_;
    EditAddress->Color=Color;
//    if ( !Edit_ ) EditAddress->Text="";
}
//---------------------------------------------------------------------------
void TFormComputers::SetListViewComputersLine(TListItem *Item_)
{
    MComputer *Computer=(MComputer*)Item_->Data;
    TStrings *SubItems=Item_->SubItems;

    Item_->SubItemImages[0]=Computer->GroupColor;
    SubItems->Strings[0]=IntToStr(Computer->Number);
    Item_->SubItemImages[1]=Computer->NotUsed? 11: 4;
    SubItems->Strings[1]=Computer->Address;
}
//---------------------------------------------------------------------------

