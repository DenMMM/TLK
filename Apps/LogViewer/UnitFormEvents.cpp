//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
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
    SortMode=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::FormShow(TObject *Sender)
{
    Filter=mcfAll;
    CheckBoxCompAUpd->Checked=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ListViewEvents->Items->Clear();
    ListViewComputers->Items->Clear();
    Action=caFree;
    FormMain->WindowClose(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::ListViewComputersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::ListViewEventsSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    int Time=0;
    double Money=0.;
    int hours, min;
    char line[50];
    int pos;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewEvents->Selected; Item;
        Item=ListViewEvents->GetNextItem(Item,sdAll,is) )
    {
        switch(((MLogRecord*)Item->Data)->TypeID())
        {
            case mlrRun:
                Time+=((MLogRecordRun*)Item->Data)->WorkTime;
                Money+=((MLogRecordRun*)Item->Data)->Cost;
                break;
            default: break;
        }
    }
    //
    hours=Time/60; min=Time%60;
    pos=0; if ( hours>0 ) pos=sprintf(line+pos,"%.2i ���. ",hours);
    sprintf(line+pos,"%.2i ���.",min);
    LabelSelTime->Caption=line;
    LabelSelMoney->Caption=FloatToStrF(Money,ffCurrency,8,2);

    if ( CheckBoxCompAUpd->Checked&&
        Selected&&Item->Focused ) ButtonCompUpdClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::ListViewEventsColumnClick(TObject *Sender,
      TListColumn *Column)
{
    SortMode=Column->Index;
    ListViewEvents->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormEvents::ListViewEventsCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
    MLogRecordRun *RecordRun1, *RecordRun2;
    unsigned Type1, Type2;
    unsigned CompCmds[]={mlrRun,mlrFine,mlrExchange,
        mlrLock,mlrPause,mlrOpen,mlrWtLocker,
        mlrPowerOn,mlrReboot,mlrShutdown};

    Type1=((MLogRecord*)Item1->Data)->TypeID(); RecordRun1=(MLogRecordRun*)Item1->Data;
    Type2=((MLogRecord*)Item2->Data)->TypeID(); RecordRun2=(MLogRecordRun*)Item2->Data;
    switch(SortMode)
    {
        case 1:
            Compare=DComp(RecordRun1->SystemTime,RecordRun2->SystemTime);
            break;
        case 2:
            // ��������� ������� �� ������� � �����������
            Type1=MemSrch(CompCmds,CompCmds+10,Type1)!=NULL;
            Type2=MemSrch(CompCmds,CompCmds+10,Type2)!=NULL;
            // ���������� ������ �����������
            if ( Type1&&Type2 ) Compare=DComp(RecordRun1->Number,RecordRun2->Number);
            else Compare=(Type1||Type2)?(Type1?-1:1):0;
            break;
        case 3:
/*            // ��������� �� ������ ���������� ��������
            Compare=strcmp(Item1->SubItems->Strings[2].c_str(),
                Item2->SubItems->Strings[2].c_str()); */
            if ( Type1==Type2 )
            {
                if ( Type1==mlrRun ) Compare=DComp(RecordRun1->Tariff,RecordRun2->Tariff);
                else if ( Type1==mlrFine ) Compare=DComp(((MLogRecordFine*)RecordRun1)->Fine,
                    ((MLogRecordFine*)RecordRun2)->Fine);
                else if ( Type1==mlrExchange ) Compare=DComp(((MLogRecordExchange*)RecordRun1)->To,
                    ((MLogRecordExchange*)RecordRun2)->To);
                else if ( Type1==mlrLogIn ) Compare=DComp(((MLogRecordLogIn*)RecordRun1)->User,
                    ((MLogRecordLogIn*)RecordRun2)->User);
                else Compare=0;
            } else Compare=DComp(Type1,Type2);
/*            // ��������� � ������ ������ �������� �������, �����������, ������ �����
                Type1==mlrRun?-1:(
                Type2==mlrRun?1:(
                Type1==mlrFine?-1:(
                Type2==mlrFine?1:(
                Type1==mlrLogIn?-1:(
                Type2==mlrLogIn?1:DComp(Type1,Type2)))))); */
            break;
        case 4:
/*            // ��������� �� ������ ���������� ��������
            Compare=strcmp(Item1->SubItems->Count<=3?"":Item1->SubItems->Strings[3].c_str(),
                Item2->SubItems->Count<=3?"":Item2->SubItems->Strings[3].c_str()); */
            if ( Type1==Type2 )
            {
                if ( Type1==mlrRun )
                {
                    if ( (Compare=DComp(RecordRun1->Type,RecordRun2->Type))==0 )
                        switch(RecordRun1->Type)
                        {
                            case mttHours: Compare=DComp(RecordRun1->WorkTime,
                                RecordRun2->WorkTime); break;
                            case mttFlyPacket: Compare=DComp(RecordRun1->SizeTime,
                                 RecordRun2->SizeTime); break;
                            case mttPacket: Compare=DComp(RecordRun1->BeginTime,
                                RecordRun2->BeginTime); break;
                            default: break;
                        }
                } else if ( Type1==mlrFine )
                    Compare=DComp(((MLogRecordFine*)RecordRun1)->Time,
                        ((MLogRecordFine*)RecordRun2)->Time);
                else Compare=0;
            } else Compare=
                Type1==mlrRun?-1:(
                Type2==mlrRun?1:(
                Type1==mlrFine?-1:(
                Type2==mlrFine?1:0)));
            break;
        case 5:
            if ( Type1==Type2 ) Compare=Type1==mlrRun?
                DComp(RecordRun1->Cost,RecordRun2->Cost):0;
            else Compare=
                Type1==mlrRun?-1:(
                Type2==mlrRun?1:0);
            break;
        default: Compare=0; break;
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

    if ( PanelState->Height&&ListViewEvents->ItemFocused&&
//    if ( ListViewEvents->ItemFocused&&
        ProcessComputersState((MLogRecord*)ListViewEvents->ItemFocused->Data,
        &States,&Tariffs) ) UpdateListViewComputers(false,&States,&Tariffs);
}
//---------------------------------------------------------------------------
bool TFormEvents::CheckFilter(MStateInfo *Info_, int FreeTime_)
{
    bool result=false;
    switch(Filter)
    {
        case mcfAll: result=true; break;
        case mcfFree:
            if ( (Info_->State==mcsFree)||
                ((Info_->State&mcsWork)&&(Info_->ToEndWork<FreeTime_)) ) result=true;
            break;
        case mcfService: if ( Info_->State&mcsOpen ) result=true; break;
        default: break;
    }
    return result;
}
//---------------------------------------------------------------------------
void TFormEvents::SetListViewComputersLine(TListItem *Item_, MStateInfo *Info_,
    MTariffs *Tariffs_)
{
    TStrings *SubItems=Item_->SubItems;
    char line[50];
    int icon;

    // ����� ����������
    if ( Info_->Changes&mdcNumber )
    {
        sprintf(line,"%i",Info_->Number);
        SubItems->Strings[0]=line;
    }
    // ����� ������
    if ( Info_->Changes&mdcState )
    {
        unsigned int State=Info_->State;
        if ( State&mcsWtTLK ) { icon=6; strcpy(line,"��� TLK"); }
        else if ( State&mcsOpen ) { icon=5; strcpy(line,"���������"); }
        else if ( State&mcsPause ) { icon=4; strcpy(line,"�������������"); }
        else if ( State&mcsLock ) { icon=3; strcpy(line,"������� !"); }
        else if ( State&mcsFine ) { icon=2; strcpy(line,"����� !!!"); }
        else if ( State&mcsWork ) { icon=1; strcpy(line,"������"); }
        else if ( State&mcsFree ) { icon=0; strcpy(line,"��������"); }
        else { icon=-1; strcpy(line,""); }
        Item_->SubItemImages[1]=icon;
        SubItems->Strings[1]=line;
    }
    // �������� ������
    if ( Info_->Changes&mdcTariff )
    {
        MTariff *Tariff=Tariffs_->Search(Info_->TariffID);
        if ( Tariff ) SubItems->Strings[2]=Tariff->Name;
        else SubItems->Strings[2]="";
    }
    // ����� ������
    if ( Info_->Changes&mdcWorkTime )
    {
        if ( Info_->State&mcsWork )
        {
            sprintf(line,"%i ���. %.2i ���.",Info_->WorkTime/60,Info_->WorkTime%60);
            SubItems->Strings[3]=line;
        } else
        {
            SubItems->Strings[3]="";
            SubItems->Strings[4]="";
            SubItems->Strings[5]="";
        }
    }
    // ������� ������� �������� �������� � �� �������
    if ( Info_->State&mcsWork )
    {
        sprintf(line,"%.2i:%.2i",Info_->ToEndWork/60,Info_->ToEndWork%60);
        SubItems->Strings[4]=line;
        sprintf(line,"%.2i:%.2i",Info_->EndWorkTime/60,Info_->EndWorkTime%60);
        SubItems->Strings[5]=line;
    }
    // ����� ������
    if ( Info_->Changes&mdcFineTime )
    {
        if ( Info_->State&mcsFine )
        {
            sprintf(line,"%i ���.",Info_->FineTime);
            SubItems->Strings[6]=line;
        } else
        {
            SubItems->Strings[6]="";
            SubItems->Strings[7]="";
        }
    }
    // ������� ������� ������ ��������
    if ( Info_->State&mcsFine )
    {
        sprintf(line,"%.2i:%.2i",Info_->ToEndFine/60,Info_->ToEndFine%60);
        SubItems->Strings[7]=line;
    }
}
//---------------------------------------------------------------------------
void TFormEvents::UpdateListViewComputers(bool Full_, MStates *States_, MTariffs *Tariffs_)
{
    TListItem *Item;
    MState *State;
    MStateInfo Info;
    int i;

    // ������� ������, �� �������������� � ���������� ����������
    i=ListViewComputers->Items->Count-1;
    while(i>=0)
    {
        Item=ListViewComputers->Items->Item[i];
        if ( !States_->Search((int)Item->Data) ) Item->Delete();
        i--;
    }

    // ������� �� ������ ����������, �� ���������� ��� ������, � ��������� �����
    for ( State=(MState*)States_->First; State; State=(MState*)State->Next )
    {
        //
        State->StateInfo(&Info);
        //
        Item=ListViewComputers->FindData(0,(void*)Info.Number,true,false);
        // ��������� �������� �� ��������� ��� ������������ ������
        if ( !CheckFilter(&Info,2) )
            { if ( Item ) Item->Delete(); continue; }
        // ���� ��������� �� ������� � ������, �� ��������� ���
        if ( !Item )
        {
            Item=ListViewComputers->Items->Add();
            Item->Data=(void*)Info.Number;
            Info.Changes=mdcAll;
        }
        // ��������� ���������� � �������
        if ( Full_ ) Info.Changes=mdcAll;
        SetListViewComputersLine(Item,&Info,Tariffs_);
    }
}
//---------------------------------------------------------------------------
bool TFormEvents::Open(MLogFile *File_, MLogRecord *Begin_, MLogRecord *End_)
{
    MTariffs Tariffs;
    MFines Fines;
    MUsers Users;
    SYSTEMTIME ss_time;
    char line[256];
    TListItem *Item;

    // ������ ���������
    ListViewEvents->Items->Clear();
    ListViewComputers->Items->Clear();
    //
    if ( Begin_==NULL ) goto error;
    for ( ; Begin_!=End_->Next; Begin_=(MLogRecord*)Begin_->Next )
    {
        // ����� ������
        Item=ListViewEvents->Items->Add();
        Item->ImageIndex=-1;
        Item->Data=Begin_;
        // ����� �������
        if ( Int64ToSystemTime(&((MLogRecordEvent*)Begin_)->SystemTime,&ss_time) )
            sprintf(line,"%4d.%02d.%02d - %02d:%02d:%02d",
                ss_time.wYear,ss_time.wMonth,ss_time.wDay,
                ss_time.wHour,ss_time.wMinute,ss_time.wSecond);
        else *line=0;
        Item->SubItems->Add(line);
        // ��������
        switch(Begin_->TypeID())
        {
            case mlrBegin:
                Item->SubItems->Add("");
                Item->SubItems->Add("���� ���� �����");
                break;
            case mlrEnd:
                Item->SubItems->Add("");
                Item->SubItems->Add("���� ���� ������");
                break;
            case mlrStart:
                Item->SubItems->Add("");
                Item->SubItems->Add("������ ���������� �������");
                break;
            case mlrWork:
                Item->SubItems->Add("");
                Item->SubItems->Add("������ ���������� ��������");
                break;
            case mlrStop:
                Item->SubItems->Add("");
                Item->SubItems->Add("������ ���������� ����������");
                break;

            case mlrConfig:
                Item->SubItems->Add("");
                if ( ((MLogRecordConfig*)Begin_)->Open ) Item->SubItems->Add("��������� �������");
                else  Item->SubItems->Add("��������� �������");
                break;
            case mlrComputers:
                Item->SubItems->Add("");
                Item->SubItems->Add("������� ������ �����������");
                break;
            case mlrTariffs:
                Item->SubItems->Add("");
                Item->SubItems->Add("�������� ������");
                break;
            case mlrFines:
                Item->SubItems->Add("");
                Item->SubItems->Add("�������� ������");
                break;
            case mlrUsers:
                Item->SubItems->Add("");
                Item->SubItems->Add("������� ������ �������������");
                break;
            case mlrOptions:
                Item->SubItems->Add("");
                Item->SubItems->Add("�������� ����� ���������");
                break;

            case mlrLogIn:
                Item->SubItems->Add("");
                MUser *usr;
                usr=Users.Search(((MLogRecordLogIn*)Begin_)->User);
                sprintf(line,"����� �����(�) '%s'",usr?usr->Name:"???");
                Item->SubItems->Add(line);
                break;
            case mlrLogOut:
                Item->SubItems->Add("");
                Item->SubItems->Add("����� ��������");
                break;

            case mlrRun:
                MLogRecordRun *rcdr;
                MTariff *trf;
                rcdr=(MLogRecordRun*)Begin_;
                trf=Tariffs.Search(rcdr->Tariff);
                Item->SubItems->Add(IntToStr(rcdr->Number));
                Item->SubItems->Add(trf?trf->Name:"???");
                switch(rcdr->Type)
                {
                    case mttHours:
                        sprintf(line,"%i ���. %.2i ���.",
                            rcdr->WorkTime/60,rcdr->WorkTime%60);
                        break;
                    case mttFlyPacket:
                        sprintf(line,"�� %i ���. %.2i ���. (%i:%.2i)",
                            rcdr->SizeTime/60,rcdr->SizeTime%60,
                            rcdr->WorkTime/60,rcdr->WorkTime%60);
                        break;
                    case mttPacket:
                        int end;
                        end=rcdr->EndTime>=(24*60)? rcdr->EndTime-24*60: rcdr->EndTime;
                        sprintf(line,"� %i:%.2i �� %i:%.2i (%i:%.2i)",
                            rcdr->BeginTime/60,rcdr->BeginTime%60,end/60,end%60,
                            rcdr->WorkTime/60,rcdr->WorkTime%60);
                        break;
                    default: *line=0; break;
                }
                Item->SubItems->Add(line);
                Item->SubItems->Add(FloatToStrF(rcdr->Cost,ffCurrency,8,2));
                break;
            case mlrFine:
                MLogRecordFine *rcdf;
                MFine *fn;
                rcdf=(MLogRecordFine*)Begin_;
                Item->SubItems->Add(IntToStr(rcdf->Number));
                fn=Fines.Search(rcdf->Fine);
                sprintf(line,"����� '%s'",fn?fn->Description:"???");
                Item->SubItems->Add(line);
                if ( rcdf->Time==(24*60) ) sprintf(line,"��� �����");
                else if ( rcdf->Time<0 ) sprintf(line,"����� %i ���.",-rcdf->Time);
                else sprintf(line,"�������� %i ���.",rcdf->Time);
                Item->SubItems->Add(line);
                break;
            case mlrExchange:
                MLogRecordExchange *rcde;
                rcde=(MLogRecordExchange*)Begin_;
                Item->SubItems->Add(IntToStr(rcde->From));
                sprintf(line,"��������� �� �%i",rcde->To);
                Item->SubItems->Add(line);
                break;
            case mlrLock:
                MLogRecordLock *rcdl;
                rcdl=(MLogRecordLock*)Begin_;
                Item->SubItems->Add(IntToStr(rcdl->Number));
                Item->SubItems->Add(rcdl->Apply?"�������":"������");
                break;
            case mlrPause:
                MLogRecordPause *rcdp;
                rcdp=(MLogRecordPause*)Begin_;
                Item->SubItems->Add(IntToStr(rcdp->Number));
                Item->SubItems->Add(rcdl->Apply?"����� ��������������":"����� ��������");
                break;
            case mlrOpen:
                MLogRecordOpen *rcdo;
                rcdo=(MLogRecordOpen*)Begin_;
                Item->SubItems->Add(IntToStr(rcdo->Number));
                Item->SubItems->Add(rcdl->Apply?"������ ��� ���������":"������ ����� ���������");
                break;
            case mlrWtLocker:
                break;
            case mlrPowerOn:
                MLogRecordPowerOn *rcdpwr;
                rcdpwr=(MLogRecordPowerOn*)Begin_;
                Item->SubItems->Add(IntToStr(rcdpwr->Number));
                Item->SubItems->Add("������� �� ���������");
                break;
            case mlrReboot:
                MLogRecordReboot *rcdprb;
                rcdprb=(MLogRecordReboot*)Begin_;
                Item->SubItems->Add(IntToStr(rcdprb->Number));
                Item->SubItems->Add("������� �� ������������");
                break;
            case mlrShutdown:
                MLogRecordShutdown *rcdpsd;
                rcdpsd=(MLogRecordShutdown*)Begin_;
                Item->SubItems->Add(IntToStr(rcdpsd->Number));
                Item->SubItems->Add("������� �� ����������");
                break;

            case mlrDataShellState:
                Item->SubItems->Add("");
                Item->SubItems->Add("������ �� ������ ����������");
                break;
            case mlrDataStates:
                Item->SubItems->Add("");
                Item->SubItems->Add("������ �� �����������");
                break;
            case mlrDataTariffs:
                Item->SubItems->Add("");
                Item->SubItems->Add("������ �� �������");
                MLogRecordDataTariffs *rcddtrf;
                rcddtrf=(MLogRecordDataTariffs*)Begin_;
                Tariffs.Clear();
                for ( unsigned i=0; i<rcddtrf->NumTariffs; i++ )
                {
                    trf=(MTariff*)Tariffs.Add();
                    trf->SetTariffData(rcddtrf->Tariffs+i);
                }
                break;
            case mlrDataFines:
                Item->SubItems->Add("");
                Item->SubItems->Add("������ �� �������");
                MLogRecordDataFines *rcddfn;
                rcddfn=(MLogRecordDataFines*)Begin_;
                Fines.Clear();
                for ( unsigned i=0; i<rcddfn->NumFines; i++ )
                {
                    fn=(MFine*)Fines.Add();
                    fn->SetFineData(rcddfn->Fines+i);
                }
                break;
            case mlrDataUsers:
                Item->SubItems->Add("");
                Item->SubItems->Add("������ �� �������������");
                MLogRecordDataUsers *rcddusr;
                rcddusr=(MLogRecordDataUsers*)Begin_;
                Users.Clear();
                for ( unsigned i=0; i<rcddusr->NumUsers; i++ )
                {
                    usr=(MUser*)Users.Add();
                    usr->SetUserData(rcddusr->Users+i);
                }
                break;
            default: break;
        }
    }

    Caption="�������  -  "; Caption=Caption+File_->Name;
    FormMain->WindowOpen(File_,this);
    FormMain->WindowCaption(this,"�������");
    return true;
error:
    Close();
    return false;
}
//---------------------------------------------------------------------------







