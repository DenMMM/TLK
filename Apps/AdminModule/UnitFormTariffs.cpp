//---------------------------------------------------------------------------
#include <vcl.h>
#include <string.h>
#include <memory>
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
    // Копируем список тарифов в буфер
	TmpTariffs=*Tariffs;

    // Формируем их список
	for ( MTariffsItem* tariff=TmpTariffs.gFirst();
		tariff; tariff=tariff->gNext() )
    {
        TListItem *item;
        item=ListViewNames->Items->Add();
        item->Data=tariff;
        SetListViewNamesLine(item);
    }

    // Заполняем список групп программ
    for ( int i=1; i<=8; i++ ) CheckListBoxApps->Items->Add(IntToStr(i));
	// Формируем список компьютеров
	for ( MComputersItem* comp=Computers->gFirst();
		comp; comp=comp->gNext() )
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
    // Чистим интерфейсные элементы
    ListViewNames->Items->Clear();
    EditName->Text=L"";
    CheckBoxReboot->Checked=false;
    CheckListBoxApps->Items->Clear();
    ListViewComputers->Items->Clear();
    // Чистим буфер
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
    if ( dynamic_cast<TListView&>(*Sender).SelCount!=1 )
    {
        SetEdit(false);
        return;
    } else
        SetEdit(true);

	auto tariff=reinterpret_cast<MTariffsItem*>(
		ListViewNames->Selected->Data);
    EditName->Text=tariff->Name.c_str();
    // Проставляем группы программ
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
    CheckBoxDesktop->Checked=tariff->Programs&mgpDesktop;
    // Проставляем для каких компьютеров используется тариф
    int i=0;
	for ( MComputersItem* comp=Computers->gFirst();
		comp; comp=comp->gNext() )
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

    // Добавляем новый тариф в буфер
	MTariffsItem* tariff=TmpTariffs.Add();
    tariff->Name=L"Новый тариф";
    // Добавляем строку в список и связываем ее с тарифом
    TListItem *item=ListViewNames->Items->Add();
    item->Data=tariff;
    SetListViewNamesLine(item);
    // Обновляем интерфейс
    ListViewNames->ItemFocused=item;
    ListViewNames->Selected=nullptr;
    ListViewNames->Selected=item;
    ActiveControl=EditName;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonDelClick(TObject *Sender)
{
    // Удаляем тарифы из списка
    TItemStates is=TItemStates()<<isSelected;
    TListItem *item=ListViewNames->Selected, *next;
    while(item)
    {
        // Удаляем тариф из буфера
        TmpTariffs.Del(reinterpret_cast<MTariffsItem*>(item->Data));
        // Удаляем строку из списка
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
    if ( ListViewNames->Selected==nullptr ) return;

    EditName->Text=EditName->Text.Trim();
	auto tariff=reinterpret_cast<MTariffsItem*>(
		ListViewNames->Selected->Data);
    tariff->Name=EditName->Text.c_str();
    SetListViewNamesLine(ListViewNames->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::CheckBoxRebootExit(TObject *Sender)
{
    if ( ListViewNames->Selected==nullptr ) return;

	auto item=reinterpret_cast<MTariffsItem*>(
		ListViewNames->Selected->Data);
	TCheckBox &cbox=dynamic_cast<TCheckBox&>(*Sender);

	item->Reboot=cbox.Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::CheckBoxRouteExit(TObject *Sender)
{
	if ( ListViewNames->Selected==nullptr ) return;

	auto item=reinterpret_cast<MTariffsItem*>(
		ListViewNames->Selected->Data);
	TCheckBox &cbox=dynamic_cast<TCheckBox&>(*Sender);

	if ( cbox.Checked )
		item->Programs|=mgpRoute;
	else
		item->Programs&=~mgpRoute;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::CheckBoxDesktopExit(TObject *Sender)
{
	if ( ListViewNames->Selected==nullptr ) return;

	auto item=reinterpret_cast<MTariffsItem*>(
		ListViewNames->Selected->Data);
	TCheckBox &cbox=dynamic_cast<TCheckBox&>(*Sender);

	if ( cbox.Checked )
		item->Programs|=mgpDesktop;
	else
		item->Programs&=~mgpDesktop;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::CheckListBoxAppsExit(TObject *Sender)
{
	if ( ListViewNames->Selected==nullptr ) return;

	unsigned progs=0;
	if ( CheckListBoxApps->Checked[0] ) progs|=mgp1;
	if ( CheckListBoxApps->Checked[1] ) progs|=mgp2;
	if ( CheckListBoxApps->Checked[2] ) progs|=mgp3;
	if ( CheckListBoxApps->Checked[3] ) progs|=mgp4;
	if ( CheckListBoxApps->Checked[4] ) progs|=mgp5;
    if ( CheckListBoxApps->Checked[5] ) progs|=mgp6;
    if ( CheckListBoxApps->Checked[6] ) progs|=mgp7;
    if ( CheckListBoxApps->Checked[7] ) progs|=mgp8;
	reinterpret_cast<MTariffsItem*>(
		ListViewNames->Selected->Data)->Programs=progs;

    dynamic_cast<TCheckListBox&>(*Sender).ItemIndex=-1;
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
    if ( ListViewNames->Selected==nullptr ) return;
    // Заполняем массив номеров компьютеров
    int count=0;
    TListItems *items=ListViewComputers->Items;
    for ( int i=0; (i<items->Count)&&(i<sizeof(TmpComps)); i++ )
    {
        TListItem *item=items->Item[i];
		if ( item->Checked )
			TmpComps[count++]=reinterpret_cast<MComputersItem*>(
				item->Data)->Number;
    }
    // Сохраняем копию этого массива в тарифе
	auto tariff=reinterpret_cast<MTariffsItem*>(
		ListViewNames->Selected->Data);
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
	std::unique_ptr <TFormTariffTimes> form;

    if ( ListViewNames->Selected==nullptr ) return;

    try
    {
		auto tariff=reinterpret_cast<MTariffsItem*>(
			ListViewNames->Selected->Data);
        // Открываем диалог редактирования
		form.reset(new TFormTariffTimes(0));
		form->Execute(tariff->Times, tariff->Name.c_str(), Left+30, Top+30);
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffs::ButtonSaveClick(TObject *Sender)
{
    // Замещаем тарифы записями из буфера
    Tariffs->Move(&TmpTariffs);
    // Задаем ID-номера для новых тарифов
    Tariffs->SetUUIDs();
    // Сохраняем в файле
    if ( !Tariffs->Save() )
    {
        ShellState->State|=mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,3,MB_APPLMODAL|MB_OK|MB_ICONERROR,Tariffs->gLastErr());
        return;
    }
    // Запись в логах
    if ( !Log->AddTariffs(Tariffs.get()) )
    {
        // Настройки сохранили, но без отображения их в логе работать не дадим
        ShellState->State|=mssErrorLog|mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
    }
    // Обновляем список компьютеров в главном окне
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
    CheckBoxDesktop->Enabled=Edit_;
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
    Item_->Caption=reinterpret_cast<MTariffsItem*>(Item_->Data)->Name.c_str();
}
//---------------------------------------------------------------------------


