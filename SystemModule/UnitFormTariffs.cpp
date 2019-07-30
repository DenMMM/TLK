//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormTariffs.h"
#include "UnitFormTariffTimes.h"
#include "UnitFormMain.h"
#include "UnitComputers.h"
#include "UnitCommon.h"
#include "UnitOptionsLoadSave.h"
#include "UnitLogs.h"
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
    TMPTariffs=new MTariffs;
    TMPTariff=new MTariff;

    *TMPTariffs=*Tariffs;
    // Определяем максимальное значение ID-кода, использованного с компьютерами
    NewID=0;
    for ( MTariff *Tariff=(MTariff*)TMPTariffs->FirstItem; Tariff;
        Tariff=(MTariff*)Tariff->NextItem ) if ( NewID<Tariff->ID ) NewID=Tariff->ID;
    NewID++;
    // Формируем список компьютеров
    ListViewComputers->Items->Clear();
    for ( MComputer *Computer=(MComputer*)Computers->FirstItem; Computer;
        Computer=(MComputer*)Computer->NextItem )
    {
        TListItem *NewItem;
        NewItem=ListViewComputers->Items->Add();
        NewItem->Checked=true;
        NewItem->Caption=IntToStr(Computer->Number);
        NewItem->ImageIndex=Computer->GroupColor;
    }
    // Формируем список тарифов
    ListViewNames->Items->Clear();
    for ( MTariff *Tariff=(MTariff*)TMPTariffs->FirstItem; Tariff;
        Tariff=(MTariff*)Tariff->NextItem )
    {
        TListItem *NewItem;
        NewItem=ListViewNames->Items->Add();
        NewItem->SubItems->Add(Tariff->Name);
        NewItem->SubItemImages[0]=Tariff->IconNumber;
    }
    // Задаем начальные значения для временного тарифа и диалоговых элементов
    TMPTariff->Name="undefined";
//
    EditName->Text=TMPTariff->Name;
    //
    ActiveControl=ListViewNames;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // Чистим интерфейсные элементы
    ListViewNames->Items->Clear();
    ListViewComputers->Items->Clear();
    delete TMPTariff;
    delete TMPTariffs;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ListViewNamesChange(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
    if ( (Change!=ctState)||(!Item->Focused) ) return;

    // Копируем выбранный тариф во временный
    *TMPTariff=*((MTariff*)TMPTariffs->Item(Item->Index));
    //
    EditName->Text=TMPTariff->Name;
    ListViewNames->SmallImages->GetIcon(TMPTariff->IconNumber,ImageIcon->Picture->Icon);
    {
        unsigned int Pages=TMPTariff->GamesPages;
        CheckBoxPage1->Checked=Pages&mgp1;
        CheckBoxPage2->Checked=Pages&mgp2;
        CheckBoxPage3->Checked=Pages&mgp3;
        CheckBoxPage4->Checked=Pages&mgp4;
        CheckBoxPage5->Checked=Pages&mgp5;
        CheckBoxPage6->Checked=Pages&mgp6;
        CheckBoxPage7->Checked=Pages&mgp7;
    }
    //
    CheckBoxReboot->Checked=TMPTariff->Reboot;
    // Проставляем для каких компьютеров используется тариф
    int i=0;
    for ( MComputer *Computer=(MComputer*)(Computers->FirstItem); Computer;
        Computer=(MComputer*)(Computer->NextItem) )
    {
        ListViewComputers->Items->Item[i]->Checked=
            (bool)TMPTariff->Computers.SearchID(Computer->ID);
        i++;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ListViewComputersKeyPress(TObject *Sender,
      char &Key)
{
    if ( Key=='+' ) BitBtnSetSelCompClick(BitBtnSetSelComp);
    else if ( Key=='-' ) BitBtnSetSelCompClick(BitBtnResetSelComp);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::BitBtnSetSelCompClick(TObject *Sender)
{
    bool Checked;
    if ( Sender==BitBtnSetSelComp ) Checked=true;
    else if ( Sender==BitBtnResetSelComp ) Checked=false;
    else return;

    TListItems *ListItems=ListViewComputers->Items;
    for ( int i=0, j=ListItems->Count; i<j; i++ )
        if ( ListItems->Item[i]->Selected ) ListItems->Item[i]->Checked=Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::BitBtnSetAllCompClick(TObject *Sender)
{
    bool Checked;
    if ( Sender==BitBtnSetAllComp ) Checked=true;
    else if ( Sender==BitBtnResetAllComp ) Checked=false;
    else return;

    TListItems *ListItems=ListViewComputers->Items;
    for ( int i=0, j=ListItems->Count; i<j; i++ ) ListItems->Item[i]->Checked=Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::EditNameExit(TObject *Sender)
{
    AnsiString Line=EditName->Text.Trim();
    if ( Line!="" ) TMPTariff->Name=Line;
    EditName->Text=TMPTariff->Name;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::BitBtnSelectIconClick(TObject *Sender)
{
    OpenPictureDialog->InitialDir=PrgDir+"icons";
    if ( !OpenPictureDialog->Execute() ) return;
    ImageIcon->Picture->LoadFromFile(OpenPictureDialog->FileName);
    TMPTariff->IconFileName=OpenPictureDialog->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::CheckBoxPage1Click(TObject *Sender)
{
    unsigned int Page;

    if ( Sender==CheckBoxPage1 ) Page=mgp1;
    else if ( Sender==CheckBoxPage2 ) Page=mgp2;
    else if ( Sender==CheckBoxPage3 ) Page=mgp3;
    else if ( Sender==CheckBoxPage4 ) Page=mgp4;
    else if ( Sender==CheckBoxPage5 ) Page=mgp5;
    else if ( Sender==CheckBoxPage6 ) Page=mgp6;
    else if ( Sender==CheckBoxPage7 ) Page=mgp7;
    else return;

    if ( ((TCheckBox*)Sender)->Checked ) TMPTariff->GamesPages|=Page;
    else TMPTariff->GamesPages&=~Page;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::BitBtnSaveClick(TObject *Sender)
{
    *Tariffs=*TMPTariffs;
    // Устраняем возможно возникшие неопределенности из-за удаления тарифа(ов)
    // и при их наличии сохраняем откорректированные параметры состояния компьютеров
    if ( CorrectComputersData(Computers,Tariffs) ) Computers->Save();
    // Сохраняем тарифы
    TariffsSave(Tariffs);
    //
    FormMain->CreateListViewComputers();

    LogCompRun(NULL,(MTariff*)11,0,0.);
}
//---------------------------------------------------------------------------
void TFormTariffs::SetListViewNamesLine(int Number_)
{
    TListItem *Item=ListViewNames->Items->Item[Number_];
    MTariff *Tariff=(MTariff*)TMPTariffs->Item(Number_);

    Item->SubItemImages[0]=Tariff->IconNumber;
    Item->SubItems->Strings[0]=Tariff->Name;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonAddClick(TObject *Sender)
{
    TMPTariff->IconNumber=ListViewNames->SmallImages->AddIcon(ImageIcon->Picture->Icon);

    TMPTariff->Computers.Clear();
    TListItems *Items=ListViewComputers->Items;
    for ( int i=0, j=Items->Count; i<j; i++ )
    {
        MListOfIDsID *NewComputerID;
        if ( !Items->Item[i]->Checked ) continue;
        NewComputerID=(MListOfIDsID*)TMPTariff->Computers.Add();
        NewComputerID->ID=((MComputer*)Computers->Item(i))->ID;
    }

    MTariff *NewTariff;
    NewTariff=(MTariff*)TMPTariffs->Add();
    *NewTariff=*TMPTariff;
    NewTariff->ID=NewID;
    NewID++;

    TListItem *NewItem;
    NewItem=ListViewNames->Items->Add();
    NewItem->SubItems->Add("");
    SetListViewNamesLine(NewItem->Index);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonDeleteClick(TObject *Sender)
{
    TListItems *ListItems=ListViewNames->Items;
    // Удаляем выбранные строки из списка тарифов и сами тарифы
    for ( int i=ListItems->Count-1; i>=0; i-- )
    {
        if ( !ListItems->Item[i]->Selected ) continue;
        ListItems->Item[i]->Delete();
        int IconNumber;
        if ( (IconNumber=((MTariff*)TMPTariffs->Item(i))->IconNumber)>=0 )
        {
            // Корректируем список иконок и их номера для других тарифов
            ListViewNames->SmallImages->Delete(IconNumber);
            for ( MTariff *Tariff=(MTariff*)TMPTariffs->FirstItem; Tariff;
                Tariff=(MTariff*)Tariff->NextItem )
            {
                if ( Tariff->IconNumber>IconNumber ) Tariff->IconNumber--;
            }
        }
        TMPTariffs->Delete(i);
    }
    // Обновляем иконки в списке тарифов из-за возможного изменения их номеров
    for ( int i=0, j=ListViewNames->Items->Count; i<j; i++ ) SetListViewNamesLine(i);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonChangeClick(TObject *Sender)
{
    TListItem *ListItem;
    if ( (ListItem=ListViewNames->ItemFocused)==NULL ) return;

    TMPTariff->Computers.Clear();
    TListItems *Items=ListViewComputers->Items;
    for ( int i=0, j=Items->Count; i<j; i++ )
    {
        MListOfIDsID *NewComputerID;
        if ( !Items->Item[i]->Checked ) continue;
        NewComputerID=(MListOfIDsID*)TMPTariff->Computers.Add();
        NewComputerID->ID=((MComputer*)Computers->Item(i))->ID;
    }

    *((MTariff*)(TMPTariffs->Item(ListItem->Index)))=*TMPTariff;
    if ( TMPTariff->IconNumber>=0 ) ListViewNames->SmallImages->ReplaceIcon
        (TMPTariff->IconNumber,ImageIcon->Picture->Icon);
    SetListViewNamesLine(ListItem->Index);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonPortsClick(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonTimesClick(TObject *Sender)
{
    FormTariffTimes->Left=Left+30;
    FormTariffTimes->Top=Top+30;
    FormTariffTimes->Caption="Время для тарифа \'"+TMPTariff->Name+"\'";
    FormTariffTimes->Execute(&TMPTariff->Times);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::CheckBoxRebootClick(TObject *Sender)
{
    TMPTariff->Reboot=CheckBoxReboot->Checked;
}
//---------------------------------------------------------------------------

