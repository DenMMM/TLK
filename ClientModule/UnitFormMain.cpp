//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <inifiles.hpp>
#include "UnitFormMain.h"
#include "UnitOptionsLoadSave.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
MState *State;
MThreadNetSync *ThreadNetSync;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    WSADATA WSAData;
    if ( ::WSAStartup(0x0002,&WSAData)!=0 )
    {
        ::MessageBox(Application->Handle,
            "Не установлена требуемая версия библиотеки WinSock или протокол TCP/IP.\nПрограмма не может быть запущена !",
            "TimeLocker - ошибка",MB_OK|MB_ICONERROR|MB_APPLMODAL);
        Close(); return;
    }

    ::GetWindowsDirectory(win_dir,MAX_PATH);
    strcat(win_dir,"\\");

    State=new MState;
    State->InitCriticalSection();
    ThreadNetSync=new MThreadNetSync(true);
    ThreadNetSync->FreeOnTerminate=true;

/*    char file_name[MAX_PATH];
    strcpy(file_name,win_dir);
    strcat(file_name,"SYSTEM.INI");
    TIniFile *IniFile=new TIniFile(file_name);
    if ( IniFile!=NULL )
    {
        ::GetModuleFileName(NULL,file_name,MAX_PATH);
        IniFile->WriteString("boot","shell",file_name);
        IniFile->UpdateFile();
        delete IniFile;
    }*/

    TimeToReboot=0;
    MessageTimer=0;
    State->Lock();
    if ( !StateLoad(State) ) StateSave(State);
    SetState();
    State->UnLock();

    TreeViewGames->Color=(TColor)0x0248422C;
    TreeViewGames->Font->Color=(TColor)0x02D2C66F;
    TimerTimer(Timer);
    Timer->Enabled=true;
    ThreadNetSync->Resume();
    ThreadNetSync->Pause(false);
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
    ThreadNetSync->Exit();
    State->DelCriticalSection();
    delete State;
    ::WSACleanup();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
    static unsigned int timer_1000msec=0;
    static HANDLE message_bitmap=NULL;
    static BITMAP bitmap;

    double SystemTime=(double)(Date()+Time());

    // Показываем сообщение о скором окончании времени
    if ( MessageTimer&&message_bitmap ) DrawMessage(message_bitmap,bitmap);
    // Позиционируем окно программы
    if ( Width!=Screen->Width ) { Left=0; Width=Screen->Width; }
    if ( Height!=Screen->Height ) { Top=0; Height=Screen->Height; }

    if ( (++timer_1000msec)>=(1000/Timer->Interval) )
    {
        timer_1000msec=0;
        // Выводим текущее время
        LabelSysTime->Caption=Time().FormatString("hh:nn");
        State->Lock();
        // Проверяем окончание времени работы и времени штрафа
        if ( State->ControlWorkTime() ) { TimeToReboot=1; StateSave(State); State->NeedUpdate=true; }
        else if ( State->ControlFineTime() ) { StateSave(State); State->NeedUpdate=true; }
        //
        if ( State->SizeWorkTime )
        {
            // Выводим оставшееся время
            double endtime=State->StartWorkTime+State->SizeWorkTime/(24.*60);
            if ( State->State&(mcsPause|mcsAuto) ) endtime+=SystemTime-State->StopTimerTime;
            LabelWorkTime->Caption=((TDate)endtime-SystemTime).FormatString("hh:nn");
            // Если до окончания времени осталось 120 секунд, то запускаем показ сообщения
            int timetoend=(State->SizeWorkTime-(SystemTime-State->StartWorkTime)*24*60)*6;
            if ( (timetoend==12)&&(!MessageTimer) )
            {
                MessageTimer=1;
                // Загружаем картинку с сообщением на все время его показа
                char file_name[MAX_PATH];
                strcpy(file_name,win_dir);
                strcat(file_name,"TIMEWARN.BMP");
                message_bitmap=::LoadImage(NULL,file_name,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
                if ( message_bitmap!=NULL )
                    if ( ::GetObject(message_bitmap,sizeof(BITMAP),&bitmap)==NULL )
                        { ::DeleteObject(message_bitmap); message_bitmap=NULL; }
            }
        } else { LabelWorkTime->Caption="--:--";}
        //
        if ( MessageTimer&&((++MessageTimer)>=20) )
        {
            MessageTimer=0;
            // Удаляем из памяти загруженную ранее картинку
            if ( message_bitmap ) { ::DeleteObject(message_bitmap); message_bitmap=NULL; }
        }
        // Проверяем время последнего контакта с админским модулем
        if ( State->ControlPingTime() ) { StateSave(State); State->NeedUpdate=true; }
        // Если таймер перезагрузки истек, то помечаем, что нужно перезагрузиться
        if ( TimeToReboot&&((++TimeToReboot)>=WaitRebootTime) )
        {
            TimeToReboot=0; State->State|=mcsReboot;
            State->NeedUpdate=true;
        }
        State->UnLock();
    }

    // Блокируем CTRL+ALT+DEL
//    ::SystemParametersInfo(SPI_SCREENSAVERRUNNING,1,NULL,0);
    // Корректируем поведение программы в соответствии с режимом работы
    State->Lock();
    unsigned int st=State->State;
    if ( (st&(mcsFree|mcsFine|mcsLock|mcsPause|mcsNotUse))&&(!(st&mcsAuto)) ) HideWindows(true);
    else HideWindows(false);
    if ( State->NeedUpdate ) { State->NeedUpdate=false; SetState(); }
    State->UnLock();
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
    Tag=Exit(EWX_REBOOT);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::AddGamesToTree(MGames *Games_, TTreeNode *TreeNode_, TImageList *ImageList_)
{
    static TTreeNode *NewTreeNode;
    static HICON icon;

    if ( Games_==NULL ) return;
    for ( MGame *Game=(MGame*)Games_->FirstItem; Game; Game=(MGame*)Game->NextItem )
    {
        NewTreeNode=TreeViewGames->Items->AddChild(TreeNode_,Game->Name);
        // Иконка
        icon=::ExtractIcon(NULL,Game->IconFile.c_str(),0);
        if ( (icon==NULL)||(((int)icon)==1) )
        {
            NewTreeNode->ImageIndex=-1; NewTreeNode->SelectedIndex=-1;
        } else
        {
            NewTreeNode->ImageIndex=::ImageList_ReplaceIcon((HIMAGELIST)ImageList_->Handle,-1,icon);
            NewTreeNode->SelectedIndex=NewTreeNode->ImageIndex;
        }
        // Командная строка для запуска
        NewTreeNode->Data=(void*)new char[Game->CommandLine.Length()+1];
        strcpy((char*)NewTreeNode->Data,Game->CommandLine.c_str());
        // Подуровни дерева
        AddGamesToTree(Game->SubGames,NewTreeNode,ImageList_);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ShowGames(unsigned int Pages_)
{
    TreeViewGames->Items->Clear();
    ImageListGamesIcons->Clear();
    // Если не заданы группы программ для загрузки, то убираем панель программ...
    if ( Pages_==mgpNone ) { PanelGames->Hide(); return; }
    // иначе добавляем в список указанные программы.
    MGames *Games=new MGames; if ( Games==NULL ) return;
    State->LockGames();
    if ( GamesLoad(Games) )
    {
        if ( Pages_&mgp1 ) AddGamesToTree(((MGame*)Games->Item(0))->SubGames,NULL,ImageListGamesIcons);
        if ( Pages_&mgp2 ) AddGamesToTree(((MGame*)Games->Item(1))->SubGames,NULL,ImageListGamesIcons);
        if ( Pages_&mgp3 ) AddGamesToTree(((MGame*)Games->Item(2))->SubGames,NULL,ImageListGamesIcons);
        if ( Pages_&mgp4 ) AddGamesToTree(((MGame*)Games->Item(3))->SubGames,NULL,ImageListGamesIcons);
        if ( Pages_&mgp5 ) AddGamesToTree(((MGame*)Games->Item(4))->SubGames,NULL,ImageListGamesIcons);
    }
    State->UnLockGames();
    delete Games;
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
void __fastcall TFormMain::DrawMessage(HANDLE message_bitmap, BITMAP &bitmap)
{
    HWND dsk_wnd;
    HDC dsk_dc, comp_dc;
    HANDLE old_bitmap;

    if ( (dsk_wnd=::GetDesktopWindow())==NULL ) return;
    if ( (dsk_dc=::GetWindowDC(dsk_wnd))==NULL ) return;
    if ( (comp_dc=::CreateCompatibleDC(dsk_dc))==NULL )
        { ::ReleaseDC(dsk_wnd,dsk_dc); return; }
    old_bitmap=::SelectObject(comp_dc,message_bitmap);
    ::BitBlt(dsk_dc,0,0,bitmap.bmWidth,bitmap.bmHeight,
        comp_dc,0,0,SRCINVERT);
    old_bitmap=::SelectObject(comp_dc,old_bitmap);
    ::DeleteDC(comp_dc);
    ::ReleaseDC(dsk_wnd,dsk_dc);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetState()
{
    LabelComputerNumber->Caption=IntToStr(State->Number);
    unsigned int state=State->State;
    //
    unsigned int showgames;
    int ImageMessage;
    if ( state&mcsAuto ) { showgames=mgpAll; ImageMessage=mimNone; TimeToReboot=0; MessageTimer=100; }
    else if ( state&mcsPause ) { showgames=mgpNone; ImageMessage=mimTimePaused; MessageTimer=100; }
    else if ( state&mcsLock ) { showgames=mgpNone; ImageMessage=mimPaused; }
    else if ( state&mcsFine ) { showgames=mgpNone; ImageMessage=mimFine; }
    else if ( state&mcsWork ) { showgames=State->GamesPages; ImageMessage=mimNone; TimeToReboot=0; }
    else if ( state&mcsFree ) { showgames=mgpNone; MessageTimer=100;
        if ( TimeToReboot ) ImageMessage=mimEndTime; else ImageMessage=mimLocked; }
    else { showgames=mgpNone; ImageMessage=mimNone; }
    ShowGames(showgames);
    ShowImageMessage(ImageMessage);
    //
    SpeedButtonOptions->Enabled=state&mcsAuto;
    //
    if ( state&mcsWtLocker )
    {
/*        char file_name[MAX_PATH];
        strcpy(file_name,win_dir);
        strcat(file_name,"SYSTEM.INI");
        TIniFile *IniFile=new TIniFile(file_name);
        if ( IniFile==NULL ) return;
        strcpy(file_name,win_dir);
        strcat(file_name,"EXPLORER.EXE");
        IniFile->WriteString("boot","shell",file_name);
        IniFile->UpdateFile();
        delete IniFile;*/
        State->State&=~mcsWtLocker;
        StateSave(State);
//        Tag=Exit(EWX_REBOOT);
    } else if ( state&mcsReboot )
    {
        State->State&=~mcsReboot;
        StateSave(State);
        Tag=Exit(EWX_REBOOT);
//        Tag=Exit(EWX_REBOOT|EWX_FORCE);
    } else if ( state&mcsShutdown )
    {
        State->State&=~mcsShutdown;
        StateSave(State);
//        Tag=true; Close();
        Tag=Exit(EWX_SHUTDOWN);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonOptionsClick(TObject *Sender)
{
    Tag=Exit(EWX_LOGOFF);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::HideWindows(bool Hide_)
{
    HWND Wnd;
    char buff[24+1];

    // Определяем текущее верхнее окно в системе
    Wnd=::GetForegroundWindow();
    // Провеяем можно ли это окно свернуть
    if ( (Wnd==Handle)||(Wnd==Application->Handle) ) return;
    if ( ::GetClassName(Wnd,buff,24)&&(!strcmp(buff,"WindowsScreenSaverClass")) ) return;
    // Сворачиваем окно
    if ( Hide_ )
    {
        ::ShowWindow(Wnd,SW_MINIMIZE);
        ::ShowWindow(Handle,SW_SHOWNORMAL);
        ::SetForegroundWindow(Handle);
    }
}
//---------------------------------------------------------------------------

