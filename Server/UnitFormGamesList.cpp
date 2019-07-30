//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormGamesList.h"
#include "UnitFormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormGamesList *FormGamesList;
//---------------------------------------------------------------------------
__fastcall TFormGamesList::TFormGamesList(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ListViewGames->Items->Clear();
    ListViewGames->AllocBy=0;
    EditGameName->Text="";
    EditCommandLine->Text="";
    EditIconFile->Text="";
    MemoGameDesciption->Lines->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::FormDeactivate(TObject *Sender)
{
    WindowState=wsMinimized;
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::FormShow(TObject *Sender)
{
    LoadGamesList(WinDir+"\\TmLockCl.gms");
    ActiveControl=ListViewGames;
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::BitBtnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
int __fastcall TFormGamesList::LoadGamesList(AnsiString FileName)
{
    TListItem *ListItem;

    ListViewGames->Items->Clear();
    try { MemoBuffer->Lines->LoadFromFile(FileName); }
    catch ( EFOpenError *Error ) { return -1; }
    ListViewGames->AllocBy=MemoBuffer->Lines->Count/4;
    for ( int i=0, j=MemoBuffer->Lines->Count/4; i<j; i++ )
    {
        ListItem=ListViewGames->Items->Add();
        ListItem->Caption=MemoBuffer->Lines->Strings[i*4+0];
        ListItem->SubItems->Add(MemoBuffer->Lines->Strings[i*4+0]);
        ListItem->SubItems->Add(MemoBuffer->Lines->Strings[i*4+1]);
        ListItem->SubItems->Add(MemoBuffer->Lines->Strings[i*4+2]);
        ListItem->SubItems->Add(MemoBuffer->Lines->Strings[i*4+3]);
    }
    MemoBuffer->Lines->Clear();

    return 0;
}
//---------------------------------------------------------------------------
int __fastcall TFormGamesList::SaveGamesList(AnsiString FileName)
{
    TStrings *SubItems;

    MemoBuffer->Lines->Clear();
    for ( int i=0, j=ListViewGames->Items->Count; i<j; i++ )
    {
        SubItems=ListViewGames->Items->Item[i]->SubItems;
        MemoBuffer->Lines->Add(SubItems->Strings[0]);
        MemoBuffer->Lines->Add(SubItems->Strings[1]);
        MemoBuffer->Lines->Add(SubItems->Strings[2]);
        MemoBuffer->Lines->Add(SubItems->Strings[3]);
    }

    try { MemoBuffer->Lines->SaveToFile(FileName); }
    catch ( EFCreateError *Error ) { MemoBuffer->Lines->Clear(); return -1; }
    MemoBuffer->Lines->Clear();

    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::ListViewGamesChange(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
    if ( Change!=ctState ) return;
    EditGameName->Text=Item->SubItems->Strings[0];
    EditCommandLine->Text=Item->SubItems->Strings[1];
    EditIconFile->Text=Item->SubItems->Strings[2];
    MemoGameDesciption->Text=Item->SubItems->Strings[3];
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::BitBtnAddClick(TObject *Sender)
{
    TListItem *ListItem;
    
    ListItem=ListViewGames->Items->Add();
    ListItem->Caption=EditGameName->Text;
    ListItem->SubItems->Add(EditGameName->Text);
    ListItem->SubItems->Add(EditCommandLine->Text);
    ListItem->SubItems->Add(EditIconFile->Text);
    ListItem->SubItems->Add(MemoGameDesciption->Text);
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::BitBtnDeleteClick(TObject *Sender)
{
    if ( ListViewGames->ItemFocused==NULL ) return;
    ListViewGames->ItemFocused->Delete();
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::BitBtnLoadClick(TObject *Sender)
{
    if ( !OpenDialog->Execute() ) return;
    if ( LoadGamesList(OpenDialog->FileName)<0 )
    {
        Application->MessageBox("Не удалось считать файл со списком игр !",
            "Ошибка - Time Locker Server",MB_OK+MB_ICONERROR);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::BitBtnSaveClick(TObject *Sender)
{
    if ( ((TObject*)Sender)==BitBtnSave )
    {
        if ( !SaveDialog->Execute() ) return;
        if ( SaveGamesList(SaveDialog->FileName)<0 )
        {
            Application->MessageBox("Не удалось сохранить файл со списком игр !",
                "Ошибка - Time Locker Server",MB_OK+MB_ICONERROR);
        }
    } else
    {
        if ( SaveGamesList(WinDir+"\\TmLockCl.gms")<0 )
        {
            Application->MessageBox("Не удалось сохранить файл со списком игр по умолчанию !",
                "Ошибка - Time Locker Server",MB_OK+MB_ICONERROR);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::EditGameNameExit(TObject *Sender)
{
    if ( ListViewGames->ItemFocused==NULL ) return;
    ListViewGames->ItemFocused->Caption=EditGameName->Text;
    ListViewGames->ItemFocused->SubItems->Strings[0]=EditGameName->Text;
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::EditCommandLineExit(TObject *Sender)
{
    if ( ListViewGames->ItemFocused==NULL ) return;
    ListViewGames->ItemFocused->SubItems->Strings[1]=EditCommandLine->Text;
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::EditIconFileExit(TObject *Sender)
{
    if ( ListViewGames->ItemFocused==NULL ) return;
    ListViewGames->ItemFocused->SubItems->Strings[2]=EditIconFile->Text;
}
//---------------------------------------------------------------------------
void __fastcall TFormGamesList::MemoGameDesciptionExit(TObject *Sender)
{
    if ( ListViewGames->ItemFocused==NULL ) return;
    ListViewGames->ItemFocused->SubItems->Strings[3]=MemoGameDesciption->Text;
}
//---------------------------------------------------------------------------

