//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormOrder.h"
#include "UnitFormMain.h"
#include "UnitTariffs.h"
#include "UnitOptionsLoadSave.h"
#include "UnitCommon.h"
#include "UnitLogs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormOrder *FormOrder;
//---------------------------------------------------------------------------
__fastcall TFormOrder::TFormOrder(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::FormShow(TObject *Sender)
{
    TariffsForComputers=new MListOfTariffs;
    TimesForTariffHours=new MListOfTariffTimes;
    TimesForTariffPackets=new MListOfTariffTimes;
    CurrentOrder=new MOrderComputer;
    OrderCost=0.;

    // Сохраняем время открытия диалога
    TDate CurrentDateTime=TDate::CurrentDateTime();
    switch(CurrentDateTime.DayOfWeek())
    {
        case 1: DayOfWeek1=mwdSa; DayOfWeek2=mwdSu; DayOfWeek3=mwdMo; break;
        case 2: DayOfWeek1=mwdSu; DayOfWeek2=mwdMo; DayOfWeek3=mwdTu; break;
        case 3: DayOfWeek1=mwdMo; DayOfWeek2=mwdTu; DayOfWeek3=mwdWe; break;
        case 4: DayOfWeek1=mwdTu; DayOfWeek2=mwdWe; DayOfWeek3=mwdTh; break;
        case 5: DayOfWeek1=mwdWe; DayOfWeek2=mwdTh; DayOfWeek3=mwdFr; break;
        case 6: DayOfWeek1=mwdTh; DayOfWeek2=mwdFr; DayOfWeek3=mwdSa; break;
        case 7: DayOfWeek1=mwdFr; DayOfWeek2=mwdSa; DayOfWeek3=mwdSu; break;
        default: DayOfWeek1=DayOfWeek2=DayOfWeek3=0; break;
    }
    unsigned short h, m, a;
    CurrentDateTime.DecodeTime(&h,&m,&a,&a);
    MinutesFromBeginDay=h*60+m;

    // Заносим выбранные компьютеры в список для оформления заказа
    ListViewComputersForOrder->Items->Clear();
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=FormMain->ListViewComputers->Selected; Item;
        Item=FormMain->ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MComputer *Computer;
        Computer=(MComputer*)Item->Data;
        if ( !(Computer->State&mcsFree) ) continue;
        TListItem *NewItem;
        NewItem=ListViewComputersForOrder->Items->Add();
        NewItem->Data=(void*)Computer;
        NewItem->Selected=true;
        SetListViewComputersForOrderLine(NewItem);
    }

    // Мелкие настройки
//    ComboBoxTariffTime->Items->Clear();
//    ComboBoxTariffTime->Text="";
//    ListViewOrderComputers->Items->Clear();
    ComboBoxTariffTime->Style=csDropDownList;
    ProgressBarDialogUseTime->Position=ProgressBarDialogUseTime->Min;
    ListViewOrderComputers->Tag=false;
    RadioButtonCash->Checked=true;
    ActiveControl=ListViewComputersForOrder;
    TimerDialogLock->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::FormHide(TObject *Sender)
{
    TimerDialogLock->Enabled=false;
    ListViewOrderComputers->Items->Clear();
    ListViewComputersForOrder->Items->Clear();
    ComboBoxTariffName->Items->Clear();
    ComboBoxTariffTime->Items->Clear();
    ComboBoxTariffTime->Text="";
    delete TariffsForComputers;
    delete TimesForTariffHours;
    delete TimesForTariffPackets;
    delete CurrentOrder;
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::TimerDialogLockTimer(TObject *Sender)
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
void __fastcall TFormOrder::ListViewComputersForOrderEnter(TObject *Sender)
{
    ListViewOrderComputers->Selected=NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ListViewComputersForOrderSelectItem(
      TObject *Sender, TListItem *Item, bool Selected)
{
    // Заполняем список тарифов, пригодных для всех выбранных компьютеров,
    CreateTariffsForComputersList(ListViewComputersForOrder);
    // Заполняем заново диалог выбора тарифа и очищаем диалог выбора времени
    CreateTariffsNamesList();
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ListViewOrderComputersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->Data=(void*)new MOrderComputer;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ListViewOrderComputersDeletion(TObject *Sender,
      TListItem *Item)
{
    delete (MOrderComputer*)Item->Data;
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ListViewOrderComputersEnter(TObject *Sender)
{
    ListViewComputersForOrder->Selected=NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ListViewOrderComputersSelectItem(
      TObject *Sender, TListItem *Item, bool Selected)
{
    // Заполняем список тарифов, пригодных для всех выбранных компьютеров
    CreateTariffsForComputersList(ListViewOrderComputers);
    // Заполняем заново диалог выбора тарифа и очищаем диалог выбора времени
    CreateTariffsNamesList();
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::RadioButtonCashClick(TObject *Sender)
{
    if ( Sender==RadioButtonCash )
    {
        SetGroupStateCash(true);
        SetGroupStateDesc(false);
        SetGroupStateSubs(false);
    } else if ( Sender==RadioButtonDesc )
    {
        SetGroupStateCash(false);
        SetGroupStateDesc(true);
        SetGroupStateSubs(false);
    } else if ( Sender==RadioButtonSubs )
    {
        SetGroupStateCash(false);
        SetGroupStateDesc(false);
        SetGroupStateSubs(true);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ComboBoxTariffNameChange(TObject *Sender)
{
    int ItemIndex;
    if ( (ItemIndex=ComboBoxTariffName->ItemIndex)<0 ) return;
    CurrentOrder->TimeWorkSize=0;
    CurrentOrder->Discount=0;
    CurrentOrder->Cost=0.;
    CurrentOrder->Tariff=((MListOfTariffsTariff*)TariffsForComputers->Item(ItemIndex))->Tariff;
    // Заполняем списки времен, чтобы можно было их использовать с тарифом
    CreateTimesForTariffList();
    // Заполняем заново диалог выбора времени по тарифу
    CreateTariffsTimeTypesList();
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ComboBoxTariffTimeChange(TObject *Sender)
{
    int ItemIndex, SizeTime;
    // Проверяем время вводится вручную или взято из списка
    if ( (ItemIndex=ComboBoxTariffTime->ItemIndex)<0 )
    {
        int UserTime=0;
        // Определяем время, введенное пользователем (некорректные значения равны нулю)
        try
        {
            int pos, a;
            AnsiString line=ComboBoxTariffTime->Text;
            // Определяем часы
            if ( (pos=line.AnsiPos(TimeSeparator))>=2 )
            {
                a=StrToInt(line.SubString(1,pos-1));
                if ( (a>=0)&&(a<=24) ) UserTime=a*60;
            }
            // Определяем минуты
            if ( pos<line.Length() )
            {
                a=StrToInt(line.SubString(pos+1,line.Length()-pos));
                if ( (a>=0)&&(a<60) ) UserTime+=a;
                else UserTime=0;
            }
        } catch ( EConvertError *Error ) { UserTime=0; }
        // Окончательно проверяем корректность введенного пользователем времени
        if ( (UserTime<=0)||(UserTime>(24*60)) )
        {
            CurrentOrder->TimeWorkSize=0;
            CurrentOrder->Cost=0.;
            ComboBoxTariffTime->Color=(TColor)0x025A5AEF;
            LabelRealSizeTime->Caption="";
            LabelCost->Caption="";
            return;
        }
        // Для последующих проверок увеличиваем полученное значение на текущее время
        UserTime+=MinutesFromBeginDay;

        int TimeFromBeginDay=MinutesFromBeginDay;
        MTariffTime *TariffTime;
        CurrentOrder->Cost=0.;
        // Вычисляем стоимость заказа и определяем максимальное время работы
        while(true)
        {
            MListOfTariffTimesTariffTime *ListOfTariffTimesTariffTime;
            // Ищем подходящее время
            for ( ListOfTariffTimesTariffTime=(MListOfTariffTimesTariffTime*)
                TimesForTariffHours->FirstItem; ListOfTariffTimesTariffTime;
                ListOfTariffTimesTariffTime=(MListOfTariffTimesTariffTime*)
                ListOfTariffTimesTariffTime->NextItem )
            {
                TariffTime=ListOfTariffTimesTariffTime->TariffTime;
                // Тариф действует со вчерашнего дня и закончит сегодня
                if ( (TariffTime->WeekDays&DayOfWeek1)&&
                    ((SizeTime=TariffTime->EndTime-24*60-TimeFromBeginDay)>0) ) break;;
                // Тариф начнет действовать сегодня и закончит сегодня или завтра
                if ( (TariffTime->WeekDays&DayOfWeek2)&&
                    (TimeFromBeginDay>=TariffTime->BeginTime)&&
                    ((SizeTime=TariffTime->EndTime-TimeFromBeginDay)>0) ) break;;
                // Тариф начнет действовать завтра и закончит завтра или послезавтра
                if ( (TariffTime->WeekDays&DayOfWeek3)&&
                    (TimeFromBeginDay>=(TariffTime->BeginTime+24*60))&&
                    ((SizeTime=TariffTime->EndTime+24*60-TimeFromBeginDay)>0) ) break;
            }
            // Проверяем хватило ли тарифной сетки для указанного пользователем времени
            if ( !ListOfTariffTimesTariffTime )
            {
                SizeTime=TimeFromBeginDay-MinutesFromBeginDay;
                // Выводим максимально возможное время вместо введенного пользователем
                ComboBoxTariffTime->Color=(TColor)0x028CF4F4;
                ComboBoxTariffTime->Text=((TDate)(SizeTime/(24*60.))).FormatString("h:nn");
                break;
            }
            // Проверяем не будет ли превышено указанное пользователем время
            if ( (TimeFromBeginDay+SizeTime-UserTime)>=0 )
            {
                CurrentOrder->Cost+=(UserTime-TimeFromBeginDay)/60.*TariffTime->Cost;
                SizeTime=UserTime-MinutesFromBeginDay;
                ComboBoxTariffTime->Color=clWindow;
                // Прекращаем расчет
                break;
            } else
            {
                // Увеличиваем стоимость
                CurrentOrder->Cost+=SizeTime/60.*TariffTime->Cost;
                // Переносим время окончания работы
                TimeFromBeginDay+=SizeTime;
            }
        }
    } else
    {
        MTariffTime *TariffTime=((MListOfTariffTimesTariffTime*)TimesForTariffPackets->Item(ItemIndex))->TariffTime;
        // Определяем время, которое будет назначено для компьютеров
        if ( (TariffTime->WeekDays&DayOfWeek1)&&
            ((SizeTime=TariffTime->EndTime-24*60-MinutesFromBeginDay)>0) );
        else if ( (TariffTime->WeekDays&DayOfWeek2)&&  // Вторая проверка только для надежности
            (MinutesFromBeginDay>=TariffTime->BeginTime)&&
            ((SizeTime=TariffTime->EndTime-MinutesFromBeginDay)>0) );
        else SizeTime=0;
        // Если задан "плавающий" пакет и возможное время работы больше заданного
        // пользователем, то учитываем только заданное время
        if ( (TariffTime->SizeTime)&&(TariffTime->SizeTime<SizeTime) )
        {
            SizeTime=TariffTime->SizeTime;
            ComboBoxTariffTime->Color=clWindow;
        } else ComboBoxTariffTime->Color=(TColor)0x028CF4F4;
        // Стоимость не зависит от того, меньше ли время максимально возможного по тарифу
        CurrentOrder->Cost=TariffTime->Cost;
    }
    // Округляем стоимость
    {
        int a=(int)((CurrentOrder->Cost-(int)CurrentOrder->Cost)*100.);
        CurrentOrder->Cost=(int)CurrentOrder->Cost;
        if ( a>=50 ) CurrentOrder->Cost+=0.50;
        else if ( (a!=0)&&(CurrentOrder->Cost<0.50) ) CurrentOrder->Cost=0.50;
    }
    // Задаем время работы
    CurrentOrder->TimeWorkSize=SizeTime;
    //
    {
        int h=SizeTime/60, m=SizeTime%60;
        AnsiString line;
        if ( h ) line=IntToStr(h)+" час. ";
        line+=IntToStr(m/10)+IntToStr(m%10)+" мин.";
        LabelRealSizeTime->Caption=line;
    }
    // Выводим стоимость для всех выбранных компьютеров, а не для одного
    if ( ListViewOrderComputers->Selected ) LabelCost->Caption=FloatToStrF(CurrentOrder->Cost*
        ListViewOrderComputers->SelCount,ffCurrency,8,2);
    else LabelCost->Caption=FloatToStrF(CurrentOrder->Cost*
        ListViewComputersForOrder->SelCount,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ButtonAddClick(TObject *Sender)
{
    if ( !CurrentOrder->TimeWorkSize ) return;
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputersForOrder->Selected, *NewItem; Item; )
    {
        // Добавляем очередной компьютер в список для запуска
        TListItem *NewItem;
        NewItem=ListViewOrderComputers->Items->Add();
        CurrentOrder->Computer=(MComputer*)Item->Data;
        *((MOrderComputer*)NewItem->Data)=*CurrentOrder;
        SetListViewOrderComputersLine(NewItem); NewItem->Selected=true;
        // Увеличиваем общую стоимость заказа
        OrderCost+=CurrentOrder->Cost;
        // Удаляем компьютер из списка для оформления заказа
        NewItem=ListViewComputersForOrder->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
    //
    ComboBoxTariffName->Items->Clear();
    ComboBoxTariffTime->Items->Clear();
    TariffsForComputers->Clear();
    TimesForTariffHours->Clear();
    TimesForTariffPackets->Clear();
    CurrentOrder->TimeWorkSize=0;
    // Обновляем информацию о стоимости всего заказа
    LabelOrderCost->Caption=FloatToStrF(OrderCost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ButtonChangeClick(TObject *Sender)
{
    if ( !CurrentOrder->TimeWorkSize ) return;
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewOrderComputers->Selected; Item;
        Item=ListViewOrderComputers->GetNextItem(Item,sdAll,is) )
    {
        MOrderComputer *OrderComputer;
        OrderComputer=(MOrderComputer*)Item->Data;
        OrderCost-=OrderComputer->Cost;
        MComputer *Computer;
        Computer=OrderComputer->Computer;
        *OrderComputer=*CurrentOrder;
        OrderComputer->Computer=Computer;
        SetListViewOrderComputersLine(Item);
        OrderCost+=OrderComputer->Cost;
    }
    //
    ComboBoxTariffName->Items->Clear();
    ComboBoxTariffTime->Items->Clear();
    TariffsForComputers->Clear();
    TimesForTariffHours->Clear();
    TimesForTariffPackets->Clear();
    CurrentOrder->TimeWorkSize=0;
    //
    LabelOrderCost->Caption=FloatToStrF(OrderCost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ButtonDeleteClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewOrderComputers->Selected, *NewItem; Item; )
    {
        MOrderComputer *OrderComputer;
        OrderComputer=(MOrderComputer*)Item->Data;
        // Добавляем очередной компьютер в список для оформления заказа
        NewItem=ListViewComputersForOrder->Items->Add();
        NewItem->Data=(void*)OrderComputer->Computer;
        SetListViewComputersForOrderLine(NewItem);
        // Уменьшаем общую стоимость заказа
        OrderCost-=OrderComputer->Cost;
        // Удаляем компьютер из списка оформленных
        NewItem=ListViewOrderComputers->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
    // Обновляем информацию о стоимости всего заказа
    LabelOrderCost->Caption=FloatToStrF(OrderCost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::ButtonRunClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewOrderComputers->Selected, *NewItem; Item; )
    {
        MOrderComputer *OrderComputer;
        OrderComputer=(MOrderComputer*)Item->Data;
        // Добавляем запись в лог
        if ( (State->CurrentUser!=NULL)&&(!LogCompRun(OrderComputer->Computer,
            OrderComputer->Tariff, OrderComputer->TimeWorkSize,OrderComputer->Cost)) )
        {
            Item=ListViewOrderComputers->GetNextItem(Item,sdAll,is); continue;
        } else if ( (State->CurrentUser==NULL)&&(State->CurrentAdmin!=NULL) )
            LogCompRun(OrderComputer->Computer,(MTariff*)0,
            OrderComputer->TimeWorkSize,OrderComputer->Cost);
        // Запускаем компьютер
        OrderComputer->Computer->CmdRun(OrderComputer->Tariff->ID,
            OrderComputer->Tariff->GamesPages,OrderComputer->TimeWorkSize);
        // Если по тарифу его нужно перезагрузить, то делаем это
        if ( OrderComputer->Tariff->Reboot ) OrderComputer->Computer->CmdReboot();
        // Уменьшаем общую стоимость заказа
        OrderCost-=OrderComputer->Cost;
        // Удаляем компьютер из списка оформленных
        NewItem=ListViewOrderComputers->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
    // Обновляем информацию о стоимости всего заказа
    LabelOrderCost->Caption=FloatToStrF(OrderCost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::SetGroupStateCash(bool State_)
{
    bool enabled;
    TColor color;

    if ( State_ ) { enabled=true; color=clWindow; }
    else { enabled=false; color=clBtnFace; }

    LabelCostText->Enabled=enabled;
    LabelCost->Enabled=enabled;
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::SetGroupStateDesc(bool State_)
{
    bool enabled;
    TColor color;

    if ( State_ ) { enabled=true; color=clWindow; }
    else { enabled=false; color=clBtnFace; }

    ComboBoxDescNumber->Enabled=enabled;
    ComboBoxDescNumber->Color=color;
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::SetGroupStateSubs(bool State_)
{
    bool enabled;
    TColor color;

    if ( State_ ) { enabled=true; color=clWindow; }
    else { enabled=false; color=clBtnFace; }

    LabelSubscriptionNumberText->Enabled=enabled;
    EditSubscriptionNumber->Enabled=enabled;
    EditSubscriptionNumber->Color=color;
    LabelRemaindTimeText->Enabled=enabled;
    LabelRemaindTime->Enabled=enabled;
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::SetListViewComputersForOrderLine(TListItem *Item_)
{
    MComputer *Computer_=(MComputer*)Item_->Data;
    Item_->ImageIndex=Computer_->GroupColor;
    Item_->Caption=IntToStr(Computer_->Number);
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::SetListViewOrderComputersLine(TListItem *Item_)
{
    MOrderComputer *OrderComputer_=(MOrderComputer*)Item_->Data;
    AnsiString line;
    Item_->SubItems->Strings[0]=IntToStr(OrderComputer_->Computer->Number);
    Item_->SubItemImages[0]=OrderComputer_->Computer->GroupColor;
    Item_->SubItems->Strings[1]=OrderComputer_->Tariff->Name;
    Item_->SubItemImages[1]=OrderComputer_->Tariff->IconNumber;
    int h=OrderComputer_->TimeWorkSize/60, m=OrderComputer_->TimeWorkSize%60;
    if (h) line+=IntToStr(h)+" час. ";
    line+=IntToStr(m/10)+IntToStr(m%10)+" мин.";
    Item_->SubItems->Strings[2]=line;
    if ( OrderComputer_->Discount ) line=IntToStr(OrderComputer_->Discount);
    else line="";
    Item_->SubItems->Strings[3]=line;
    Item_->SubItems->Strings[4]=FloatToStrF(OrderComputer_->Cost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::CreateTariffsForComputersList(TListView *ListView_)
{
    MComputer **Computers;
    int Count=0;

    if ( ListView_->SelCount==0 ) { TariffsForComputers->Clear(); return; }

    // Создаем массив с указателями на выбранные компьютеры
    Computers=new MComputer*[ListView_->SelCount];
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListView_->Selected; Item;
        Item=ListView_->GetNextItem(Item,sdAll,is) )
    {
        if ( ListView_->Name=="ListViewComputersForOrder" )
            Computers[Count]=(MComputer*)Item->Data;
        else if ( ListView_->Name=="ListViewOrderComputers" )
            Computers[Count]=((MOrderComputer*)Item->Data)->Computer;
        else Computers[Count]=NULL;
        Count++;
    }

    TariffsForComputers->Clear();
    // Определяем пригодные для использования с выбранными компьютерами тарифы и заносим их в список
    for ( MTariff *Tariff=(MTariff*)Tariffs->FirstItem; Tariff; Tariff=(MTariff*)Tariff->NextItem )
    {
        MListOfTariffsTariff *ListOfTariffsTariff;
        // Проверяем для всех ли компьютеров применим этот тариф
        for ( int i=0; i<Count; i++ ) if ( !Tariff->Computers.SearchID(Computers[i]->ID) ) goto next;
        // Если тариф подходит, то заносим его в список
        ListOfTariffsTariff=(MListOfTariffsTariff*)TariffsForComputers->Add();
        ListOfTariffsTariff->Tariff=Tariff;
next:
    }

    delete[] Computers;
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::CreateTariffsNamesList()
{
    // Очищаем далог имен тарифов и типов по времени для них
    ComboBoxTariffName->Items->Clear();
    ComboBoxTariffTime->Items->Clear();
    ComboBoxTariffTime->Text="";
    ComboBoxTariffTime->Color=clWindow;
    LabelCost->Caption="";
    // Заполняем далог выбора тарифов
    for ( MListOfTariffsTariff *ListOfTariffsTariff=
        (MListOfTariffsTariff*)TariffsForComputers->FirstItem; ListOfTariffsTariff;
        ListOfTariffsTariff=(MListOfTariffsTariff*)ListOfTariffsTariff->NextItem )
    {
        ComboBoxTariffName->Items->Add(ListOfTariffsTariff->Tariff->Name);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::CreateTimesForTariffList()
{
    TimesForTariffHours->Clear();
    TimesForTariffPackets->Clear();
    // Определяем какое время можно использовать с выбранным тарифом
    for ( MTariffTime *TariffTime=(MTariffTime*)CurrentOrder->Tariff->Times.FirstItem;
        TariffTime; TariffTime=(MTariffTime*)TariffTime->NextItem )
    {
        MListOfTariffTimesTariffTime *ListOfTariffTimesTariffTime;
        // Для пакетных тарифов жесткая проверка
        if ( TariffTime->SizeTime>=0 )
        {
            if ( TariffTime->EndTime>=(24*60) )
            {
                // Тариф начнет действовать сегодня и закончит завтра
                if ( (TariffTime->WeekDays&DayOfWeek2)&&
                    (MinutesFromBeginDay>=TariffTime->BeginTime) ) goto add;
                // Тариф продолжает действовать со вчерашнего дня
                if ( (TariffTime->WeekDays&DayOfWeek1)&&
                    (MinutesFromBeginDay<(TariffTime->EndTime-24*60)) ) goto add;
            } else
            {
                // Тариф действует только сегодня (без перехода между сутками)
                if ( (TariffTime->WeekDays&DayOfWeek2)&&
                    (MinutesFromBeginDay>=TariffTime->BeginTime)&&
                    (MinutesFromBeginDay<TariffTime->EndTime) ) goto add;
            }
        } else
        {
            // Добавляем тип тарифа по времени (один из почасовых)
            ListOfTariffTimesTariffTime=(MListOfTariffTimesTariffTime*)TimesForTariffHours->Add();
            ListOfTariffTimesTariffTime->TariffTime=TariffTime;
        }
        continue;
add:
        // Добавляем тип тарифа по времени (один из пакетных)
        ListOfTariffTimesTariffTime=(MListOfTariffTimesTariffTime*)TimesForTariffPackets->Add();
        ListOfTariffTimesTariffTime->TariffTime=TariffTime;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormOrder::CreateTariffsTimeTypesList()
{
    ComboBoxTariffTime->Items->Clear();
    ComboBoxTariffTime->Text="";
    ComboBoxTariffTime->Color=clWindow;
    if ( TimesForTariffHours->CountItems )
    {
        if ( TimesForTariffPackets->CountItems ) ComboBoxTariffTime->Style=csDropDown;
        else ComboBoxTariffTime->Style=csSimple;
    } else ComboBoxTariffTime->Style=csDropDownList;
    LabelCost->Caption="";
    for ( MListOfTariffTimesTariffTime *ListOfTariffTimesTariffTime=
        (MListOfTariffTimesTariffTime*)TimesForTariffPackets->FirstItem;
        ListOfTariffTimesTariffTime; ListOfTariffTimesTariffTime=
        (MListOfTariffTimesTariffTime*)ListOfTariffTimesTariffTime->NextItem )
    {
        MTariffTime *TariffTime;
        TariffTime=ListOfTariffTimesTariffTime->TariffTime;
        // Заносим в диалог выбора времени
        if ( TariffTime->SizeTime )
        {
            // "Плавающий" пакетный
            int h=TariffTime->SizeTime/60, m=TariffTime->SizeTime%60;
            AnsiString line;
            if (h) line+=IntToStr(h)+" час. ";
            line+=IntToStr(m/10)+IntToStr(m%10)+" мин.";
            ComboBoxTariffTime->Items->Add(line);
        } else
        {
            // Фиксированный пакетный
            ComboBoxTariffTime->Items->Add( ((TDate)(TariffTime->BeginTime/(24.*60)))
                .FormatString("С h:nn до ")+((TDate)(TariffTime->EndTime/(24.*60)))
                .FormatString("h:nn") );
        }
    }
    LabelCost->Caption="";
}
//---------------------------------------------------------------------------

