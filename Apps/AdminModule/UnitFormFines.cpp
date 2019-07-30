//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormFines.h"
#include "UnitFormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormFines::TFormFines(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::FormShow(TObject *Sender)
{
    // �������� ������� ������ � �����
    TmpFines.Copy(Fines);

    // ��������� �� ������
    for ( MFine *fine=(MFine*)TmpFines.gFirst();
        fine; fine=(MFine*)fine->gNext() )
    {
        TListItem *item;
        item=ListViewFines->Items->Add();
        item->Data=fine;
        SetListViewFinesLine(item);
    }
    // ��������� ��� �������
    ListViewFines->AlphaSort();

    // ��������� ������ ��� ������ ������� ������
    ComboBoxTime->Items->Add("��� �����");
    for ( int i=1; i<=60; i++ ) ComboBoxTime->Items->Add(IntToStr(i));

    EditDescription->MaxLength=MAX_FineDescrLen;
    
    SetEdit(false);
    ActiveControl=ListViewFines;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // ������ ������������ ��������
    ListViewFines->Items->Clear();
    EditDescription->Text="";
    ComboBoxTime->Items->Clear();
    // ������ �����
    TmpFines.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ListViewFinesInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;  // ���������� ������ VCL
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ListViewFinesSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( ((TListView*)Sender)->SelCount!=1 )
    {
        SetEdit(false);
        return;
    } else
        SetEdit(true);

    MFine *fine=(MFine*)ListViewFines->Selected->Data;
    EditDescription->Text=fine->Descr.c_str();
    ComboBoxTime->ItemIndex=fine->Time==(24*60)? 0: fine->Time;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::EditDescriptionExit(TObject *Sender)
{
    if ( ListViewFines->Selected==NULL ) return;

    EditDescription->Text=EditDescription->Text.Trim();
    MFine *fine=(MFine*)ListViewFines->Selected->Data;
    fine->Descr=EditDescription->Text.c_str();
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
    if ( ListViewFines->Items->Count>=MAX_Fines )
    {
        ResMessageBox(Handle,0,22,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    // ��������� � ����� ����� �����
    MFine *fine=(MFine*)TmpFines.Add();
    fine->Descr="����� �����";
    fine->Time=1;
    // ��������� ������ � ������ � ��������� �� � �������
    TListItem *item=ListViewFines->Items->Add();
    item->Data=fine;
    // ��������� ���������
    SetListViewFinesLine(item);
    ListViewFines->AlphaSort();
    ListViewFines->ItemFocused=item;
    ListViewFines->Selected=NULL;
    ListViewFines->Selected=ListViewFines->ItemFocused;
    ActiveControl=EditDescription;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ButtonDelClick(TObject *Sender)
{
    // ������� ������ �� ������
    TItemStates is=TItemStates()<<isSelected;
    TListItem *item=ListViewFines->Selected, *next;
    while(item)
    {
        // ������� ����� �� ������
        TmpFines.Del((MFine*)item->Data);
        // ������� ������ �� ������
        next=ListViewFines->GetNextItem(item,sdAll,is);
        item->Delete();
        item=next;
    }
    //
    ListViewFines->Selected=ListViewFines->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ButtonSaveClick(TObject *Sender)
{
    // �������� �������� �� ������ �������
    Fines->Move(&TmpFines);
    // ������ ID-������ ��� �����
    Fines->SetIDs();
    // ��������� � �����
    if ( !Fines->Save() )
    {
        ShellState->State|=mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,3,MB_APPLMODAL|MB_OK|MB_ICONERROR,Fines->gLastErr());
        return;
    }
    // ������ � �����
    if ( !Log->AddFines(Fines) )
    {
        // ��������� ���������, �� ��� ����������� �� � ���� �������� �� �����
        ShellState->State|=mssErrorLog|mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
    }
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
    MFine *fine=(MFine*)Item_->Data;
    Item_->Caption=fine->Descr.c_str();
    if ( fine->Time==(24*60) ) Item_->SubItems->Strings[0]="��� �����";
    else Item_->SubItems->Strings[0]=IntToStr(fine->Time)+" ���.";
}
//---------------------------------------------------------------------------

