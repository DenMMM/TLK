//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormMain.h"
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
void __fastcall TFormMain::ButtonSelectClick(TObject *Sender)
{
    if ( !OpenDialog->Execute() ) return;

    int PayTime=0;
    double PayMany=0.;

    MEvents *Events=new MEvents;
    if ( Events==NULL ) goto error;
    if ( !Events->Load(OpenDialog->FileName) )
    {
        ::MessageBox(Application->Handle, "Файл лога поврежден !",
            "LogViewer - ошибка",MB_OK|MB_ICONERROR|MB_APPLMODAL);
        goto error;
    }
    //
    LabelOpenTime->Caption=DateTimeToStr((TDateTime)Events->OpenTime);
    LabelCloseTime->Caption=DateTimeToStr((TDateTime)Events->CloseTime);
    {
        int time=(Events->CloseTime-Events->OpenTime)*(24.*60);
        int h=time/60, m=time%60;
        LabelLengthTime->Caption=IntToStr(h)+" час. "+IntToStr(m/10)+IntToStr(m%10)+" мин.";
    }
    EditName->Text=Events->Name;
    EditJob->Text=Events->Job;
    // Заносим события в таблицу и попутно считаем общие данные
    ListViewEvents->Items->Clear();
    for ( MEvent *Event=(MEvent*)Events->FirstItem; Event;
        Event=(MEvent*)Event->NextItem )
    {
        TListItem *NewListItem;
        TStrings *SubItems;
        NewListItem=ListViewEvents->Items->Add();
        SubItems=NewListItem->SubItems;

        AnsiString line;
        line=Event->TariffName;

        SubItems->Strings[0]=((TDateTime)Event->Time).FormatString("dd - hh:nn:ss");
        if ( line=="!*" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Запуск компьютера под сисадмином";
            int h=Event->TimeSize/60, m=Event->TimeSize%60;
            SubItems->Strings[3]=IntToStr(h)+" час. "+IntToStr(m/10)+IntToStr(m%10)+" мин.";
            SubItems->Strings[4]=FloatToStrF(Event->Cost,ffCurrency,8,2);
            *((double*)NewListItem->Data)=Event->Cost;
        } else if ( line=="!a" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Перевод в автономный режим";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!n" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Перевод в нормальный режим работы";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!w" )
        {
            SubItems->Strings[2]=" ! Отключение программы";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!l" )
        {
            SubItems->Strings[2]=" ! Включение программы";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!s" )
        {
            SubItems->Strings[2]=" ! В программу вошел сисадмин";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!u" )
        {
            SubItems->Strings[2]=" ! Сисадмин вышел или вошел другой пользователь";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!g" )
        {
            SubItems->Strings[2]=" ! Изменение списка игр";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!f" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Штраф";
            SubItems->Strings[3]=IntToStr(Event->TimeSize)+" мин.";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!r" )
        {
            SubItems->Strings[2]=" ! Программа запущена";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!c" )
        {
            SubItems->Strings[2]=" ! Программа закрыта";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!t" )
        {
            SubItems->Strings[2]=" ! Изменения в тарифах";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!o" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Пересадка ==> (компьютер закрыт)";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!i" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Пересадка <== (компьютер открыт)";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!L" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Прикрыт";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!U" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Открыт";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!P" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Приостановлен";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!R" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Запущен";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!B" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Перезагрузка";
            *((double*)NewListItem->Data)=0.;
        } else if ( line=="!S" )
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=" ! Выключен";
            *((double*)NewListItem->Data)=0.;
        } else
        {
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            SubItems->Strings[2]=Event->TariffName;
            int h=Event->TimeSize/60, m=Event->TimeSize%60;
            SubItems->Strings[3]=IntToStr(h)+" час. "+IntToStr(m/10)+IntToStr(m%10)+" мин.";
            SubItems->Strings[4]=FloatToStrF(Event->Cost,ffCurrency,8,2);
            *((double*)NewListItem->Data)=Event->Cost;
            PayTime+=Event->TimeSize; PayMany+=Event->Cost;
        }
    }
    //
    {
        int h=PayTime/60, m=PayTime%60;
        LabelPayTime->Caption=IntToStr(h)+" час. "+IntToStr(m/10)+IntToStr(m%10)+" мин.";
        LabelPayMany->Caption=FloatToStrF(PayMany,ffCurrency,8,2);
    }

error:
    delete Events;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ButtonSaveReportClick(TObject *Sender)
{
    if ( !SaveDialog->Execute() ) return;

    MemoReport->Lines->Add("Заведен: "+LabelOpenTime->Caption);
    MemoReport->Lines->Add("Закрыт:  "+LabelCloseTime->Caption);
    MemoReport->Lines->Add("Длительность: "+LabelLengthTime->Caption);
    MemoReport->Lines->Add("---------------------------------------------");
    MemoReport->Lines->Add("Имя: "+EditName->Text);
    MemoReport->Lines->Add("Должность: "+EditJob->Text);
    MemoReport->Lines->Add("---------------------------------------------");
    MemoReport->Lines->Add("Оплаченное время: "+LabelPayTime->Caption);
    MemoReport->Lines->Add("           Сумма: "+LabelPayMany->Caption);
    MemoReport->Lines->Add("---------------------------------------------");
    MemoReport->Lines->Add("  Сумма по файлу: "+LabelSelPayMany->Caption);
    MemoReport->Lines->Add("---------------- Начало лога ----------------");
    //
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewEvents->Selected; Item;
        Item=ListViewEvents->GetNextItem(Item,sdAll,is) )
    {
        TStrings *SubItems;
        AnsiString line;

        SubItems=Item->SubItems;
        line=SubItems->Strings[0];
        if ( SubItems->Strings[1]!="" ) line+="  №"+SubItems->Strings[1];
        if ( SubItems->Strings[2]!="" ) line+="  "+SubItems->Strings[2];
        if ( SubItems->Strings[3]!="" ) line+="  на "+SubItems->Strings[3];
        if ( SubItems->Strings[4]!="" ) line+="  за "+SubItems->Strings[4];
        MemoReport->Lines->Add(line);
    }
    //
    MemoReport->Lines->Add("---------------- Конец лога ----------------");
    MemoReport->Lines->SaveToFile(SaveDialog->FileName);
    MemoReport->Lines->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetListViewEventsLine(TListItem *Item_, MEvent *Event_)
{
    Item_->SubItems->Strings[0]=((TDateTime)Event_->Time).FormatString("dd - hh:nn:ss");
    Item_->SubItems->Strings[1]=IntToStr(Event_->ComputerNumber/10)+IntToStr(Event_->ComputerNumber%10);
    Item_->SubItems->Strings[2]=Event_->TariffName;
    int h=Event_->TimeSize/60, m=Event_->TimeSize%60;
    Item_->SubItems->Strings[3]=IntToStr(h)+" час. "+IntToStr(m/10)+IntToStr(m%10)+" мин.";
    Item_->SubItems->Strings[4]=FloatToStrF(Event_->Cost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewEventsInsert(TObject *Sender,
      TListItem *Item)
{
    Item->Data=(void*)new double;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewEventsDeletion(TObject *Sender,
      TListItem *Item)
{
    delete (double*)Item->Data;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewEventsColumnClick(TObject *Sender,
      TListColumn *Column)
{
    int ColumnNumber=Column->Index-1;
    for ( int i=0, j=ListViewEvents->Items->Count; i<j; i++ )
    {
        TListItem *Item;
        Item=ListViewEvents->Items->Item[i];
        Item->Caption=Item->SubItems->Strings[ColumnNumber];
    }
    ListViewEvents->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewEventsSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    double SelMany=0.;
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewEvents->Selected; Item;
        Item=ListViewEvents->GetNextItem(Item,sdAll,is) ) SelMany+=*((double*)Item->Data);
    LabelSelPayMany->Caption=FloatToStrF(SelMany,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

