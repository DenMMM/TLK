//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <cwchar>
#pragma hdrstop

#include "UnitFormEvents.h"
#include "UnitAnalyse.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormEvents *FormEvents;
//---------------------------------------------------------------------------
__fastcall TFormEvents::TFormEvents(TComponent* Owner)
    : TForm(Owner)
{
	EventsLog=nullptr;
    EventSort=0;
    StateFilter=mcfAll;
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::FormCreate(TObject *Sender)
{
    ListViewComputers->DoubleBuffered=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ListViewEvents->Items->Clear();
    ListViewComputers->Items->Clear();
    Action=caFree;
    FormMain->WindowClose(this);        /// ����� �� ??
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::ListViewComputersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
    Item->SubItems->Add(L"");
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::ListViewComputersCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
    int num1=(int)Item1->Data;
    int num2=(int)Item2->Data;
    Compare = num1==num2? 0: (num1<num2? -1: +1);
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::ListViewEventsSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    int Time=0;
    double Money=0.;
    int hours, min;
    wchar_t line[50];
    int pos;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewEvents->Selected; Item;
        Item=ListViewEvents->GetNextItem(Item,sdAll,is) )
	{
		auto &LogItem=*reinterpret_cast<const MLogRecordsItem*>(Item->Data);

		switch ( LogItem.gTypeID() )
		{
			case MLogRecords::mlrRun:
				Time+=static_cast<const MLogRecords::CompRun&>(LogItem).WorkTime;
				Money+=static_cast<const MLogRecords::CompRun&>(LogItem).Cost;
				break;

			default:
				break;
        }
    }
    //
    hours=Time/60; min=Time%60;
	pos=0;
	if ( hours>0 ) pos=swprintf(
		line+pos, sizeof(line)-pos,
		L"%.2i ���. ",
		hours);
	swprintf(
		line+pos, sizeof(line)-pos,
		L"%.2i ���.",
		min);
    LabelSelTime->Caption=line;
    LabelSelMoney->Caption=FloatToStrF(Money,ffCurrency,8,2);

    if ( CheckBoxCompAUpd->Checked&&
        Selected&&Item->Focused ) ButtonCompUpdClick(nullptr);
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::ListViewEventsColumnClick(TObject *Sender,
      TListColumn *Column)
{
    EventSort=Column->Index;
    ListViewEvents->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::ListViewEventsCompare(TObject *Sender,
	  TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
	auto &LogItem1=*reinterpret_cast<const MLogRecordsItem*>(Item1->Data);
	auto LogType1=LogItem1.gTypeID();

	auto &LogItem2=*reinterpret_cast<const MLogRecordsItem*>(Item2->Data);
	auto LogType2=LogItem2.gTypeID();

	switch(EventSort)
	{
		case 1:
			Compare=DComp(
				LogItem1.SystemTime,
				LogItem2.SystemTime);
			break;
		case 2:
			// ���������� ������ �����������
			Compare=DComp(
				GetCompNum(LogItem1),
				GetCompNum(LogItem2));
			break;
		case 3:
/*            // ��������� �� ������ ���������� ��������
			Compare=strcmp(Item1->SubItems->Strings[2].c_str(),
				Item2->SubItems->Strings[2].c_str()); */
			if ( LogType1==LogType2 )
			{
				switch(LogType1)
				{
					case MLogRecords::CompRun::TypeID:
						Compare=DComp(
							static_cast<const MLogRecords::CompRun&>(LogItem1).Tariff,
							static_cast<const MLogRecords::CompRun&>(LogItem2).Tariff);
						break;
					case MLogRecords::CompFine::TypeID:
						Compare=DComp(
							static_cast<const MLogRecords::CompFine&>(LogItem1).Fine,
							static_cast<const MLogRecords::CompFine&>(LogItem2).Fine);
						break;
					case MLogRecords::CompExchange::TypeID:
						Compare=DComp(
							static_cast<const MLogRecords::CompExchange&>(LogItem1).To,
							static_cast<const MLogRecords::CompExchange&>(LogItem2).To);
						break;
					case MLogRecords::AppLogIn::TypeID:
						Compare=DComp(
							static_cast<const MLogRecords::AppLogIn&>(LogItem1).User,
							static_cast<const MLogRecords::AppLogIn&>(LogItem2).User);
						break;
					default:
						Compare=0;
                        break;
				}
			} else
			{
				Compare=DComp(LogType1, LogType2);
			}
/*            // ��������� � ������ ������ �������� �������, �����������, ������ �����
				LogType1==mlrRun?-1:(
				LogType2==mlrRun?1:(
				LogType1==mlrFine?-1:(
				LogType2==mlrFine?1:(
				LogType1==mlrLogIn?-1:(
				LogType2==mlrLogIn?1:DComp(LogType1,LogType2)))))); */
			break;
		case 4:
/*            // ��������� �� ������ ���������� ��������
			Compare=strcmp(Item1->SubItems->Count<=3?"":Item1->SubItems->Strings[3].c_str(),
				Item2->SubItems->Count<=3?"":Item2->SubItems->Strings[3].c_str()); */
			if ( LogType1==LogType2 )
			{
				if ( LogType1==MLogRecords::CompRun::TypeID )
				{
					const MLogRecords::CompRun
						&RunItem1=static_cast<const MLogRecords::CompRun&>(LogItem1),
						&RunItem2=static_cast<const MLogRecords::CompRun&>(LogItem2);

					Compare=DComp(
						RunItem1.Type,
						RunItem2.Type);

					if ( Compare==0 )
					{
						switch(RunItem1.Type)
						{
							case mttHours:
								Compare=DComp(
									RunItem1.WorkTime,
									RunItem2.WorkTime);
								break;
							case mttFlyPacket:
								Compare=DComp(
									RunItem1.SizeTime,
									RunItem2.SizeTime);
								break;
							case mttPacket:
								Compare=DComp(
									RunItem1.BeginTime,
									RunItem2.BeginTime);
								break;
							default: break;
						}
					}
				} else if ( LogType1==MLogRecords::CompFine::TypeID )
					Compare=DComp(
						static_cast<const MLogRecords::CompFine&>(LogItem1).Time,
						static_cast<const MLogRecords::CompFine&>(LogItem2).Time);
				else Compare=0;
			} else
			{
				Compare=
					LogType1==MLogRecords::CompRun::TypeID? -1: (
					LogType2==MLogRecords::CompRun::TypeID? 1: (
					LogType1==MLogRecords::CompFine::TypeID? -1: (
					LogType2==MLogRecords::CompFine::TypeID? 1: 0)));
			}
			break;
		case 5:
			if ( LogType1==LogType2 )
			{
				const MLogRecords::CompRun
					&RunItem1=static_cast<const MLogRecords::CompRun&>(LogItem1),
					&RunItem2=static_cast<const MLogRecords::CompRun&>(LogItem2);

				Compare=
					LogType1==MLogRecords::CompRun::TypeID?
					DComp(RunItem1.Cost, RunItem2.Cost): 0;
			} else
			{
				Compare=
					LogType1==MLogRecords::CompRun::TypeID? -1: (
					LogType2==MLogRecords::CompRun::TypeID? 1: 0);
			}
			break;
		default:
			Compare=0;
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::CheckBoxCompAUpdClick(TObject *Sender)
{
	ButtonCompUpd->Enabled=!CheckBoxCompAUpd->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::ButtonCompUpdClick(TObject *Sender)
{
	MStates States;
	MTariffs Tariffs;

	if (
//		ListViewEvents->ItemFocused &&
		PanelState->Height&&ListViewEvents->ItemFocused &&
		ProcessComputersState(
            *EventsLog,
			MLogRecords::const_iterator(
			reinterpret_cast<MLogRecordsItem*>(
				ListViewEvents->ItemFocused->Data),
				EventsLog),
			States, Tariffs) )
	{
			UpdateListViewComputers(false, States, Tariffs);
	}
}
//---------------------------------------------------------------------------
bool TFormEvents::CheckFilter(const MStatesInfo &Info_, int FreeTime_)
{
	bool result=false;

	switch(StateFilter)
	{
		case mcfAll:
			result=true;
			break;
		case mcfFree:
			if ( (Info_.State==mcsFree)||
				((Info_.State&mcsWork)&&(Info_.ToEndWork<FreeTime_)) ) result=true;
			break;
		case mcfService:
			if ( Info_.State&mcsOpen ) result=true;
			break;
		default: break;
	}

	return result;
}
//---------------------------------------------------------------------------
int TFormEvents::GetCompNum(const MLogRecordsItem &Item_)
{
	switch(Item_.gTypeID())
	{
		case MLogRecords::CompRun::TypeID:
			return static_cast<const MLogRecords::CompRun&>(Item_).Number;

		case MLogRecords::CompFine::TypeID:
			return static_cast<const MLogRecords::CompFine&>(Item_).Number;

		case MLogRecords::CompExchange::TypeID:
			return static_cast<const MLogRecords::CompExchange&>(Item_).From;

		case MLogRecords::ModeLock::TypeID:
			return static_cast<const MLogRecords::ModeLock&>(Item_).Number;

		case MLogRecords::ModePause::TypeID:
			return static_cast<const MLogRecords::ModePause&>(Item_).Number;

		case MLogRecords::ModeOpen::TypeID:
			return static_cast<const MLogRecords::ModeOpen&>(Item_).Number;

		case MLogRecords::CmdPowerOn::TypeID:
			return static_cast<const MLogRecords::CmdPowerOn&>(Item_).Number;

		case MLogRecords::CmdReboot::TypeID:
			return static_cast<const MLogRecords::CmdReboot&>(Item_).Number;

		case MLogRecords::CmdShutdown::TypeID:
			return static_cast<const MLogRecords::CmdShutdown&>(Item_).Number;

		default:
			return -1;
	}
}
//---------------------------------------------------------------------------
void TFormEvents::SetListViewComputersLine(
	TListItem *Item_,
	const MStatesInfo &Info_,
	const MTariffs &Tariffs_)
{
	TStrings *SubItems=Item_->SubItems;
	wchar_t line[50];
	int icon;

	// ����� ����������
	if ( Info_.Changes&mdcNumber )
	{
		swprintf(
			line, sizeof(line),
			L"%i",
			Info_.Number);
        SubItems->Strings[0]=line;
    }
    // ����� ������
    if ( Info_.Changes&mdcState )
    {
        unsigned int State=Info_.State;
		if ( State&mcsOpen ) { icon=5; wcscpy(line, L"���������"); }
		else if ( State&mcsPause ) { icon=4; wcscpy(line, L"�������������"); }
		else if ( State&mcsLock ) { icon=3; wcscpy(line, L"������� !"); }
		else if ( State&mcsFine ) { icon=2; wcscpy(line, L"����� !!!"); }
		else if ( State&mcsWork ) { icon=1; wcscpy(line, L"������"); }
		else if ( State&mcsFree ) { icon=0; wcscpy(line, L"��������"); }
		else { icon=-1; wcscpy(line, L""); }
        Item_->SubItemImages[1]=icon;
        SubItems->Strings[1]=line;
    }
    // �������� ������
    if ( Info_.Changes&mdcTariff )
    {
		auto iTariff=Tariffs_.SrchUUID(Info_.TariffID);
		if ( iTariff!=Tariffs_.end() ) SubItems->Strings[2]=iTariff->Name.c_str();
		else SubItems->Strings[2]=L"";
	}
	// ����� ������
	if ( Info_.Changes&mdcWorkTime )
	{
		if ( Info_.State&mcsWork )
		{
			swprintf(line, L"%i ���. %.2i ���.",Info_.WorkTime/60,Info_.WorkTime%60);
			SubItems->Strings[3]=line;
        } else
        {
            SubItems->Strings[3]=L"";
			SubItems->Strings[4]=L"";
            SubItems->Strings[5]=L"";
        }
    }
    // ������� ������� �������� �������� � �� �������
    if ( Info_.State&mcsWork )
    {
		swprintf(line, L"%.2i:%.2i",Info_.ToEndWork/60,Info_.ToEndWork%60);
		SubItems->Strings[4]=line;
        swprintf(line, L"%.2i:%.2i",Info_.EndWorkTime/60,Info_.EndWorkTime%60);
        SubItems->Strings[5]=line;
    }
    // ����� ������
    if ( Info_.Changes&mdcFineTime )
    {
        if ( Info_.State&mcsFine )
        {
            swprintf(line, L"%i ���.",Info_.FineTime);
            SubItems->Strings[6]=line;
        } else
        {
			SubItems->Strings[6]=L"";
            SubItems->Strings[7]=L"";
        }
    }
    // ������� ������� ������ ��������
	if ( Info_.State&mcsFine )
	{
		swprintf(line, L"%.2i:%.2i",Info_.ToEndFine/60,Info_.ToEndFine%60);
		SubItems->Strings[7]=line;
	}
}
//---------------------------------------------------------------------------
void TFormEvents::UpdateListViewComputers(
	bool Full_,
	MStates &States_,
	const MTariffs &Tariffs_)
{
	// ������� ������, �� �������������� � ���������� ����������
	for ( int i=ListViewComputers->Items->Count-1; i>=0; i-- )
	{
		TListItem *Item=ListViewComputers->Items->Item[i];
		if ( States_.Search((int)Item->Data)!=States_.end() ) Item->Delete();
	}

	// ������� �� ������ ����������, �� ���������� ��� ������, � ��������� �����
	for ( auto &State: States_ )
	{
		//
		MStatesInfo Info=State.GetInfo();
		//
		TListItem *Item=ListViewComputers->FindData(
			0, reinterpret_cast<void*>(Info.Number),    /// "�������" cast
			true, false);
		// ��������� �������� �� ��������� ��� ������������ ������
		if ( !CheckFilter(Info,2) )
		{
			if ( Item ) Item->Delete();
			continue;
		}
		// ���� ��������� �� ������� � ������, �� ��������� ���
		if ( !Item )
		{
			Item=ListViewComputers->Items->Add();
			Item->Data=reinterpret_cast<void*>(Info.Number);
			Info.Changes=mdcAll;
		}
		// ��������� ���������� � �������
		if ( Full_ ) Info.Changes=mdcAll;
		SetListViewComputersLine(Item, Info, Tariffs_);
	}
}
//---------------------------------------------------------------------------
bool TFormEvents::Open(
	const MLogFile &File_,
	MLogRecords::const_iterator Begin_,
	MLogRecords::const_iterator End_)
{
	// ������ ���������
	ListViewEvents->Items->Clear();
	ListViewComputers->Items->Clear();

	Caption=UnicodeString(L"�������  -  ")+File_.Name.c_str();

	EventsLog=&File_.Records;
	EventsBegin=Begin_;
	EventsEnd=End_;

	StateFilter=mcfAll;
	EventSort=0;
	CheckBoxCompAUpd->Checked=true;
	NEventComputers->Checked=true;
	UpdateListViewEvents();

	FormMain->WindowOpen(File_,this);
	FormMain->WindowCaption(this, "�������");	/// Unicode support ???
	return true;
error:
    Close();
    return false;
}
//---------------------------------------------------------------------------
MTariffs TFormEvents::MakeTariffs(
	const MLogRecords::DataTariffs &LogRecord_)
{
	MTariffs Tariffs;
	for ( const auto &Ld: LogRecord_.Items )
	{
		Tariffs.Add().sFromLog(Ld);
	}
	return Tariffs;
}
//---------------------------------------------------------------------------
MFines TFormEvents::MakeFines(
	const MLogRecords::DataFines &LogRecord_)
{
	MFines Fines;
	for ( const auto &Ld: LogRecord_.Items )
	{
		Fines.Add().sFromLog(Ld);
	}
	return Fines;
}
//---------------------------------------------------------------------------
MUsers TFormEvents::MakeUsers(
	const MLogRecords::DataUsers &LogRecord_)
{
	MUsers Users;
	for ( const auto &Ld: LogRecord_.Items )
	{
		Users.Add().sFromLog(Ld);
	}
	return Users;
}
//---------------------------------------------------------------------------
bool TFormEvents::CheckEventFilter(unsigned char TypeID_)
{
    bool result=false;

    if ( NEventAdminMdl->Checked )
    {
        unsigned char et[]={
			MLogRecords::mlrStart,
			MLogRecords::mlrStop,
			MLogRecords::mlrConfig,
			MLogRecords::mlrComputers,
			MLogRecords::mlrTariffs,
			MLogRecords::mlrFines,
			MLogRecords::mlrUsers,
			MLogRecords::mlrOptions};
		result=result||MemSrch(et,et+sizeof(et),TypeID_);
    }

    if ( NEventUsers->Checked )
    {
        unsigned char et[]={
			MLogRecords::mlrLogIn,
			MLogRecords::mlrLogOut};
        result=result||MemSrch(et,et+sizeof(et),TypeID_);
    }

    if ( NEventComputers->Checked )
    {
        unsigned char et[]={
			MLogRecords::mlrRun,
			MLogRecords::mlrFine,
			MLogRecords::mlrExchange,
			MLogRecords::mlrLock,
			MLogRecords::mlrPause,
			MLogRecords::mlrOpen,
			MLogRecords::mlrPowerOn,
			MLogRecords::mlrReboot,
			MLogRecords::mlrShutdown};
        result=result||MemSrch(et,et+sizeof(et),TypeID_);
    }

    if ( NEventService->Checked )
    {
        unsigned char et[]={
			MLogRecords::mlrBegin,
			MLogRecords::mlrEnd,
			MLogRecords::mlrDataShState,
			MLogRecords::mlrDataStates,
			MLogRecords::mlrDataTariffs,
			MLogRecords::mlrDataFines,
			MLogRecords::mlrDataUsers};
		result=result||MemSrch(et,et+sizeof(et),TypeID_);
	}

    return result;
}
//---------------------------------------------------------------------------
void TFormEvents::UpdateListViewEvents()
{
    MTariffs Tariffs;
    MFines Fines;
    MUsers Users;
    SYSTEMTIME ss_time;
    wchar_t line[256];
    TListItem *Item;

	ListViewEvents->Items->Clear();

	for ( MLogRecords::const_iterator Begin_=EventsBegin, End_=EventsEnd;
		Begin_!=End_; ++Begin_ )
    {
        // ������� ������ �������/�������/�������������
        switch(Begin_->gTypeID())
        {
			case MLogRecords::DataTariffs::TypeID:
				Tariffs=MakeTariffs(
					static_cast<const MLogRecords::DataTariffs&>(*Begin_));
				break;
			case MLogRecords::DataFines::TypeID:
				Fines=MakeFines(
					static_cast<const MLogRecords::DataFines&>(*Begin_));
				break;
			case MLogRecords::DataUsers::TypeID:
				Users=MakeUsers(
					static_cast<const MLogRecords::DataUsers&>(*Begin_));
				break;
            default:
                break;
        }

        // �������� �������� �� ������ ���������� ������ ��������
		if ( !CheckEventFilter(Begin_->gTypeID()) ) continue;

        // ����� ������
        Item=ListViewEvents->Items->Add();
        Item->ImageIndex=-1;
        Item->Data=const_cast<MLogRecordsItem*>(&*Begin_);
        // ����� �������
        if ( Int64ToSystemTime(Begin_->SystemTime, ss_time) )
			swprintf(
				line, sizeof(line),
				L"%4d.%02d.%02d - %02d:%02d:%02d",
				ss_time.wYear, ss_time.wMonth, ss_time.wDay,
				ss_time.wHour, ss_time.wMinute, ss_time.wSecond);
		else *line=L'\0';
        Item->SubItems->Add(line);

        // ��������
        switch(Begin_->gTypeID())
        {
			case MLogRecords::LogBegin::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"���� ���� �����");
				break;
			case MLogRecords::LogEnd::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"���� ���� ������");
				break;
			case MLogRecords::AppStart::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"������ ���������� �������");
				break;
			case MLogRecords::AppStop::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"������ ���������� ����������");
				break;

			case MLogRecords::AppConfig::TypeID:
				Item->SubItems->Add(L"");
				if ( static_cast<const MLogRecords::AppConfig&>(*Begin_).Opened )
					Item->SubItems->Add(L"��������� �������");
				else
					Item->SubItems->Add(L"��������� �������");
				break;
			case MLogRecords::ChComputers::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"������� ������ �����������");
				break;
			case MLogRecords::ChTariffs::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"�������� ������");
				break;
			case MLogRecords::ChFines::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"�������� ������");
				break;
			case MLogRecords::ChUsers::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"������� ������ �������������");
				break;
			case MLogRecords::ChOptions::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"�������� ����� ���������");
				break;

			case MLogRecords::AppLogIn::TypeID:
			{
				Item->SubItems->Add(L"");
				auto iUsr=Users.SrchUUID(
					static_cast<const MLogRecords::AppLogIn&>(*Begin_).User);
				swprintf(
					line, sizeof(line),
					L"����� �����(�) '%s'",
					iUsr!=Users.end()? iUsr->Name.c_str(): L"???");
				Item->SubItems->Add(line);
			}
				break;
			case MLogRecords::AppLogOut::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"����� ��������");
				break;

			case MLogRecords::CompRun::TypeID:
			{
				auto &rcdr=static_cast<const MLogRecords::CompRun&>(*Begin_);
				auto iTrf=Tariffs.SrchUUID(rcdr.Tariff);

				Item->SubItems->Add(IntToStr(rcdr.Number));
				Item->SubItems->Add(iTrf!=Tariffs.end()? iTrf->Name.c_str(): L"???");
				switch(rcdr.Type)
				{
					case mttHours:
						swprintf(line, L"%i ���. %.2i ���.",
							rcdr.WorkTime/60,rcdr.WorkTime%60);
						break;
					case mttFlyPacket:
						swprintf(line, L"�� %i ���. %.2i ���. (%i:%.2i)",
							rcdr.SizeTime/60,rcdr.SizeTime%60,
							rcdr.WorkTime/60,rcdr.WorkTime%60);
						break;
					case mttPacket:
						int end;
						end=rcdr.EndTime>=(24*60)? rcdr.EndTime-24*60: rcdr.EndTime;
						swprintf(line, L"� %i:%.2i �� %i:%.2i (%i:%.2i)",
							rcdr.BeginTime/60,rcdr.BeginTime%60,end/60,end%60,
							rcdr.WorkTime/60,rcdr.WorkTime%60);
						break;
					default: *line=0; break;
				}
				Item->SubItems->Add(line);
				Item->SubItems->Add(FloatToStrF(rcdr.Cost,ffCurrency,8,2));
			}
				break;
			case MLogRecords::CompFine::TypeID:
			{
				auto &rcdf=static_cast<const MLogRecords::CompFine&>(*Begin_);
				auto iFn=Fines.SrchUUID(rcdf.Fine);

				Item->SubItems->Add(IntToStr(rcdf.Number));
				swprintf(
					line, sizeof(line),
					L"����� '%s'",
					iFn!=Fines.end()? iFn->Descr.c_str(): L"???");
				Item->SubItems->Add(line);
				if ( rcdf.Time==(24*60) )
				{
					swprintf(
						line, sizeof(line),
						L"��� �����");
				} else if ( rcdf.Time<0 )
				{
					swprintf(
						line, sizeof(line),
						L"����� %i ���.",
						-rcdf.Time);
				} else
				{
					swprintf(
						line, sizeof(line),
						L"�������� %i ���.",
						rcdf.Time);
				}
				Item->SubItems->Add(line);
			}
				break;
			case MLogRecords::CompExchange::TypeID:
			{
				auto &rcde=static_cast<const MLogRecords::CompExchange&>(*Begin_);

				Item->SubItems->Add(IntToStr(rcde.From));
				swprintf(
					line, sizeof(line),
					L"��������� �� �%i",
					rcde.To);
				Item->SubItems->Add(line);
			}
				break;
			case MLogRecords::ModeLock::TypeID:
			{
				auto &rcdl=static_cast<const MLogRecords::ModeLock&>(*Begin_);

				Item->SubItems->Add(IntToStr(rcdl.Number));
				Item->SubItems->Add(rcdl.Apply? L"�������": L"������");
			}
				break;
			case MLogRecords::ModePause::TypeID:
			{
				auto &rcdp=static_cast<const MLogRecords::ModePause&>(*Begin_);

				Item->SubItems->Add(IntToStr(rcdp.Number));
				Item->SubItems->Add(rcdp.Apply? L"����� ��������������": L"����� ��������");
			}
				break;
			case MLogRecords::ModeOpen::TypeID:
			{
				auto &rcdo=static_cast<const MLogRecords::ModeOpen&>(*Begin_);

				Item->SubItems->Add(IntToStr(rcdo.Number));
				Item->SubItems->Add(rcdo.Apply? L"������ ��� ���������": L"������ ����� ���������");
			}
				break;
			case MLogRecords::CmdPowerOn::TypeID:
			{
				auto &rcdpwr=static_cast<const MLogRecords::CmdPowerOn&>(*Begin_);

				Item->SubItems->Add(IntToStr(rcdpwr.Number));
				Item->SubItems->Add(L"������� �� ���������");
			}
				break;
			case MLogRecords::CmdReboot::TypeID:
			{
				auto &rcdprb=static_cast<const MLogRecords::CmdReboot&>(*Begin_);

				Item->SubItems->Add(IntToStr(rcdprb.Number));
				Item->SubItems->Add(L"������� �� ������������");
			}
				break;
			case MLogRecords::CmdShutdown::TypeID:
			{
				auto &rcdpsd=static_cast<const MLogRecords::CmdShutdown&>(*Begin_);

				Item->SubItems->Add(IntToStr(rcdpsd.Number));
				Item->SubItems->Add(L"������� �� ����������");
			}
				break;

			case MLogRecords::DataShell::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"������ �� ������ ����������");
				break;
			case MLogRecords::DataStates::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"������ �� �����������");
				break;
			case MLogRecords::DataTariffs::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"������ �� �������");
				break;
			case MLogRecords::DataFines::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"������ �� �������");
				break;
			case MLogRecords::DataUsers::TypeID:
				Item->SubItems->Add(L"");
				Item->SubItems->Add(L"������ �� �������������");
				break;

			default: break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::NEventAllClick(TObject *Sender)
{
    if ( Sender==NEventAll )
    {
        NEventComputers->Checked=true;
        NEventUsers->Checked=true;
        NEventAdminMdl->Checked=true;
        NEventService->Checked=true;
    } 
    UpdateListViewEvents();
}
//---------------------------------------------------------------------------

