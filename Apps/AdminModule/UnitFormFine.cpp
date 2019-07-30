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
    MState *State;
    MApplyFine *ApplyFine;
    // Заносим выбранные компьютеры в список для штрафования
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=FormMain->ListViewComputers->Selected; Item;
        Item=FormMain->ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        TListItem *NewItem;
        // Проверяем применим ли к компьютеру штраф
        State=States->Search((int)Item->Data);
        if ( !State->CmdFine(0,true) ) continue;
        //
        NewItem=ListViewFines->Items->Add();
        ApplyFine=(MApplyFine*)NewItem->Data;
        ApplyFine->Number=State->Associated();
        ApplyFine->Fine=NULL;
        ApplyFine->Wait=false;
        ApplyFine->Warn=false;
        NewItem->Selected=true;
        SetListViewFinesLine(NewItem);
    }
    // Заполняем список времен штрафов
    for ( MFine *Fine=(MFine*)Fines->First; Fine; Fine=(MFine*)Fine->Next )
    {
        char line[MAX_FineDescLength+13+1]; *line=0;
        strcat(line,Fine->Description);
        strcat(line,"  (");
        if ( Fine->Time==(24*60) ) strcat(line,"все время)");
        else sprintf(line+strlen(line),"%.2i мин.)",Fine->Time);
        ComboBoxTime->Items->Add(line);
    }

    CheckBoxWait->Enabled=false; CheckBoxWait->Checked=true;
    CheckBoxWarn->Enabled=false; CheckBoxWarn->Checked=false;
    ActiveControl=ComboBoxTime;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::FormHide(TObject *Sender)
{
    // Чистим интерфейсные элементы
    ListViewFines->Items->Clear();
    ComboBoxTime->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ListViewFinesInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;  // Устранение ошибки VCL
    Item->Data=new MApplyFine;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ListViewFinesDeletion(TObject *Sender,
      TListItem *Item)
{
    delete (MApplyFine*)Item->Data;
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
    MApplyFine *ApplyFine;
    int Time;
    MState *State;

    // Удаляем оштрафованые компьютеры из списка
    TItemStates is=TItemStates()<<isSelected;
    TListItem *Item=ListViewFines->Selected, *NextItem;
    while(Item)
    {
        NextItem=ListViewFines->GetNextItem(Item,sdAll,is);
        ApplyFine=(MApplyFine*)Item->Data;
        if ( ApplyFine->Fine!=NULL )
        {
            State=States->Search(ApplyFine->Number);
            // Проверяем возможно ли применить команду
            if ( State->CmdFine(0,true) )
            {
                Time=ApplyFine->Wait||(ApplyFine->Fine->Time==(24*60))?
                    ApplyFine->Fine->Time: -ApplyFine->Fine->Time;
                // Добавляем запись в лог
                if ( !Log->AddFine(State->Associated(),ApplyFine->Fine->ID,Time) )
                {
                    ShellState->State|=mssErrorLog; FormMain->SetShell();
                    FormMain->MessageBoxError(1,2); break;
                }
                // Применяем команду
                State->CmdFine(Time,false);
                // Удаляем строку из списка
                Item->Delete();
            }
        }
        Item=NextItem;
    }
    // Сохраняем новые состояния
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; FormMain->SetShell();
        FormMain->MessageBoxError(1,3);
    }
    // Обновляем список компьютеров
    FormMain->UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::SetListViewFinesLine(TListItem *Item_)
{
    MApplyFine *SelFine=(MApplyFine*)Item_->Data;
    MComputer *Computer=Computers->Search(SelFine->Number);

    // Номер компьютера и цвет его группы
    Item_->SubItemImages[0]=Computer->GroupColor;
    Item_->SubItems->Strings[0]=IntToStr(SelFine->Number);
    //
    if ( SelFine->Fine!=NULL )
    {
        // Описание штрафа
        Item_->SubItems->Strings[1]=SelFine->Fine->Description;
        // Время штрафа
        if ( SelFine->Warn ) Item_->SubItems->Strings[2]="";
        else
        {
            int Time=SelFine->Fine->Time;
            if ( Time==(24*60) ) Item_->SubItems->Strings[2]="Все время";
            else Item_->SubItems->Strings[2]=IntToStr(Time);
        }
        // Ожидание
        if ( (SelFine->Fine->Time==(24*60))||SelFine->Warn||
            (!SelFine->Wait) ) Item_->SubItems->Strings[3]="";
        else Item_->SubItems->Strings[3]="Да";
    }
}
//---------------------------------------------------------------------------

