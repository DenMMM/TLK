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
    // Копируем текущие штрафы в буфер
	TmpFines=*Fines;

    // Формируем их список
	for ( auto &fine: TmpFines )
	{
		TListItem *item;
		item=ListViewFines->Items->Add();
		item->Data=&fine;
		SetListViewFinesLine(item);
    }
    // Сортируем для красоты
    ListViewFines->AlphaSort();

    // Формируем список для выбора времени штрафа
    ComboBoxTime->Items->Add(L"Все время");
    for ( int i=1; i<=60; i++ ) ComboBoxTime->Items->Add(IntToStr(i));

    EditDescription->MaxLength=MAX_FineDescrLen;
    
    SetEdit(false);
	ActiveControl=ListViewFines;

	// Добавим энтропии
	BasicRand.event();
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // Чистим интерфейсные элементы
    ListViewFines->Items->Clear();
    EditDescription->Text=L"";
    ComboBoxTime->Items->Clear();
    // Чистим буфер
    TmpFines.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ListViewFinesInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;  // Устранение ошибки VCL
    Item->SubItems->Add(L"");
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ListViewFinesSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( dynamic_cast<TListView&>(*Sender).SelCount!=1 )
    {
        SetEdit(false);
        return;
    } else
        SetEdit(true);

    auto *fine=reinterpret_cast<MFinesItem*>(ListViewFines->Selected->Data);
    EditDescription->Text=fine->Descr.c_str();
    ComboBoxTime->ItemIndex=fine->Time==(24*60)? 0: fine->Time;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::EditDescriptionExit(TObject *Sender)
{
    if ( ListViewFines->Selected==nullptr ) return;

    EditDescription->Text=EditDescription->Text.Trim();
    auto *fine=reinterpret_cast<MFinesItem*>(ListViewFines->Selected->Data);
    fine->Descr=EditDescription->Text.c_str();
    SetListViewFinesLine(ListViewFines->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ComboBoxTimeClick(TObject *Sender)
{
    if ( ListViewFines->Selected==nullptr ) return;

	int ItemIndex=dynamic_cast<TComboBox&>(*Sender).ItemIndex;
	reinterpret_cast<MFinesItem*>(ListViewFines->Selected->Data)->Time=
		ItemIndex==0? 24*60: ItemIndex;
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

    // Добавляем в буфер новый штраф
	MFinesItem& fine=TmpFines.Add();
	fine.Descr=L"Новый штраф";
    fine.Time=1;
    // Добавляем строку в список и связываем ее с тарифом
    TListItem *item=ListViewFines->Items->Add();
    item->Data=&fine;
    // Обновляем интерфейс
    SetListViewFinesLine(item);
    ListViewFines->AlphaSort();
    ListViewFines->ItemFocused=item;
    ListViewFines->Selected=nullptr;
    ListViewFines->Selected=ListViewFines->ItemFocused;
    ActiveControl=EditDescription;
}
//---------------------------------------------------------------------------
void __fastcall TFormFines::ButtonDelClick(TObject *Sender)
{
    // Удаляем штрафы из списка
    TItemStates is=TItemStates()<<isSelected;
    TListItem *item=ListViewFines->Selected, *next;
    while(item)
    {
        // Удаляем штраф из буфера
		TmpFines.Del(
			MFines::const_iterator(
			reinterpret_cast<MFinesItem*>(item->Data)
			));
        // Удаляем строку из списка
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
    // Замещаем штрафами из буфера текущие
    *Fines=std::move(TmpFines);
    // Задаем ID-номера для новых
    Fines->SetUUIDs();
    // Сохраняем в файле
    if ( !Fines->Save() )
    {
        ShellState->State|=mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,3,MB_APPLMODAL|MB_OK|MB_ICONERROR,Fines->gLastErr());
        return;
    }
    // Запись в логах
    if ( !Log->AddFines(Fines.get()) )
    {
        // Настройки сохранили, но без отображения их в логе работать не дадим
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
//    if ( !Edit_ ) EditDescription->Text=L"";
    LabelTime->Enabled=Edit_;
    ComboBoxTime->Enabled=Edit_;
    ComboBoxTime->Color=Color;
//    if ( !Edit_ ) ComboBoxTime->ItemIndex=-1;
}
//---------------------------------------------------------------------------
void TFormFines::SetListViewFinesLine(TListItem *Item_)
{
    auto *fine=reinterpret_cast<MFinesItem*>(Item_->Data);
    Item_->Caption=fine->Descr.c_str();
    if ( fine->Time==(24*60) ) Item_->SubItems->Strings[0]=L"Все время";
    else Item_->SubItems->Strings[0]=IntToStr(fine->Time)+L" мин.";
}
//---------------------------------------------------------------------------

