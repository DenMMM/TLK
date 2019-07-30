//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include "UnitFormMain.h"
#include "UnitFormComputers.h"
#include "UnitFormTariffs.h"
#include "UnitFormFines.h"
#include "UnitFormUsers.h"
#include "UnitFormComputersConfig.h"
#include "UnitFormOrder.h"
#include "UnitFormFine.h"
#include "UnitFormLogIn.h"
#include "UnitFormChangePassword.h"
#include "UnitCommon.h"
#include "UnitOptionsLoadSave.h"
#include "UnitLogs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Trayicon"
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
MComputers *Computers;
MTariffs *Tariffs;
MFines *Fines;
MUsers *Users;
MState *State;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    NComputers->Enabled=false;
    NTariffs->Enabled=false;
    NFines->Enabled=false;
    NUsers->Enabled=false;
    BitBtnNewOrder->Enabled=false;
    BitBtnFine->Enabled=false;
    BitBtnLock->Enabled=false;
    BitBtnUnLock->Enabled=false;
    NPause->Enabled=false;
    NRun->Enabled=false;
    NReboot->Enabled=false;
    NShutdown->Enabled=false;
    NAutoMode->Enabled=false;
    NNormalMode->Enabled=false;
    NWithoutLocker->Enabled=false;
    NWithLocker->Enabled=false;
    NComputersConfig->Enabled=false;

    WSADATA WSAData;
    if ( ::WSAStartup(0x0002,&WSAData)!=0 )
    {
        ::MessageBox(Application->Handle,
            "Не установлена требуемая версия библиотеки WinSock или протокол TCP/IP.\nПрограмма не может быть запущена !",
            "TimeLocker - ошибка",MB_OK|MB_ICONERROR|MB_APPLMODAL);
        Close(); return;
    }

    Computers=new MComputers; ::InitializeCriticalSection(&CS_ComputersStateFile);
    Tariffs=new MTariffs;
    Fines=new MFines;
    Users=new MUsers;
    State=new MState;

    PrgDir=ExtractFilePath(Application->ExeName);

    if ( !State->Load() )
    {
        ::MessageBox(Application->Handle,"Служебные данные повреждены !\nЗапуск программы невозможен.",
            "TimeLocker - ошибка",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
        State->CurrentUser=NULL; Close(); return;
    }
    if ( !Computers->Load() )
    {
        ::MessageBox(Application->Handle,"Ошибка при загрузке списка компьютеров !",
            "TimeLocker - предупреждение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
    }
    if ( !TariffsLoad(Tariffs,ImageListIcons) )
    {
        ::MessageBox(Application->Handle,"Ошибка при загрузке списка тарифов !",
            "TimeLocker - предупреждение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
    }
    if ( !FinesLoad(Fines) )
    {
        ::MessageBox(Application->Handle,"Ошибка при загрузке списка штрафов !",
            "TimeLocker - предупреждение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
    }
    if ( !UsersLoad(Users) )
    {
        ::MessageBox(Application->Handle,"Ошибка при загрузке списка пользователей !",
            "TimeLocker - предупреждение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
    }
    State->CurrentUser=Users->SearchID((unsigned int)State->CurrentUser);
    State->CurrentAdmin=Users->SearchID((unsigned int)State->CurrentAdmin);

    if ( State->CurrentAdmin ) StatusBar->Panels->Items[0]->Text="Админ: "+State->CurrentAdmin->Login;
    if ( State->CurrentUser==NULL )
    {
        FormLogIn->Left=(Screen->Width-FormLogIn->Width)/2;
        FormLogIn->Top=(Screen->Height-FormLogIn->Height)/2;
        if ( FormLogIn->ShowModal()!=mrOk ) { Close(); return; }
    } else
    {
        SetAccess(State->CurrentUser);
        Caption="TimeLocker - "+State->CurrentUser->Login+" ("+State->CurrentUser->JobTitle+")";
    }

    NFilterUsedClick(NFilterUsed);
    TimerTimer(Timer);
    Timer->Enabled=true;

    if ( State->CurrentAdmin ) LogCompRun(NULL,(MTariff*)9,0,0.);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormHide(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    // Если ни один пользователь не вошел в программу, то и спрашивать некого
    if ( State->CurrentUser==NULL ) return;
    // Спрашиваем подтверждение на закрытие программы
    if ( ::MessageBox(Application->Handle,"Вы уверены, что хотите закрыть TimeLocker ?",
        "TimeLocker",MB_YESNO|MB_ICONQUESTION|MB_APPLMODAL)!=IDYES ) CanClose=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    if ( State->CurrentAdmin ) LogCompRun(NULL,(MTariff*)10,0,0.);

    Timer->Enabled=false;
    delete Computers; ::DeleteCriticalSection(&CS_ComputersStateFile);
    ::WSACleanup();
    delete Tariffs;
    delete Fines;
    delete Users;
    delete State;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if ( !Shift.Contains(ssCtrl) ) return;
    switch(Key)
    {
        case 'R': if ( BitBtnNewOrder->Enabled ) BitBtnNewOrderClick(BitBtnNewOrder); break;
        case 'A': if ( BitBtnAddTime->Enabled ) BitBtnNewOrderClick(BitBtnAddTime); break;
        case 'F': if ( BitBtnFine->Enabled ) BitBtnNewOrderClick(BitBtnFine); break;
        default: break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ApplicationEventsHint(TObject *Sender)
{
    StatusBar->SimpleText=Application->Hint;
    StatusBar->SimplePanel=(Application->Hint!="");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
    static unsigned int timer_1000msec=0;
    static unsigned int timer_10000msec=0;

    if ( (++timer_1000msec)>=(1000/Timer->Interval) )
    {
        timer_1000msec=0;
        // Выводим текущее системное время
        TDate SystemTime=Time();
        LabelTimeSecs->Caption=SystemTime.FormatString("ss");
        LabelTimeHoursMins->Caption=SystemTime.FormatString("hh : mm");
        //
        Computers->Timer();
    }
    // Обновляем информацию об оставшемся времени работы и времени до окончания штрафа
    if ( (++timer_10000msec)>=(10000/Timer->Interval) )
    {
        timer_10000msec=0;
        TListItem *Item;
        MComputer *Computer;
        unsigned int SetColumns;
        TListItems *Items=ListViewComputers->Items;
        for ( int i=0, j=Items->Count; i<j; i++ )
        {
            Item=Items->Item[i]; Computer=(MComputer*)Item->Data;
            if ( !(Computer->State&mcsWork) ) continue;
            SetColumns=0;
            if ( !(Computer->State&(mcsPause|mcsAuto)) ) SetColumns|=mlcWorkTimeToEnd;
            if ( Computer->State&mcsFine ) SetColumns|=mlcFineTimeToEnd;
            Computer->Lock();
            SetListViewComputersLine(Item,SetColumns);
            Computer->UnLock();
        }
    }
    // Обновляем другую информацию о состоянии компьютеров
    for ( MComputer *Computer=(MComputer*)Computers->FirstItem; Computer;
        Computer=(MComputer*)Computer->NextItem )
    {
        unsigned int SetColumns, DataChanges;
        SetColumns=0;
        Computer->Lock();
        DataChanges=Computer->DataChanges;
        if ( DataChanges&mdcState ) SetColumns|=mlcState;
        if ( DataChanges&mdcTariff ) SetColumns|=mlcTariff|mlcWorkTime|mlcWorkTimeToEnd;
        if ( DataChanges&mdcWorkTime ) SetColumns|=mlcWorkTime|mlcWorkTimeToEnd;
        if ( DataChanges&mdcFineTime ) SetColumns|=mlcFineTime|mlcFineTimeToEnd;
        if ( DataChanges&mdcNetState ) SetColumns|=mlcNet;
        Computer->DataChanges=0;
        CorrectListViewComputers(Computer,SetColumns);
        //
        if ( SetColumns&mlcNet )
        {
            TListItem *Item;
            Item=FormComputersConfig->ListViewComputers->FindData(0,Computer,true,false);
            if ( Item ) FormComputersConfig->SetListViewComputersLine(Item);
        }
        Computer->UnLock();
    }
    //
    ProgressBarNetProcess->Position=Computers->GetPosition();

    // Определяем время, через которое будет необходимое количество компьютеров
    double SysTime=(double)(Date()+Time());
    MListOfComputers *ListOfComputers=new MListOfComputers;
    for ( MComputer *Computer=(MComputer*)Computers->FirstItem; Computer;
        Computer=(MComputer*)Computer->NextItem )
    {
        if ( (!(Computer->State&(mcsFree|mcsWork)))||
            (Computer->State&(mcsAuto|mcsPause)) ) continue;
        MListOfComputersComputer *NewComputer;
        NewComputer=(MListOfComputersComputer*)ListOfComputers->Add();
        NewComputer->Computer=Computer;
    }
    for ( MListOfComputersComputer *Computer1=
        (MListOfComputersComputer*)ListOfComputers->FirstItem; Computer1;
        Computer1=(MListOfComputersComputer*)Computer1->NextItem )
    {
        for ( MListOfComputersComputer *Computer2=
            (MListOfComputersComputer*)Computer1->NextItem; Computer2;
            Computer2=(MListOfComputersComputer*)Computer2->NextItem )
        {
            MComputer *Comp1, *Comp2;
            int TimeToEnd1, TimeToEnd2;
            Comp1=Computer1->Computer;
            Comp2=Computer2->Computer;
            if ( Comp1->State&mcsFree ) TimeToEnd1=0;
            else TimeToEnd1=((Comp1->StartWorkTime+Comp1->SizeWorkTime/(24.*60))-SysTime)*(24*60);
            if ( Comp2->State&mcsFree ) TimeToEnd2=0;
            else TimeToEnd2=((Comp2->StartWorkTime+Comp2->SizeWorkTime/(24.*60))-SysTime)*(24*60);
            if ( TimeToEnd2<TimeToEnd1 )
            {
                Comp1=Computer1->Computer;
                Computer1->Computer=Computer2->Computer;
                Computer2->Computer=Comp1;
            }
        }
    }
    {
        int Time;
        MListOfComputersComputer *Computer;
        Computer=(MListOfComputersComputer*)ListOfComputers->Item(0);
        if ( Computer==NULL ) LabelMinTime->Caption="...";
        else
        {
            if ( Computer->Computer->State&mcsFree ) Time=0;
            else Time=((Computer->Computer->StartWorkTime+Computer->Computer->SizeWorkTime/
                (24.*60))-SysTime)*(24*60);
            LabelMinTime->Caption=IntToStr(Time/60)+" час. "+IntToStr(Time%60)+" мин.";
        }
        Computer=(MListOfComputersComputer*)ListOfComputers->Item(ComboBoxNeedCompNum->ItemIndex);
        if ( Computer==NULL ) LabelMaxTime->Caption="...";
        else
        {
            if ( Computer->Computer->State&mcsFree ) Time=0;
            else Time=((Computer->Computer->StartWorkTime+Computer->Computer->SizeWorkTime/
                (24.*60))-SysTime)*(24*60);
            LabelMaxTime->Caption=IntToStr(Time/60)+" час. "+IntToStr(Time%60)+" мин.";
        }
    }
    {
        AnsiString WaitCompLine;
        int i=ComboBoxNeedCompNum->ItemIndex;
        for ( MListOfComputersComputer *Computer=
            (MListOfComputersComputer*)ListOfComputers->FirstItem; Computer&&(i>=0);
            Computer=(MListOfComputersComputer*)Computer->NextItem, i-- )
            WaitCompLine+=IntToStr(Computer->Computer->Number)+",";
        LabelNeedComputers->Caption=WaitCompLine;
    }
    delete ListOfComputers;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnNewOrderClick(TObject *Sender)
{
    TPoint local_button_coord;
    TForm *show_form;

    if ( Sender==BitBtnAlarm );  // show_form=FormAlarm;
    else if ( Sender==BitBtnNewOrder ) show_form=FormOrder;
    else if ( Sender==BitBtnAddTime );  // show_form=FormAddTime;
    else if ( Sender==BitBtnFine ) show_form=FormFine;
    else return;

    local_button_coord.x=PanelButtons->Left+BitBtnNewOrder->Left-show_form->Width-2;
    local_button_coord.y=PanelButtons->Top+BitBtnNewOrder->Top-40;
    local_button_coord=ClientToScreen(local_button_coord);
    show_form->Left=local_button_coord.x;
    show_form->Top=local_button_coord.y;
    show_form->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NToTrayClick(TObject *Sender)
{
    TrayIcon->Minimize();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLockClick(TObject *Sender)
{
//    FormLocked->Left=Left+(Width-FormLocked->Width)/2;
//    FormLocked->Top=Top+(Height-FormLocked->Height)/2;
//    FormLocked->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TrayIconMinimize(TObject *Sender)
{
    TrayIcon->Hide=false;
    TrayIcon->Visible=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TrayIconRestore(TObject *Sender)
{
    TrayIcon->Hide=true;
    TrayIcon->Visible=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogInClick(TObject *Sender)
{
    FormLogIn->Left=Left+(Width-FormLogIn->Width)/2;
    FormLogIn->Top=Top+(Height-FormLogIn->Height)/2;
    FormLogIn->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnViewFilterClick(TObject *Sender)
{
    TPoint local_button_coord;

    local_button_coord.x=PanelButtons->Left+BitBtnViewFilter->Left+4;
    local_button_coord.y=PanelButtons->Top+BitBtnViewFilter->Top+
        BitBtnViewFilter->Height-4;
    local_button_coord=ClientToScreen(local_button_coord);
    PopupMenuViewFilter->Popup(local_button_coord.x,local_button_coord.y);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnAdditionalsClick(TObject *Sender)
{
    TPoint local_button_coord;

    local_button_coord.x=PanelButtons->Left+BitBtnAdditionals->Left+4;
    local_button_coord.y=PanelButtons->Top+BitBtnAdditionals->Top+
        BitBtnAdditionals->Height-4;
    local_button_coord=ClientToScreen(local_button_coord);
    PopupMenuAdditionals->Popup(local_button_coord.x,local_button_coord.y);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NFilterUsedClick(TObject *Sender)
{
    unsigned int Filter;
    if ( Sender==NFilterUsed ) Filter=mcfUsed;
    else if ( Sender==NFilterUsed ) Filter=mcfUsed;
    else if ( Sender==NFilterFree ) Filter=mcfFree;
    else if ( Sender==NFilterOccupied ) Filter=mcfOccupied;
    else if ( Sender==NFilterAll ) Filter=mcfAll;
    else return;
    ((TMenuItem*)Sender)->Default=true;
    ((TMenuItem*)Sender)->Checked=true;
    ComputersFilter=Filter;
    CreateListViewComputers();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NComputersClick(TObject *Sender)
{
    TForm *form;
    if ( Sender==NComputers ) form=FormComputers;
    else if ( Sender==NTariffs ) form=FormTariffs;
    else if ( Sender==NFines ) form=FormFines;
    else if ( Sender==NUsers ) form=FormUsers;
    else if ( Sender==NComputersConfig ) form=FormComputersConfig;
    else if ( Sender==NChangePassword ) form=FormChangePassword;
    else return;

    form->Left=Left+30;
    form->Top=Top+50;
    form->ShowModal();
}
//---------------------------------------------------------------------------
unsigned int __fastcall TFormMain::ComputersFilterMask()
{
    unsigned int FMask;
    switch(ComputersFilter)
    {
        case mcfAll: FMask=mcsFree|mcsWork|mcsWtLocker|mcsNotUse; break;
        case mcfUsed: FMask=mcsFree|mcsWork; break;
        case mcfFree: FMask=mcsFree; break;
        case mcfOccupied: FMask=mcsWork; break;
        default: FMask=0; break;
    }
    return FMask;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetListViewComputersLine(TListItem *Item_, unsigned int Columns_)
{
    TStrings *SubItems=Item_->SubItems;
    MComputer *Computer=(MComputer*)Item_->Data;
    int icon_number;
    char line[64];
    double SystemTime=(double)(Date()+Time());

    // Цвет группы и номер компьютера
    if ( Columns_&mlcNumber )
    {
        Item_->SubItemImages[0]=Computer->GroupColor;
        SubItems->Strings[0]=IntToStr(Computer->Number);
    }
    // Состояние сети
    if ( Columns_&mlcNet )
    {
        unsigned int NetSync=Computer->NetSync|Computer->NotSync;
        if ( NetSync&mnsState ) icon_number=12;
        else if ( NetSync&mnsConfig ) icon_number=13;
        else if ( NetSync&mnsGames ) icon_number=14;
        else if ( Computer->NetPresent ) icon_number=-1;
        else icon_number=15;
        Item_->SubItemImages[1]=icon_number;
    }
    // Режим работы компьютера
    if ( Columns_&mlcState )
    {
        if ( Computer->State&mcsNotUse ) { icon_number=11; strcpy(line,"не используется"); }
        else if ( Computer->State&mcsWtLocker ) { icon_number=10; strcpy(line,"Без TimeLocker'а"); }
        else if ( Computer->State&mcsAuto ) { icon_number=9; strcpy(line,"настройка"); }
        else if ( Computer->State&mcsPause ) { icon_number=8; strcpy(line,"приостановлен"); }
        else if ( Computer->State&mcsLock ) { icon_number=7; strcpy(line,"Прикрыт !"); }
        else if ( Computer->State&mcsFine ) { icon_number=6; strcpy(line,"Штраф !!!"); }
        else if ( Computer->State&mcsWork ) { icon_number=5; strcpy(line,"Работа"); }
        else if ( Computer->State&mcsFree ) { icon_number=4; strcpy(line,"СВОБОДЕН"); }
        else { icon_number=0; strcpy(line,"??????????"); }
        Item_->SubItemImages[2]=icon_number;
        SubItems->Strings[2]=line;
    }
    // Тариф
    if ( Columns_&mlcTariff )
    {
        MTariff *Tariff=Tariffs->SearchID(Computer->TariffID);
        if ( Tariff ) { icon_number=Tariff->IconNumber; strcpy(line,Tariff->Name.c_str()); }
        else { icon_number=-1; strcpy(line,""); }
        Item_->SubItemImages[3]=icon_number;
        SubItems->Strings[3]=line;
    }
    // Время работы
    if ( Columns_&(mlcWorkTime|mlcTariff) )
    {
        if (Computer->SizeWorkTime) sprintf(line,"%d час. %02d мин.",
            Computer->SizeWorkTime/60,Computer->SizeWorkTime%60);
        else strcpy(line,"");
        SubItems->Strings[4]=line;
    }
    // Сколько времени осталось и до скольки
    if ( Columns_&(mlcWorkTimeToEnd|mlcWorkTime|mlcTariff) )
    {
        if ( Computer->SizeWorkTime )
        {
            double endtime=Computer->StartWorkTime+Computer->SizeWorkTime/(double)(24*60);
            if ( Computer->State&(mcsPause|mcsAuto) ) endtime+=SystemTime-Computer->StopTimerTime;
            SubItems->Strings[5]=((TDate)(endtime-SystemTime)).FormatString("hh:nn");
            SubItems->Strings[6]=((TDate)endtime).FormatString("hh:nn");
        } else { SubItems->Strings[5]=""; SubItems->Strings[6]=""; }
    }
    // Время штрафа и сколько он еще продлится
    if ( Columns_&(mlcFineTime|mlcTariff) )
    {
        if ( Computer->SizeFineTime ) SubItems->Strings[7]=IntToStr(Computer->SizeFineTime)+" мин.";
        else SubItems->Strings[7]="";
    }
    // Сколько еще продлится штраф
    if ( Columns_&(mlcFineTimeToEnd|mlcFineTime|mlcTariff) )
    {
        if ( Computer->SizeFineTime ) SubItems->Strings[8]=((TDate)(Computer->StartFineTime+
            Computer->SizeFineTime/(24.*60)-SystemTime)).FormatString("hh:nn");
        else SubItems->Strings[8]="";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::CreateListViewComputers()
{
    unsigned int FMask=ComputersFilterMask();
    int CompNum=1;
    ListViewComputers->Items->Clear();
    ComboBoxNeedCompNum->Items->Clear();
    for ( MComputer *Computer=(MComputer*)Computers->FirstItem; Computer;
        Computer=(MComputer*)Computer->NextItem )
    {
        ComboBoxNeedCompNum->Items->Add(IntToStr(CompNum)); CompNum++;
        if ( !(Computer->State&FMask) ) continue;
        TListItem *NewItem;
        NewItem=ListViewComputers->Items->Add();
        NewItem->Data=(void*)Computer;
        Computer->Lock();
        SetListViewComputersLine(NewItem,mlcAll);
        Computer->UnLock();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::CorrectListViewComputers(MComputer *Computer_, unsigned int Columns_)
{
    TListItem *Item;
    unsigned int FMask=ComputersFilterMask();

    // Проверка наличия компьютера в списке
    Item=ListViewComputers->FindData(0,(void*)Computer_,true,false);
    // Если компьютер есть, то проверяем не надо ли его удалить из списка
    if ( Item )
    {
        if ( !(Computer_->State&FMask) ) Item->Delete();
        else SetListViewComputersLine(Item,Columns_);
    } else
    {
        // Проверяем можно ли добавить компьютер в список
        if ( Computer_->State&FMask )
        {
            int i=0;
            TListItems *Items=ListViewComputers->Items;
            // Определяем под каким номером компьютер должен быть добавлен в список
            for ( MComputer *Computer=(MComputer*)Computers->FirstItem; Computer;
                Computer=(MComputer*)Computer->NextItem )
            {
                if ( Computer==Computer_ ) break;
                if ( Computer==Items->Item[i]->Data ) i++;
            }
            // Добавляем (вставляем) компьютер в список
            Item=ListViewComputers->Items->Insert(i);
            Item->Data=(void*)Computer_;
            SetListViewComputersLine(Item,Columns_);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnExchangeClick(TObject *Sender)
{
    if ( ListViewComputers->SelCount!=2 )
    {
        ::MessageBox(Application->Handle,"Нужно выбрать ДВА компьютера !",
            "TimeLocker - ошибка",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
        return;
    }

    TItemStates is=TItemStates()<<isSelected;
    TListItem *Item1, *Item2;
    Item1=ListViewComputers->Selected;
    Item2=ListViewComputers->GetNextItem(Item1,sdAll,is);
    MComputer *Computer1, *Computer2;
    Computer1=(MComputer*)Item1->Data;
    Computer2=(MComputer*)Item2->Data;

    //
    if ( (Computer1->SizeWorkTime&&Computer2->SizeWorkTime)||
        (!(Computer1->SizeWorkTime||Computer2->SizeWorkTime)) ) return;

    //
    MTariff *Tariff;
    if ( Computer1->SizeWorkTime )
    {
        Tariff=(MTariff*)Tariffs->SearchID(Computer1->TariffID);
        if ( !Tariff->Computers.SearchID(Computer2->ID) ) return;
    } else
    {
        Tariff=(MTariff*)Tariffs->SearchID(Computer2->TariffID);
        if ( !Tariff->Computers.SearchID(Computer1->ID) ) return;
    }

    // Пометка в логе
    if ( State->CurrentAdmin )
    {
        bool result;
        if ( Computer1->SizeWorkTime )
        {
            result=LogCompRun(Computer1,(MTariff*)12,0,0.)&&
                LogCompRun(Computer2,(MTariff*)13,0,0.);
        } else
        {
            result=LogCompRun(Computer2,(MTariff*)12,0,0.)&&
                LogCompRun(Computer1,(MTariff*)13,0,0.);
        }
        if ( !result ) return;
    }
    // Переносим режимы работы компьютеров
    Computer1->CmdExchange(Computer2);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnLockClick(TObject *Sender)
{
    bool lock;
    if ( Sender==BitBtnLock ) lock=true;
    else if ( Sender==BitBtnUnLock ) lock=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MComputer *Computer;
        Computer=(MComputer*)Item->Data;
        // Помечаем в логе
        if ( State->CurrentAdmin )
        {
            if ( lock ) { if ( !LogCompRun(Computer,(MTariff*)14,0,0.) ) return; }
            else { if ( !LogCompRun(Computer,(MTariff*)15,0,0.) ) return; }
        }
        Computer->CmdLock(lock);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NPauseClick(TObject *Sender)
{
    bool pause;
    if ( Sender==NPause ) pause=true;
    else if ( Sender==NRun ) pause=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MComputer *Computer;
        Computer=(MComputer*)Item->Data;
        // Помечаем в логе
        if ( State->CurrentAdmin )
        {
            if ( pause ) { if ( !LogCompRun(Computer,(MTariff*)16,0,0.) ) return; }
            else { if ( !LogCompRun(Computer,(MTariff*)17,0,0.) ) return; }
        }
        Computer->CmdPause(pause);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NAutoModeClick(TObject *Sender)
{
    bool automode;
    if ( Sender==NAutoMode ) automode=true;
    else if ( Sender==NNormalMode ) automode=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MComputer *Computer;
        Computer=(MComputer*)Item->Data;
        // Пометка в логе о переводе компьютера в автономный режим
        if ( State->CurrentAdmin )
        {
            bool result;
            if ( automode ) result=LogCompRun(Computer,(MTariff*)1,0,0.);
            else result=LogCompRun(Computer,(MTariff*)2,0,0);
            if ( !result ) continue;
        }
        Computer->CmdAuto(automode);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NWithoutLockerClick(TObject *Sender)
{
    bool wtlocker;
    if ( Sender==NWithoutLocker ) wtlocker=true;
    else if ( Sender==NWithLocker ) wtlocker=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MComputer *Computer;
        Computer=(MComputer*)Item->Data;
        // Пометка в логе о переводе компьютера в автономный режим
        if ( State->CurrentAdmin )
        {
            bool result;
            if ( wtlocker ) result=LogCompRun(Computer,(MTariff*)3,0,0.);
            else result=LogCompRun(Computer,(MTariff*)4,0,0);
            if ( !result ) return;
        }
        Computer->CmdWtLocker(wtlocker);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NNotInUseClick(TObject *Sender)
{
/*    bool notuse;
    if ( Sender==NNotInUse ) notuse=true;
    else if ( Sender==NInUse ) notuse=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
        ((MComputer*)Item->Data)->CmdNotUse(notuse); */
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewComputersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->SubItems->Add("");
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
void __fastcall TFormMain::NLogOutClick(TObject *Sender)
{
    if ( (State->CurrentUser!=NULL)&&(State->CurrentUser==State->CurrentAdmin) )
    {
        // Заканчиваем смену
        LogClose(); State->CurrentAdmin=NULL; StatusBar->Panels->Items[0]->Text="";
        ::MessageBox(Application->Handle,"Смена завершена.",
            "TimeLocker - сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
    } else if ( State->CurrentAdmin ) LogCompRun(NULL,(MTariff*)6,0,0.);
    State->CurrentUser=NULL; State->Save();
    Caption="TimeLocker";
    FormLogIn->Left=Left+(Width-FormLogIn->Width)/2;
    FormLogIn->Top=Top+(Height-FormLogIn->Height)/2;
    if ( FormLogIn->ShowModal()!=mrOk ) Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NAboutClick(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetAccess(MUser *User_)
{
    unsigned int rights;
    bool enabled;

    if ( User_!=NULL ) rights=User_->Rights;
    // Настройки, доступные только сисадмину
    enabled=!User_;
    NComputers->Enabled=enabled;
    NTariffs->Enabled=enabled;
    NFines->Enabled=enabled;
    NUsers->Enabled=enabled;
    // Обслуживание клиентов
    enabled=(!User_)||(rights&murClientsServices);
    BitBtnNewOrder->Enabled=enabled;
    BitBtnFine->Enabled=enabled;
    BitBtnLock->Enabled=enabled;
    BitBtnUnLock->Enabled=enabled;
    NReboot->Enabled=enabled;
    NShutdown->Enabled=enabled;
    NPause->Enabled=enabled;
    NRun->Enabled=enabled;
    // Функции, требующиеся при обслуживании компьютеров
    enabled=(!User_)||(rights&murComputersServices);
    NReboot->Enabled=NReboot->Enabled||enabled;
    NShutdown->Enabled=NShutdown->Enabled||enabled;
    NAutoMode->Enabled=enabled;
    NNormalMode->Enabled=enabled;
    NWithoutLocker->Enabled=enabled;
    NWithLocker->Enabled=enabled;
    NComputersConfig->Enabled=enabled;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NRebootClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MComputer *Computer;
        Computer=(MComputer*)Item->Data;
        // Помечаем в логе
        if ( (State->CurrentAdmin)&&
            (!LogCompRun(Computer,(MTariff*)18,0,0.)) ) return;
        Computer->CmdReboot();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NShutdownClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MComputer *Computer;
        Computer=(MComputer*)Item->Data;
        // Помечаем в логе
        if ( (State->CurrentAdmin)&&
            (!LogCompRun(Computer,(MTariff*)19,0,0.)) ) return;
        Computer->CmdShutdown();
    }
}
//---------------------------------------------------------------------------

