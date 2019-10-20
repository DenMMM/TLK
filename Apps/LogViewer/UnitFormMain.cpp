//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormMain.h"
#include "UnitFormEvents.h"
#include "UnitFormUsersUpTime.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    for ( int i=MDIChildCount-1; i>=0; i-- ) MDIChildren[i]->Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NOpenClick(TObject *Sender)
{
	MLogFile *File=nullptr;
	wchar_t file_name[MAX_PATH+1];
	TMenuItem *Menu;

	if ( !OpenDialog->Execute() ) return;
	//
	File=new MLogFile;                     		/// try {} catch()
	File->Name=OpenDialog->FileName.c_str();
    // ��������� ���� ����
    File->Records.LoadFrom(File->Name,ENC_Code);
    //
	if ( ::GetFileTitle(
		File->Name.c_str(),
		file_name,
		sizeof(file_name))<0 ) goto error;

    Menu=new TMenuItem(NWindows);
    Menu->Caption=file_name;
    Menu->Tag=(int)File;
    NWindows->Add(Menu);
    // ������������ ��� ������
    FormEvents=new TFormEvents(this);
	FormEvents->Open(
		File,
		File->Records.gFirst(),
		File->Records.gLast());

	FormUsersUpTime=new TFormUsersUpTime(this);
	FormUsersUpTime->Open(
		File,
		File->Records.gFirst(),
		File->Records.gLast());

error:
    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NWindowsClick(TObject *Sender)
{
	if ( reinterpret_cast<TMenuItem*>(Sender)->Tag )
	{
		reinterpret_cast<TForm*>(
			dynamic_cast<TMenuItem&>(*Sender).Tag		/// ����� ������� ����
			)->Show();
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void TFormMain::WindowOpen(MLogFile *File_, TForm *Window_)
{
    TMenuItem *Menu;
    for ( int i=NWindows->Count-1; i>=0; i-- )
    {
        if ( NWindows->Items[i]->Tag!=((int)File_) ) continue;
        Menu=new TMenuItem(NWindows->Items[i]);
        Menu->Caption=Window_->Caption;
        Menu->Tag=(int)Window_;
        Menu->OnClick=&NWindowsClick;
        NWindows->Items[i]->Add(Menu);
        break;
    }
}
//---------------------------------------------------------------------------
void TFormMain::WindowCaption(TForm *Window_, char *Caption_)
{
    TMenuItem *SubItem;
    for ( int i=NWindows->Count-1; i>=0; i-- )
    {
        SubItem=NWindows->Items[i];
        for ( int j=SubItem->Count-1; j>=0; j-- )
        {
            if ( SubItem->Items[j]->Tag!=((int)Window_) ) continue;
            SubItem->Items[j]->Caption=Caption_;
            break;
        }
    }
}
//---------------------------------------------------------------------------
void TFormMain::WindowClose(TForm *Window_)
{
    TMenuItem *SubItem;
    for ( int i=NWindows->Count-1; i>=0; i-- )
    {
        SubItem=NWindows->Items[i];
        for ( int j=SubItem->Count-1; j>=0; j-- )
        {
            if ( SubItem->Items[j]->Tag!=((int)Window_) ) continue;
            SubItem->Remove(SubItem->Items[j]);
            // ������� ������, ���� � ������ ������ �� ���� ���� �� ��������
            if ( SubItem->Count==0 )
            {
                delete reinterpret_cast<MLogFile*>(SubItem->Tag);
                NWindows->Remove(SubItem);
            }
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NCascadeClick(TObject *Sender)
{
    Cascade();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NTileClick(TObject *Sender)
{
    Tile();
}
//---------------------------------------------------------------------------

