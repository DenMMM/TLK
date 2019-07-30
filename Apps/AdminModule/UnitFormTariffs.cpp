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
//---------------------------------------------------------------------------
__fastcall TFormTariffs::TFormTariffs(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::FormShow(TObject *Sender)
{
    // �������� ������ ������� � �����
    TmpTariffs.Copy(Tariffs);

    // ��������� �� ������
    for ( MTariff *tariff=(MTariff*)TmpTariffs.gFirst();
        tariff; tariff=(MTariff*)tariff->gNext() )
    {
        TListItem *item;
        item=ListViewNames->Items->Add();
        item->Data=tariff;
        SetListViewNamesLine(item);
    }

    // ��������� ������ ����� ��������
    for ( int i=1; i<=8; i++ ) CheckListBoxApps->Items->Add(IntToStr(i));
    // ��������� ������ �����������
    for ( MComputer *comp=(MComputer*)Computers->gFirst();
        comp; comp=(MComputer*)comp->gNext() )
    {
        TListItem *item;
        item=ListViewComputers->Items->Add();
        item->Data=comp;
        item->Caption=IntToStr(comp->Number);
        item->ImageIndex=FormMain->GetCompColorIcon(comp);
    }

    EditName->MaxLength=MAX_TariffNameLen;
    SetEdit(false);
    ActiveControl=ListViewNames;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // ������ ������������ ��������
    ListViewNames->Items->Clear();
    EditName->Text="";
    CheckBoxReboot->Checked=false;
    CheckListBoxApps->Items->Clear();
    ListViewComputers->Items->Clear();
    // ������ �����
    TmpTariffs.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ListViewNamesInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ListViewNamesSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( ((TListView*)Sender)->SelCount!=1 )
    {
        SetEdit(false);
        return;
    } else
        SetEdit(true);

    MTariff *tariff=(MTariff*)ListViewNames->Selected->Data;
    EditName->Text=tariff->Name;
    // ����������� ������ ��������
    unsigned Programs=tariff->Programs;
    CheckListBoxApps->Checked[0]=Programs&mgp1;
    CheckListBoxApps->Checked[1]=Programs&mgp2;
    CheckListBoxApps->Checked[2]=Programs&mgp3;
    CheckListBoxApps->Checked[3]=Programs&mgp4;
    CheckListBoxApps->Checked[4]=Programs&mgp5;
    CheckListBoxApps->Checked[5]=Programs&mgp6;
    CheckListBoxApps->Checked[6]=Programs&mgp7;
    CheckListBoxApps->Checked[7]=Programs&mgp8;
    //
    CheckBoxReboot->Checked=tariff->Reboot;
    CheckBoxRoute->Checked=tariff->Programs&mgpRoute;
    // ����������� ��� ����� ����������� ������������ �����
    int i=0;
    for ( MComputer *comp=(MComputer*)Computers->gFirst();
        comp; comp=(MComputer*)comp->gNext() )
    {
        ListViewComputers->Items->Item[i]->Checked=
            tariff->CheckForComp(comp->Number);
        i++;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonAddClick(TObject *Sender)
{
    if ( ListViewNames->Items->Count>=MAX_Tariffs )
    {
        ResMessageBox(Handle,0,20,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    // ��������� ����� ����� � �����
    MTariff *tariff=(MTariff*)TmpTariffs.Add();
    tariff->SetName("����� �����");
    // ��������� ������ � ������ � ��������� �� � �������
    TListItem *item=ListViewNames->Items->Add();
    item->Data=tariff;
    SetListViewNamesLine(item);
    // ��������� ���������
    ListViewNames->ItemFocused=item;
    ListViewNames->Selected=NULL;
    ListViewNames->Selected=item;
    ActiveControl=EditName;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonDelClick(TObject *Sender)
{
    // ������� ������ �� ������
    TItemStates is=TItemStates()<<isSelected;
    TListItem *item=ListViewNames->Selected, *next;
    while(item)
    {
        // ������� ����� �� ������
        TmpTariffs.Del((MTariff*)item->Data);
        // ������� ������ �� ������
        next=ListViewNames->GetNextItem(item,sdAll,is);
        item->Delete();
        item=next;
    }
    //
    ListViewComputers->Selected=ListViewComputers->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::EditNameExit(TObject *Sender)
{
    if ( ListViewNames->Selected==NULL ) return;

    MTariff *tariff=(MTariff*)ListViewNames->Selected->Data;
    tariff->SetName(((TEdit*)Sender)->Text.Trim().c_str());
    ((TEdit*)Sender)->Text=tariff->Name;
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

    unsigned progs=0;
    if ( CheckListBoxApps->Checked[0] ) progs|=mgp1;
    if ( CheckListBoxApps->Checked[1] ) progs|=mgp2;
    if ( CheckListBoxApps->Checked[2] ) progs|=mgp3;
    if ( CheckListBoxApps->Checked[3] ) progs|=mgp4;
    if ( CheckListBoxApps->Checked[4] ) progs|=mgp5;
    if ( CheckListBoxApps->Checked[5] ) progs|=mgp6;
    if ( CheckListBoxApps->Checked[6] ) progs|=mgp7;
    if ( CheckListBoxApps->Checked[7] ) progs|=mgp8;
    ((MTariff*)ListViewNames->Selected->Data)->Programs=progs;

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
    if ( ListViewNames->Selected==NULL ) return;
    // ��������� ������ ������� �����������
    int count=0;
    TListItems *items=ListViewComputers->Items;
    for ( int i=0; (i<items->Count)&&(i<sizeof(TmpComps)); i++ )
    {
        TListItem *item=items->Item[i];
        if ( item->Checked )
            TmpComps[count++]=((MComputer*)item->Data)->Number;
    }
    // ��������� ����� ����� ������� � ������
    MTariff *tariff=(MTariff*)ListViewNames->Selected->Data;
    tariff->SetComps(TmpComps,count);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonSetSelCompClick(TObject *Sender)
{
    bool checked;
    if ( Sender==ButtonSetSelComp ) checked=true;
    else if ( Sender==ButtonResSelComp ) checked=false;
    else return;

    TListItems *items=ListViewComputers->Items;
    for ( int i=0, j=items->Count; i<j; i++ )
        if ( items->Item[i]->Selected ) items->Item[i]->Checked=checked;
        
    ListViewComputersExit(ListViewComputers);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonSetAllCompClick(TObject *Sender)
{
    bool checked;
    if ( Sender==ButtonSetAllComp ) checked=true;
    else if ( Sender==ButtonResAllComp ) checked=false;
    else return;

    TListItems *items=ListViewComputers->Items;
    for ( int i=0, j=items->Count; i<j; i++ )
        items->Item[i]->Checked=checked;
    
    ListViewComputersExit(ListViewComputers);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonTimesClick(TObject *Sender)
{
    Munique_ptr <TFormTariffTimes> form;

    if ( ListViewNames->Selected==NULL ) return;

    try
    {
        MTariff *tariff=(MTariff*)ListViewNames->Selected->Data;
        // ��������� ������ ��������������
        form(new TFormTariffTimes(0));
        form.get()->Execute(&tariff->Times,tariff->Name,Left+30,Top+30);
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonSaveClick(TObject *Sender)
{
    // �������� ������ �������� �� ������
    Tariffs->Move(&TmpTariffs);
    // ������ ID-������ ��� ����� �������
    Tariffs->SetIDs();
    // ��������� � �����
    if ( !Tariffs->Save() )
    {
        ShellState->State|=mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,3,MB_APPLMODAL|MB_OK|MB_ICONERROR,Tariffs->gLastErr());
        return;
    }
    // ������ � �����
    if ( !Log->AddTariffs(Tariffs) )
    {
        // ��������� ���������, �� ��� ����������� �� � ���� �������� �� �����
        ShellState->State|=mssErrorLog|mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
    }
    // ��������� ������ ����������� � ������� ����
    FormMain->UpdateListViewComputers(true);
}
//---------------------------------------------------------------------------
void TFormTariffs::SetEdit(bool Edit_)
{
    TColor Color=Edit_? clWindow: clBtnFace;

    LabelTariffName->Enabled=Edit_;
    EditName->Enabled=Edit_;
    EditName->Color=Color;
    CheckBoxReboot->Enabled=Edit_;
    CheckBoxRoute->Enabled=Edit_;
    ButtonTimes->Enabled=Edit_;
    LabelTariffApps->Enabled=Edit_;
    CheckListBoxApps->Enabled=Edit_;
    CheckListBoxApps->Color=Color;
    LabelTariffComputers->Enabled=Edit_;
    ListViewComputers->Enabled=Edit_;
    ListViewComputers->Color=Color;
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

