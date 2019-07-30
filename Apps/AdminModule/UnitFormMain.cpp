//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdexcept.h>
#pragma hdrstop

#include "UnitFormMain.h"
#include "UnitFormRun.h"
#include "UnitFormFine.h"
#include "UnitFormLogIn.h"
#include "UnitFormOpenConfig.h"
#include "UnitFormUserPass.h"
#include "UnitFormOptionsPass.h"
#include "UnitFormComputers.h"
#include "UnitFormTariffs.h"
#include "UnitFormFines.h"
#include "UnitFormUsers.h"
#include "UnitFormOptions.h"
#include "UnitFormClient.h"
#include "UnitFormAuth.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
Mptr <MShellState> ShellState;
Mptr <MOptions> Options;
Mptr <MComputers> Computers;
Mptr <MTariffs> Tariffs;
Mptr <MFines> Fines;
Mptr <MUsers> Users;
Mptr <MStates> States;
Mptr <MSync> Sync;
Mptr <MAuth> Auth;
Mptr <MLog> Log;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
        : TForm(Owner)
{
    EnableConfig=false;
    Tag=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    try
    {

    // Создадим объекты глобального пользования
    ShellState(new MShellState);
    Options(new MOptions);
    Computers(new MComputers);
    Tariffs(new MTariffs);
    Fines(new MFines);
    Users(new MUsers);
    States(new MStates);
    Sync(new MSync);
    Auth(new MAuth);
    Log(new MLog);

    // Настроим пути хранения файлов и коды шифрования
    Options->SetDefaultKey(HKEY_LOCAL_MACHINE,
        "Software\\MMM Groups\\TLK\\3.0\\Admin","Options",ENC_Code);
    Auth->SetDefaultKey(HKEY_LOCAL_MACHINE,
        "Software\\MMM Groups\\TLK\\3.0\\Admin","Auth",ENC_Code);

    AnsiString path=ExtractFilePath(Application->ExeName), file;

    file=path+"COMPUTERS.DAT";
    Computers->SetDefaultFile(file.c_str(),ENC_Code);
    file=path+"TARIFFS.DAT";
    Tariffs->SetDefaultFile(file.c_str(),ENC_Code);
    file=path+"FINES.DAT";
    Fines->SetDefaultFile(file.c_str(),ENC_Code);
    file=path+"USERS.DAT";
    Users->SetDefaultFile(file.c_str(),ENC_Code);
    file=path+"STATES.DAT";
    States->SetDefaultFile(file.c_str(),ENC_Code);
    file=path+"ARP.DAT";
    Sync->SetARPFile(file.c_str(),ENC_Code,true);
    file=path+"LOGS";
    Log->SetDefault(file.c_str(),ENC_Code);

#ifdef _DEBUG
    if ( !Options->Load() )         /// сброс пароля
    {
        Options->SetPass("");
        Options->Save();
    }
#else
    // Загрузим основные настройки
    if ( !Options->Load() )
    {
        ResMessageBox(Handle,1,30,MB_APPLMODAL|MB_OK|MB_ICONERROR);
        // Без пароля на настройки запускаться не будем
        Tag=true; Close();
        return;
    }
#endif

    // Загрузим остальные настройки, возможно не все
    bool all_load=true;
    all_load=Auth->Load()&&all_load;
    all_load=Computers->Load()&&all_load;
    all_load=Tariffs->Load()&&all_load;
    all_load=Fines->Load()&&all_load;
    all_load=Users->Load()&&all_load;
    if ( !all_load )
    {
        ShellState->State|=mssErrorConfig; SetShell();
        ResMessageBox(Handle,1,2,MB_APPLMODAL|MB_OK|MB_ICONERROR);
    }

    // Загрузим состояния компьютеров
    if ( !States->Load() )
    {
        States->Clear();
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,7,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    } else
        // Хоть состояния и загрузились, сверим их с актуальным списком компьютеров
        States->Update(Computers);      /// опасное действие ?

    // Загрузим ARP-кэш
    if ( !Sync->LoadARP() )
        ResMessageBox(Handle,0,29,MB_APPLMODAL|MB_OK|MB_ICONWARNING,Sync->gLastErr());

    // Подготавливаем синхронизацию по сети с клиентами
    Sync->NetInit(ENC_Net,Auth);
    Sync->Associate(States,Computers);
    ProgressBarNetProcess->Max=Sync->gPCountMax();

    // Таймер нужен до логов, т.к. им нужно системное время
    Timer->Interval=1000;
    Timer->Enabled=true;
    TimerTimer(NULL);

    // Открываем лог
    if ( Log->Open() )
    {
        MUser *User;
        // Определяем пользователя, открывшего смену
        ShellState->User=Log->LastUser();
        // Проверяем есть ли он еще в списке пользователей и активен ли его логин
        if ( ((User=(MUser*)Users->SrchID(ShellState->User))==NULL)||
            (!User->Active) ) ShellState->User=0;
        // Сохраним в логе стартовые параметры, чтобы потом четко разобрать все режимы
        if ( !Log->AddStart(ShellState,States,Tariffs,Fines,Users) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,4,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
        }
    } else
    {
        // Если ошибка лога связана с нарушением его формата,
        // попытаемся начать новый файл
        if ( (Log->gLastErr()!=0)||
            (!Log->Begin(ShellState,States,Tariffs,Fines,Users)) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,4,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
        } else
            ResMessageBox(Handle,0,32,MB_APPLMODAL|MB_OK|MB_ICONWARNING);
    }

    NFilterAllClick(NFilterAll);
    StatusBar->SimplePanel=true;
    SetShell();

    TimerNet->Interval=250;
    TimerNet->Enabled=true;
    // Запускаем синхронизацию
    Sync->Start();

    // Включим буферизацию отрисовки, чтобы не мерцало
    ListViewComputers->DoubleBuffered=true;
    ProgressBarNetProcess->DoubleBuffered=true;

    }
    catch (std::bad_alloc &e)
    {
        ResMessageBox(Handle,1,13,MB_APPLMODAL|MB_OK|MB_ICONERROR);
        Tag=true; Close();
        return;
    }
    catch (std::exception &e)
    {
        ::MessageBox(Handle, e.what(),
            "TLK - ошибка",MB_APPLMODAL|MB_OK|MB_ICONERROR);
        Tag=true; Close();
        return;
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
        Tag=true; Close();
        return;
    }

    Tag=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    TimerNet->Enabled=false;
    Timer->Enabled=false;
    Sync->Stop();
    Sync->NetFree();

    // Добавим запись в лог о нормальном завершении работы
    if ( !Tag ) Log->AddStop();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
    __int64 SystemTime;

    // Берем системное время
    GetLocalTimeInt64(&SystemTime);
    // Подаем его модулю лога
    Log->Timer(SystemTime);
    // Обновляем состояния компьютеров и сохраняем в случае изменений
    if ( States->Timer(SystemTime) )
        if ( !States->Save() ) { ShellState->State|=mssErrorState; SetShell(); }
    // Обновляем показания часов
    TDate CurrentTime=Time();
    LabelTimeSecs->Caption=CurrentTime.FormatString("ss");
    LabelTimeHoursMins->Caption=CurrentTime.FormatString("hh : mm");
    // И список компьютеров
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerNetTimer(TObject *Sender)
{
    ProgressBarNetProcess->Position=Sync->gPCount();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogInClick(TObject *Sender)
{
    Mptr <TFormLogIn> form;
    unsigned User;

    try
    {
        // Открываем диалог авторизации
        form(new TFormLogIn(0));
        User=form->Execute(Users);
        if ( User==0 ) return;
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
        return;
    }
    
    // Записываем в лог событие
    if ( !Log->AddLogIn(User) )
    {
        ShellState->State|=mssErrorLog; SetShell();
        ResMessageBox(Handle,1,17,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
        return;
    }
    ShellState->User=User;
    SetShell();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogOutClick(TObject *Sender)
{
    // Запрашиваем подтверждение на закрытие смены
    if ( ResMessageBox(Handle,25,26,
        MB_APPLMODAL|MB_YESNO|MB_ICONQUESTION)!=IDYES ) return;

    // Записываем в лог событие
    if ( !Log->AddLogOut() )
    {
        ShellState->State|=mssErrorLog; SetShell();
        ResMessageBox(Handle,1,18,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
        return;
    }

    ShellState->User=0; SetShell();

    // Проверяем не пора ли начать новый файл
    if ( !Log->CheckPeriod(Options->LogPeriod) ) return;

    // Закрываем текущий лог и начинаем новый
    if ( !(Log->End()&&
        Log->Begin(ShellState,States,Tariffs,Fines,Users)) )
    {
        ShellState->State|=mssErrorLog; SetShell();
        ResMessageBox(Handle,1,6,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NClosePrgClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NConfigOpenClick(TObject *Sender)
{
    Mptr <TFormOpenConfig> form;

    if ( !(ShellState->State&mssConfig) )
    {
        try
        {
            form(new TFormOpenConfig(0));
            if ( !form->Execute(Options,Left+20,Top+50,true) ) return;
        }
        catch (Exception &ex)
        {
            Application->ShowException(&ex);
            // Не дадим через исключение изменить состояние режима настройки
            return;
        }
    }
    ShellState->State^=mssConfig; SetShell();
    Log->AddConfig(ShellState->State&mssConfig);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NComputersClick(TObject *Sender)
{
    Mptr <TForm> form;

    // Доп. проверка для большей безопасности
    if ( !EnableConfig ) return;

    try
    {
        if ( Sender==NComputers ) form(new TFormComputers(0));
        else if ( Sender==NTariffs ) form(new TFormTariffs(0));
        else if ( Sender==NFines ) form(new TFormFines(0));
        else if ( Sender==NUsers ) form(new TFormUsers(0));
        else if ( Sender==NCommon ) form(new TFormOptions(0));
        else if ( Sender==NClient ) form(new TFormClient(0));
        else if ( Sender==NAuth ) form(new TFormAuth(0));
        else return;

        form->Left=Left+30;
        form->Top=Top+50;
        form->ShowModal();
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NOptionsPasswordClick(TObject *Sender)
{
    Mptr <TFormOptionsPass> form;

    try
    {
        form(new TFormOptionsPass(0));
        if ( !form->Execute(Options,Left+100,Top+50,true) ) return;
        if ( Options->Save() ) return;
//        ShellState->State|=mssErrorConfig; SetShell();  /// не имеет смысла и опасно
        ResMessageBox(Handle,1,31,MB_APPLMODAL|MB_OK|MB_ICONERROR,Options->gLastErr());
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NUsersPasswordsClick(TObject *Sender)
{
    Mptr <TFormUserPass> form;

    try
    {
        form(new TFormUserPass(0));
        if ( !form->Execute(Users,Left+100,Top+50,true) ) return;
        if ( Users->Save() ) return;
//        ShellState->State|=mssErrorConfig; SetShell();  /// не имеет смысла и опасно
        ResMessageBox(Handle,1,3,MB_APPLMODAL|MB_OK|MB_ICONERROR,Users->gLastErr());
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnRunClick(TObject *Sender)
{
    TPoint local_button_coord;
    Mptr <TForm> form;

    try
    {
        // Создаем объект формы
        if ( (Sender==BitBtnRun)||(Sender==NCmpRun) )
        {
            form(new TFormRun(0));
            ((TFormRun*)(TForm*)form)->RunMode=true;
        } else if ( (Sender==BitBtnAdd)||(Sender==NCmpAdd) )
        {
            form(new TFormRun(0));
            ((TFormRun*)(TForm*)form)->RunMode=false;
        } else if ( (Sender==BitBtnFine)||(Sender==NCmpFine) )
        {
            form(new TFormFine(0));
        } else return;

        // Задаем удобные координаты
        local_button_coord.x=PanelButtons->Left+BitBtnRun->Left-form->Width-2;
        local_button_coord.y=PanelButtons->Top+BitBtnRun->Top-40;
        local_button_coord=ClientToScreen(local_button_coord);
        form->Left=local_button_coord.x;
        form->Top=local_button_coord.y;
        // Выводим
        form->ShowModal();
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnExchangeClick(TObject *Sender)
{
    if ( ListViewComputers->SelCount!=2 )
    {
        ResMessageBox(Handle,0,15,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MState *state1, *state2, *tmpstate;
    // Ищем описатели состояния компьютеров
    TListItem *item=ListViewComputers->Selected;
    state1=(MState*)item->Data;
    item=ListViewComputers->GetNextItem(item,sdAll,TItemStates()<<isSelected);
    state2=(MState*)item->Data;

    // Проверяем возможно ли применить команду
    // и не нужно ли поменять местами компьютеры
    if ( !state1->CmdExchange(state2,true) )
    {
        if ( !state2->CmdExchange(state1,true) )
        {
            ResMessageBox(Handle,0,34,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
            return;
        }
        tmpstate=state1;
        state1=state2;
        state2=tmpstate;
    }

    MRunTime time;
    state1->RunParam(&time);
    // Проверяем применим ли тот же тариф ко второму компьютеру
    MTariff *tariff;
    if ( !(((tariff=(MTariff*)Tariffs->SrchID(time.TariffID))!=NULL)&&
        tariff->CheckForComp(state2->Associated())) )
    {
        ResMessageBox(Handle,0,14,MB_APPLMODAL|MB_OK|MB_ICONWARNING);
        return;
    }

    // Добавляем запись в лог
    if ( !Log->AddExchange(state1->Associated(),
                           state2->Associated()) )
    {
        ShellState->State|=mssErrorLog; SetShell();
        ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
        return;
    }

    // Применяем команду
    state1->CmdExchange(state2,false);
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // Обновляем список компьютеров
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnLockClick(TObject *Sender)
{
    bool lock;
    if ( (Sender==BitBtnLock)||(Sender==NCmpLock) ) lock=true;
    else if ( (Sender==BitBtnUnLock)||(Sender==NCmpUnLock) ) lock=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MState *State=(MState*)Item->Data;
        // Проверяем возможно ли применить команду
        if ( !State->CmdLock(lock,true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddLock(State->Associated(),lock) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // Применяем команду
        State->CmdLock(lock,false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
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
        MState *State=(MState*)Item->Data;
        // Проверяем возможно ли применить команду
        if ( !State->CmdReboot(true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddReboot(State->Associated()) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // Применяем команду
        State->CmdReboot(false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
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
        MState *State=(MState*)Item->Data;
        // Проверяем возможно ли применить команду
        if ( !State->CmdPowerOn(true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddPowerOn(State->Associated()) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // Применяем команду
        State->CmdPowerOn(false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
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
        MState *State=(MState*)Item->Data;
        // Проверяем возможно ли применить команду
        if ( !State->CmdShutdown(true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddShutdown(State->Associated()) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // Применяем команду
        State->CmdShutdown(false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
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
        MState *State=(MState*)Item->Data;
        // Проверяем возможно ли применить команду
        if ( !State->CmdPause(pause,true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddPause(State->Associated(),pause) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // Применяем команду
        State->CmdPause(pause,false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // Обновляем список компьютеров
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NOpenClick(TObject *Sender)
{
    // Доп. проверка для большей безопасности
    if ( !EnableConfig ) return;

    bool open;
    if ( Sender==NOpen ) open=true;
    else if ( Sender==NClose ) open=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        MState *State=(MState*)Item->Data;
        // Проверяем возможно ли применить команду
        if ( !State->CmdOpen(open,true) ) continue;
        // Добавляем запись в лог
        if ( !Log->AddOpen(State->Associated(),open) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // Применяем команду
        State->CmdOpen(open,false);
    }
    // Сохраняем новые состояния в файле
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // Обновляем список компьютеров
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NFilterAllClick(TObject *Sender)
{
    if ( (Sender==NFilterAll)||(Sender==NCmpFilterAll) )
    {
        Filter=mcfAll;
        NFilterAll->Default=true;
        NFilterAll->Checked=true;
        NCmpFilterAll->Default=true;
        NCmpFilterAll->Checked=true;
    } else if ( (Sender==NFilterFree)||(Sender==NCmpFilterFree) )
    {
        Filter=mcfFree;
        NFilterFree->Default=true;
        NFilterFree->Checked=true;
        NCmpFilterFree->Default=true;
        NCmpFilterFree->Checked=true;
    } else if ( (Sender==NFilterService)||(Sender==NCmpFilterService) )
    {
        Filter=mcfService;
        NFilterService->Default=true;
        NFilterService->Checked=true;
        NCmpFilterService->Default=true;
        NCmpFilterService->Checked=true;
    } else return;
    // Обновляем список компьютеров
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
    int num1=((MState*)Item1->Data)->Associated();
    int num2=((MState*)Item2->Data)->Associated();
    Compare = num1==num2? 0: (num1<num2? -1: +1);
}
//---------------------------------------------------------------------------
bool TFormMain::CheckFilter(MStateInfo *Info_, int FreeTime_)
{
    bool result=false;
    switch(Filter)
    {
        case mcfAll:
            result=true;
            break;
        case mcfFree:
            if ( (Info_->State==mcsFree)||
                ((Info_->State&mcsWork)&&(Info_->ToEndWork<FreeTime_)) ) result=true;
            break;
        case mcfService:
            if ( Info_->State&mcsOpen ) result=true;
            break;
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
        Item_->SubItemImages[0]=GetCompColorIcon(Computers->Search(Info_->Number));
        sprintf(line,"%i",Info_->Number);
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
        if ( State&mcsOpen ) { icon=9; strcpy(line,"настройка"); }
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
        MTariff *Tariff=(MTariff*)Tariffs->SrchID(Info_->TariffID);
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
    MStateInfo Info;

    if ( Full_ ) ListViewComputers->Items->Clear();

    // Убираем из списка компьютеры, не подходящие под фильтр. Добавляем новые.
    for ( MState *State=(MState*)States->gFirst();
        State; State=(MState*)State->gNext() )
    {
        State->StateInfo(&Info);
        Item=ListViewComputers->FindData(0,State,true,false);
        // Проверяем подходит ли компьютер под выставленный фильтр
        if ( CheckFilter(&Info,Options->FilterFreeTime) )
        {
            // Если компьютер не занесен в список, то добавляем его
            if ( !Item )
            {
                Item=ListViewComputers->Items->Add();
                Item->Data=State;
                Info.Changes=mdcAll;
            }
        } else
        {
            if ( Item ) Item->Delete();
            continue;
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
    BitBtnRun->Enabled=enable;          NCmpRun->Enabled=enable;
    BitBtnAdd->Enabled=enable;          NCmpAdd->Enabled=enable;
    BitBtnFine->Enabled=enable;         NCmpFine->Enabled=enable;
    BitBtnExchange->Enabled=enable;     NCmpExchange->Enabled=enable;
    BitBtnLock->Enabled=enable;         NCmpLock->Enabled=enable;
    BitBtnUnLock->Enabled=enable;       NCmpUnLock->Enabled=enable;
    BitBtnAdditionals->Enabled=enable;
    // Приостановка компьютеров
    NPause->Enabled=((ShellState->User!=0)&&(Options->UsersRights&murPause))||
        (ShellState->State&mssConfig);
    NRun->Enabled=NPause->Enabled;

    // Настройки, доступные только сисадмину
    NConfigOpen->Caption=ShellState->State&mssConfig? "Закрыть": "Открыть...";
    enable=(ShellState->State&mssConfig)&&(!(ShellState->State&mssErrorLog));
    EnableConfig=enable;
    NAdmin->Enabled=enable;
    NComputers->Enabled=enable;
    NTariffs->Enabled=enable;
    NFines->Enabled=enable;
    NUsers->Enabled=enable;
    NCommon->Enabled=enable;
    NClient->Enabled=enable;
    NAuth->Enabled=enable;
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
    if ( (User=(MUser*)Users->SrchID(ShellState->User))!=NULL ) line+=User->Login;
    else line+="смена не начата";
    if ( ShellState->State&mssConfig ) line+=" (открыты настройки)";
    Caption=line;
    // Строка состояния
    line="";
    if ( ShellState->State&mssErrorConfig ) line+="Ошибка работы с конфигурацией, ";
    if ( ShellState->State&mssErrorState ) line+="Не удалось сохранить последнее состояние, ";
    if ( ShellState->State&mssErrorLog ) line+="Ошибка работы с логом, ";
    StatusBar->SimpleText=line;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogResetClick(TObject *Sender)
{
    if ( Log->Begin(ShellState,States,Tariffs,Fines,Users) )
    {
        ShellState->State&=~mssErrorLog; SetShell();
    } else
        ResMessageBox(Handle,1,6,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
}
//---------------------------------------------------------------------------
int TFormMain::GetCompColorIcon(MComputer *Comp_)
{
    int icon;

    if ( Comp_==NULL ) return -1;
    switch(Comp_->Color)
    {
        case mgcAqua:   icon=0; break;
        case mgcGreen:  icon=1; break;
        case mgcRed:    icon=2; break;
        case mgcYellow: icon=3; break;
        default:        icon=-1; break;
    }

    return icon;
}
//---------------------------------------------------------------------------

