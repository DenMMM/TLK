//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormMainOptions.h"
#include "UnitFormMain.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMainOptions *FormMainOptions;
//---------------------------------------------------------------------------
__fastcall TFormMainOptions::TFormMainOptions(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::FormShow(TObject *Sender)
{
    TListItem *NewItem;
    AnsiString Line;
    int a, b;

    EditPasswordToConnect->Text=Options.PasswordToConnect;
    ListViewClients->AllocBy=MaxClients;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        NewItem=ListViewClients->Items->Add();
        NewItem->SubItems->Add(IntToStr(Options.Clients[i].Number));
        NewItem->SubItems->Add(Options.Clients[i].Address);
        if ( Options.Clients[i].Number<10 ) Line="0";
        Line+=NewItem->SubItems->Strings[0]+NewItem->SubItems->Strings[1];
        NewItem->Caption=Line;
    }
    EditClientNumber->Text=IntToStr(ListViewClients->Items->Count+1);

    ListViewTariffs->AllocBy=MaxTariffs;
    for ( int i=0, j=Options.TariffNumber; i<j; i++ )
    {
        NewItem=ListViewTariffs->Items->Add();
        if ( Options.Tariffs[i].Begin>=0 )
        {
            a=Options.Tariffs[i].Begin/60;
            b=Options.Tariffs[i].Begin%60;
            NewItem->SubItems->Add(IntToStr(a/10)+IntToStr(a%10)+
                ":"+IntToStr(b/10)+IntToStr(b%10));
        }
        else NewItem->SubItems->Add ( "" );
        a=Options.Tariffs[i].Size/60;
        b=Options.Tariffs[i].Size%60;
        NewItem->SubItems->Add(IntToStr(a/10)+IntToStr(a%10)+
                ":"+IntToStr(b/10)+IntToStr(b%10));
        if ( NewItem->SubItems->Strings[0]=="" ) Line="     ";
        else Line=NewItem->SubItems->Strings[0];
        Line+=NewItem->SubItems->Strings[1];
        NewItem->Caption=Line;
    }
    RadioGroupTariffType->ItemIndex=0;
    EditBeginTimeHours->Text="0";
    EditBeginTimeMinutes->Text="0";
    EditSizeTimeHours->Text="1";
    EditSizeTimeMinutes->Text="0";

    EditPasswordToConnect->Tag=0;
    ListViewClients->Tag=0;
    ListViewTariffs->Tag=0;

    ActiveControl=ListViewClients;
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::RadioGroupTariffTypeClick(TObject *Sender)
{
    if ( RadioGroupTariffType->ItemIndex )
    {
        Label4->Enabled=true;
        EditBeginTimeHours->Enabled=true;
        Label7->Enabled=true;
        EditBeginTimeMinutes->Enabled=true;
    }
    else
    {
        Label4->Enabled=false;
        EditBeginTimeHours->Enabled=false;
        Label7->Enabled=false;
        EditBeginTimeMinutes->Enabled=false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::EditClientNumberEnter(TObject *Sender)
{
    EditClientNumber->Tag=StrToInt(EditClientNumber->Text);
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::EditClientNumberExit(TObject *Sender)
{
    int a;

    try { a=StrToInt(EditClientNumber->Text); }
    catch ( EConvertError *Error ) { EditClientNumber->Text=IntToStr(EditClientNumber->Tag); }
    if ( (a<1)||(a>99) ) EditClientNumber->Text=IntToStr(EditClientNumber->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::BitBtnAddClientClick(TObject *Sender)
{
    TListItem *NewItem;
    AnsiString Line;

    if ( ListViewClients->Items->Count>=MaxClients ) return;
    if ( EditClientAddress->Text=="" ) { EditClientAddress->SetFocus(); return; }
    NewItem=ListViewClients->Items->Add();
    NewItem->SubItems->Add(EditClientNumber->Text);
    NewItem->SubItems->Add(EditClientAddress->Text);
    if ( EditClientNumber->Text.Length()<2 ) Line="0";
    Line+=EditClientNumber->Text+EditClientAddress->Text;
    NewItem->Caption=Line;
    EditClientNumber->Text=IntToStr(ListViewClients->Items->Count+1);
    ListViewClients->Tag=1;
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::BitBtnDeleteClientClick(TObject *Sender)
{
    if ( ListViewClients->ItemFocused==NULL ) return;
    ListViewClients->ItemFocused->Delete();
    EditClientNumber->Text=IntToStr(ListViewClients->Items->Count+1);
    if ( ListViewClients->ItemFocused!=NULL )
        ListViewClients->ItemFocused->Selected=true;
    ListViewClients->Tag=1;
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::BitBtnAddTariffClick(TObject *Sender)
{
    TListItem *NewItem;
    AnsiString Line;
    int a, b;

    if ( ListViewTariffs->Items->Count>=MaxTariffs ) return;
    NewItem=ListViewTariffs->Items->Add();
    if ( EditBeginTimeHours->Enabled )
    {
        a=StrToInt(EditBeginTimeHours->Text);
        b=StrToInt(EditBeginTimeMinutes->Text);
        NewItem->SubItems->Add(IntToStr(a/10)+IntToStr(a%10)+
            ":"+IntToStr(b/10)+IntToStr(b%10));
    }
    else NewItem->SubItems->Add("");

    a=StrToInt(EditSizeTimeHours->Text);
    b=StrToInt(EditSizeTimeMinutes->Text);
    NewItem->SubItems->Add(IntToStr(a/10)+IntToStr(a%10)+
        ":"+IntToStr(b/10)+IntToStr(b%10));
    if ( NewItem->SubItems->Strings[0]=="" ) Line="     ";
    else Line=NewItem->SubItems->Strings[0];
    Line+=NewItem->SubItems->Strings[1];
    NewItem->Caption=Line;

    ListViewTariffs->Tag=1;
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::BitBtnDeleteTariffClick(TObject *Sender)
{
    if ( ListViewTariffs->ItemFocused==NULL ) return;
    ListViewTariffs->ItemFocused->Delete();
    if ( ListViewTariffs->ItemFocused!=NULL )
        ListViewTariffs->ItemFocused->Selected=true;
    ListViewTariffs->Tag=1;
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::BitBtnSaveClick(TObject *Sender)
{
    AnsiString Line1, Line2;
    int a, b;

    if ( FormMain->BitBtnRefresh->Tag )
    {
        Application->MessageBox("Нельзя сохранить настройки, пока выполняется какое-либо задание !",
            "Внимание - Time Locker Server",MB_OK+MB_ICONWARNING);
        return;
    }

    if ( EditPasswordToConnect->Tag )
        Options.PasswordToConnect=EditPasswordToConnect->Text;
    if ( ListViewClients->Tag )
    {
        // Разбираем настройки клиентских компьютеров
        Options.ClientNumber=ListViewClients->Items->Count;
        for ( int i=0, j=Options.ClientNumber; i<j; i++ )
        {
            Options.Clients[i].Number=StrToInt(ListViewClients->Items->Item[i]->SubItems->Strings[0]);
            Options.Clients[i].Address=ListViewClients->Items->Item[i]->SubItems->Strings[1];
            Options.Clients[i].NotInUse=false;
        }
        // Формируем заново список компьютеров
        FormMain->CreateComputerList();
        // Устанавливаем заново индикаторы выполнения заданий
        FormMain->SetProgressBars();
    }
    if ( ListViewTariffs->Tag )
    {
        // Разбираем настройки тарифов
        Options.TariffNumber=ListViewTariffs->Items->Count;
        for ( int i=0, j=Options.TariffNumber; i<j; i++ )
        {
            if ( (Line1=ListViewTariffs->Items->Item[i]->SubItems->Strings[0])!="" )
            {
                a=Line1.AnsiPos(':');
                b=Line1.Length();
                Line2=Line1.SubString(a+1,b-a);
                Line1.Delete(a,b-a+1);
                Options.Tariffs[i].Begin=StrToInt(Line1)*60+StrToInt(Line2);
            } else Options.Tariffs[i].Begin=-1;
            Line1=ListViewTariffs->Items->Item[i]->SubItems->Strings[1];
            a=Line1.AnsiPos(':');
            b=Line1.Length();
            Line2=Line1.SubString(a+1,b-a);
            Line1.Delete(a,b-a+1);
            Options.Tariffs[i].Size=StrToInt(Line1)*60+StrToInt(Line2);
        }
        // Формируем заново диалог выбора тарифов
        FormMain->CreateTariffsList();
    }
    // Сохраняем новые настройки в файле конфигурации
    if ( Options.Save() )
    {
        Application->MessageBox("Не удалось сохранить файл настроек 'TmLockSr.cfg' !\nПри следующем запуске программы будут использованы прежние настройки !",
            "Ошибка - Time Locker Server",MB_OK+MB_ICONERROR);
    }

    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::BitBtnCancelClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::EditBeginTimeHoursEnter(TObject *Sender)
{
    ((TEdit*)Sender)->Tag=StrToInt(((TEdit*)Sender)->Text);
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::EditBeginTimeHoursExit(TObject *Sender)
{
    int a;

    try { a=StrToInt(((TEdit*)Sender)->Text); }
    catch ( EConvertError *Error )
    {
        ((TEdit*)Sender)->Text=IntToStr(((TEdit*)Sender)->Tag);
        return;
    }
    if ( (a<0)||(a>23) ) ((TEdit*)Sender)->Text=IntToStr(((TEdit*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::EditBeginTimeMinutesExit(TObject *Sender)
{
    int a;

    try { a=StrToInt(((TEdit*)Sender)->Text); }
    catch ( EConvertError *Error )
    {
        ((TEdit*)Sender)->Text=IntToStr(((TEdit*)Sender)->Tag);
        return;
    }
    if ( (a<0)||(a>=60) ) ((TEdit*)Sender)->Text=IntToStr(((TEdit*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::FormDeactivate(TObject *Sender)
{
    WindowState=wsMinimized;
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // Чистим память до следующего запуска настройщика
    ListViewClients->Items->Clear();
    ListViewClients->AllocBy=0;
    ListViewTariffs->Items->Clear();
    ListViewTariffs->AllocBy=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::EditPasswordToConnectExit(
      TObject *Sender)
{
    if ( ((TEdit*)Sender)->Text!=Options.PasswordToConnect )
        EditPasswordToConnect->Tag=1;
}
//---------------------------------------------------------------------------
void __fastcall TFormMainOptions::ListViewClientsChange(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
    if ( Change!=ctState ) return;
    EditClientNumber->Text=Item->SubItems->Strings[0];
    EditClientAddress->Text=Item->SubItems->Strings[1];
}
//---------------------------------------------------------------------------

