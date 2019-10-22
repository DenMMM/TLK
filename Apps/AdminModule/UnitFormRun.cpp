//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdexcept>
#include <cwchar>
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

    // �������� ������ ����������� � ���������� ��� ��� �������
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *item=FormMain->ListViewComputers->Selected; item;
        item=FormMain->ListViewComputers->GetNextItem(item,sdAll,is) )
    {
        auto state=reinterpret_cast<MStatesItem*>(item->Data);
        MTariffRunTimesItem tmptime;
        // ����������� ��������� ��� �������/����������
        state->RunParam(&tmptime);
        // ��������� ����� �� ���������/�������� �����
        if ( tmptime.MaxTime==0 ) continue;
        // �������������� ��������, ����������� ��� ������ ������������� �������
        if ( RunMode )
        {
            // ��������� �� ��������� �� ��� ���������� �����
            if ( tmptime.TariffID!=0 ) continue;
        } else
        {
            // ��������� ���������� �� ��� �����, �� �������� ��� ������� ���������,
            // � �������� �� �� ��� ��� ���� ����� � ����������
            MTariffsItem* tariff=Tariffs->SrchUUID(tmptime.TariffID);
			if ( (tariff==nullptr)||
				(!tariff->CheckForComp(tmptime.Number)) ) continue;
            // ��������� ����� � ������ ��� �������������, ���� �� ��� �� ��������
            if ( !UseTariffs.Search(tmptime.TariffID) )
            {
				MTariffsInfoItem* info=UseTariffs.Add();
				tariff->GetInfo(info);
			}
		}

		// �������� ��������� �������/����������
		MTariffRunTimesItem* time=CompTimes.Add();
		*time=tmptime;
        // ��������� ������ � ������ ����������� � ��������� � �����������
        TListItem *newitem=ListViewComputers->Items->Add();
        newitem->Data=time;
        // ��� �������� ����� �������� ���������� � �������
        if ( RunMode ) newitem->Selected=true;
        SetListViewComputersLine(newitem);
    }

    if ( RunMode )
    {
        // ���������� ���������� ����� �������� �������
        if ( RunMode ) GetLocalTimeInt64(&OpenDialogTime);
        // ����� ������ �������, ��������� ��� ������������� � ��� �����
        Tariffs->GetForTime(OpenDialogTime,&UseTariffs);
    }

    // ������� � ������ ������, ������� ����� ������������ � ��� �����
	// ��� � ���������� ������������ (��� ���������� �������)
	for ( const auto &Info: UseTariffs )
	{
		ComboBoxTariff->Items->Add(Info.Name.c_str());
	}

    // ������������ ��������� � ������������ � ������� ������ �������
    if ( RunMode )
    {
		Caption=L"������ �����������";
		ComboBoxTariff->Enabled=true;
		BitBtnRun->Caption=L"���������";
	} else
	{
		Caption=L"���������� �������";
		ComboBoxTariff->Enabled=false;
        BitBtnRun->Caption=L"��������";
    }

    // ��������� ���� ����� ������� �� ������� ������ ������
    ComboBoxTime->Enabled=false;
    ComboBoxTime->Color=clBtnFace;
    ComboBoxTime->Style=csSimple;

    // �������������� ������ ����������� ��������
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
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::FormClose(TObject *Sender, TCloseAction &Action)
{
    TimerDialogLock->Enabled=false;
    // ������ ������������ ��������
    ListViewComputers->Items->Clear();
    ComboBoxTariff->Clear();
    ComboBoxTime->Clear();
    LabelAllCost->Caption=L"";
    // ������� ������
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

	auto Time=reinterpret_cast<MTariffRunTimesItem*>(
		ListViewComputers->Selected->Data);
    int Num=0;

	for ( const auto &Info: UseTariffs )
	{
		if ( Info.ID==Time->TariffID )
		{
			OpenDialogTime=Time->StartTime;
			SelTariffID=Time->TariffID;
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
	// ���������� ����� ����� ��� ������
	if ( RunMode ) SelTariffID=
		UseTariffs.GetItem(ComboBoxTariff->ItemIndex)->ID;

	MTariffsItem *Tariff=Tariffs->SrchUUID(SelTariffID);
	// ������ ��� ��������� ����������� �����, ���� �� �������� � ���
	if ( RunMode )
	{
		TItemStates is=TItemStates()<<isSelected;
		for ( TListItem *Item=ListViewComputers->Selected; Item;
			Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
		{
			auto Time=reinterpret_cast<MTariffRunTimesItem*>(Item->Data);
			if ( !Tariff->CheckForComp(Time->Number) ) continue;
			if ( Time->TariffID==SelTariffID ) continue;
			Time->TariffID=SelTariffID;
			Time->Type=mttUndefined;
			SetListViewComputersLine(Item);
		}
	}

	ComboBoxTime->Clear();

	// ����������� ��� ������ ������ �������
	Tariff->GetRunTimes(OpenDialogTime,&UseTimes);

	// ���� ������� ��� ������ � ��� ����� ����, �� ��������� ���� �������
	auto iTime=UseTimes.begin();
	auto iEnd=UseTimes.end();

	if ( iTime==iEnd )
	{
		ComboBoxTime->Enabled=false;
		ComboBoxTime->Color=clBtnFace;
		ComboBoxTime->Style=csSimple;
		return;
	}
	ComboBoxTime->Enabled=true;
	ComboBoxTime->Color=clWindow;

	// ��������� ���� ���������� �������
	if ( iTime->Type==mttHours )
	{
		ComboBoxTime->Style=iTime->gNext()?csDropDown:csSimple;
		UseTimes.Del(&(*iTime));
		iTime=UseTimes.begin();
	} else
	{
		ComboBoxTime->Style=csDropDownList;
	}
	// ������� � ������ ������
	while ( iTime!=iEnd )
	{
		wchar_t line[30];

		switch( iTime->Type )
		{
			case mttFlyPacket:
				swprintf(
					line, sizeof(line),
					L"�� %i ���. %.2i ���.",
					iTime->SizeTime/60,
					iTime->SizeTime%60);
				break;
			case mttPacket:
				int EndTime;
				EndTime=iTime->EndTime; if ( EndTime>=(24*60) ) EndTime-=24*60;
				swprintf(
					line, sizeof(line),
					L"� %i:%.2i �� %i:%.2i",
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

	// ��������� �������� �� ����� ��������� ������
	if ( RunMode ) UpdateFullCost();
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ComboBoxTimeClick(TObject *Sender)
{
	MTariffRunTimesItem *SelTime, *Time;
    MTariffsItem *Tariff;

	Tariff=Tariffs->SrchUUID(SelTariffID);
    SelTime=UseTimes.GetItem(ComboBoxTime->ItemIndex);

    ComboBoxTime->Color=clWindow;

    // ����������� ��� ��������� ����������� ����� � �����, ���� �� �������� � ���
    // ������������ ���������
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        Time=reinterpret_cast<MTariffRunTimesItem*>(Item->Data);
        if ( RunMode )
        {
            // ��������� �������� �� � ���������� ��������� ����� �����
            if ( !Tariff->CheckForComp(Time->Number) ) continue;
            //
            // ������ ��� ���������� �����
            Time->TariffID=SelTariffID;
        } else
        {
            // ��������� �������� �� � ���������� ��������� ����� �����
            if ( Time->TariffID!=SelTariffID ) continue;
        }
        // ������ ��� ���������� ��������� ������
        Time->Type=SelTime->Type;
        Time->BeginTime=SelTime->BeginTime;
        Time->EndTime=SelTime->EndTime;
        Time->SizeTime=SelTime->SizeTime;
        // ������������ ����� ������ � ���������
        Tariff->Cost(Time,Options->CostPrecision);
        //
        SetListViewComputersLine(Item);
    }

    // ��������� �������� �� ����� ��������� ������
    UpdateFullCost();
}
//---------------------------------------------------------------------------
void __fastcall TFormRun::ComboBoxTimeChange(TObject *Sender)
{
    wchar_t hours[10], *minutes;
    int h, m, time;
	MTariffRunTimesItem *SelTime, *Time;
    MTariffsItem *Tariff;
    TItemStates is;

    if ( (ComboBoxTime->ItemIndex>=0)||
        ((Tariff=Tariffs->SrchUUID(SelTariffID))==nullptr) ) return;

	wcscpy(hours,ComboBoxTime->Text.c_str());
	// ���� � ������ ����������� ����� � �����
	minutes=wcschr(hours,L':');
    //
    if ( minutes==nullptr )
    {
        // ��������� ����� ��� �������� ������ � ������� (������ �� �����)
        if ( swscanf(hours,L"%d",&m)!=1 ) goto error; h=0;
    } else
    {
        // ����� ������ �� ��� � ��������� ��-�����������
        *(minutes++)=0;
		if ( (swscanf(hours,L"%d",&h)!=1)||(h<0)||(h>24) ) goto error;
		if ( *minutes==0 ) m=0;
        else if( (swscanf(minutes,L"%d",&m)!=1)||(m<0)||(m>59) ) goto error;
    }
    // ������������ �������� ����� ������ � ��������� ��� �� �����������
    time=h*60+m; if ( (time<=0)||(time>24*60) ) goto error;

    ComboBoxTime->Color=clWindow;

    // ����������� ��� ��������� ����������� ����� � �����, ���� �� �������� � ���
    // ������������ ���������
    is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
		Time=reinterpret_cast<MTariffRunTimesItem*>(Item->Data);
		if ( RunMode )
        {
            // ��������� �������� �� � ���������� ��������� ����� �����
            if ( !Tariff->CheckForComp(Time->Number) ) continue;
            //
            // ������ ��� ���������� �����
            Time->TariffID=SelTariffID;
        } else
        {
            // ��������� �������� �� � ���������� ��������� ����� �����
            if ( Time->TariffID!=SelTariffID ) continue;
        }
        // ������ ��� ���������� ��������� ������
        Time->Type=mttHours;
        Time->SizeTime=time;
        // ������������ ����� ������ � ���������
        Tariff->Cost(Time,Options->CostPrecision);
        //
        SetListViewComputersLine(Item);
    }

    // ��������� �������� �� ����� ��������� ������
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
        auto time=reinterpret_cast<MTariffRunTimesItem*>(item->Data);
        next=ListViewComputers->GetNextItem(item,sdAll,is);
        if ( time->TariffID&&(time->Type!=mttUndefined)&&time->WorkTime )
        {
			MStatesItem *state=States->Search(time->Number);     ///
            MTariffsItem* tariff=Tariffs->SrchUUID(time->TariffID);
            // ��������� �������� �� ��������� �������
            if ( state->CmdRun(tariff,time,true) )
            {
                // ��������� ������ � ���
                if ( !Log->AddRun(time) )
                {
                    ShellState->State|=mssErrorLog; FormMain->SetShell();
                    ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
                    break;
                }
                // ��������� �������
                state->CmdRun(tariff,time,false);
                // ������� ������ �� ������
                item->Delete();
            }
        }
        item=next;
    }
    // ��������� ����� ���������
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; FormMain->SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // ��������� ������ �����������
    FormMain->UpdateListViewComputers(false);

    // ��������� �������� �� ����� ��������� ������
    UpdateFullCost();
}
//---------------------------------------------------------------------------
void TFormRun::SetListViewComputersLine(TListItem *Item_)
{
    auto RunTime=reinterpret_cast<MTariffRunTimesItem*>(Item_->Data);
    TStrings *SubItems=Item_->SubItems;
    MComputersItem *Computer;
    wchar_t line[100];
    int icon;

    Computer=Computers->Search(RunTime->Number);
    if ( Computer==nullptr ) return;

    Item_->SubItemImages[0]=FormMain->GetCompColorIcon(Computer);
	swprintf(
		line, sizeof(line),
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
		SubItems->Strings[1]=UseTariffs.Search(RunTime->TariffID)->Name.c_str();
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
				line, sizeof(line),
				L"%i ���. %.2i ���.",
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
		auto time=reinterpret_cast<MTariffRunTimesItem*>(
			ListViewComputers->Items->Item[i]->Data);
		if ( time->TariffID&&
			(time->Type!=mttUndefined)&&
			time->WorkTime ) cost+=time->Cost;
    }

    LabelAllCost->Caption=FloatToStrF(cost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------

