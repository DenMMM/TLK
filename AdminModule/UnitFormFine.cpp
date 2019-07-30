//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormFine.h"
#include "UnitFormMain.h"
#include "UnitFines.h"
#include "UnitOptionsLoadSave.h"
#include "UnitLogs.h"
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
    CurrentFineComputer=new MFineComputer;

    // Заносим выбранные компьютеры в список для штрафования
    ListViewComputersForFine->Items->Clear();
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=FormMain->ListViewComputers->Selected; Item;
        Item=FormMain->ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MComputer *Computer;
        Computer=(MComputer*)Item->Data;
        if ( !(Computer->State&mcsWork) ) continue;
        TListItem *NewItem;
        NewItem=ListViewComputersForFine->Items->Add();
        NewItem->Data=(void*)Computer;
        NewItem->Selected=true;
        SetListViewComputersForFineLine(NewItem);
    }

    // Заполняем список времен штрафов
    ComboBoxFineTime->Items->Clear();
    for ( MFine *Fine=(MFine*)Fines->FirstItem; Fine; Fine=(MFine*)Fine->NextItem )
        ComboBoxFineTime->Items->Add(IntToStr(Fine->Time)+" мин. - "+Fine->Comment);
    //
    RadioButtonCertainTime->Checked=true;
    TimerDialogLock->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::FormHide(TObject *Sender)
{
    TimerDialogLock->Enabled=false;
    ListViewComputersForFine->Items->Clear();
    ListViewFineComputers->Items->Clear();
    ComboBoxFineTime->Items->Clear();
    delete CurrentFineComputer;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::TimerDialogLockTimer(TObject *Sender)
{
    if ( ProgressBarDialogUseTime->Position<ProgressBarDialogUseTime->Max )
    {
        ProgressBarDialogUseTime->Position++;
    } else
    {
        TimerDialogLock->Enabled=false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ListViewComputersForFineEnter(TObject *Sender)
{
    ListViewFineComputers->Selected=NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::RadioButtonCertainTimeClick(TObject *Sender)
{
    if ( Sender==RadioButtonCertainTime )
    {
        SetGroupStateCertTime(true);
        SetGroupStateAllTime(false);
        CurrentFineComputer->FineTime=24*60+1;
        ComboBoxFineTimeChange(ComboBoxFineTime);
    } else if ( Sender==RadioButtonAllTime )
    {
        SetGroupStateCertTime(false);
        SetGroupStateAllTime(true);
        CurrentFineComputer->FineTime=0;
        CheckBoxReturnManyClick(CheckBoxReturnMany);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ComboBoxFineTimeChange(TObject *Sender)
{
    int ItemIndex;
    if ( (ItemIndex=ComboBoxFineTime->ItemIndex)<0 ) return;
    CurrentFineComputer->FineTime=((MFine*)Fines->Item(ItemIndex))->Time;
    if ( !CheckBoxEnableWaiting->Checked )
        CurrentFineComputer->FineTime=-CurrentFineComputer->FineTime;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::CheckBoxEnableWaitingClick(TObject *Sender)
{
    if ( CurrentFineComputer->FineTime>(24*60) ) return;
    if ( (CheckBoxEnableWaiting->Checked&&(CurrentFineComputer->FineTime<0))||
         ((!CheckBoxEnableWaiting->Checked)&&(CurrentFineComputer->FineTime>0)) )
        CurrentFineComputer->FineTime=-CurrentFineComputer->FineTime;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::CheckBoxReturnManyClick(TObject *Sender)
{
    CurrentFineComputer->EnableReturnMany=CheckBoxReturnMany->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::BitBtnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::SetListViewComputersForFineLine(TListItem *Item_)
{
    MComputer *Computer_=(MComputer*)Item_->Data;
    Item_->ImageIndex=Computer_->GroupColor;
    Item_->Caption=IntToStr(Computer_->Number);
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::SetListViewFineComputersLine(TListItem *Item_)
{
    MFineComputer *FineComputer_=(MFineComputer*)Item_->Data;
    AnsiString line;
    // Выставляем номер компьютера и цвет его группы
    Item_->SubItems->Strings[0]=IntToStr(FineComputer_->Computer->Number);
    Item_->SubItemImages[0]=FineComputer_->Computer->GroupColor;
    // Название тарифа и его иконка
    int TariffID;
    if ( (TariffID=FineComputer_->Computer->TariffID)!=0 )
    {
        MTariff *Tariff=Tariffs->SearchID(TariffID);
        Item_->SubItems->Strings[1]=Tariff->Name;
        Item_->SubItemImages[1]=Tariff->IconNumber;
    } else
    {
        Item_->SubItems->Strings[1]="";
        Item_->SubItemImages[1]=-1;
    }
    // Время штрафа
    int a=FineComputer_->FineTime;
    if ( a )
    {
        if ( a<0 ) line=IntToStr(-a); else line=IntToStr(a);
        line+=" мин.";
    } else line="Все время";
    Item_->SubItems->Strings[2]=line;
    // Ожидание
    if ( a ) { if ( a<0 ) line="Нет"; else line="Да"; }
    else line="";
    Item_->SubItems->Strings[3]=line;
    // Сумма для возврата
    if ( (!a)&&FineComputer_->EnableReturnMany )
        line=FloatToStrF(FineComputer_->ReturnMany,ffCurrency,8,2);
    else line="";
    Item_->SubItems->Strings[4]=line;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::SetGroupStateCertTime(bool State_)
{
    bool enabled;
    TColor color;

    if ( State_ ) { enabled=true; color=clWindow; }
    else { enabled=false; color=clBtnFace; }

    ComboBoxFineTime->Enabled=enabled;
    ComboBoxFineTime->Color=color;
    CheckBoxEnableWaiting->Enabled=enabled;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::SetGroupStateAllTime(bool State_)
{
    bool enabled;

    if ( State_ ) enabled=true;
    else enabled=false;

//    CheckBoxReturnMany->Enabled=enabled;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ButtonAddClick(TObject *Sender)
{
    if ( CurrentFineComputer->FineTime>(24*60) ) return;
    // Обрабатываем выбранные компьютеры
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputersForFine->Selected, *NewItem; Item; )
    {
        // Добавляем очередной компьютер в список для штрафования
        TListItem *NewItem;
        NewItem=ListViewFineComputers->Items->Add();
        CurrentFineComputer->Computer=(MComputer*)Item->Data;
        if ( CurrentFineComputer->EnableReturnMany )
        {
            // Определяем сумму возвращаемых денег
//            double ReturnMany;
        }
        *((MFineComputer*)NewItem->Data)=*CurrentFineComputer;
        SetListViewFineComputersLine(NewItem); NewItem->Selected=true;
        // Удаляем компьютер из списка для штрафования
        NewItem=ListViewComputersForFine->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
    //
    ComboBoxFineTime->ItemIndex=-1;
    CheckBoxEnableWaiting->Checked=false;
    CheckBoxReturnMany->Checked=false;
    RadioButtonCertainTime->Checked=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ButtonChangeClick(TObject *Sender)
{
    if ( CurrentFineComputer->FineTime>(24*60) ) return;
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewFineComputers->Selected; Item;
        Item=ListViewFineComputers->GetNextItem(Item,sdAll,is) )
    {
        MFineComputer *FineComputer;
        FineComputer=(MFineComputer*)Item->Data;
        MComputer *Computer;
        Computer=FineComputer->Computer;
        if ( CurrentFineComputer->EnableReturnMany )
        {
            // Определяем сумму возвращаемых денег
//            double ReturnMany;
        }
        *FineComputer=*CurrentFineComputer;
        FineComputer->Computer=Computer;
        SetListViewFineComputersLine(Item);
    }
    //
    ComboBoxFineTime->ItemIndex=-1;
    CheckBoxEnableWaiting->Checked=false;
    CheckBoxReturnMany->Checked=false;
    RadioButtonCertainTime->Checked=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ButtonDeleteClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewFineComputers->Selected, *NewItem; Item; )
    {
        MFineComputer *FineComputer;
        FineComputer=(MFineComputer*)Item->Data;
        // Добавляем очередной компьютер в список для штрафования
        NewItem=ListViewComputersForFine->Items->Add();
        NewItem->Data=(void*)FineComputer->Computer;
        SetListViewComputersForFineLine(NewItem);
        // Удаляем компьютер из списка оформленных
        NewItem=ListViewFineComputers->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ButtonFineClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewFineComputers->Selected, *NewItem; Item; )
    {
        MFineComputer *FineComputer;
        FineComputer=(MFineComputer*)Item->Data;
        //
        if ( State->CurrentAdmin ) LogCompRun(FineComputer->Computer,(MTariff*)8,
            FineComputer->FineTime,0.);
        // Применяем штраф к компьютеру
        FineComputer->Computer->CmdFine(FineComputer->FineTime);
        // Удаляем компьютер из списка для применения штрафа
        NewItem=ListViewFineComputers->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ListViewFineComputersEnter(TObject *Sender)
{
    ListViewComputersForFine->Selected=NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ListViewFineComputersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->Data=(void*)new MFineComputer;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormFine::ListViewFineComputersDeletion(TObject *Sender,
      TListItem *Item)
{
    delete (MFineComputer*)Item->Data;
}
//---------------------------------------------------------------------------

