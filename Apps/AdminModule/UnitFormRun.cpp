﻿//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdexcept>
#include <cwchar>
#pragma hdrstop

#include "UnitFormRun.h"
#include "UnitFormMain.h"
#include "UnitStates.h"
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
		auto &state=*reinterpret_cast<const MStatesItem*>(item->Data);
		// Запрашиваем параметры для запуска/добавления
		MTariffRunTimesItem tmptime=state.GetRunParam();
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
			auto iTariff=Tariffs->SrchUUID(tmptime.TariffID);
			if ( (iTariff==Tariffs->end())||
				(!iTariff->CheckForComp(tmptime.Number)) ) continue;
			// Добавляем тариф в список для использования, если он еще не добавлен
			if ( UseTariffs.Search(tmptime.TariffID)==UseTariffs.end() )
			{
				UseTariffs.Add()=iTariff->GetInfo();
			}
		}

		// Копируем параметры запуска/добавления
		MTariffRunTimesItem& time=CompTimes.Add();
		time=tmptime;
		// Добавляем строку в список компьютеров и связываем с параметрами
		TListItem *newitem=ListViewComputers->Items->Add();
		newitem->Data=&time;
		// Для удобства сразу выделяем компьютеры в таблице
		if ( RunMode ) newitem->Selected=true;
		SetListViewComputersLine(newitem);
	}

    if ( RunMode )
    {
        // Определяем абсолютное время открытия диалога
        if ( RunMode ) GetLocalTimeInt64(OpenDialogTime);
        // Берем список тарифов, пригодных для использования в это время
		UseTariffs=Tariffs->GetForTime(OpenDialogTime);
    }

    // Заносим в список тарифы, которые можно использовать в это время
	// или с выбранными компьютерами (при добавлении времени)
	for ( const auto &Info: UseTariffs )
	{
		ComboBoxTariff->Items->Add(Info.Name.c_str());
	}

    // Корректируем интерфейс в соответствии с режимом работы диалога
    if ( RunMode )
    {
		Caption=L"Запуск компьютеров";
		ComboBoxTariff->Enabled=true;
		BitBtnRun->Caption=L"Запустить";
	} else
	{
		Caption=L"Добавление времени";
		ComboBoxTariff->Enabled=false;
        BitBtnRun->Caption=L"Добавить";
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

	ActiveControl=
		RunMode?
		static_cast<TWinControl*>(ComboBoxTariff):
		static_cast<TWinControl*>(ListViewComputers);

	// Добавим энтропии
	BasicRand.event();
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::FormClose(TObject *Sender, TCloseAction &Action)
{
    TimerDialogLock->Enabled=false;
    // Чистим интерфейсные элементы
    ListViewComputers->Items->Clear();
    ComboBoxTariff->Clear();
    ComboBoxTime->Clear();
    LabelAllCost->Caption=L"";
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
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
    Item->SubItems->Add(L"");
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ListViewComputersSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( RunMode||(!Selected)||(ListViewComputers->Selected==nullptr) ) return;

	auto &Time=*reinterpret_cast<const MTariffRunTimesItem*>(
		ListViewComputers->Selected->Data);
	int Num=0;

	for ( const auto &Info: UseTariffs )
	{
		if ( Info.ID==Time.TariffID )
		{
			OpenDialogTime=Time.StartTime;
			SelTariffID=Time.TariffID;
			ComboBoxTariff->ItemIndex=Num;
			ComboBoxTariffClick(nullptr);
			break;
		}
		Num++;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ComboBoxTariffClick(TObject *Sender)
{
	// Определяем какой тариф был выбран
	if ( RunMode ) SelTariffID=
		UseTariffs.GetItem(ComboBoxTariff->ItemIndex).ID;

	auto iTariff=Tariffs->SrchUUID(SelTariffID);
	/// проверить 'iTariff==Tariffs->end()' ?

	// Задаем для выбранных компьютеров тариф, если он применим к ним
	if ( RunMode )
	{
		TItemStates is=TItemStates()<<isSelected;
		for ( TListItem *Item=ListViewComputers->Selected; Item;
			Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
		{
			auto &Time=*reinterpret_cast<MTariffRunTimesItem*>(Item->Data);
			if ( !iTariff->CheckForComp(Time.Number) ) continue;
			if ( Time.TariffID==SelTariffID ) continue;
			Time.TariffID=SelTariffID;
			Time.Type=mttUndefined;
			SetListViewComputersLine(Item);
		}
	}

	ComboBoxTime->Clear();

	// Запрашиваем для тарифа список пакетов
	UseTimes=iTariff->GetRunTimes(OpenDialogTime);

	// Если пакетов для тарифа в это время нету, то запрещаем ввод времени
	auto iTime=UseTimes.cbegin();
	auto iEnd=UseTimes.cend();

	if ( iTime==iEnd )
	{
		ComboBoxTime->Enabled=false;
		ComboBoxTime->Color=clBtnFace;
		ComboBoxTime->Style=csSimple;
		return;
	}
	ComboBoxTime->Enabled=true;
	ComboBoxTime->Color=clWindow;

	// Позволяем ввод почасового времени
	if ( iTime->Type==mttHours )
	{
		auto iTemp=iTime; ++iTemp;
		ComboBoxTime->Style=(iTemp!=iEnd)? csDropDown: csSimple;
		UseTimes.Del(iTime);
		iTime=UseTimes.begin();
	} else
	{
		ComboBoxTime->Style=csDropDownList;
	}
	// Заносим в список пакеты
	while ( iTime!=iEnd )
	{
		wchar_t line[30];

		switch( iTime->Type )
		{
			case mttFlyPacket:
				swprintf(
					line, sizeof(line)/sizeof(line[0]),
					L"На %i час. %.2i мин.",
					iTime->SizeTime/60,
					iTime->SizeTime%60);
				break;
			case mttPacket:
				int EndTime;
				EndTime=iTime->EndTime; if ( EndTime>=(24*60) ) EndTime-=24*60;
				swprintf(
					line, sizeof(line)/sizeof(line[0]),
					L"С %i:%.2i до %i:%.2i",
					iTime->BeginTime/60,
					iTime->BeginTime%60,
					EndTime/60,
					EndTime%60);
				break;
			default:
				*line=0;
				break;
		}
		ComboBoxTime->Items->Add(line);
		++iTime;
	}

	// Обновляем сведения об общей стоимости заказа
	if ( RunMode ) UpdateFullCost();
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ComboBoxTimeClick(TObject *Sender)
{
	auto iTariff=Tariffs->SrchUUID(SelTariffID);
	MTariffRunTimesItem& SelTime=UseTimes.GetItem(ComboBoxTime->ItemIndex);

	ComboBoxTime->Color=clWindow;

	// Проставляем для выбранных компьютеров время и тариф, если он применим к ним
	// Рассчитываем стоимость
	TItemStates is=TItemStates()<<isSelected;
	for ( TListItem *Item=ListViewComputers->Selected; Item;
		Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
	{
		auto &Time=*reinterpret_cast<MTariffRunTimesItem*>(Item->Data);

		if ( RunMode )
		{
			// Проверяем применим ли к компьютеру выбранный ранее тариф
			if ( !iTariff->CheckForComp(Time.Number) ) continue;
			//
			// Задаем для компьютера тариф
			Time.TariffID=SelTariffID;
		} else
		{
			// Проверяем применим ли к компьютеру выбранный ранее тариф
			if ( Time.TariffID!=SelTariffID ) continue;
		}
		// Задаем для компьютера параметры пакета
		Time.Type=SelTime.Type;
		Time.BeginTime=SelTime.BeginTime;
		Time.EndTime=SelTime.EndTime;
		Time.SizeTime=SelTime.SizeTime;
		// Рассчитываем время работы и стоимость
		iTariff->Cost(Time,Options->CostPrecision);
		//
		SetListViewComputersLine(Item);
	}

	// Обновляем сведения об общей стоимости заказа
	UpdateFullCost();
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ComboBoxTimeChange(TObject *Sender)
{
    wchar_t hours[10], *minutes;
    int h, m, time;
	MTariffRunTimesItem *SelTime, *Time;
	TItemStates is;

	if ( ComboBoxTime->ItemIndex>=0 ) return;

	auto iTariff=Tariffs->SrchUUID(SelTariffID);
	if ( iTariff==Tariffs->end() ) return;

	wcscpy(hours,ComboBoxTime->Text.c_str());
	// Ищем в строке разделитель часов и минут
	minutes=wcschr(hours,L':');
    //
    if ( minutes==nullptr )
    {
        // Разбираем время как заданное только в минутах (вплоть до суток)
        if ( swscanf(hours,L"%d",&m)!=1 ) goto error; h=0;
    } else
    {
        // Делим строку на две и разбираем по-отдельности
        *(minutes++)=0;
		if ( (swscanf(hours,L"%d",&h)!=1)||(h<0)||(h>24) ) goto error;
		if ( *minutes==0 ) m=0;
        else if( (swscanf(minutes,L"%d",&m)!=1)||(m<0)||(m>59) ) goto error;
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
		Time=reinterpret_cast<MTariffRunTimesItem*>(Item->Data);
		if ( RunMode )
        {
            // Проверяем применим ли к компьютеру выбранный ранее тариф
            if ( !iTariff->CheckForComp(Time->Number) ) continue;
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
        iTariff->Cost(*Time,Options->CostPrecision);
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
    TItemStates is=TItemStates()<<isSelected;
	TListItem *item=ListViewComputers->Selected;

    while(item)
    {
		auto &time=*reinterpret_cast<MTariffRunTimesItem*>(item->Data);
		TListItem *next=ListViewComputers->GetNextItem(item,sdAll,is);

		if (
			time.TariffID &&
			(time.Type!=mttUndefined) &&
			time.WorkTime )
		{
			auto iState=States->Search(time.Number);     ///
			auto iTariff=Tariffs->SrchUUID(time.TariffID);

			// Проверяем возможно ли применить команду
			if ( iState->CmdRun(*iTariff,time,true) )
			{
				// Добавляем запись в лог
				if ( !Log->AddRun(time) )
				{
					ShellState->State|=mssErrorLog; FormMain->SetShell();
					ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
					break;
				}
				// Применяем команду
				iState->CmdRun(*iTariff,time,false);
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
    auto *RunTime=reinterpret_cast<MTariffRunTimesItem*>(Item_->Data);
    TStrings *SubItems=Item_->SubItems;
	wchar_t line[100];
	int icon;

	auto iComputer=Computers->Search(RunTime->Number);
	if ( iComputer==Computers->end() ) return;

    Item_->SubItemImages[0]=FormMain->GetCompColorIcon(&*iComputer);
	swprintf(
		line, sizeof(line)/sizeof(line[0]),
		L"%i",
		RunTime->Number);
	SubItems->Strings[0]=line;
    if ( RunTime->TariffID==0 )
    {
		SubItems->Strings[1]=L"";
		SubItems->Strings[2]=L"";
		SubItems->Strings[3]=L"";
	} else
	{
		auto iT=UseTariffs.Search(RunTime->TariffID);
		SubItems->Strings[1]=
			iT==UseTariffs.end()?
			L"???":		/// throw ?
			iT->Name.c_str();

		if ( RunTime->Type==mttUndefined )
		{
			SubItems->Strings[2]=L"";
            SubItems->Strings[3]=L"";
        } else
        {
            if ( (RunTime->Type==mttHours)||(RunTime->Type==mttFlyPacket) )
                icon=RunTime->WorkTime<RunTime->SizeTime?17:-1;
            else icon =-1;
            Item_->SubItemImages[2]=icon;
			swprintf(
				line, sizeof(line)/sizeof(line[0]),
				L"%i час. %.2i мин.",
				RunTime->WorkTime/60,
				RunTime->WorkTime%60);
            SubItems->Strings[2]=line;
            SubItems->Strings[3]=FloatToStrF(RunTime->Cost,ffCurrency,8,2);
        }
    }
}
//---------------------------------------------------------------------------
void TFormRun::UpdateFullCost()
{
    double cost=0.;

	for ( int i=0, j=ListViewComputers->Items->Count; i<j; i++ )
	{
		auto *time=reinterpret_cast<MTariffRunTimesItem*>(
			ListViewComputers->Items->Item[i]->Data);
		if ( time->TariffID&&
			(time->Type!=mttUndefined)&&
			time->WorkTime ) cost+=time->Cost;
    }

    LabelAllCost->Caption=FloatToStrF(cost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------

