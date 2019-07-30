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

    ListViewFiles->Visible=true;
    ListViewFiles->Height=PanelLog->Height*2/3;
    ButtonShowFiles->Enabled=true;

    ShowFiles(ExtractFilePath(OpenDialog->FileName));

    if ( !ShowLog(OpenDialog->FileName) )
    {
        ::MessageBox(Application->Handle, "Файл лога поврежден или к нему нет доступа !",
            "LogViewer - ошибка",MB_OK|MB_ICONERROR|MB_APPLMODAL);
    }
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
void __fastcall TFormMain::ButtonShowFilesClick(TObject *Sender)
{
    ListViewFiles->Visible=!ListViewFiles->Visible;
    if ( ListViewFiles->Visible ) ListViewFiles->Height=PanelLog->Height*2/3;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewEventsInsert(TObject *Sender,
      TListItem *Item)
{
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
        Item=ListViewEvents->GetNextItem(Item,sdAll,is) )
        if ( Item->Data!=NULL ) SelMany+=*((double*)Item->Data);
    LabelSelPayMany->Caption=FloatToStrF(SelMany,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
bool TFormMain::ShowFiles(AnsiString FilePath_)
{
    HANDLE hFindFile;
    WIN32_FIND_DATA FindData;
    MEvents Events;

    ListViewFiles->Items->Clear();

    hFindFile=::FindFirstFile((FilePath_+"*.TLG").c_str(),&FindData);
    if ( hFindFile==INVALID_HANDLE_VALUE ) return false;
    do
    {
        // Берем информацию о файле
        if ( !Events.LoadInfo(FindData.cFileName) ) continue;
        // Заносим файл в список для просмотра
        TListItem *NewItem;
        NewItem=ListViewFiles->Items->Add();
        NewItem->SubItems->Add(FindData.cFileName);
        NewItem->SubItems->Add(((TDateTime)Events.OpenTime).FormatString("dd mmm yyyy (ddd) - hh:nn"));
        NewItem->SubItems->Add(Events.Name);
    } while(FindNextFile(hFindFile,&FindData));
    ::FindClose(hFindFile);

    return true;
}
//---------------------------------------------------------------------------
bool TFormMain::ShowLog(AnsiString FileName_)
{
    int PayTime=0;
    double PayMany=0.;

    MEvents *Events=new MEvents;
    if ( Events==NULL ) goto error;
    if ( !Events->Load(FileName_) ) goto error;
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

        // Дата и время события
        SubItems->Strings[0]=((TDateTime)Event->Time).FormatString("dd - hh:nn:ss");
        // Служебные события, не привязанные к номеру компьютера
        if ( line=="!r" ) SubItems->Strings[2]=" ! Программа запущена";
        else if ( line=="!c" ) SubItems->Strings[2]=" ! Программа закрыта";
        else if ( line=="!s" ) SubItems->Strings[2]=" ! В программу вошел сисадмин";
        else if ( line=="!u" ) SubItems->Strings[2]=" ! Сисадмин вышел или вошел другой пользователь";
        else if ( line=="!t" ) SubItems->Strings[2]=" ! Изменения в тарифах";
        else if ( line=="!g" ) SubItems->Strings[2]=" ! Изменение списка игр";
        else
        {
            // События, привязанные к номеру компьютера, но не связанные с финансами
            SubItems->Strings[1]=IntToStr(Event->ComputerNumber/10)+
                IntToStr(Event->ComputerNumber%10);
            if ( line=="!o" ) SubItems->Strings[2]=" ! Пересадка ==> (компьютер закрыт)";
            else if ( line=="!i" ) SubItems->Strings[2]=" ! Пересадка <== (компьютер открыт)";
            else if ( line=="!L" ) SubItems->Strings[2]=" ! Прикрыт";
            else if ( line=="!U" ) SubItems->Strings[2]=" ! Открыт";
            else if ( line=="!P" ) SubItems->Strings[2]=" ! Время приостановлено";
            else if ( line=="!R" ) SubItems->Strings[2]=" ! Время запущено";
            else if ( line=="!B" ) SubItems->Strings[2]=" ! Перезагрузка";
            else if ( line=="!S" ) SubItems->Strings[2]=" ! Выключен";
            else if ( line=="!a" ) SubItems->Strings[2]=" ! Открыт для настройки";
            else if ( line=="!n" ) SubItems->Strings[2]=" ! Закрыт после настройки";
            else if ( line=="!w" ) SubItems->Strings[2]=" ! Отключение программы";
            else if ( line=="!l" ) SubItems->Strings[2]=" ! Включение программы";
            else if ( line=="!f" )
            {
                SubItems->Strings[2]=" ! Штраф";
                if ( Event->TimeSize<0 ) SubItems->Strings[3]=IntToStr(-Event->TimeSize)+" мин.";
                else if ( Event->TimeSize==0 ) SubItems->Strings[3]="Все время";
                else SubItems->Strings[3]="Ожидание  "+IntToStr(Event->TimeSize)+" мин.";
            } else
            {
                // События, связанные с финансами
                NewListItem->Data=(void*)new double;
                *((double*)NewListItem->Data)=Event->Cost;
                if ( line=="!*" )
                {
                    SubItems->Strings[2]=" ! Запуск компьютера под сисадмином";
                    int h=Event->TimeSize/60, m=Event->TimeSize%60;
                    SubItems->Strings[3]=IntToStr(h)+" час. "+IntToStr(m/10)+IntToStr(m%10)+" мин.";
                    SubItems->Strings[4]=FloatToStrF(Event->Cost,ffCurrency,8,2);
                } else
                {
                    SubItems->Strings[2]=Event->TariffName;
                    int h=Event->TimeSize/60, m=Event->TimeSize%60;
                    SubItems->Strings[3]=IntToStr(h)+" час. "+IntToStr(m/10)+IntToStr(m%10)+" мин.";
                    SubItems->Strings[4]=FloatToStrF(Event->Cost,ffCurrency,8,2);
                    PayTime+=Event->TimeSize; PayMany+=Event->Cost;
                }
            }
        }
    }
    //
    {
        int h=PayTime/60, m=PayTime%60;
        LabelPayTime->Caption=IntToStr(h)+" час. "+IntToStr(m/10)+IntToStr(m%10)+" мин.";
        LabelPayMany->Caption=FloatToStrF(PayMany,ffCurrency,8,2);
    }

    delete Events;
    return true;
error:
    delete Events;
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewFilesSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( (((TListView*)Sender)->SelCount!=1)||(!Selected) ) return;
    if ( !ShowLog(Item->SubItems->Strings[0]) )
    {
        AnsiString Message;
        Message+="Не удалось открыть файл лога ";
        Message+=Item->SubItems->Strings[0];
        Message+=" !\nВозможно, он был перемещен в другое место или носитель с файлом сейчас недоступен.";
        ::MessageBox(Application->Handle, Message.c_str(),
            "LogViewer - ошибка",MB_OK|MB_ICONERROR|MB_APPLMODAL);
    }
}
//---------------------------------------------------------------------------

