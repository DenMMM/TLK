//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "UnitFormFine.h"
#include "UnitFormMain.h"
#include "UnitStates.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormFine *FormFine;
//---------------------------------------------------------------------------
__fastcall TFormFine::TFormFine(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::FormShow(TObject *Sender)
{
    // Выделим буфер сразу под все выбранные компьютеры
    ApplyFines.Alloc(FormMain->ListViewComputers->SelCount);

    unsigned i=0;
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *item=FormMain->ListViewComputers->Selected; item;
        item=FormMain->ListViewComputers->GetNextItem(item,sdAll,is) )
    {
        // Проверим применим ли к компьютеру штраф
        auto state=reinterpret_cast<MStatesItem*>(item->Data);
        if ( !state->CmdFine(0,true) ) continue;
        // Заполним доп. атрибуты
        ApplyFines[i].State=state;
        ApplyFines[i].Number=state->Associated();
        ApplyFines[i].Fine=nullptr;
        ApplyFines[i].Wait=false;
        ApplyFines[i].Warn=false;
        // Добавим строку в список компьютеров и свяжем с атрибутами
        TListItem *newitem=ListViewFines->Items->Add();
        newitem->Data=&ApplyFines[i];
        // Выберем ее для удобства
        newitem->Selected=true;
        SetListViewFinesLine(newitem);
        i++;
    }

    // Заполняем список времен штрафов
	for ( MFinesItem* fine=Fines->gFirst();
		fine; fine=fine->gNext() )
	{
		UnicodeString line=fine->Descr.c_str();
		line+=L"  (";

		if ( fine->Time==(24*60) ) line+=L"все время)";
		else
		{
			wchar_t time[8+1];
			swprintf(time, sizeof(time), L"%.2i мин.)", fine->Time);
			line+=time;
		}

		ComboBoxTime->Items->Add(line);
	}

	CheckBoxWait->Enabled=false; CheckBoxWait->Checked=true;
	CheckBoxWarn->Enabled=false; CheckBoxWarn->Checked=false;
	ActiveControl=ComboBoxTime;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::FormClose(TObject *Sender, TCloseAction &Action)
{
    // Чистим интерфейсные элементы
    ListViewFines->Items->Clear();
    ComboBoxTime->Items->Clear();
    // Чистим буфер
    ApplyFines.Alloc(0);
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ListViewFinesInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;  // Устранение ошибки VCL
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
    Item->SubItems->Add(L"");
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ComboBoxTimeClick(TObject *Sender)
{
	bool Wait, Warn;

    if ( ComboBoxTime->ItemIndex<0 ) return;
	MFinesItem *SelFine=Fines->GetItem(ComboBoxTime->ItemIndex);
//	if ( SelFine==nullptr ) return;         /// теперь выше throw

//    CheckBoxWarn->Enabled=true;
    Warn=CheckBoxWarn->Checked;
    if ( (SelFine->Time==(24*60))||Warn )
    {
        CheckBoxWait->Enabled=false;
        Wait=false;
    } else
    {
        CheckBoxWait->Enabled=true;
        Wait=CheckBoxWait->Checked;
    }

    // Заносим новые значения штрафа для выбранных компьютеров
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewFines->Selected; Item;
        Item=ListViewFines->GetNextItem(Item,sdAll,is) )
    {
        MApplyFine *ApplyFine;
        ApplyFine=reinterpret_cast<MApplyFine*>(Item->Data);
        ApplyFine->Fine=SelFine;
        ApplyFine->Wait=Wait;
        ApplyFine->Warn=Warn;
        SetListViewFinesLine(Item);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::BitBtnFineClick(TObject *Sender)
{
    // Удаляем оштрафованые компьютеры из списка
    TItemStates is=TItemStates()<<isSelected;
    TListItem *item=ListViewFines->Selected, *next;
    while(item)
    {
        next=ListViewFines->GetNextItem(item,sdAll,is);
        auto appfine=reinterpret_cast<MApplyFine*>(item->Data);
        if ( appfine->Fine!=nullptr )
        {
            MStatesItem *state=appfine->State;
            // Проверяем возможно ли применить команду
            if ( state->CmdFine(0,true) )
            {
                int time=
                        appfine->Wait||
                       (appfine->Fine->Time==(24*60))?
                        appfine->Fine->Time:
                       -appfine->Fine->Time;
                // Добавляем запись в лог
                if ( !Log->AddFine(appfine->Number,appfine->Fine->gUUID(),time) )
                {
                    ShellState->State|=mssErrorLog; FormMain->SetShell();
                    ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
                    break;
                }
                // Применяем команду
                state->CmdFine(time,false);
                // Удаляем строку из списка
                item->Delete();
            }
        }
        item=next;
    }
    // Сохраняем новые состояния
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; FormMain->SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // Обновляем список компьютеров
    FormMain->UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::SetListViewFinesLine(TListItem *Item_)
{
    auto selfine=reinterpret_cast<MApplyFine*>(Item_->Data);
    MComputersItem *comp=Computers->Search(selfine->Number);

    // Номер компьютера и цвет его группы
    Item_->SubItemImages[0]=FormMain->GetCompColorIcon(comp);
    Item_->SubItems->Strings[0]=IntToStr(selfine->Number);

    if ( selfine->Fine==nullptr ) return;

    // Описание штрафа
    Item_->SubItems->Strings[1]=selfine->Fine->Descr.c_str();
    // Время штрафа
	if ( selfine->Warn ) Item_->SubItems->Strings[2]=L"";
	else
	{
		int Time=selfine->Fine->Time;
		if ( Time==(24*60) ) Item_->SubItems->Strings[2]=L"Все время";
		else Item_->SubItems->Strings[2]=IntToStr(Time);
	}
	// Ожидание
	if ( (selfine->Fine->Time==(24*60))||selfine->Warn||
        (!selfine->Wait) ) Item_->SubItems->Strings[3]=L"";
	else Item_->SubItems->Strings[3]=L"Да";
}
//---------------------------------------------------------------------------

