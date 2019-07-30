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
        MState *state=(MState*)item->Data;
        if ( !state->CmdFine(0,true) ) continue;
        // Заполним доп. атрибуты
        ApplyFines[i].State=state;
        ApplyFines[i].Number=state->Associated();
        ApplyFines[i].Fine=NULL;
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
    for ( MFine *fine=(MFine*)Fines->gFirst(); fine;
        fine=(MFine*)fine->gNext() )
    {
        char line[MAX_FineDescrLen+13+1]; *line=0;
        strcat(line,fine->Descr);
        strcat(line,"  (");
        if ( fine->Time==(24*60) ) strcat(line,"все время)");
        else sprintf(line+strlen(line),"%.2i мин.)",fine->Time);
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
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ComboBoxTimeClick(TObject *Sender)
{
    MFine *SelFine;
    bool Wait, Warn;

    SelFine=(MFine*)Fines->Item(ComboBoxTime->ItemIndex);
    if ( SelFine==NULL ) return;

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
        ApplyFine=(MApplyFine*)Item->Data;
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
        MApplyFine *appfine=(MApplyFine*)item->Data;
        if ( appfine->Fine!=NULL )
        {
            MState *state=appfine->State;
            // Проверяем возможно ли применить команду
            if ( state->CmdFine(0,true) )
            {
                int time=
                        appfine->Wait||
                       (appfine->Fine->Time==(24*60))?
                        appfine->Fine->Time:
                       -appfine->Fine->Time;
                // Добавляем запись в лог
                if ( !Log->AddFine(appfine->Number,appfine->Fine->gItemID(),time) )
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
    MApplyFine *selfine=(MApplyFine*)Item_->Data;
    MComputer *comp=Computers->Search(selfine->Number);

    // Номер компьютера и цвет его группы
    Item_->SubItemImages[0]=FormMain->GetCompColorIcon(comp);
    Item_->SubItems->Strings[0]=IntToStr(selfine->Number);

    if ( selfine->Fine==NULL ) return;

    // Описание штрафа
    Item_->SubItems->Strings[1]=selfine->Fine->Descr;
    // Время штрафа
    if ( selfine->Warn ) Item_->SubItems->Strings[2]="";
    else
    {
        int Time=selfine->Fine->Time;
        if ( Time==(24*60) ) Item_->SubItems->Strings[2]="Все время";
        else Item_->SubItems->Strings[2]=IntToStr(Time);
    }
    // Ожидание
    if ( (selfine->Fine->Time==(24*60))||selfine->Warn||
        (!selfine->Wait) ) Item_->SubItems->Strings[3]="";
    else Item_->SubItems->Strings[3]="Да";
}
//---------------------------------------------------------------------------

