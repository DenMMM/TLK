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
bool TFormUsersUpTime::Open(MLogFile *File_, MLogRecord *Begin_, MLogRecord *End_)
{
    MUser *user;
    __int64 time;
    int hours, min;
    char line[50];
    int pos;
    SYSTEMTIME ss_time_b, ss_time_e;
    TListItem *Item;

    ProcessUsersUpTime(Begin_,End_,&Users,&Times);
    ListViewUpTimes->Items->Clear();
    for ( MUserUpTime *Time=(MUserUpTime*)Times.gFirst();
        Time; Time=(MUserUpTime*)Time->gNext() )
    {
        Item=ListViewUpTimes->Items->Add();
        Item->ImageIndex=-1;
        Item->Data=(void*)Time;
        if ( (user=(MUser*)Users.SrchID(Time->User))==NULL ) Item->SubItems->Add("");
        else Item->SubItems->Add(user->Name.c_str());
        // Время
        if ( Int64ToSystemTime(&Time->BeginTime,&ss_time_b) )
                sprintf(line,"%4d.%02d.%02d - %02d:%02d:%02d",
                ss_time_b.wYear,ss_time_b.wMonth,ss_time_b.wDay,
                ss_time_b.wHour,ss_time_b.wMinute,ss_time_b.wSecond);
        else *line=0;
        Item->SubItems->Add(line);
        //
        if ( (Time->EndTime!=0)&&(Int64ToSystemTime(&Time->EndTime,&ss_time_e)) )
        {
            pos=0;
            if ( ss_time_b.wYear!=ss_time_e.wYear ) goto year;
            if ( ss_time_b.wMonth!=ss_time_e.wMonth ) goto month;
            if ( ss_time_b.wDay!=ss_time_e.wDay )
            {
                goto day;
year:           pos+=sprintf(line+pos,"%4d.",ss_time_e.wYear);
month:          pos+=sprintf(line+pos,"%02d.",ss_time_e.wMonth);
day:            pos+=sprintf(line+pos,"%02d - ",ss_time_e.wDay);
            }
            sprintf(line+pos,"%02d:%02d:%02d",
                ss_time_e.wHour,ss_time_e.wMinute,ss_time_e.wSecond);
            Item->SubItems->Add(line);
            //
            time=Time->EndTime-Time->BeginTime;
            hours=time/(60*60*10000000i64);
            min=(time%(60*60*10000000i64))/(60*10000000i64);
            pos=0; if ( hours>0 ) pos=sprintf(line+pos,"%.2i час. ",hours);
            sprintf(line+pos,"%.2i мин.",min);
            Item->SubItems->Add(line);
        } else
        {
            Item->SubItems->Add("");
            Item->SubItems->Add("");
        }
        //
        Item->SubItems->Add(FloatToStrF(Time->Gains,ffCurrency,8,2));
    }

    Caption="Смены  -  "; Caption=Caption+File_->Name;
    FormMain->WindowOpen(File_,this);
    FormMain->WindowCaption(this,"Смены");
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsersUpTime::ListViewUpTimesSelectItem(
      TObject *Sender, TListItem *Item, bool Selected)
{
    MUserUpTime *UpTime;
    __int64 Time=0;
    double Money=0.;
    int hours, min;
    char line[50];
    int pos;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewUpTimes->Selected; Item;
        Item=ListViewUpTimes->GetNextItem(Item,sdAll,is) )
    {
        UpTime=(MUserUpTime*)Item->Data;
        if ( UpTime->EndTime==0 ) continue;
        Time+=UpTime->EndTime-UpTime->BeginTime;
        Money+=UpTime->Gains;
    }
    //
    hours=Time/(60*60*10000000i64);
    min=(Time%(60*60*10000000i64))/(60*10000000i64);
    pos=0; if ( hours>0 ) pos=sprintf(line+pos,"%.2i час. ",hours);
    sprintf(line+pos,"%.2i мин.",min);
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
    MUserUpTime *Time1, *Time2;
    MUser *User1, *User2;
    __int64 time1, time2;

    Time1=(MUserUpTime*)Item1->Data;
    Time2=(MUserUpTime*)Item2->Data;
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
        default: break;
    }
}
//---------------------------------------------------------------------------

