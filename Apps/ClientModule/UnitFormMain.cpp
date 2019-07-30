//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdexcept.h>
#pragma hdrstop

#include "UnitFormMain.h"
#include "UnitFormGames.h"
#include "UnitFormDesk.h"
#include "UnitTariffs.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
char win_dir[MAX_PATH+1];
MClOptions *Options=NULL;
MStateCl *State=NULL;
MSyncCl *Sync=NULL;
MSendCl *Send=NULL;
MAuth *Auth=NULL;
MMessage *Message=NULL;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TForm(Owner)
{
    TimeToReboot=0;
    TimeMsgEndTime=0;
    MessageAreShowed=false;
    PanelMenu->DoubleBuffered=true;       /// глючит
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    try
    {

    Options=new MClOptions;
    State=new MStateCl;
    Sync=new MSyncCl;
    Send=new MSendCl;
    Auth=new MAuth;
    Message=new MMessage;

    // Настраиваем пути для файлов
    ::GetWindowsDirectory(win_dir,MAX_PATH-20);
    strcat(win_dir,"\\");
    {
        char file[MAX_PATH+1];
        strcpy(file,win_dir); strcat(file,"TLK.GMS");
        State->SetDefault(
            HKEY_LOCAL_MACHINE,"Software\\MMM Groups\\TLK\\3.0\\Client","State",    //path//
            HKEY_LOCAL_MACHINE,"Software\\MMM Groups\\TLK\\3.0\\Client","Options",  //path//
            file,ENC_Code);
        Auth->SetDefaultKey(
            HKEY_LOCAL_MACHINE,"Software\\MMM Groups\\TLK\\3.0\\Client","Auth",ENC_Code);    //path//
        strcpy(file,win_dir); strcat(file,"TIMEWARN.BMP");
        Message->SetFile(file);
    }

    // Загружаем настройки, заменяя их "дефолтом" при необходимости
    if ( !Auth->Load() ) Auth->Save();
    if ( !State->Load() ) State->Save();
    if ( !State->GetOptions(Options) )
    {
        Options->ToEndTime=2;
        Options->MessageTime=15;
        Options->MsgEndTime=10;
        Options->RebootWait=20;
        Options->AutoLockTime=15;
        Options->Flags=0;
        State->NewOptions(Options);
    }

    // Подготовка сети
    if ( !(Sync->NetInit(ENC_Net,Auth)&&
        Send->NetInit(State,ENC_Net,Auth)) )
    {
        ResMessageBox(Handle,1,2,MB_APPLMODAL|MB_OK|MB_ICONERROR);
        Tag=true; Close();
        return;
    }

    // Настроим внешний вид
    FormGames->TreeViewGames->Font->Color=(TColor)0x02D2C66F;
    SetTransp(Options->Flags&mcoTransp);

    Timer->Interval=1000;
    TimerTimer(NULL);
    Timer->Enabled=true;

    // Запуск сети
    Sync->Associate(State);
    Sync->Start();
    Send->Start();

    // Запускаем программы из раздела автозапуска
    RegExecList(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

    }
    catch (std::bad_alloc &e)
    {
        ResMessageBox(Handle,1,3,MB_APPLMODAL|MB_OK|MB_ICONERROR);
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
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose=Tag;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    Timer->Enabled=false;
    Send->Stop(); Send->NetFree();
    Sync->Stop(); Sync->NetFree();
    Message->Stop();
    delete Send;
    delete Sync;
    delete State;
    delete Message;
    delete Options;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormResize(TObject *Sender)
{
    FormGames->Left=0;
    FormGames->Top=PanelMenu->Height;
    FormGames->Width=Screen->Width-PanelMenu->Width;
    FormGames->Height=Screen->Height-PanelMenu->Height;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
    __int64 SystemTime;
    MStateInfo Info;

    // Позиционируем окно программы
    if ( Width!=Screen->Width ) { Left=0; Width=Screen->Width; }
    if ( Height!=Screen->Height ) { Top=0; Height=Screen->Height; }

    // Выводим текущее время
    LabelSysTime->Caption=Time().FormatString("hh:nn");
    GetLocalTimeInt64(&SystemTime);
    LockDesk.UpdateSysTime(SystemTime);
    // Если время показа сообщения об окончании времени истекло, сменим сообщение
    if ( TimeMsgEndTime&&((--TimeMsgEndTime)==0) ) ShowImageMessage(mimLocked);
    // Если таймер перезагрузки истек, то помечаем, что нужно перезагрузиться
    if ( TimeToReboot&&((--TimeToReboot)==0) ) State->CmdReboot();
    //
    if ( State->Timer(SystemTime) ) State->Save();
    State->StateInfo(&Info);
    SetState(&Info);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonRebootClick(TObject *Sender)
{
    // Команда отработает в TFormMain::SetState()
    State->CmdReboot();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonOptionsClick(TObject *Sender)
{
    Tag=WinExit(EWX_LOGOFF|EWX_FORCE);
}
//---------------------------------------------------------------------------
void TFormMain::SetState(MStateInfo *Info_)
{
    // Запрашиваем новые настройки
    if ( Info_->Changes&mdcOptions )
    {
        State->GetOptions(Options);
        // Обновим режим прозрачности
        SetTransp(Options->Flags&mcoTransp);
    }
    // Обновляем номер компьютера
    if ( Info_->Changes&mdcNumber )
    {
        LabelCompNum->Caption=IntToStr(Info_->Number);
        LabelCompNumShad->Caption=LabelCompNum->Caption;
        FormDesk->LabelCompNum->Caption=LabelCompNum->Caption;
        LockDesk.UpdateCompNum(Info_->Number);
    }
    // Режим работы
    if ( Info_->Changes&mdcState )
    {
        if ( Info_->State&mcsOpen )
        {
            // Открыт для настройки
            TimeToReboot=0;
            TimeMsgEndTime=0;
            Message->Stop();
            ShowImageMessage(mimNone);
            ShowGames(mgpAll);
        } else if ( Info_->State&mcsFree )
        {
            Message->Stop();
            LabelWorkTime->Caption="--:--";
            FormDesk->LabelWorkTime->Caption=LabelWorkTime->Caption;
            LockDesk.UpdateWorkTime(0);
            ShowGames(mgpNone);
            if ( Info_->Changes&mdcWorkTime )
            {
                // Время закончилось
                TimeToReboot=Options->RebootWait;
                TimeMsgEndTime=Options->MsgEndTime;
                ShowImageMessage(TimeMsgEndTime? mimEndTime: mimLocked);
            } else
            {
                // Компьютер закрыт
                TimeToReboot=0;
                TimeMsgEndTime=0;
                ShowImageMessage(mimLocked);
            }
        } else if ( Info_->State&mcsWork )
        {
            if ( Info_->Changes&mdcWorkTime )
            {
                // Компьютер запущен
                TimeToReboot=0;
                TimeMsgEndTime=0;
                Message->Stop(); MessageAreShowed=false;
            }
            // Дополнительные режимы
            if ( Info_->State&mcsPause )
            {
                // Время приостановлено
                ShowGames(mgpNone);
                ShowImageMessage(mimTimePaused);
            } else if ( Info_->State&mcsLock )
            {
                // Прикрыт
                ShowGames(mgpNone);
                ShowImageMessage(mimPaused);
            } else if ( Info_->State&mcsFine )
            {
                // Штраф
                ShowGames(mgpNone);
                ShowImageMessage(mimFine);
            } else
            {
                // Работа
                ShowImageMessage(mimNone);
                ShowGames(Info_->Programs);
            }
        }
        SpeedButtonOptions->Enabled=Info_->State&mcsOpen;
    }
    //
    if ( Info_->Changes&mdcWorkTime ) MessageAreShowed=false;
    // Выводим оставшееся время
    if ( Info_->State&mcsWork )
    {
        char line[5+1];
        sprintf(line,"%.2i:%.2i",Info_->ToEndWork/60,Info_->ToEndWork%60);
        LabelWorkTime->Caption=line;
        FormDesk->LabelWorkTime->Caption=line;
        LockDesk.UpdateWorkTime(Info_->ToEndWork);
        // Если до окончания времени осталось менее двух минут, запускаем показ сообщения
        if ( Options->ToEndTime&&(!MessageAreShowed)&&
            (Info_->ToEndWork<=Options->ToEndTime) )
        {
            MessageAreShowed=true;
            Message->Show(Options->MessageTime);
        }
    }

    if ( Info_->Changes&mdcPrograms )
    {
        // Список программ для запуска
        if ( Info_->State==mcsWork ) ShowGames(Info_->Programs);
        else if ( Info_->State&mcsOpen ) ShowGames(mgpAll);
    }
    //
    if ( Info_->Changes&mdcCommands )
    {
        if ( Info_->Commands&mccShutdown )
        {
            State->Save();
#ifndef _DEBUG
            Tag=WinExit(EWX_POWEROFF|EWX_FORCE);
#endif
            return;
        } else if ( Info_->Commands&mccReboot )
        {
            State->Save();
#ifndef _DEBUG
            Tag=WinExit(EWX_REBOOT|EWX_FORCE);
#endif                      
            return;
        }
    }
}
//---------------------------------------------------------------------------
void TFormMain::ShowGames(unsigned Pages_)
{
    FormGames->ShowGames(Pages_);
}
//---------------------------------------------------------------------------
void TFormMain::ShowImageMessage(int Message_)
{
    if ( Message_==mimNone ) LockDesk.Hide();
    else
    {
        char file_name[MAX_PATH+1];
        strcpy(file_name,win_dir);
        switch(Message_)
        {
            case mimEndTime: strcat(file_name,"TIMEOUT.BMP"); break;
            case mimLocked: strcat(file_name,"LOCK.BMP"); break;
            case mimPaused: strcat(file_name,"ADMNLOCK.BMP"); break;
            case mimFine: strcat(file_name,"FINE.BMP"); break;
            case mimTimePaused: strcat(file_name,"PAUSE.BMP"); break;
            default: return;
        }
        LockDesk.Show(file_name);
    }
}
//---------------------------------------------------------------------------
void TFormMain::SetTransp(bool Transp_)
{
    if ( Transp_ )
    {
        FormMain->TransparentColor=true;
        FormMain->TransparentColorValue=clFuchsia;
        FormMain->Color=clFuchsia;
        FormGames->Parent=NULL;
        FormGames->TransparentColor=true;
        FormGames->TransparentColorValue=clFuchsia;
        FormGames->Color=clFuchsia;
        FormGames->AlphaBlend=true;
        FormGames->AlphaBlendValue=220;
        FormGames->TreeViewGames->Color=(TColor)0x00000000;
        LockDesk.SetTransp(true);
    } else
    {
        FormMain->TransparentColor=false;
        FormMain->Color=clBlack;
        FormGames->Parent=FormMain;
        FormGames->TransparentColor=false;
        FormGames->Color=clBlack;
        FormGames->AlphaBlend=false;
        FormGames->TreeViewGames->Color=(TColor)0x0248422C;
        LockDesk.SetTransp(false);
    }
}
//---------------------------------------------------------------------------

