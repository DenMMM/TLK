//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormComputers.h"
#include "UnitFormMain.h"
#include "UnitCommon.h"
#include "UnitOptionsLoadSave.h"
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
    MCompParams *CompParams=new MCompParams;
    Computers->Get(CompParams);
    for ( MCompParam *CompParam=(MCompParam*)CompParams->FirstItem; CompParam;
        CompParam=(MCompParam*)CompParam->NextItem )
    {
        TListItem *NewListItem;
        NewListItem=ListViewComputers->Items->Add();
        *((MCompParam*)NewListItem->Data)=*CompParam;
        SetListViewComputersLine(NewListItem);
    }
    delete CompParams;

    TMPCompParam=new MCompParam;
    // Задаем значения для нового компьютера (не выбранного еще из списка)
    TMPCompParam->Number=1;
    TMPCompParam->Address="195.0.0.1";
    TMPCompParam->GroupColor=-1;
    EditNumber->Text=IntToStr(TMPCompParam->Number);
    EditAddress->Text=TMPCompParam->Address;
    ShapeMarkerColor->Brush->Color=clNone;

    // Мелкие настройки
    RadioButtonJust->Checked=true;
    ActiveControl=RadioButtonJust;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::FormHide(TObject *Sender)
{
    ListViewComputers->Items->Clear();
    delete TMPCompParam;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ListViewComputersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->Data=(void*)new MCompParam;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ListViewComputersSelectItem(
      TObject *Sender, TListItem *Item, bool Selected)
{
    if ( !Selected ) return;
    //
    *TMPCompParam=*((MCompParam*)Item->Data);
    //
    EditNumber->Text=IntToStr(TMPCompParam->Number);
    EditAddress->Text=TMPCompParam->Address;
    TColor Color;
    switch(TMPCompParam->GroupColor)
    {
        case mgcAqua: Color=clAqua; break;
        case mgcLime: Color=clLime; break;
        case mgcRed: Color=clRed; break;
        case mgcYellow: Color=clYellow; break;
        default: Color=clNone; break;
    }
    ShapeMarkerColor->Brush->Color=Color;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ListViewComputersDeletion(TObject *Sender,
      TListItem *Item)
{
    delete (MCompParam*)Item->Data;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::RadioButtonJustClick(TObject *Sender)
{
    if ( Sender==RadioButtonJust )
    {
        SetGroupStateJust(true);
        SetGroupStatePattern(false);
        SetGroupStateNet(false);
    } else if ( Sender==RadioButtonPattern )
    {
        SetGroupStateJust(false);
        SetGroupStatePattern(true);
        SetGroupStateNet(false);
    } else if ( Sender==RadioButtonNet )
    {
        SetGroupStateJust(false);
        SetGroupStatePattern(false);
        SetGroupStateNet(true);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::EditAddressExit(TObject *Sender)
{
    AnsiString Line=EditAddress->Text.Trim();
    if ( (Line!="")&&(Line.Length()<16) ) TMPCompParam->Address=Line;
    EditAddress->Text=TMPCompParam->Address;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::EditNumberExit(TObject *Sender)
{
    int a;
    try { a=StrToInt(EditNumber->Text); }
    catch ( EConvertError *Error ) { goto error; }
    if ( (a<=0)||(a>MAX_Computers) ) goto error;
    TMPCompParam->Number=a;
error:
    EditNumber->Text=IntToStr(TMPCompParam->Number);
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::BitBtnMarkerNoneClick(TObject *Sender)
{
    int GroupColor;
    TColor Color;

    if ( Sender==BitBtnMarkerNone ) { GroupColor=mgcNone; Color=clBlack; }
    else if ( Sender==BitBtnMarkerRed ) { GroupColor=mgcRed; Color=clRed; }
    else if ( Sender==BitBtnMarkerLime ) { GroupColor=mgcLime; Color=clLime; }
    else if ( Sender==BitBtnMarkerAqua ) { GroupColor=mgcAqua; Color=clAqua; }
    else if ( Sender==BitBtnMarkerYellow ) { GroupColor=mgcYellow; Color=clYellow; }
    else { GroupColor=-1; Color=clNone; }

    TMPCompParam->GroupColor=GroupColor;
    ShapeMarkerColor->Brush->Color=Color;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ButtonAddClick(TObject *Sender)
{
    if ( Computers->CountItems>=MAX_Computers ) return;
    // Добавляем новый пункт в список компьютеров
    TListItem *NewItem=ListViewComputers->Items->Add();
    TMPCompParam->ID=0;
    *((MCompParam*)NewItem->Data)=*TMPCompParam;
    SetListViewComputersLine(NewItem);
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ButtonDeleteClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    TListItem *Item=ListViewComputers->Selected, *NextItem;
    while(Item)
    {
        NextItem=ListViewComputers->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NextItem;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ButtonChangeClick(TObject *Sender)
{
    if ( ListViewComputers->SelCount>=2 )
    {
        // Задаем цвет для всех выбранных в таблице компьютеров
        TItemStates is=TItemStates()<<isSelected;
        TListItem *Item=ListViewComputers->Selected;
        while(Item)
        {
            ((MCompParam*)Item->Data)->GroupColor=TMPCompParam->GroupColor;
            SetListViewComputersLine(Item);
            Item=ListViewComputers->GetNextItem(Item,sdAll,is);
        }
    } else if ( ListViewComputers->ItemFocused )
    {
        // Задаем новые параметры компьютеру
        TListItem *Item=ListViewComputers->ItemFocused;
        *((MCompParam*)Item->Data)=*TMPCompParam;
        SetListViewComputersLine(Item);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::BitBtnSaveClick(TObject *Sender)
{
    // Сохраняем новый список компьютеров
    MCompParams *CompParams=new MCompParams;
    TListItems *Items=ListViewComputers->Items;
    for ( int i=0, j=Items->Count; i<j; i++ )
    {
        MCompParam *NewCompParam;
        NewCompParam=(MCompParam*)CompParams->Add();
        *NewCompParam=*((MCompParam*)Items->Item[i]->Data);
    }
    Computers->Set(CompParams);
    delete CompParams;
    // Устраняем возможно возникшие неопределенности из-за удаления компьютера(ов)
    // и при их наличии сохраняем откорректированные тарифы
    if ( CorrectTariffsData(Tariffs,Computers) ) TariffsSave(Tariffs);
    //
    FormMain->CreateListViewComputers();
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::SetListViewComputersLine(TListItem *Item_)
{
    MCompParam *CompParam=(MCompParam*)Item_->Data;
    TStrings *SubItems=Item_->SubItems;

    Item_->Caption=IntToStr(CompParam->Number/10)+IntToStr(CompParam->Number%10);
    Item_->SubItemImages[0]=CompParam->GroupColor;
    SubItems->Strings[0]=IntToStr(CompParam->Number);
    SubItems->Strings[1]=CompParam->Address;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::SetGroupStateJust(bool State_)
{
    bool enabled;
    TColor color;

    if ( State_ ) { enabled=true; color=clWindow; }
    else { enabled=false; color=clBtnFace; }

    LabelAddress->Enabled=enabled;
    EditAddress->Enabled=enabled;
    EditAddress->Color=color;
    LabelNumber->Enabled=enabled;
    EditNumber->Enabled=enabled;
    EditNumber->Color=color;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::SetGroupStatePattern(bool State_)
{
    bool enabled;
    TColor color;

    if ( State_ ) { enabled=true; color=clWindow; }
    else { enabled=false; color=clBtnFace; }

    LabelPattern->Enabled=enabled;
    EditPattern->Enabled=enabled;
    EditPattern->Color=color;
    LabelBeginNumber->Enabled=enabled;
    EditBeginNumber->Enabled=enabled;
    EditBeginNumber->Color=color;
    LabelEndNumber->Enabled=enabled;
    EditEndNumber->Enabled=enabled;
    EditEndNumber->Color=color;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::SetGroupStateNet(bool State_)
{
    bool enabled;
    TColor color;

    if ( State_ ) { enabled=true; color=clWindow; }
    else { enabled=false; color=clBtnFace; }

    LabelGlobalPattern->Enabled=enabled;
    EditGlobalPattern->Enabled=enabled;
    EditGlobalPattern->Color=color;
}
//---------------------------------------------------------------------------

