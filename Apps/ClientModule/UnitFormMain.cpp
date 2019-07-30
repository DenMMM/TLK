//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdexcept.h>
#pragma hdrstop

#include "UnitFormMain.h"
#include "UnitTariffs.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
char win_dir[MAX_PATH];
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
    ::GetWindowsDirectory(win_dir,MAX_PATH);
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
        Options->RebootWait=20;
        Options->AutoLockTime=15;
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

    TreeViewGames->Color=(TColor)0x0248422C;
    TreeViewGames->Font->Color=(TColor)0x02D2C66F;
    TimeToReboot=0;
    MessageAreShowed=false;

    Timer->Interval=500;
    TimerTimer(NULL);
    Timer->Enabled=true;

    // Запуск сети
    Sync->Associate(State);
    Sync->Start();
    Send->Start();

    // Запускаем программы из раздела автозапуска
    RegExecList(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
//    RegExecList(HKEY_LOCAL_MACHINE,"SOFTWARE\\MMM Groups\\TLK\\3.0\\Client\\Run");

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
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
    static unsigned int timer_1000msec=1000;
    __int64 SystemTime;
    MStateInfo Info;

    // Позиционируем окно программы
    if ( Width!=Screen->Width ) { Left=0; Width=Screen->Width; }
    if ( Height!=Screen->Height ) { Top=0; Height=Screen->Height; }

    if ( (++timer_1000msec)>=(1000/Timer->Interval) )
    {
        timer_1000msec=0;
        // Выводим текущее время
        LabelSysTime->Caption=Time().FormatString("hh:nn");
        // Если таймер перезагрузки истек, то помечаем, что нужно перезагрузиться
        if ( TimeToReboot&&((--TimeToReboot)==0) )
        {
            TimeToReboot=0;
            ShowImageMessage(mimLocked);
            State->CmdReboot();
        }
    }
    //
    GetLocalTimeInt64(&SystemTime);
    if ( State->Timer(SystemTime) ) State->Save();
    State->StateInfo(&Info);
    SetState(&Info);

    // Блокируем CTRL+ALT+DEL (для Win98)
//    ::ShowWindow(Handle,SW_HIDE);
//    BOOL old;
//    ::SystemParametersInfo(SPI_SCREENSAVERRUNNING,0,&old,0);
//    ::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,TRUE,&old,0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TreeViewGamesDeletion(TObject *Sender,
      TTreeNode *Node)
{
    delete[] (char*)Node->Data;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TreeViewGamesDblClick(TObject *Sender)
{
    TTreeNode *TreeNode;
    if ( (TreeNode=TreeViewGames->Selected)==NULL ) return;
    if ( TreeNode->HasChildren ) return;
    AnsiString path, command;
    int pos;
    command=(char*)TreeNode->Data;
    //
    STARTUPINFO si;
    memset(&si,0,sizeof(STARTUPINFO));
    si.cb=sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;
    // Запускаем игру
    if ( (pos=command.AnsiPos("\""))==NULL ) return;
    path=command.SubString(pos+1,command.Length()-pos);
    if ( (pos=path.AnsiPos("\""))==NULL ) return;
    path.Delete(pos,path.Length()-pos+1);
    path=ExtractFilePath(path);
    ::CreateProcess(NULL,command.c_str(),NULL,NULL,false,
        CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,NULL,
        path.c_str(),&si,&pi);

/*    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset(&si,0,sizeof(STARTUPINFO));
    si.cb=sizeof(STARTUPINFO);
    ::CreateProcess(NULL,(char*)TreeNode->Data,NULL,NULL,FALSE,
        CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
        NULL,(char*)TreeNode->Data,&si,&pi);*/
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
    Tag=WinExit(EWX_LOGOFF);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetState(MStateInfo *Info_)
{
    char line[50];

    // Запрашиваем новые настройки
    if ( Info_->Changes&mdcOptions ) State->GetOptions(Options);
    // Обновляем номер компьютера
    if ( Info_->Changes&mdcNumber ) LabelComputerNumber->Caption=IntToStr(Info_->Number);
    // Режим работы
    if ( Info_->Changes&mdcState )
    {
        if ( Info_->State&mcsOpen )
        {
            // Открыт для настройки
            TimeToReboot=0; Message->Stop();
            ShowImageMessage(mimNone);
            ShowGames(mgpAll);
        } else if ( Info_->State&mcsFree )
        {
            Message->Stop();
            LabelWorkTime->Caption="--:--";
            ShowGames(mgpNone);
            if ( Info_->Changes&mdcWorkTime )
            {
                // Время закончилось
                TimeToReboot=Options->RebootWait;
                ShowImageMessage(Options->RebootWait?mimEndTime:mimLocked);
            } else
            {
                // Компьютер закрыт
                TimeToReboot=0;
                ShowImageMessage(mimLocked);
            }
        } else if ( Info_->State&mcsWork )
        {
            if ( Info_->Changes&mdcWorkTime )
            {
                // Компьютер запущен
                TimeToReboot=0;
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
        // Подавляем заставку на свободном компьютере
//        ::SystemParametersInfo(SPI_SCREENSAVERRUNNING,    /// не работает...
//            Info_->State&mcsFree?1:0,NULL,0);
    }
    //
    if ( Info_->Changes&mdcWorkTime ) MessageAreShowed=false;
    // Выводим оставшееся время
    if ( Info_->State&mcsWork )
    {
        sprintf(line,"%i:%.2i",Info_->ToEndWork/60,Info_->ToEndWork%60);
        LabelWorkTime->Caption=line;
        // Если до окончания времени осталось менее двух минут, запускаем показ сообщения
        if ( Options->ToEndTime&&(!MessageAreShowed)&&
            (Info_->ToEndWork<=Options->ToEndTime) )
        {
            MessageAreShowed=true;
            Message->Show(Options->MessageTime);
        }
    }
    //
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
            Tag=WinExit(EWX_POWEROFF);        ///
            return;
        } else if ( Info_->Commands&mccReboot )
        {
            State->Save();
            Tag=WinExit(EWX_REBOOT);          ///
            return;
        }
    }
    // Принудительно сворачиваем окна, как можем...
    if ( (Info_->State&(mcsFree|mcsFine|mcsLock|mcsPause))&&
        (!(Info_->State&mcsOpen)) ) HideWindows(true);
    else HideWindows(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::AddGamesToTree(MGames *Games_, TTreeNode *TreeNode_, TImageList *ImageList_)
{
    TTreeNode *NewTreeNode;
    HICON icon;
    char *cmdline;

    if ( Games_==NULL ) return;
    for ( MGame *Game=(MGame*)Games_->gFirst(); Game;
        Game=(MGame*)Game->gNext() )
    {
        NewTreeNode=TreeViewGames->Items->AddChild(TreeNode_,Game->Name);
        // Иконка
        icon=::ExtractIcon(NULL,Game->Icon,0);
        if ( (icon==NULL)||(((int)icon)==1) )
        {
            NewTreeNode->ImageIndex=-1;
            NewTreeNode->SelectedIndex=-1;
        } else
        {
            NewTreeNode->ImageIndex=
                ::ImageList_ReplaceIcon((HIMAGELIST)ImageList_->Handle,-1,icon);
            NewTreeNode->SelectedIndex=
                NewTreeNode->ImageIndex;
        }
        // Командная строка для запуска
        cmdline=new char[strlen(Game->Command)+1];
        strcpy(cmdline,Game->Command);
        NewTreeNode->Data=(void*)cmdline;
        // Подуровни дерева
        AddGamesToTree(Game->SubGames,NewTreeNode,ImageList_);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ShowGames(unsigned int Pages_)
{
    MGames Games;
    MGame *Game;

    TreeViewGames->Items->Clear();
    ImageListGamesIcons->Clear();
    // Если не заданы группы программ для загрузки, то убираем панель программ...
    if ( Pages_==mgpNone ) { PanelGames->Hide(); return; }
    // иначе добавляем в список указанные программы.
    if ( State->GetGames(&Games) )
    {
        if ( (Pages_&mgp1)&&((Game=(MGame*)Games.Item(0))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp2)&&((Game=(MGame*)Games.Item(1))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp3)&&((Game=(MGame*)Games.Item(2))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp4)&&((Game=(MGame*)Games.Item(3))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp5)&&((Game=(MGame*)Games.Item(4))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp6)&&((Game=(MGame*)Games.Item(5))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp7)&&((Game=(MGame*)Games.Item(6))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp8)&&((Game=(MGame*)Games.Item(7))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
    }
    PanelGames->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ShowImageMessage(int Message_)
{
    if ( Message_!=mimNone )
    {
        char file_name[MAX_PATH];
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
        try { ImageMessage->Picture->LoadFromFile(file_name); }
        catch (EFOpenError *error) { ImageMessage->Picture->Assign(NULL); }
        ImageMessage->Show();
    } else
    {
        ImageMessage->Picture->Assign(NULL);
        ImageMessage->Hide();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::HideWindows(bool Hide_)
{
    HWND Wnd;
    char buff[23+1];

    // Определяем текущее верхнее окно в системе
    Wnd=::GetForegroundWindow();
    // Провеяем можно ли это окно свернуть
    if ( (Wnd==Handle)||
         (Wnd==Application->Handle)||
         (!::IsWindow(Wnd)) ) return;
    // Не заставка ли это
    if ( ::GetClassName(Wnd,buff,23)&&
        (!strcmp(buff,"WindowsScreenSaverClass")) ) return;
    // Сворачиваем окно
    if ( Hide_ )
    {
        ::ShowWindow(Wnd,SW_MINIMIZE);      ///
        ::ShowWindow(Handle,SW_SHOWNORMAL); ///
        ::SetForegroundWindow(Handle);      ///
    }
}
//---------------------------------------------------------------------------

