//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "UnitFormUsersUpTime.h"
#include "UnitAnalyse.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormUsersUpTime *FormUsersUpTime;
//---------------------------------------------------------------------------
__fastcall TFormUsersUpTime::TFormUsersUpTime(TComponent* Owner)
    : TForm(Owner)
{
    SortMode=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsersUpTime::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ListViewUpTimes->Items->Clear();
//    FormMain->FormClose(Sender,Action);
    Action=caFree;
    FormMain->WindowClose(this);
}
//---------------------------------------------------------------------------
bool TFormUsersUpTime::Open(
	const MLogFile &File_,
	MLogRecords::const_iterator iBegin_,
	MLogRecords::const_iterator iEnd_)
{
	__int64 time;
	int hours, min;
	wchar_t line[50];
	int pos;
	SYSTEMTIME ss_time_b, ss_time_e;

	ProcessUsersUpTime(iBegin_,iEnd_,Users,Times);
	ListViewUpTimes->Items->Clear();

	for ( auto &Time: Times )
	{
		TListItem *Item=ListViewUpTimes->Items->Add();
		Item->ImageIndex=-1;
		Item->Data=&Time;

		auto iUser=Users.SrchUUID(Time.User);
		if ( iUser==Users.end() ) Item->SubItems->Add(L"");
		else Item->SubItems->Add(iUser->Name.c_str());

		// Время
		if ( Int64ToSystemTime(Time.BeginTime,ss_time_b) )
		{
			swprintf(
				line, sizeof(line),
				L"%4d.%02d.%02d - %02d:%02d:%02d",
				ss_time_b.wYear, ss_time_b.wMonth, ss_time_b.wDay,
				ss_time_b.wHour, ss_time_b.wMinute, ss_time_b.wSecond);
		} else
		{
			*line=L'\0';
		}
		Item->SubItems->Add(line);
		//
		if (
			Time.EndTime!=0 &&
			Int64ToSystemTime(Time.EndTime,ss_time_e) )
		{
			pos=0;
			if ( ss_time_b.wYear!=ss_time_e.wYear ) goto year;
			if ( ss_time_b.wMonth!=ss_time_e.wMonth ) goto month;
			if ( ss_time_b.wDay!=ss_time_e.wDay )
			{
				goto day;
year:           pos+=swprintf(line+pos, L"%4d.",ss_time_e.wYear);
month:          pos+=swprintf(line+pos, L"%02d.",ss_time_e.wMonth);
day:            pos+=swprintf(line+pos, L"%02d - ",ss_time_e.wDay);
			}
			swprintf(line+pos, L"%02d:%02d:%02d",
				ss_time_e.wHour,ss_time_e.wMinute,ss_time_e.wSecond);
			Item->SubItems->Add(line);
			//
			time=Time.EndTime-Time.BeginTime;
			hours=time/(60*60*10000000i64);
			min=(time%(60*60*10000000i64))/(60*10000000i64);
			pos=0; if ( hours>0 ) pos=swprintf(line+pos, L"%.2i час. ",hours);
			swprintf(line+pos, L"%.2i мин.",min);
			Item->SubItems->Add(line);
		} else
		{
			Item->SubItems->Add(L"");
			Item->SubItems->Add(L"");
		}
		//
		Item->SubItems->Add(FloatToStrF(Time.Gains,ffCurrency,8,2));
	}

	Caption=UnicodeString(L"Смены  -  ")+File_.Name.c_str();
	FormMain->WindowOpen(File_,this);
	FormMain->WindowCaption(this, "Смены");         /// Unicode support ???
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsersUpTime::ListViewUpTimesSelectItem(
	  TObject *Sender, TListItem *Item, bool Selected)
{
	__int64 Time=0;
	double Money=0.;
	int hours, min;
	wchar_t line[50];
	int pos;

	TItemStates is=TItemStates()<<isSelected;
	for ( TListItem *Item=ListViewUpTimes->Selected; Item;
		Item=ListViewUpTimes->GetNextItem(Item,sdAll,is) )
	{
		auto UpTime=reinterpret_cast<MUserUpTime*>(Item->Data);
		// Игнорируем еще не закрытые смены
		if ( UpTime->EndTime==0 ) continue;
        //
		Time+=UpTime->EndTime-UpTime->BeginTime;
		Money+=UpTime->Gains;
	}
	//
	hours=Time/(60*60*10000000i64);
	min=(Time%(60*60*10000000i64))/(60*10000000i64);
	pos=0; if ( hours>0 ) pos=swprintf(
		line+pos, sizeof(line)-pos,
		L"%.2i час. ",
		hours);
	swprintf(
		line+pos, sizeof(line)-pos,
		L"%.2i мин.",
		min);
    LabelSelTime->Caption=line;
    LabelSelMoney->Caption=FloatToStrF(Money,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormUsersUpTime::ListViewUpTimesColumnClick(
      TObject *Sender, TListColumn *Column)
{
    SortMode=Column->Index;
    ListViewUpTimes->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormUsersUpTime::ListViewUpTimesCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
	auto Time1=reinterpret_cast<MUserUpTime*>(Item1->Data);
	auto Time2=reinterpret_cast<MUserUpTime*>(Item2->Data);
	MUsersItem *User1, *User2;
	__int64 time1, time2;

    switch(SortMode)
    {
        case 1:
/*            if ( (User1=Users.Search(Time1->User))&&
                (User2=Users.Search(Time2->User)) )
                Compare=strcmp(User1->Name,User2->Name); */
            Compare=DComp(Time1->User,Time2->User);
			break;

		case 2:
			time1=Time1->BeginTime;
			time2=Time2->BeginTime;
			goto time;

		case 3:
			time1=Time1->EndTime;
			time2=Time2->EndTime;
			goto time;

		case 4:
			time1=Time1->EndTime-Time1->BeginTime;
			time2=Time2->EndTime-Time2->BeginTime;
time:       Compare=DComp(time1,time2);
			break;

		case 5:
			Compare=DComp(Time1->Gains,Time2->Gains);
			break;

		default:
			break;
    }
}
//---------------------------------------------------------------------------

