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
//---------------------------------------------------------------------------
__fastcall TFormComputers::TFormComputers(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::FormShow(TObject *Sender)
{
    // �������� ������ ����������� � �����
    TmpComputers.Copy(Computers);

    // ��������� �� ������
    for ( MComputer *comp=(MComputer*)TmpComputers.gFirst();
        comp; comp=(MComputer*)comp->gNext() )
    {
        TListItem *item;
        item=ListViewComputers->Items->Add();
        item->Data=comp;
        SetListViewComputersLine(item);
    }
    // ���������
    ListViewComputers->AlphaSort();

    EditNumber->MaxLength=3;
    EditAddress->MaxLength=MAX_CompAddrLen;

    SetEdit(false);
    ActiveControl=ListViewComputers;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // ������ ������������ ��������
    ListViewComputers->Items->Clear();
    EditNumber->Text="";
    EditAddress->Text="";
    // ������ �����
    TmpComputers.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ListViewComputersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ListViewComputersSelectItem(
      TObject *Sender, TListItem *Item, bool Selected)
{
    if ( ((TListView*)Sender)->SelCount!=1 )
    {
        SetEdit(false);
        return;
    } else SetEdit(true);

    MComputer *comp=(MComputer*)ListViewComputers->Selected->Data;
    EditNumber->Text=IntToStr(comp->Number);
    EditAddress->Text=comp->Address;
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

    MComputer *comp=(MComputer*)ListViewComputers->Selected->Data;

    // �������� ��������������� ��������� ����� � �����
    int Number;
    try { Number=StrToInt(((TEdit*)Sender)->Text); }
    catch ( EConvertError *Error ) { goto error; }
    // ��������� ���������� ��������
    if ( (Number<=0)||(Number>MAX_Comps) ) goto error;
    comp->Number=Number;
    
error:
    ((TEdit*)Sender)->Text=IntToStr(comp->Number);
    SetListViewComputersLine(ListViewComputers->Selected);
    ListViewComputers->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::EditAddressExit(TObject *Sender)
{
    if ( ListViewComputers->Selected==NULL ) return;

    MComputer *comp=(MComputer*)ListViewComputers->Selected->Data;
    comp->SetAddress(((TEdit*)Sender)->Text.Trim().c_str());
    ((TEdit*)Sender)->Text=comp->Address;
    SetListViewComputersLine(ListViewComputers->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::BitBtnNoneClick(TObject *Sender)
{
    int color;
    if ( Sender==BitBtnNone ) color=mgcNone;
    else if ( Sender==BitBtnRed ) color=mgcRed;
    else if ( Sender==BitBtnLime ) color=mgcGreen;
    else if ( Sender==BitBtnAqua ) color=mgcAqua;
    else if ( Sender==BitBtnYellow ) color=mgcYellow;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *item=ListViewComputers->Selected; item;
        item=ListViewComputers->GetNextItem(item,sdAll,is) )
    {
        ((MComputer*)item->Data)->Color=color;
        SetListViewComputersLine(item);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::BitBtnUsedClick(TObject *Sender)
{
    bool NotUsed=Sender==BitBtnUsed? false: true;
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *item=ListViewComputers->Selected; item;
        item=ListViewComputers->GetNextItem(item,sdAll,is) )
    {
        ((MComputer*)item->Data)->NotUsed=NotUsed;
        SetListViewComputersLine(item);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ButtonAddClick(TObject *Sender)
{
    if ( ListViewComputers->Items->Count>=MAX_Comps )
    {
        ResMessageBox(Handle,0,19,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    // ��������� � ����� ��������� � ������� �� '1' ������, ��� � ����������
    MComputer *comp=(MComputer*)TmpComputers.Add();
    MComputer *selcomp=ListViewComputers->Selected==NULL?
        NULL: (MComputer*)ListViewComputers->Selected->Data;
    comp->Number= selcomp ? selcomp->Number+1 : 1;
    char Address[MAX_CompAddrLen+1];
    sprintf(Address,"192.168.1.%i",comp->Number);
    comp->SetAddress(Address);
    // �������� ������ � ������ � ������� � �����������
    TListItem *item=ListViewComputers->Items->Add();
    item->Data=comp;
    // �������� ���������
    SetListViewComputersLine(item);
    ListViewComputers->AlphaSort();
    ListViewComputers->ItemFocused=item;
    ListViewComputers->Selected=NULL;
    ListViewComputers->Selected=ListViewComputers->ItemFocused;
    ActiveControl=EditNumber;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ButtonDelClick(TObject *Sender)
{
    // ������� ���������� �� ������
    TItemStates is=TItemStates()<<isSelected;
    TListItem *item=ListViewComputers->Selected, *next;
    while(item)
    {
        // ������� ��������� �� ������
        TmpComputers.Del((MComputer*)item->Data);
        // ������� ������ �� ������
        next=ListViewComputers->GetNextItem(item,sdAll,is);
        item->Delete();
        item=next;
    }
    //
    ListViewComputers->Selected=ListViewComputers->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputers::ButtonSaveClick(TObject *Sender)
{
    // �������� ������ ����������� �������� �� ������
    Computers->Move(&TmpComputers);
    // ��������� � �����
    if ( !Computers->Save() )
    {
        ShellState->State|=mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,3,MB_APPLMODAL|MB_OK|MB_ICONERROR,Computers->gLastErr());
        return;
    }
    // ������������� ������� ��������
    Sync->Stop();
    if ( States->Update(Computers) )
    {
        // ������ � �����
        if ( !Log->AddComputers(States) )
        {
            // ��������� ���������, �� ��� ����������� �� � ���� �������� �� �����
            ShellState->State|=mssErrorLog|mssErrorConfig; FormMain->SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
        }
        // ��������� ����� ��������� � �����
        if ( !States->Save() )
        {
            ShellState->State|=mssErrorState; FormMain->SetShell();
            ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
        }
        //
        Sync->Associate(States,Computers);      
        FormMain->ProgressBarNetProcess->Max=Sync->gPCountMax();
        // ��������� ������ ����������� � ������� ����
        FormMain->UpdateListViewComputers(true);
    }
    Sync->Start();
}
//---------------------------------------------------------------------------
void TFormComputers::SetEdit(bool Edit_)
{
    TColor Color=Edit_? clWindow: clBtnFace;

    LabelNumber->Enabled=Edit_;
    EditNumber->Enabled=Edit_;
    EditNumber->Color=Color;
    LabelAddress->Enabled=Edit_;
    EditAddress->Enabled=Edit_;
    EditAddress->Color=Color;
}
//---------------------------------------------------------------------------
void TFormComputers::SetListViewComputersLine(TListItem *Item_)
{
    MComputer *comp=(MComputer*)Item_->Data;
    TStrings *SubItems=Item_->SubItems;

    Item_->SubItemImages[0]=FormMain->GetCompColorIcon(comp);
    SubItems->Strings[0]=IntToStr(comp->Number);
    Item_->SubItemImages[1]=comp->NotUsed? 11: 4;
    SubItems->Strings[1]=comp->Address;
}
//---------------------------------------------------------------------------

