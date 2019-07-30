//---------------------------------------------------------------------------
#include <vcl.h>
#include <string.h>
#pragma hdrstop

#include "UnitFormTariffs.h"
#include "UnitFormTariffTimes.h"
#include "UnitFormMain.h"
#include "UnitComputers.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTariffs *FormTariffs;
//---------------------------------------------------------------------------
__fastcall TFormTariffs::TFormTariffs(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::FormShow(TObject *Sender)
{
    // Заполняем список групп программ
    for ( int i=1; i<=8; i++ ) CheckListBoxApps->Items->Add(IntToStr(i));
    // Формируем список компьютеров
    for ( MComputer *Computer=(MComputer*)Computers->First;
        Computer; Computer=(MComputer*)Computer->Next )
    {
        TListItem *NewItem;
        NewItem=ListViewComputers->Items->Add();
        NewItem->Data=Computer;
        NewItem->Caption=IntToStr(Computer->Number);
        NewItem->ImageIndex=Computer->GroupColor;
    }

    // Формируем список тарифов
    for ( MTariff *Tariff=(MTariff*)Tariffs->First;
        Tariff; Tariff=(MTariff*)Tariff->Next )
    {
        TListItem *NewItem;
        NewItem=ListViewNames->Items->Add();
        ((MTariff*)NewItem->Data)->Copy(Tariff);
        SetListViewNamesLine(NewItem);
    }

    EditName->MaxLength=MAX_TariffNameLength;
    SetEdit(false);
    ActiveControl=ListViewNames;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // Чистим интерфейсные элементы
    ListViewNames->Items->Clear();
    EditName->Text="";
    CheckBoxReboot->Checked=false;
    CheckListBoxApps->Items->Clear();
    ListViewComputers->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ListViewNamesInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
    Item->Data=new MTariff;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ListViewNamesDeletion(TObject *Sender,
      TListItem *Item)
{
    delete (MTariff*)Item->Data;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ListViewNamesSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( ((TListView*)Sender)->SelCount!=1 ) { SetEdit(false); return; }
    else SetEdit(true);

    MTariff *SelTariff=(MTariff*)ListViewNames->Selected->Data;
    //
    EditName->Text=SelTariff->Name;
    //
    unsigned int Programs=SelTariff->Programs;
    CheckListBoxApps->Checked[0]=Programs&mgp1;
    CheckListBoxApps->Checked[1]=Programs&mgp2;
    CheckListBoxApps->Checked[2]=Programs&mgp3;
    CheckListBoxApps->Checked[3]=Programs&mgp4;
    CheckListBoxApps->Checked[4]=Programs&mgp5;
    CheckListBoxApps->Checked[5]=Programs&mgp6;
    CheckListBoxApps->Checked[6]=Programs&mgp7;
    CheckListBoxApps->Checked[7]=Programs&mgp8;
    //
    CheckBoxReboot->Checked=SelTariff->Reboot;
    CheckBoxRoute->Checked=SelTariff->Programs&mgpRoute;
    // Проставляем для каких компьютеров используется тариф
    int i=0;
    for ( MComputer *Computer=(MComputer*)Computers->First;
        Computer; Computer=(MComputer*)Computer->Next, i++ )
    {
        ListViewComputers->Items->Item[i]->Checked=
            SelTariff->CheckForComputer(Computer->Number);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonAddClick(TObject *Sender)
{
    if ( ListViewNames->Items->Count>=MAX_Tariffs )
    {
        ::MessageBox(Handle,"Большее количество тарифов не поддерживается.",
            "Сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
        return;
    }

    TListItem *Item=ListViewNames->Items->Add();
    MTariff *Tariff=(MTariff*)Item->Data;
    Tariff->SetName("Новый тариф");
    SetListViewNamesLine(Item);
    //
    ListViewNames->ItemFocused=Item;
    ListViewNames->Selected=NULL;
    ListViewNames->Selected=Item;
    ActiveControl=EditName;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonDelClick(TObject *Sender)
{
    // Удаляем тарифы из списка
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewNames->Selected, *NewItem; Item; )
    {
        NewItem=ListViewNames->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
    //
    ListViewComputers->Selected=ListViewComputers->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::EditNameExit(TObject *Sender)
{
    if ( ListViewNames->Selected==NULL ) return;

    MTariff *Tariff=(MTariff*)ListViewNames->Selected->Data;
    Tariff->SetName(((TEdit*)Sender)->Text.Trim().c_str());
    ((TEdit*)Sender)->Text=Tariff->Name;
    SetListViewNamesLine(ListViewNames->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::CheckBoxRebootExit(TObject *Sender)
{
    if ( ListViewNames->Selected==NULL ) return;

    ((MTariff*)ListViewNames->Selected->Data)->Reboot=
        ((TCheckBox*)Sender)->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::CheckBoxRouteExit(TObject *Sender)
{
    if ( ListViewNames->Selected==NULL ) return;

    if ( ((TCheckBox*)Sender)->Checked )
        ((MTariff*)ListViewNames->Selected->Data)->Programs|=mgpRoute;
    else ((MTariff*)ListViewNames->Selected->Data)->Programs&=~mgpRoute;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::CheckListBoxAppsExit(TObject *Sender)
{
    if ( ListViewNames->Selected==NULL ) return;

    unsigned int Programs=0;
    if ( CheckListBoxApps->Checked[0] ) Programs|=mgp1;
    if ( CheckListBoxApps->Checked[1] ) Programs|=mgp2;
    if ( CheckListBoxApps->Checked[2] ) Programs|=mgp3;
    if ( CheckListBoxApps->Checked[3] ) Programs|=mgp4;
    if ( CheckListBoxApps->Checked[4] ) Programs|=mgp5;
    if ( CheckListBoxApps->Checked[5] ) Programs|=mgp6;
    if ( CheckListBoxApps->Checked[6] ) Programs|=mgp7;
    if ( CheckListBoxApps->Checked[7] ) Programs|=mgp8;
    ((MTariff*)ListViewNames->Selected->Data)->Programs=Programs;

    ((TCheckListBox*)Sender)->ItemIndex=-1;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ListViewComputersKeyPress(TObject *Sender,
      char &Key)
{
    if ( Key=='+' ) ButtonSetSelCompClick(ButtonSetSelComp);
    else if ( Key=='-' ) ButtonSetSelCompClick(ButtonResSelComp);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ListViewComputersExit(TObject *Sender)
{
    MTariff *Tariff;
    int CompNum, *Computers, *Comps;
    TListItems *Items;
    TListItem *Item;

    if ( ListViewNames->Selected==NULL ) return;
    Tariff=(MTariff*)ListViewNames->Selected->Data;
    Items=ListViewComputers->Items;
    // Считаем сколько компьютеров выделено
    CompNum=0;
    for ( int i=0; i<Items->Count; i++ ) if ( Items->Item[i]->Checked ) CompNum++;
    if ( (Computers=new int[CompNum])==NULL ) return;
    // Заполняем массив номеров компьютеров
    Comps=Computers;
    for ( int i=0; i<Items->Count; i++ )
    {
        Item=Items->Item[i];
        if ( Item->Checked ) { *Comps=((MComputer*)Item->Data)->Number; Comps++; }
    }
    // Сохраняем копию этого массива для тарифа
    Tariff->SetComputers(Computers,CompNum);
    delete[] Computers;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonSetSelCompClick(TObject *Sender)
{
    bool Checked;
    if ( Sender==ButtonSetSelComp ) Checked=true;
    else if ( Sender==ButtonResSelComp ) Checked=false;
    else return;

    TListItems *ListItems=ListViewComputers->Items;
    for ( int i=0, j=ListItems->Count; i<j; i++ )
        if ( ListItems->Item[i]->Selected ) ListItems->Item[i]->Checked=Checked;
    ListViewComputersExit(ListViewComputers);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonSetAllCompClick(TObject *Sender)
{
    bool Checked;
    if ( Sender==ButtonSetAllComp ) Checked=true;
    else if ( Sender==ButtonResAllComp ) Checked=false;
    else return;

    TListItems *ListItems=ListViewComputers->Items;
    for ( int i=0, j=ListItems->Count; i<j; i++ ) ListItems->Item[i]->Checked=Checked;
    ListViewComputersExit(ListViewComputers);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonTimesClick(TObject *Sender)
{
    if ( ListViewNames->Selected==NULL ) return;

    MTariff *Tariff=(MTariff*)ListViewNames->Selected->Data;
    FormTariffTimes->Left=Left+30;
    FormTariffTimes->Top=Top+30;
    FormTariffTimes->Execute(&Tariff->Times,Tariff->Name);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonSaveClick(TObject *Sender)
{
    Tariffs->Clear();

    TListItems *Items=ListViewNames->Items;
    for ( int i=0; i<Items->Count; i++ )
    {
        MTariff *NewTariff;
        NewTariff=(MTariff*)Tariffs->Add();
        NewTariff->Copy((MTariff*)Items->Item[i]->Data);
    }
    // Задаем ID-номера для новых тарифов
    Tariffs->SetIDs();
    // Запись в логах
    Log->AddTariffs(Tariffs);
    // Сохраняем тарифы
    Tariffs->Save();
    //
    FormMain->UpdateListViewComputers(true);
}
//---------------------------------------------------------------------------
void TFormTariffs::SetEdit(bool Edit_)
{
    TColor Color=Edit_? clWindow: clBtnFace;

    LabelTariffName->Enabled=Edit_;
    EditName->Enabled=Edit_;
    EditName->Color=Color;
//    if ( !Edit_ ) EditName->Text="";
    CheckBoxReboot->Enabled=Edit_;
    CheckBoxRoute->Enabled=Edit_;
    ButtonTimes->Enabled=Edit_;
//    if ( !Edit_ ) CheckBoxReboot->Checked=false;
    LabelTariffApps->Enabled=Edit_;
    CheckListBoxApps->Enabled=Edit_;
    CheckListBoxApps->Color=Color;
//    if ( !Edit_ )
//    {
//        for ( int i=0, j=CheckListBoxApps->Items->Count; i<j; i++ )
//            CheckListBoxApps->Checked[i]=false;
//    }

    LabelTariffComputers->Enabled=Edit_;
    ListViewComputers->Enabled=Edit_;
    ListViewComputers->Color=Color;
//    if ( !Edit_ )
//    {
//        TListItems *Items=ListViewComputers->Items;
//        for ( int i=0; i<Items->Count; i++ ) Items->Item[i]->Checked=false;
//    }
    ButtonSetSelComp->Enabled=Edit_;
    ButtonSetAllComp->Enabled=Edit_;
    ButtonResSelComp->Enabled=Edit_;
    ButtonResAllComp->Enabled=Edit_;
}
//---------------------------------------------------------------------------
void TFormTariffs::SetListViewNamesLine(TListItem *Item_)
{
    Item_->Caption=((MTariff*)Item_->Data)->Name;
}
//---------------------------------------------------------------------------

