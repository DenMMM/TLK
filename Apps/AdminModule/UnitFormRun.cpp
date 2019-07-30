//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdexcept>
#pragma hdrstop

#include "UnitFormRun.h"
#include "UnitFormMain.h"
#include "UnitStates.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TFormRun *FormRun;
//---------------------------------------------------------------------------
__fastcall TFormRun::TFormRun(TComponent* Owner)
    : TForm(Owner)
{
    RunMode=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::FormShow(TObject *Sender)
{
    SelTariffID=0;
    CompTimes.Clear();
    UseTimes.Clear();
    UseTariffs.Clear();

    // Заполним список компьютеров и подходящих для них тарифов
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *item=FormMain->ListViewComputers->Selected; item;
        item=FormMain->ListViewComputers->GetNextItem(item,sdAll,is) )
    {
        MState *state=(MState*)item->Data;
        MRunTime tmptime;
        // Запрашиваем параметры для запуска/добавления
        state->RunParam(&tmptime);
        // Проверяем можно ли поставить/добавить время
        if ( tmptime.MaxTime==0 ) continue;
        // Дополнительные проверки, специфичные для режима использования диалога
        if ( RunMode )
        {
            // Проверяем не выставлен ли для компьютера тариф
            if ( tmptime.TariffID!=0 ) continue;
        } else
        {
            // Проверяем существует ли еще тариф, по которому был запущен компьютер,
            // и применим ли до сих пор этот тариф к компьютеру
            MTariff *tariff=(MTariff*)Tariffs->SrchID(tmptime.TariffID);
            if ( (tariff==NULL)||(!tariff->CheckForComp(tmptime.Number)) ) continue;
            // Добавляем тариф в список для использования, если он еще не добавлен
            if ( !UseTariffs.Search(tmptime.TariffID) )
            {
                MTariffInfo *info=(MTariffInfo*)UseTariffs.Add();
                tariff->GetInfo(info);
            }
        }

        // Копируем параметры запуска/добавления
        MRunTime *time=(MRunTime*)CompTimes.Add();
        time->Copy(&tmptime);
        // Добавляем строку в список компьютеров и связываем с параметрами
        TListItem *newitem=ListViewComputers->Items->Add();
        newitem->Data=time;
        // Для удобства сразу выделяем компьютеры в таблице
        if ( RunMode ) newitem->Selected=true;
        SetListViewComputersLine(newitem);
    }

    if ( RunMode )
    {
        // Определяем абсолютное время открытия диалога
        if ( RunMode ) GetLocalTimeInt64(&OpenDialogTime);
        // Берем список тарифов, пригодных для использования в это время
        Tariffs->GetForTime(OpenDialogTime,&UseTariffs);
    }

    // Заносим в список тарифы, которые можно использовать в это время
    // или с выбранными компьютерами (при добавлении времени)
    for ( MTariffInfo *Info=(MTariffInfo*)UseTariffs.gFirst();
        Info; Info=(MTariffInfo*)Info->gNext() )
        ComboBoxTariff->Items->Add(Info->Name);

    // Корректируем интерфейс в соответствии с режимом работы диалога
    if ( RunMode )
    {
        Caption="Запуск компьютеров";
        ComboBoxTariff->Enabled=true;
        BitBtnRun->Caption="Запустить";
    } else
    {
        Caption="Добавление времени";
        ComboBoxTariff->Enabled=false;
        BitBtnRun->Caption="Добавить";
    }

    // Блокируем поле ввода времени до первого выбора тарифа
    ComboBoxTime->Enabled=false;
    ComboBoxTime->Color=clBtnFace;
    ComboBoxTime->Style=csSimple;

    // Инициализируем таймер пользования диалогом
    ProgressBarTime->Min=0;
    ProgressBarTime->Max=Options->CostDialogTime*60;
    ProgressBarTime->Position=0;
    TimerDialogLock->Interval=1000;
    TimerDialogLock->Enabled=true;
    BitBtnRun->Enabled=true;

    ActiveControl=RunMode?(TWinControl*)ComboBoxTariff:
        (TWinControl*)ListViewComputers;
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::FormClose(TObject *Sender, TCloseAction &Action)
{
    TimerDialogLock->Enabled=false;
    // Чистим интерфейсные элементы
    ListViewComputers->Items->Clear();
    ComboBoxTariff->Clear();
    ComboBoxTime->Clear();
    LabelAllCost->Caption="";
    // Очищаем буферы
    CompTimes.Clear();
    UseTariffs.Clear();
    UseTimes.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::TimerDialogLockTimer(TObject *Sender)
{
    if ( ProgressBarTime->Position<ProgressBarTime->Max )
        ProgressBarTime->Position++;
    else
    {
        TimerDialogLock->Enabled=false;
        BitBtnRun->Enabled=false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ListViewComputersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ListViewComputersSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( RunMode||(!Selected)||(ListViewComputers->Selected==NULL) ) return;

    MRunTime *Time=(MRunTime*)ListViewComputers->Selected->Data;
    int Num=0;

    for ( MTariffInfo *Info=(MTariffInfo*)UseTariffs.gFirst();
        Info; Info=(MTariffInfo*)Info->gNext() )
    {
        if ( Info->ID==Time->TariffID )
        {
            OpenDialogTime=Time->StartTime;
            SelTariffID=Time->TariffID;
            ComboBoxTariff->ItemIndex=Num;
            ComboBoxTariffClick(NULL);
            break;
        }
        Num++;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ComboBoxTariffClick(TObject *Sender)
{
    MTariff *Tariff;
    MRunTime *Time;
    char line[30];

    // Определяем какой тариф был выбран
    if ( RunMode ) SelTariffID=((MTariffInfo*)UseTariffs.
        Item(ComboBoxTariff->ItemIndex))->ID;
    Tariff=(MTariff*)Tariffs->SrchID(SelTariffID);
    // Задаем для выбранных компьютеров тариф, если он применим к ним
    if ( RunMode )
    {
        TItemStates is=TItemStates()<<isSelected;
        for ( TListItem *Item=ListViewComputers->Selected; Item;
            Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
        {
            Time=(MRunTime*)Item->Data;
            if ( !Tariff->CheckForComp(Time->Number) ) continue;
            if ( Time->TariffID==SelTariffID ) continue;
            Time->TariffID=SelTariffID;
            Time->Type=mttUndefined;
            SetListViewComputersLine(Item);
        }
    }

    ComboBoxTime->Clear();
    // Запрашиваем для тарифа список пакетов
    Tariff->GetRunTimes(OpenDialogTime,&UseTimes);
    // Если пакетов для тарифа в это время нету, то запрещаем ввод времени
    Time=(MRunTime*)UseTimes.gFirst();
    if ( Time==NULL )
    {
        ComboBoxTime->Enabled=false;
        ComboBoxTime->Color=clBtnFace;
        ComboBoxTime->Style=csSimple;
        return;
    } else
    {
        ComboBoxTime->Enabled=true;
        ComboBoxTime->Color=clWindow;
    }
    // Позволяем ввод почасового времени
    if ( Time->Type==mttHours )
    {
        ComboBoxTime->Style=Time->gNext()?csDropDown:csSimple;
        UseTimes.Del(Time);
        Time=(MRunTime*)UseTimes.gFirst();
    } else ComboBoxTime->Style=csDropDownList;
    // Заносим в список пакеты
    while(Time)
    {
        switch(Time->Type)
        {
            case mttFlyPacket:
                sprintf(line,"На %i час. %.2i мин.",
                    Time->SizeTime/60,Time->SizeTime%60);
                break;
            case mttPacket:
                int EndTime;
                EndTime=Time->EndTime; if ( EndTime>=(24*60) ) EndTime-=24*60;
                sprintf(line,"С %i:%.2i до %i:%.2i",
                    Time->BeginTime/60,Time->BeginTime%60,EndTime/60,EndTime%60);
                break;
            default: *line=0; break;
        }
        ComboBoxTime->Items->Add(line);
        Time=(MRunTime*)Time->gNext();
    }

    // Обновляем сведения об общей стоимости заказа
    if ( RunMode ) UpdateFullCost();
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ComboBoxTimeClick(TObject *Sender)
{
    MRunTime *SelTime, *Time;
    MTariff *Tariff;

    Tariff=(MTariff*)Tariffs->SrchID(SelTariffID);
    SelTime=(MRunTime*)UseTimes.Item(ComboBoxTime->ItemIndex);

    ComboBoxTime->Color=clWindow;

    // Проставляем для выбранных компьютеров время и тариф, если он применим к ним
    // Рассчитываем стоимость
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        Time=(MRunTime*)Item->Data;
        if ( RunMode )
        {
            // Проверяем применим ли к компьютеру выбранный ранее тариф
            if ( !Tariff->CheckForComp(Time->Number) ) continue;
            //
            // Задаем для компьютера тариф
            Time->TariffID=SelTariffID;
        } else
        {
            // Проверяем применим ли к компьютеру выбранный ранее тариф
            if ( Time->TariffID!=SelTariffID ) continue;
        }
        // Задаем для компьютера параметры пакета
        Time->Type=SelTime->Type;
        Time->BeginTime=SelTime->BeginTime;
        Time->EndTime=SelTime->EndTime;
        Time->SizeTime=SelTime->SizeTime;
        // Рассчитываем время работы и стоимость
        Tariff->Cost(Time,Options->CostPrecision);
        //
        SetListViewComputersLine(Item);
    }

    // Обновляем сведения об общей стоимости заказа
    UpdateFullCost();
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ComboBoxTimeChange(TObject *Sender)
{
    char hours[10], *minutes;
    int h, m, time;
    MRunTime *SelTime, *Time;
    MTariff *Tariff;
    TItemStates is;

    if ( (ComboBoxTime->ItemIndex>=0)||
        ((Tariff=(MTariff*)Tariffs->SrchID(SelTariffID))==NULL) ) return;

    strcpy(hours,ComboBoxTime->Text.c_str());
    // Ищем в строке разделитель часов и минут
    minutes=strchr(hours,':');
    //
    if ( minutes==NULL )
    {
        // Разбираем время как заданное только в минутах (вплоть до суток)
        if ( sscanf(hours,"%d",&m)!=1 ) goto error; h=0;
    } else
    {
        // Делим строку на две и разбираем по-отдельности
        *(minutes++)=0;
        if ( (sscanf(hours,"%d",&h)!=1)||(h<0)||(h>24) ) goto error;
        if ( *minutes==0 ) m=0;
        else if( (sscanf(minutes,"%d",&m)!=1)||(m<0)||(m>59) ) goto error;
    }
    // Подсчитываем желаемое время работы и проверяем его на ограничения
    time=h*60+m; if ( (time<=0)||(time>24*60) ) goto error;

    ComboBoxTime->Color=clWindow;

    // Проставляем для выбранных компьютеров время и тариф, если он применим к ним
    // Рассчитываем стоимость
    is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        Time=(MRunTime*)Item->Data;
        if ( RunMode )
        {
            // Проверяем применим ли к компьютеру выбранный ранее тариф
            if ( !Tariff->CheckForComp(Time->Number) ) continue;
            //
            // Задаем для компьютера тариф
            Time->TariffID=SelTariffID;
        } else
        {
            // Проверяем применим ли к компьютеру выбранный ранее тариф
            if ( Time->TariffID!=SelTariffID ) continue;
        }
        // Задаем для компьютера параметры пакета
        Time->Type=mttHours;
        Time->SizeTime=time;
        // Рассчитываем время работы и стоимость
        Tariff->Cost(Time,Options->CostPrecision);
        //
        SetListViewComputersLine(Item);
    }

    // Обновляем сведения об общей стоимости заказа
    UpdateFullCost();

    return;
error:
    ComboBoxTime->Color=clRed;
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::BitBtnRunClick(TObject *Sender)
{
    //
    TItemStates is=TItemStates()<<isSelected;
    TListItem *item=ListViewComputers->Selected, *next;
    while(item)
    {
        MRunTime *time=(MRunTime*)item->Data;
        next=ListViewComputers->GetNextItem(item,sdAll,is);
        if ( time->TariffID&&(time->Type!=mttUndefined)&&time->WorkTime )
        {
            MState *state=States->Search(time->Number);     ///
            MTariff *tariff=(MTariff*)Tariffs->SrchID(time->TariffID);
            // Проверяем возможно ли применить команду
            if ( state->CmdRun(tariff,time,true) )
            {
                // Добавляем запись в лог
                if ( !Log->AddRun(time) )
                {
                    ShellState->State|=mssErrorLog; FormMain->SetShell();
                    ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
                    break;
                }
                // Применяем команду
                state->CmdRun(tariff,time,false);
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

    // Обновляем сведения об общей стоимости заказа
    UpdateFullCost();
}
//---------------------------------------------------------------------------
void TFormRun::SetListViewComputersLine(TListItem *Item_)
{
    MRunTime *RunTime=(MRunTime*)Item_->Data;
    TStrings *SubItems=Item_->SubItems;
    MComputer *Computer;
    char line[100];
    int icon;

    Computer=(MComputer*)Computers->Search(RunTime->Number);
    if ( Computer==NULL ) return;

    Item_->SubItemImages[0]=FormMain->GetCompColorIcon(Computer);
    sprintf(line,"%i",RunTime->Number);
    SubItems->Strings[0]=line;
    if ( RunTime->TariffID==0 )
    {
        SubItems->Strings[1]="";
        SubItems->Strings[2]="";
        SubItems->Strings[3]="";
    } else
    {
        SubItems->Strings[1]=UseTariffs.Search(RunTime->TariffID)->Name;
        if ( RunTime->Type==mttUndefined )
        {
            SubItems->Strings[2]="";
            SubItems->Strings[3]="";
        } else
        {
            if ( (RunTime->Type==mttHours)||(RunTime->Type==mttFlyPacket) )
                icon=RunTime->WorkTime<RunTime->SizeTime?17:-1;
            else icon =-1;
            Item_->SubItemImages[2]=icon;
            sprintf(line,"%i час. %.2i мин.",
                RunTime->WorkTime/60,RunTime->WorkTime%60);
            SubItems->Strings[2]=line;
            SubItems->Strings[3]=FloatToStrF(RunTime->Cost,ffCurrency,8,2);
        }
    }
}
//---------------------------------------------------------------------------
void TFormRun::UpdateFullCost()
{
    double cost=0.;
    MRunTime *time;

    for ( int i=0, j=ListViewComputers->Items->Count; i<j; i++ )
    {
        time=(MRunTime*)ListViewComputers->Items->Item[i]->Data;
        if ( time->TariffID&&
            (time->Type!=mttUndefined)&&time->WorkTime ) cost+=time->Cost;
    }

    LabelAllCost->Caption=FloatToStrF(cost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------

