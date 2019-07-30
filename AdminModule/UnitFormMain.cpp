//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "UnitFormMain.h"
#include "UnitFormLogIn.h"
#include "UnitFormComputers.h"
#include "UnitFormTariffs.h"
#include "UnitFormFines.h"
#include "UnitFormUsers.h"
#include "UnitFormClient.h"
#include "UnitFormOptions.h"
#include "UnitFormUserPassword.h"
#include "UnitFormRun.h"
#include "UnitFormFine.h"
#include "UnitCommon.h"
#include "UnitFormOptionsPassword.h"
#include "UnitFormOpenConfig.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
MShellState *ShellState;
MOptions *Options;
MComputers *Computers;
MTariffs *Tariffs;
MFines *Fines;
MUsers *Users;
MStates *States;
MSync *Sync;
MLog *Log;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    ShellState=new MShellState;
    Options=new MOptions;
    Computers=new MComputers;
    Tariffs=new MTariffs;
    Fines=new MFines;
    Users=new MUsers;
    States=new MStates;
    Sync=new MSync;
    Log=new MLog;

    {
        char dir[MAX_PATH+1], file[MAX_PATH+1];
        strcpy(dir,ExtractFilePath(Application->ExeName).c_str());
        strcpy(file,dir); strcat(file,"OPTIONS.DAT");
        Options->SetDefaultFile(file,0);
        strcpy(file,dir); strcat(file,"COMPUTERS.DAT");
        Computers->SetDefaultFile(file,0);
        strcpy(file,dir); strcat(file,"TARIFFS.DAT");
        Tariffs->SetDefaultFile(file,0);
        strcpy(file,dir); strcat(file,"FINES.DAT");
        Fines->SetDefaultFile(file,0);
        strcpy(file,dir); strcat(file,"USERS.DAT");
        Users->SetDefaultFile(file,0);
        strcpy(file,dir); strcat(file,"STATES.DAT");
        States->SetDefaultFile(file,0);
        strcpy(file,dir); strcat(file,"ARP.DAT");
        Sync->SetARPFile(file,0);
        Sync->SetAutoSaveARP(true);
        strcpy(file,dir); strcat(file,"LOGS");
        Log->SetDefault(file,0);
    }

    // Загружаем настройки
    if ( !(Options->Load()&&
        Computers->Load()&&
        Tariffs->Load()&&
        Fines->Load()&&
        Users->Load()) )
    {
        ShellState->State|=mssErrorConfig; //SetShell();
        MessageBoxError(1,2);
        Tag=true; Close(); return;
    }
    // Загружаем состояния компьютеров
    if ( !States->Load() )
    {
        States->Clear();
        ShellState->State|=mssErrorState; //SetShell();
        MessageBoxError(1,7);
    } else States->Update(Computers);
    
    if ( !Sync->LoadARP() ) MessageBoxError(1,2);

    // Запускаем синхронизацию по сети с клиентами
    Sync->NetInit();
    Sync->Associate(States,Computers);
    ProgressBarNetProcess->Max=Sync->GetCounterMax();

    TimerTimer(NULL);
    Timer->Interval=200;
    Timer->Enabled=true;
    NFilterAllClick(NFilterAll);
    StatusBar->SimplePanel=true;

    // Открываем лог
    if ( Log->Open() )
    {
        MUser *User;
        // Определяем пользователя, открывшего смену
        ShellState->User=Log->LastUser();
        // Проверяем есть ли он еще в списке пользователей и активен ли его логин
        if ( ((User=Users->Search(ShellState->User))==NULL)||
            (!User->Active) ) ShellState->User=0;
        //
        if ( Log->AddStart(ShellState,States,Tariffs,Fines,Users) ) Sync->Start();
    } else ShellState->State|=mssErrorLog;

    SetShell();
    Tag=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    if ( !Tag )
    {
        Log->AddStop();
    }

    Timer->Enabled=false;
    Sync->Stop(); Sync->NetFree();

    delete ShellState;
    delete Options;
    delete Computers;
    delete Tariffs;
    delete Fines;
    delete Users;
    delete States;
    delete Sync;
    delete Log;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
    static unsigned int timer_1000msec=1000;
    __int64 SystemTime;

    if ( (++timer_1000msec)>=(1000/Timer->Interval) )
    {
        timer_1000msec=0;
        //
        GetLocalTimeInt64(&SystemTime);
        if ( States->Timer(SystemTime) )
            if ( !States->Save() ) { ShellState->State|=mssErrorState; SetShell(); }
        Log->Timer(SystemTime);
        // Выводим текущее системное время
        TDate CurrentTime=Time();
        LabelTimeSecs->Caption=CurrentTime.FormatString("ss");
        LabelTimeHoursMins->Caption=CurrentTime.FormatString("hh : mm");
        UpdateListViewComputers(false);
    }
    ProgressBarNetProcess->Position=Sync->GetCounter();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogInClick(TObject *Sender)
{
    unsigned User;

    // Открываем диалог авторизации
    if ( (User=FormLogIn->Execute(Users))==0 ) return;
    // Записываем в лог событие
    if ( Log->AddLogIn(User) ) ShellState->User=User;
    else
    {
        ShellState->State|=mssErrorLog;
        ::MessageBox(Application->Handle,
            TEXT("Смена не открыта из-за ошибки !"),
            "TLK - ошибка",MB_ICONERROR|MB_OK);
    }
    SetShell();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogOutClick(TObject *Sender)
{
    // Запрашиваем подтверждение на закрытие смены
    if ( ::MessageBox(Application->Handle,
        TEXT("Вы уверены, что хотите закрыть смену ?\n")
        TEXT("После закрытия смены не будет начисляться рабочее время."),
        "TLK - закрытие смены",MB_ICONQUESTION|MB_YESNO|MB_APPLMODAL)!=IDYES ) return;

    // Записываем в лог событие
    if ( Log->AddLogOut() )
    {
        ShellState->User=0;
        // Проверяем не пора ли начать новый файл
        if ( Log->CheckPeriod(Options->LogPeriod) )
        {
            // Закрываем текущий лог и начинаем новый
            if ( !(Log->End()&&
                Log->Begin(ShellState,States,Tariffs,Fines,Users)) )
            {
                ShellState->State|=mssErrorLog;
                ::MessageBox(Application->Handle,
                    TEXT("Не удалось сменить файл лога !"),
                    "TLK - ошибка",MB_ICONERROR|MB_OK);
            }
        }
    } else
    {
        ShellState->State|=mssErrorLog;
        ::MessageBox(Application->Handle,
            TEXT("Смена не закрыта из-за ошибки !"),
            "TLK - ошибка",MB_ICONERROR|MB_OK);
    }
    SetShell();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NClosePrgClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NConfigOpenClick(TObject *Sender)
{
    if ( !(ShellState->State&mssConfig) )
        if ( !FormOpenConfig->Execute(Options,Left+20,Top+50,true) ) return;
    ShellState->State^=mssConfig;
    SetShell();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NComputersClick(TObject *Sender)
{
    TForm *form;
    if ( Sender==NComputers ) form=FormComputers;
    else if ( Sender==NTariffs ) form=FormTariffs;
    else if ( Sender==NFines ) form=FormFines;
    else if ( Sender==NUsers ) form=FormUsers;
    else if ( Sender==NUsers ) form=FormUsers;
    else if ( Sender==NClient ) form=FormClient;
    else if ( Sender==NCommon ) form=FormOptions;
    else return;

    form->Left=Left+30;
    form->Top=Top+50;
    form->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NUsersPasswordsClick(TObject *Sender)
{
    if ( Sender==NUsersPasswords )
    {
        FormUserPassword->Execute(Users,Left+100,Top+50,true);
        Users->Save();
    } else if ( Sender==NOptionsPassword )
    {
        FormOptionsPassword->Execute(Options,Left+100,Top+50,true);
        Options->Save();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnRunClick(TObject *Sender)
{
    TPoint local_button_coord;
    TForm *show_form;

    if ( Sender==BitBtnAlarm );  // show_form=FormAlarm;
    else if ( Sender==BitBtnRun ) { show_form=FormRun; FormRun->RunMode=true; }
    else if ( Sender==BitBtnAdd ) { show_form=FormRun; FormRun->RunMode=false; }
    else if ( Sender==BitBtnFine ) show_form=FormFine;
    else return;

    local_button_coord.x=PanelButtons->Left+BitBtnRun->Left-show_form->Width-2;
    local_button_coord.y=PanelButtons->Top+BitBtnRun->Top-40;
    local_button_coord=ClientToScreen(local_button_coord);
    show_form->Left=local_button_coord.x;
    show_form->Top=local_button_coord.y;
    show_form->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnExchangeClick(TObject *Sender)
{
    if ( ListViewComputers->SelCount!=2 )
    {
        ::MessageBox(Application->Handle,"Нужно выбрать ДВА компьютера !",
            "TLK - ошибка",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
        return;
    }

    TListItem *Item;
    MState *State1, *State2, *TmpState;
    MRunTime RunTime;
    MTariff *Tariff;

    // Ищем описатели состояния компьютеров
    Item=ListViewComputers->Selected;
    State1=States->Search((int)Item->Data);
    Item=ListViewComputers->GetNextItem(Item,sdAll,TItemStates()<<isSelected);
    State2=States->Search((int)Item->Data);

    // Проверяем возможно ли применить команду
    // и не нужно ли поменять местами компьютеры
    if ( !State1->CmdExchange(State2,true) )
    {
        if ( !State2->CmdExchange(State1,true) ) return;
        TmpState=State1; State1=State2; State2=TmpState;
    }

    // Проверяем применим ли тот же тариф ко второму компьютеру
    State1->RunParam(&RunTime);
    if ( !(((Tariff=Tariffs->Search(RunTime.TariffID))!=NULL)&&
        Tariff->CheckForComputer(State2->Associated())) ) return;

    // Добавляем запись в лог
    if ( Log->AddExchange(State1->Associated(),State2->Associated()) )
    {
        // Применяем команду
        State1->CmdExchange(State2,false);
        // Сохраняем новые состояния в файле
        if ( !States->Save() )
        {
            ShellState->State|=mssErrorState; SetShell();
            MessageBoxError(1,8);
        }
        // Обновляем список компьютеров
        UpdateListViewComputers(false);
    } else
    {
        ShellState->State|=mssErrorLog; SetShell();
        MessageBoxError(1,5);
    }
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
        MState *State;
        State=States->Search((int)Item->Data);
        // Проверяем возможно ли применить команду
        if ( !State->CmdLock(lock,true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddLock(State->Associated(),lock) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            MessageBoxError(1,5); break;
        }
        // Применяем команду
        State->CmdLock(lock,false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        MessageBoxError(1,8);
    }
    // Обновляем список компьютеров
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnAdditionalsClick(TObject *Sender)
{
    TPopupMenu *PopupMenu;
    TBitBtn *BitBtn;
    TPoint local_button_coord;

    if ( Sender==BitBtnAdditionals ) { BitBtn=BitBtnAdditionals; PopupMenu=PopupMenuAdditionals; }
    else if ( Sender==BitBtnSystem ) { BitBtn=BitBtnSystem; PopupMenu=PopupMenuSystem; }
    else if ( Sender==BitBtnViewFilter ) { BitBtn=BitBtnViewFilter; PopupMenu=PopupMenuViewFilter; }
    else return;

    local_button_coord.x=PanelButtons->Left+BitBtn->Left+4;
    local_button_coord.y=PanelButtons->Top+BitBtn->Top+BitBtn->Height-4;
    local_button_coord=ClientToScreen(local_button_coord);

    PopupMenu->Popup(local_button_coord.x,local_button_coord.y);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NRebootClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MState *State;
        State=States->Search((int)Item->Data);
        // Проверяем возможно ли применить команду
        if ( !State->CmdReboot(true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddReboot(State->Associated()) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            MessageBoxError(1,5); break;
        }
        // Применяем команду
        State->CmdReboot(false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        MessageBoxError(1,8);
    }
    // Обновляем список компьютеров
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NPowerOnClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MState *State;
        State=States->Search((int)Item->Data);
        // Проверяем возможно ли применить команду
        if ( !State->CmdPowerOn(true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddPowerOn(State->Associated()) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            MessageBoxError(1,5); break;
        }
        // Применяем команду
        State->CmdPowerOn(false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        MessageBoxError(1,8);
    }
    // Обновляем список компьютеров
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NShutdownClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MState *State;
        State=States->Search((int)Item->Data);
        // Проверяем возможно ли применить команду
        if ( !State->CmdShutdown(true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddShutdown(State->Associated()) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            MessageBoxError(1,5); break;
        }
        // Применяем команду
        State->CmdShutdown(false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        MessageBoxError(1,8);
    }
    // Обновляем список компьютеров
    UpdateListViewComputers(false);
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
        MState *State;
        State=States->Search((int)Item->Data);
        // Проверяем возможно ли применить команду
        if ( !State->CmdPause(pause,true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddPause(State->Associated(),pause) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            MessageBoxError(1,5); break;
        }
        // Применяем команду
        State->CmdPause(pause,false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        MessageBoxError(1,8);
    }
    // Обновляем список компьютеров
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NOpenClick(TObject *Sender)
{
    bool open;
    if ( Sender==NOpen ) open=true;
    else if ( Sender==NClose ) open=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MState *State;
        State=States->Search((int)Item->Data);
        // Проверяем возможно ли применить команду
        if ( !State->CmdOpen(open,true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddOpen(State->Associated(),open) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            MessageBoxError(1,5); break;
        }
        // Применяем команду
        State->CmdOpen(open,false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        MessageBoxError(1,8);
    }
    // Обновляем список компьютеров
    UpdateListViewComputers(false);
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
        MState *State;
        State=States->Search((int)Item->Data);
        // Проверяем возможно ли применить команду
        if ( !State->CmdWtLocker(wtlocker,true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddWtLocker(State->Associated(),wtlocker) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            MessageBoxError(1,5); break;
        }
        // Применяем команду
        State->CmdWtLocker(wtlocker,false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        MessageBoxError(1,8);
    }
    // Обновляем список компьютеров
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NFilterAllClick(TObject *Sender)
{
    if ( Sender==NFilterAll ) Filter=mcfAll;
    else if ( Sender==NFilterFree ) Filter=mcfFree;
    else if ( Sender==NFilterService ) Filter=mcfService;
    else return;
    ((TMenuItem*)Sender)->Default=true;
    ((TMenuItem*)Sender)->Checked=true;
    UpdateListViewComputers(true);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewComputersInsert(TObject *Sender,
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
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewComputersCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
    if ( ((int)Item1->Data)<((int)Item2->Data) ) Compare=-1;
    else if ( ((int)Item1->Data)>((int)Item2->Data) ) Compare=1;
    else Compare=0;
}
//---------------------------------------------------------------------------
bool TFormMain::CheckFilter(MStateInfo *Info_, int FreeTime_)
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
void TFormMain::SetListViewComputersLine(TListItem *Item_, MStateInfo *Info_)
{
    TStrings *SubItems=Item_->SubItems;
    char line[50];
    int icon;

    // Номер компьютера
    if ( Info_->Changes&mdcNumber )
    {
        icon=Computers->Search(Info_->Number)->GroupColor;
        sprintf(line,"%i",Info_->Number);
        Item_->SubItemImages[0]=icon;
        SubItems->Strings[0]=line;
    }
    // Состояние сети
    if ( Info_->Changes&mdcNetState )
    {
        unsigned int NetState=Info_->NetState;
        if ( NetState&mnsSyncNeed ) icon=12;
        else if ( NetState&mnsPresent ) icon=-1;
        else icon=14;
        Item_->SubItemImages[1]=icon;
    }
    // Режим работы
    if ( Info_->Changes&mdcState )
    {
        unsigned int State=Info_->State;
        if ( State&mcsWtLocker ) { icon=10; strcpy(line,"Без TLK"); }
        else if ( State&mcsOpen ) { icon=9; strcpy(line,"настройка"); }
        else if ( State&mcsPause ) { icon=8; strcpy(line,"приостановлен"); }
        else if ( State&mcsLock ) { icon=7; strcpy(line,"Прикрыт !"); }
        else if ( State&mcsFine ) { icon=6; strcpy(line,"Штраф !!!"); }
        else if ( State&mcsWork ) { icon=5; strcpy(line,"Работа"); }
        else if ( State&mcsFree ) { icon=4; strcpy(line,"СВОБОДЕН"); }
        else { icon=-1; strcpy(line,""); }
        Item_->SubItemImages[2]=icon;
        SubItems->Strings[2]=line;
    }
    // Название тарифа
    if ( Info_->Changes&mdcTariff )
    {
        MTariff *Tariff=Tariffs->Search(Info_->TariffID);
        if ( Tariff ) SubItems->Strings[3]=Tariff->Name;
        else SubItems->Strings[3]="";
    }
    // Время работы
    if ( Info_->Changes&mdcWorkTime )
    {
        if ( Info_->State&mcsWork )
        {
            sprintf(line,"%i час. %.2i мин.",Info_->WorkTime/60,Info_->WorkTime%60);
            SubItems->Strings[4]=line;
        } else
        {
            SubItems->Strings[4]="";
            SubItems->Strings[5]="";
            SubItems->Strings[6]="";
        }
    }
    // Сколько времени осталось работать и до скольки
    if ( Info_->State&mcsWork )
    {
        sprintf(line,"%.2i:%.2i",Info_->ToEndWork/60,Info_->ToEndWork%60);
        SubItems->Strings[5]=line;
        sprintf(line,"%.2i:%.2i",Info_->EndWorkTime/60,Info_->EndWorkTime%60);
        SubItems->Strings[6]=line;
    }
    // Время штрафа
    if ( Info_->Changes&mdcFineTime )
    {
        if ( Info_->State&mcsFine )
        {
            sprintf(line,"%i мин.",Info_->FineTime);
            SubItems->Strings[7]=line;
        } else
        {
            SubItems->Strings[7]="";
            SubItems->Strings[8]="";
        }
    }
    // Сколько времени штрафа осталось
    if ( Info_->State&mcsFine )
    {
        sprintf(line,"%.2i:%.2i",Info_->ToEndFine/60,Info_->ToEndFine%60);
        SubItems->Strings[8]=line;
    }
}
//---------------------------------------------------------------------------
void TFormMain::UpdateListViewComputers(bool Full_)
{
    TListItem *Item;
    MState *State;
    MStateInfo Info;
    int i;

    // Убираем записи, не сопоставленные с состоянием компьютера
    i=ListViewComputers->Items->Count-1;
    while(i>=0)
    {
        Item=ListViewComputers->Items->Item[i];
        if ( !States->Search((int)Item->Data) ) Item->Delete();
        i--;
    }

    // Убираем из списка компьютеры, не подходящие под фильтр, и добавляем новые
    for ( State=(MState*)States->First; State; State=(MState*)State->Next )
    {
        //
        State->StateInfo(&Info);
        //
        Item=ListViewComputers->FindData(0,(void*)Info.Number,true,false);
        // Проверяем подходит ли компьютер под выставленный фильтр
        if ( !CheckFilter(&Info,Options->FilterFreeTime) )
            { if ( Item ) Item->Delete(); continue; }
        // Если компьютер не занесен в список, то добавляем его
        if ( !Item )
        {
            Item=ListViewComputers->Items->Add();
            Item->Data=(void*)Info.Number;
            Info.Changes=mdcAll;
        }
        // Обновляем информацию в таблице
        if ( Full_ ) Info.Changes=mdcAll;
        SetListViewComputersLine(Item,&Info);
    }
}
//---------------------------------------------------------------------------
void TFormMain::SetShell()
{
    bool enable;
    MUser *User;
    AnsiString line;

    // Общие операции
    enable=(ShellState->State&mssConfig)||(ShellState->User!=0);
    enable=ShellState->State&(mssErrorState|mssErrorConfig)? false: enable;
    BitBtnRun->Enabled=enable;
    BitBtnAdd->Enabled=enable;
    BitBtnFine->Enabled=enable;
    BitBtnExchange->Enabled=enable;
    BitBtnLock->Enabled=enable;
    BitBtnUnLock->Enabled=enable;
    BitBtnAdditionals->Enabled=enable;
    // Приостанова компьютеров
    NPause->Enabled=((ShellState->User!=0)&&(Options->UsersRights&murPause))||
        (ShellState->State&mssConfig);
    NRun->Enabled=NPause->Enabled;

    // Настройки, доступные только сисадмину
    NConfigOpen->Caption=ShellState->State&mssConfig? "Закрыть": "Открыть...";
    enable=(ShellState->State&mssConfig)&&(!(ShellState->State&mssErrorLog));
    NAdmin->Enabled=enable;
    NComputers->Enabled=enable;
    NTariffs->Enabled=enable;
    NFines->Enabled=enable;
    NUsers->Enabled=enable;
    NCommon->Enabled=enable;
    NClient->Enabled=enable;
    BitBtnSystem->Enabled=enable;
    // Общесистемные функции
    NLogIn->Enabled=ShellState->User==0;
    NLogOut->Enabled=ShellState->User!=0;
//    NOptionsPassword->Enabled=!error;
//    NUsersPasswords->Enabled=!error;
    // Аварийный сброс лога
    NLogReset->Enabled=(ShellState->State&mssConfig)&&(ShellState->State&mssErrorLog);

    // Заголовок окна программы
    line="TLK - ";
    if ( (User=Users->Search(ShellState->User))!=NULL ) line+=User->Login;
    else line+="смена не начата";
    if ( ShellState->State&mssConfig ) line+=" (открыты настройки)";
    Caption=line;
    // Строка состояния
    line="";
    if ( ShellState->State&mssErrorConfig ) line+="Ошибка работы с конфигурацией, ";
    if ( ShellState->State&mssErrorState ) line+="Не удалось сохранить поледнее состояние, ";
    if ( ShellState->State&mssErrorLog ) line+="Ошибка работы с логом, ";
    StatusBar->SimpleText=line;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogResetClick(TObject *Sender)
{
    if ( Log->Begin(ShellState,States,Tariffs,Fines,Users) )
        { ShellState->State&=~mssErrorLog; SetShell(); }
    else MessageBoxError(1,6);
}
//---------------------------------------------------------------------------
bool TFormMain::MessageBoxError(UINT uCaption, UINT uMessage)
{
    char Caption[128], Message[128+2+128];

    // Загружаем заголовок сообщения
    if ( ::LoadString(HInstance,uCaption,Caption,128)==0 )
        sprintf(Caption,"<< Текст заголовка не найден ! >>");
    // Загружаем сообщение
    if ( ::LoadString(HInstance,uMessage,Message,128)==0 )
        sprintf(Message,"<< Текст сообщения не найден ! >>");
    // Добавляем пару пустых строк
    strcat(Message,"\n\n");
    // Добавляем к строке сообщение об ошибке Windows
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,
        ::GetLastError(),MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
        (LPTSTR)(Message+strlen(Message)),128,NULL);
    // Выводим сообщение
    ::MessageBox(Application->Handle,
        Message,Caption,MB_OK|MB_ICONERROR|MB_APPLMODAL);

    return true;
}
//---------------------------------------------------------------------------

