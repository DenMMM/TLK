//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <shellapi.h>
#include <stdio.h>
#include "UnitFormMain.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
#define ModeNotLock ((unsigned char)0)
#define ModeWork ((unsigned char)1)
#define ModeWorkPause ((unsigned char)2)
#define ModeLock ((unsigned char)3)

#define MessageNoMessage ((unsigned char)0)
#define MessageTimeOut ((unsigned char)1)
#define MessageWorkPause ((unsigned char)2)
#define MessageLock ((unsigned char)3)
#define MessageRefresh ((unsigned char)4)

#define NetMessageClientMessage ((unsigned char)0)
#define NetMessageSendMode ((unsigned char)1)
#define NetMessageAdminWarning ((unsigned char)2)

#define LogEventActive ((unsigned char)0)
#define LogEventStart ((unsigned char)1)
#define LogEventModeWork ((unsigned char)2)
#define LogEventModeWorkPause ((unsigned char)3)
#define LogEventModeLock ((unsigned char)4)
#define LogEventModeNotLock ((unsigned char)5)
#define LogEventAddTime ((unsigned char)6)
#define LogEventAdminWarning ((unsigned char)7)
#define LogEventEnterToOptions ((unsigned char)8)
//---------------------------------------------------------------------------
AnsiString WinDir;
int XScrSize=-1, YScrSize=-1;
typedef DWORD RegServProc(DWORD,DWORD);
int _mode_;
//---------------------------------------------------------------------------
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
    HANDLE process;
    HINSTANCE Library;
    RegServProc *RegServProcFunc;
    char windir[512];

    GetWindowsDirectory(windir,512);
    WinDir=windir;
    // Устанавливаем приоритет программы
    process=GetCurrentProcess();
    SetPriorityClass(process,HIGH_PRIORITY_CLASS);
    // Скрываем программу из CTRL+ALT+DEL
    if ( (Library=LoadLibrary("KERNEL32.DLL"))>=(HINSTANCE)32 )
    {
        RegServProcFunc=(RegServProc*)GetProcAddress(Library,"RegisterServiceProcess");
        if ( RegServProcFunc!=NULL ) RegServProcFunc(NULL,1);
        FreeLibrary(Library);
    }
    // Загружаем основные настройки
    if ( Options.Load() )
    {
        Application->MessageBox("Отсутствует корректный файл настроек 'TmLockCl.cfg'. Программа не может быть запущена !",
            "Ошибка - Time Locker",MB_OK+MB_ICONERROR);
        Tag=1;
        Application->Terminate();
        return;
    }
    // Загружаем информацию о состоянии программы
    if ( Options.LoadMode() )
    {
        Application->MessageBox("Отсутствует корректный файл состояния 'TmLockCl.mde'. Программа будет работать в закрытом режиме !",
            "Предупреждение - Time Locker",MB_OK+MB_ICONWARNING);
        Options.BeginTime=0; Options.SizeTime=0; Options.Mode=ModeLock;
    }
    LabelNumberComputer->Caption=IntToStr(Options.NumberComputer);
    // Записываем в лог сведения о старте программы
    AddBossLog(LogEventStart);
    // Устанавливаем текущий режим
    LoadGamesMenu();
    SetMode(Options.Mode);
    TimerTimesTimer(NULL);
    TimerTimes->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    if ( Tag )
    {
        TimerTimes->Enabled=false;
        TimerRefreshMessage->Enabled=false;
        try { NMMsg->Abort(); NMMsg->Disconnect(); NMMsg->Host=""; } catch ( ESockError *Error ) {;}
        try { NMMSGServ->Abort(); NMMSGServ->Disconnect(); NMMSGServ->Host=""; } catch ( ESockError *Error ) {;}
        Action=caFree;
    } else Action=caNone;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimesTimer(TObject *Sender)
{
    int CurrentTime, Hours, Minutes;

    // Определяем текущее время
    CurrentTime=(int)((double)Time()*24.*60.);
    Hours=CurrentTime/60; Minutes=CurrentTime%60;
    LabelTime->Caption=IntToStr(Hours/10)+IntToStr(Hours%10)+":"+
        IntToStr(Minutes/10)+IntToStr(Minutes%10);
    // Определяем оставшееся время
    if ( (Options.Mode==ModeWork)||(Options.Mode==ModeWorkPause) )
    {
        CurrentTime=Options.BeginTime+Options.SizeTime*4-
            (int)((double)(Date()+Time())*24*60*4);
        // Если время работы закончилось, то переходим в 'закрытый' режим
        if ( CurrentTime<0 )
        {
            SetMode(ModeLock);
            return;
        }
        CurrentTime/=4;
        // Выводим оставшееся время работы
        Hours=CurrentTime/60; Minutes=CurrentTime%60;
        LabelTimeToEndTime->Caption=IntToStr(Hours/10)+IntToStr(Hours%10)+":"+
            IntToStr(Minutes/10)+IntToStr(Minutes%10);
    }
    // Устанавливаем размеры окна программы
    if ( (XScrSize!=Screen->Width)||(YScrSize!=Screen->Height) )
    {
        XScrSize=Screen->Width; YScrSize=Screen->Height;
        Left=0; Top=0;
        Width=XScrSize; Height=YScrSize;
    }
    // Проверяем не пора ли разрешить запуск игр
    if ( TimerTimes->Tag>=0 )
    {
        if ( (--(TimerTimes->Tag))<=0 )
        {
            TimerTimes->Tag=-1;
            ListViewGames->Cursor=crHandPoint;
        }
    }
    // Запись в лог признака активности программы
    Options.BossLogTimer--;
    if ( Options.BossLogTimer<=0 ) AddBossLog(LogEventActive);
    // Выполнение перезагрузки или выключения компьютера
    switch ( Tag )
    {
        case 2: Tag=0; SpeedButtonRebootClick(NULL); break;
        case 3: Tag=0; BitBtnShutdownClick(NULL); break;
        default: break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerRefreshMessageTimer(TObject *Sender)
{
    if ( TimerRefreshMessage->Tag>0 ) TimerRefreshMessage->Tag--;
    else if ( !TimerRefreshMessage->Tag )
    {
        TimerRefreshMessage->Tag=-1;
        MessageImage(MessageLock);
    }
    MessageImage(MessageRefresh);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NMMSGServMSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
    char message[16+1+2+1+16+1+100+1], *curpos=message, *password;
    int size, passwsize;

    NMMSGServ->TimeOut=10000;

    // Проверяем размер сообщения
    if ( (sFrom.Length()>15)||(sMsg=="")||(sMsg.Length()>(16+1+16+1+100)) ) return;
    // Если адрес сервера изменился, а автонастройка не включена, то выходим
    if ( (Options.ServerAddress!=sFrom)&&(!Options.ServerAuto) ) return;

    // Расшифровываем сообщение
    password=Options.PasswordToConnect.c_str();
    passwsize=Options.PasswordToConnect.Length();
    size=sMsg.Length(); memcpy(message,sMsg.c_str(),size); message[size]='\0';
    for ( int i=0, j=passwsize-1; i<size; i++, j=j?(passwsize-1):(j-1) )
    {
        char a;
        message[i]=(unsigned char)(a=(char)(message[i]-password[j]))>
            (unsigned char)message[i] ? (char)(a-1):a;
    }

    curpos=message;
    // Извлекаем из сообщения пароль и проверяем его
    if ( (password=strchr(curpos,'\n'))==NULL ) return;
    *password='\0';
    if ( strcmp(curpos,Options.PasswordToConnect.c_str()) ) return;
    curpos=password+1;

    // Определяем номер компьютера
    if ( (password=strchr(curpos,'\n'))==NULL ) return;
    *password='\0';
    try { size=StrToInt(curpos); } catch ( EConvertError *Error) { return; }
    curpos=password+1;
    // Если изменился адрес сервера или номер компьютера, то применяем их
    if ( Options.ServerAuto )
    {
        passwsize=0;
        if ( Options.ServerAddress!=sFrom )
        {
            Options.ServerAddress=sFrom;
            passwsize=1;
        }
        if ( size!=Options.NumberComputer )
        {
            Options.NumberComputer=size;
            LabelNumberComputer->Caption=IntToStr(Options.NumberComputer);
            passwsize=1;
            Options.SaveMode();
        }
        if ( passwsize ) Options.Save();
    }

    // Разбираем сообщение
    if ( (password=strchr(curpos,'\n'))==NULL ) return;
    *password='\0';
    if ( !strcmp(curpos,"Work") )
    {
        // Режим 'работа'
        if ( (Options.Mode==ModeNotLock)||(Options.Mode==ModeWork) ) return;
        if ( Options.Mode==ModeLock )
        {
            try { size=StrToInt(password+1); } catch ( EConvertError *Error ) { return; }
            if ( size<=0 ) return;
            if ( size>=(24*60) ) size=24*60-1;
            Options.SizeTime=size;
        }
        SetMode(ModeWork);
    } else if ( !strcmp(curpos,"WorkPause") )
    {
        // Режим 'приостановлен администратором'
        if ( (Options.Mode==ModeNotLock)||(Options.Mode!=ModeWork) ) return;
        SetMode(ModeWorkPause);
    } else if ( !strcmp(curpos,"Lock") )
    {
        // Режим 'закрыт'
        if ( (Options.Mode==ModeNotLock)||(Options.Mode!=ModeWorkPause) ) return;
        SetMode(ModeLock);
    } else if ( !strcmp(curpos,"AddTime") )
    {
        // Добавление времени
        if ( (Options.Mode!=ModeWork)&&(Options.Mode!=ModeWorkPause) ) return;
        try { size=StrToInt(password+1); } catch ( EConvertError *Error ) { return; }
        if ( (size<=0)||(size>=(24*60)) ) return;
        Options.SizeTime+=size;
        if ( Options.SizeTime>=(24*60) ) Options.SizeTime=24*60-1;
        Options.SaveMode();
        AddBossLog(LogEventAddTime);
        SendNetMessage(NetMessageSendMode,"");
    } else if ( !strcmp(curpos,"RequestMode") )
    {
        // Запрос текущего состояния
        SendNetMessage(NetMessageSendMode,"");
    } else if ( !strcmp(curpos,"Message") )
    {
        // Сообщение администратора
        AddLogMessage(password+1);
    } else if ( !strcmp(curpos,"Reboot") )
    {
        // Перезагрузка компьютера
        if ( (Options.Mode==ModeNotLock)||(Options.Mode==ModeWork) ) return;
        Tag=2;
    } else if ( !strcmp(curpos,"Shutdown") )
    {
        // Выключение компьютера
        if ( (Options.Mode==ModeNotLock)||(Options.Mode==ModeWork) ) return;
        Tag=3;
    } else if ( !strcmp(curpos,"NotLock") )
    {
        // Автономный режим
        if ( Options.Mode!=ModeLock ) return;
        SetMode(ModeNotLock);
    } else if ( !strcmp(curpos,"Normal") )
    {
        // Нормальный режим
        if ( Options.Mode!=ModeNotLock ) return;
        SetMode(ModeLock);
    }
}
//---------------------------------------------------------------------------
int __fastcall TFormMain::SendNetMessage(int Type, AnsiString Message)
{
    char message[16+1+16+1+100+1], *curpos=message, *password;
    int size, passwsize;

    if ( (size=Message.Length())>100 ) return -1;
    // Формируем сообщение
    password=Options.PasswordToConnect.c_str();
    passwsize=Options.PasswordToConnect.Length();
    *(curpos=(char*)memcpy(curpos,password,passwsize)+passwsize)++='\n';
    switch ( Type )
    {
        case NetMessageClientMessage:
            curpos=(char*)memcpy(curpos,"Message\n",8)+8;
            size=Message.Length();
            curpos=(char*)memcpy(curpos,Message.c_str(),size)+size;
            break;
        case NetMessageSendMode:
            curpos=(char*)memcpy(curpos,"Mode\n",5)+5;
            curpos+=sprintf(curpos,"%d\n",Options.Mode);
            if ( (Options.Mode==ModeNotLock)||(Options.Mode==ModeLock) )
            {
                curpos=(char*)memcpy(curpos,"0\n0",3)+3;
            } else
            {
                curpos+=sprintf(curpos,"%d\n",Options.SizeTime);
                curpos+=sprintf(curpos,"%d",(Options.BeginTime+Options.SizeTime*4-
                    (int)((double)(Date()+Time())*24*60*4))/4);
            }
            break;
        case NetMessageAdminWarning:
            curpos=(char*)memcpy(curpos,"Warning\n",8)+8;
            break;
        default: return -1;
    }
    *curpos='\0';
    size=curpos-message;

    // Шифруем сообщение
    for ( int i=0, j=passwsize-1; i<size; i++, j=j?(passwsize-1):(j-1) )
    {
        char a;
        message[i]=(unsigned char)(a=(char)(message[i]+password[j]))<
            (unsigned char)message[i] ? (char)(a+1):a;
    }

    // Отправляем сообщение
    try
    {
        NMMsg->FromName=NMMsg->LocalIP;
        NMMsg->Host=Options.ServerAddress;
        NMMsg->TimeOut=10000;
        NMMsg->PostIt(message);
    } catch ( ESockError *Error ) { NMMsg->Disconnect(); NMMsg->Abort(); return -1; }

    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MessageImage(int Message)
{
    static HWND window, oldwindow=NULL;
    AnsiString MessageFile=WinDir+"\\", MessageText="Неизвестное сообщение";

    if ( Message!=MessageRefresh ) { XScrSize=-1; YScrSize=-1; }
    switch ( Message )
    {
        case MessageNoMessage:
            // Выключаем таймер 'назойливости' программы
            TimerRefreshMessage->Enabled=false;
            // Убираем текст и рисунок с панели сообщения
            PanelImage->Enabled=false;
            PanelImage->Caption="";
            ImageMessage->Picture->Assign(NULL);
            PanelGameDescription->Height=86; PanelGameDescription->Enabled=true;
            PanelGames->Enabled=true;
            PanelGames->BringToFront();
            // Восстанавливаем окно игры и делаем его поверх других
            if ( oldwindow!=NULL )
            {
                ShowWindow(oldwindow,SW_RESTORE);
                SetForegroundWindow(oldwindow);
            }
            return;
        case MessageTimeOut:
            MessageFile+="TimeOut.bmp"; MessageText="Ваше время закончилось !";
            break;
        case MessageWorkPause:
            MessageFile+="AdmnLock.bmp"; MessageText="Компьютер заблокирован !";
            break;
        case MessageLock:
            MessageFile+="Lock.bmp"; MessageText="Закрыто...";
            break;
        case MessageRefresh:
            // Сворачиваем текущее окно, если оно не принадлежит программе или
            // скринсейверу, восстанавливаем окно программы и делаем его поверх других
            window=GetForegroundWindow();
            if ( (window==Handle)||(window==NULL) ) return;
            if ( FindWindow("WindowsScreenSaverClass",NULL)!=NULL ) return;
            oldwindow=window;
            ShowWindow(window,SW_MINIMIZE);
            ShowWindow(Handle,SW_SHOWNORMAL);
            SetForegroundWindow(Handle);
            return;
        default: break;
    }
    // Подготавливаем сообщение
    PanelGames->Enabled=false;
    PanelGameDescription->Enabled=false; PanelGameDescription->Height=0;
    MemoGameDescription->Text="";
    try
    {
        ImageMessage->Picture->LoadFromFile(MessageFile);
        PanelImage->Caption="";
    } catch ( EFOpenError *Error )
    {
        ImageMessage->Picture->Assign(NULL);
        PanelImage->Caption=MessageText;
    }
    PanelImage->BringToFront();
    // Задаем время показа сообщения об окончании времени
    if ( Message==MessageTimeOut ) TimerRefreshMessage->Tag=20*1000/TimerRefreshMessage->Interval;
    else TimerRefreshMessage->Tag=-1;
    // Включаем таймер 'назойливости' программы
    TimerRefreshMessage->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetMode(int Mode)
{
    static STARTUPINFO startupinfoprogram;
    static PROCESS_INFORMATION processinfoprogram;
    int OldMode;

    OldMode=Options.Mode;
    switch ( Mode )
    {
        case ModeNotLock:
            Options.Mode=Mode;
            Options.SaveMode(); AddBossLog(LogEventModeNotLock);
            Options.BeginTime=0; Options.SizeTime=0;
            LabelNumberComputer->Enabled=false;
            LabelTimeToEndTime->Enabled=false;
            LabelTimeToEndTime->Caption="00:00";
            LabelMessages->Caption="";
            EditNikName->Text="Personal";
            SpeedButtonReboot->Enabled=true;
            MessageImage(MessageNoMessage);
            // Запускаем "проводник"
            startupinfoprogram.lpReserved=NULL;
            CreateProcess(NULL,(WinDir+"\\explorer.exe").c_str(),NULL,NULL,false,
                CREATE_DEFAULT_ERROR_MODE+NORMAL_PRIORITY_CLASS,NULL,WinDir.c_str(),
                &startupinfoprogram,&processinfoprogram);
            break;
        case ModeWork:
            if ( OldMode==ModeLock )
            {
                Options.BeginTime=(int)(((double)(Date()+Time()))*24*60*4);
                LabelMessages->Caption="";
                EditNikName->Text="User";
            }
            Options.Mode=Mode;
            Options.SaveMode(); AddBossLog(LogEventModeWork);
            LabelNumberComputer->Enabled=true;
            LabelTimeToEndTime->Enabled=true;
            SpeedButtonReboot->Enabled=true;
            MessageImage(MessageNoMessage);
            break;
        case ModeWorkPause:
            Options.Mode=Mode;
            Options.SaveMode(); AddBossLog(LogEventModeWorkPause);
            LabelNumberComputer->Enabled=true;
            LabelTimeToEndTime->Enabled=true;
            SpeedButtonReboot->Enabled=false;
            MessageImage(MessageWorkPause);
            break;
        case ModeLock:
            Options.Mode=Mode;
            Options.SaveMode(); AddBossLog(LogEventModeLock);
            Options.BeginTime=0; Options.SizeTime=0;
            LabelNumberComputer->Enabled=true;
            LabelTimeToEndTime->Enabled=false;
            LabelTimeToEndTime->Caption="00:00";
            LabelMessages->Caption="";
            EditNikName->Text="";
            SpeedButtonReboot->Enabled=false;
            if ( (OldMode==ModeWork)||(OldMode==ModeWorkPause) ) MessageImage(MessageTimeOut);
            else MessageImage(MessageLock);
            break;
        default: return;
    }
    SendNetMessage(NetMessageSendMode,"");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SwitchTo (TWinControl *From, TWinControl *To, TWinControl *Active)
{
    From->Hide(); To->Show();
    if ( Active ) if ( Active->CanFocus() ) Active->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnNetOptionsOkClick(TObject *Sender)
{
    // Проверяем задание адреса сервера
    if ( !CheckBoxAutoServer->Checked )
        if ( EditServerAddress->Text=="" ) { EditServerAddress->SetFocus(); return; }
    // Проверяем длину пароля на соединение с сервером
    if ( EditPasswordToConnect->Text.Length()<3 ) { EditPasswordToConnect->SetFocus(); return; }
    // Сохраняем остальные настройки
    Options.ServerAddress=EditServerAddress->Text;
    Options.ServerAuto=CheckBoxAutoServer->Checked;
    Options.PasswordToConnect=EditPasswordToConnect->Text;
    Options.NumberComputer=StrToInt(EditNumberComputer->Text);
    Options.Save(); Options.SaveMode();
    // Меняем текст с номером компьютера
    LabelNumberComputer->Caption=IntToStr(Options.NumberComputer);
    // Переключаемся в меню выбора настроек
    SwitchTo(GroupBoxNetOptions,GroupBoxOptions,BitBtnNetOptions);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnNetOptionsCancelClick(TObject *Sender)
{
    // Переключаемся в меню выбора настроек
    SwitchTo(GroupBoxNetOptions,GroupBoxOptions,BitBtnNetOptions);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::EditNumberComputerEnter(TObject *Sender)
{
    EditNumberComputer->Tag=StrToInt(EditNumberComputer->Text);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::EditNumberComputerExit(TObject *Sender)
{
    int a;
    try { a=StrToInt(EditNumberComputer->Text); }
    catch ( EConvertError *Error ) { EditNumberComputer->Text=IntToStr(EditNumberComputer->Tag); }
    if ( (a<1)||(a>99) ) EditNumberComputer->Text=IntToStr(EditNumberComputer->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::EditMessageKeyPress(TObject *Sender, char &Key)
{
    AnsiString message;

    if ( (Key!=0x0D)||(EditMessage->Text=="")||(Options.Mode==ModeLock) ) return;
    if ( EditMessage->Tag ) return;
    EditMessage->Tag=1;
    EditMessage->Enabled=false;
    EditNikName->Enabled=false;
    message="("+EditNikName->Text+") "+EditMessage->Text;
    SendNetMessage(NetMessageClientMessage,message);
    AddLogMessage(message);
    EditMessage->Text="";
    EditNikName->Enabled=true;
    EditMessage->Enabled=true;
    if ( EditMessage->CanFocus() ) EditMessage->SetFocus();
    EditMessage->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::AddLogMessage(AnsiString Message)
{
    AnsiString Messages;
    int a;

    Messages=LabelMessages->Caption+Message+"\n";
    if ( (a=Messages.Length())>78 ) Messages.Delete(1,a-78);
    a=0;
    for ( int i=Messages.Length(); i>=1; i-- )
    {
        if ( Messages[i]=='\n' )
        {
            if ( a>=3 ) { Messages.Delete(1,i); break; }
            else a++;
        }
    }
    LabelMessages->Caption=Messages;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::AddBossLog(int Type)
{
/*    char data[32], *curpos=data;
    unsigned short int year, month;
    int LogFile;

    Options.BossLogTimer=5*60;

    Date().DecodeDate(&year,&month,&year);
    sprintf(data,"\\TLC_%d%d%d%d.log",Options.NumberComputer/10,
        Options.NumberComputer%10,month/10,month%10);
    if ( (LogFile=FileOpen(WinDir+data,fmOpenRead|fmOpenWrite))<0 )
    {
        if ( (LogFile=FileCreate(WinDir+data))<0 ) return;
        *((unsigned int*)curpos)=(unsigned int)((double)(Date()+Time())*24*60);
        curpos+=4;
    } else
    {
//        FileSeek(LogFile,3,2);
//        FileRead(LogFile,curpos,3); curpos+=3;
//        FileSeek(LogFile,3,2);
    }

    *((unsigned int*)curpos)=(unsigned int)((double)(Date()+Time())*24*60);
    curpos+=4;
    *((unsigned char*)curpos)=(unsigned char)Type;
    curpos++;
    switch(Type)
    {
        case LogEventActive: break;
        case LogEventStart:
            *((unsigned char*)curpos)=(unsigned char)Options.Mode;
            curpos++;
            *((unsigned int*)curpos)=(unsigned int)(Options.BeginTime/4);
            curpos+=4;
            *((unsigned short int*)curpos)=(unsigned short int)Options.SizeTime;
            curpos+=2;
            break;
        case LogEventModeWork:
            *((unsigned short int*)curpos)=(unsigned short int)Options.SizeTime;
            curpos+=2;
            break;
        case LogEventModeWorkPause: break;
        case LogEventModeLock: break;
        case LogEventModeNotLock: break;
        case LogEventAddTime:
            *((unsigned short int*)curpos)=(unsigned short int)Options.SizeTime;
            curpos+=2;
            break;
        case LogEventAdminWarning:
            *((unsigned char*)curpos)=(unsigned char)Options.Mode;
            curpos++;
            break;
        case LogEventEnterToOptions:
            *((unsigned char*)curpos)=(unsigned char)Options.Mode;
            curpos++;
            break;
        default: break;
    }
    FileWrite(LogFile,data,curpos-data);

    FileClose(LogFile); */
}
//---------------------------------------------------------------------------
int __fastcall TFormMain::LoadGamesMenu()
{
    TListItem *ListItem;
    Graphics::TIcon *icon;
    HICON hicon;

    ListViewGames->Items->Clear();
    ImageListIcons->Clear();
    try { MemoBuffer->Lines->LoadFromFile(WinDir+"\\TmLockCl.gms"); }
    catch ( EFOpenError *Error ) { return -1; }
    // Формируем список игр и загружаем иконки
    ListViewGames->AllocBy=MemoBuffer->Lines->Count/4;
    ImageListIcons->AllocBy=ListViewGames->AllocBy+1;
    ImageListIcons->AddIcon(ImageGameIcon->Picture->Icon);
    if ( (icon=new Graphics::TIcon)==NULL ) return -1;
    for ( int i=0, j=MemoBuffer->Lines->Count/4; i<j; i++ )
    {
        // Формируем название игры в списке и путь для ее запуска
        ListItem=ListViewGames->Items->Add();
        ListItem->Caption=MemoBuffer->Lines->Strings[i*4+0];
        ListItem->SubItems->Add(MemoBuffer->Lines->Strings[i*4+1]);
        ListItem->SubItems->Add(MemoBuffer->Lines->Strings[i*4+3]);
        // Загружаем иконку
        hicon=::ExtractIcon(NULL,(MemoBuffer->Lines->Strings[i*4+2]).c_str(),0);
        if ( (hicon==NULL)||((int)hicon==1) )
        {
            ListItem->ImageIndex=0;
            continue;
        }
        icon->ReleaseHandle();
        icon->Handle=hicon;
        ImageListIcons->AddIcon(icon);
        ListItem->ImageIndex=ImageListIcons->Count-1;
    }
    delete icon;
    MemoBuffer->Lines->Clear();

    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnShutdownClick(TObject *Sender)
{
    Tag=1;
    Close();
    ExitWindowsEx(EWX_SHUTDOWN,0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewGamesChange(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
    if ( Change!=ctState ) return;
    Item->Selected=false;
    MemoGameDescription->Text=Item->SubItems->Strings[1];
    if ( MemoGameDescription->Text=="" ) MemoGameDescription->Text="Описание отсутствует...";
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewGamesDblClick(TObject *Sender)
{
    static STARTUPINFO startupinfo;
    static PROCESS_INFORMATION processinfo;
    AnsiString path, command;
    int pos;

    if ( (Options.Mode==ModeLock)||(Options.Mode==ModeWorkPause)||
        (TimerTimes->Tag>=0)||(ListViewGames->ItemFocused==NULL) ) return;

    // Запускаем игру
    command=ListViewGames->ItemFocused->SubItems->Strings[0];
    if ( (pos=command.AnsiPos("\""))==NULL ) return;
    path=command.SubString(pos+1,command.Length()-pos);
    if ( (pos=path.AnsiPos("\""))==NULL ) return;
    path.Delete(pos,path.Length()-pos+1);
    path=ExtractFilePath(path);
    if ( ::CreateProcess(NULL,command.c_str(),NULL,NULL,false,
        CREATE_DEFAULT_ERROR_MODE+NORMAL_PRIORITY_CLASS,NULL,
        path.c_str(),&startupinfo,&processinfo) )
    {
        // Запрещаем на несколько секунд запуск других игр
        TimerTimes->Tag=15;
        ListViewGames->Cursor=crHourGlass;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewGamesMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    if ( (Application->Active)&&(PanelGeneral->Visible)&&
        (!ListViewGames->Focused()) ) ListViewGames->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnNetOptionsClick(TObject *Sender)
{
    // Переходим в меню сетевых настроек
    EditServerAddress->Text=Options.ServerAddress;
    CheckBoxAutoServer->Checked=Options.ServerAuto;
    EditPasswordToConnect->Text=Options.PasswordToConnect;
    EditNumberComputer->Text=IntToStr(Options.NumberComputer);
    EditPasswordOptions->Text="";
    EditConfirmPasswordOptions->Text="";
    SwitchTo(GroupBoxOptions,GroupBoxNetOptions,EditServerAddress);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnOptionsOkClick(TObject *Sender)
{
    // Переключаемся в основное меню программы
    SwitchTo(GroupBoxOptions,PanelGeneral,EditMessage);
    // Устанавливаем новый режим
    if ( CheckBoxNotLock->Checked ) Options.Mode=ModeNotLock;
    else if ( Options.Mode==ModeNotLock ) Options.Mode=ModeLock;
    Options.Save();
    SetMode(Options.Mode);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnOptionsCancelClick(TObject *Sender)
{
    // Переключаемся в основное меню программы
    SwitchTo(GroupBoxOptions,PanelGeneral,EditMessage);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnPasswordChangeOkClick(TObject *Sender)
{
    // Проверяем совпадение пароля на опции и его подтверждения
    if ( EditPasswordOptions->Text!=EditConfirmPasswordOptions->Text )
    {
        EditPasswordOptions->Text="";
        EditPasswordOptions->SetFocus();
        EditConfirmPasswordOptions->Text="";
        return;
    }
    // Проверяем не пустой ли пароль
    if ( EditPasswordOptions->Text=="" )
    {
        EditPasswordOptions->SetFocus();
        return;
    }
    // Меняем пароль
    Options.OptionsPassword=EditPasswordOptions->Text;
    // Переключаемся в меню выбора настроек
    SwitchTo(GroupBoxPasswordChange,GroupBoxOptions,BitBtnPasswordChange);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnPasswordChangeCancelClick(TObject *Sender)
{
    // Переключаемся в меню выбора настроек
    SwitchTo(GroupBoxPasswordChange,GroupBoxOptions,BitBtnPasswordChange);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnPasswordChangeClick(TObject *Sender)
{
    // Переключаемся в меню смены пароля
    SwitchTo(GroupBoxOptions,GroupBoxPasswordChange,EditPasswordOptions);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonAboutOkClick(TObject *Sender)
{
    // Переключаемся в основное меню программы
    SwitchTo(PanelAbout,PanelGeneral,EditMessage);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonRebootClick(TObject *Sender)
{
    Tag=1;
    Close();
    ExitWindowsEx(EWX_REBOOT,0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonOptionsClick(TObject *Sender)
{
    EditEnterPassword->Text="";
    if ( !EditEnterPassword->Tag ) EditEnterPassword->Tag=3;
    LabelEnterPasswordRetry->Caption=IntToStr(EditEnterPassword->Tag);
    SwitchTo(PanelGeneral,PanelEnterPassword,EditEnterPassword);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonAboutClick(TObject *Sender)
{
    SwitchTo(PanelGeneral,PanelAbout,MemoInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonPasswordOkClick(TObject *Sender)
{
    // Провераем правильность пароля
    if ( EditEnterPassword->Text!=Options.OptionsPassword )
    {
        // Проверяем количество попыток ввода пароля
        if ( EditEnterPassword->Tag<=1 )
        {
            EditEnterPassword->Tag=3;
            SwitchTo(PanelEnterPassword,PanelGeneral,EditMessage);
            AddBossLog(LogEventAdminWarning);
            SendNetMessage(NetMessageAdminWarning,"");
            Beep();
            return;
        } else EditEnterPassword->Tag--;
        EditEnterPassword->Text="";
        EditEnterPassword->SetFocus();
        LabelEnterPasswordRetry->Caption=IntToStr(EditEnterPassword->Tag);
        return;
    }
    // Переходим в меню выбора настроек
    AddBossLog(LogEventEnterToOptions);
    EditServerAddress->Text=Options.ServerAddress;
    EditPasswordToConnect->Text=Options.PasswordToConnect;
    EditNumberComputer->Text=IntToStr(Options.NumberComputer);
    EditPasswordOptions->Text="";
    EditConfirmPasswordOptions->Text="";
    if ( Options.Mode==ModeNotLock ) CheckBoxNotLock->Checked=true;
    else CheckBoxNotLock->Checked=false;
    SwitchTo(PanelEnterPassword,GroupBoxOptions,EditServerAddress);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonPasswordCancelClick(TObject *Sender)
{
    SwitchTo(PanelEnterPassword,PanelGeneral,EditMessage);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::EditEnterPasswordKeyPress(TObject *Sender,
      char &Key)
{
    if ( Key==0x0D ) SpeedButtonPasswordOkClick(NULL);
    else if ( Key==0x1B ) SpeedButtonPasswordCancelClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::CheckBoxAutoServerClick(TObject *Sender)
{
    if ( CheckBoxAutoServer->Checked )
    {
        Label5->Enabled=false;
        EditServerAddress->Enabled=false;
        Label6->Enabled=false;
        EditNumberComputer->Enabled=false;
    } else
    {
        Label5->Enabled=true;
        EditServerAddress->Enabled=true;
        Label6->Enabled=true;
        EditNumberComputer->Enabled=true;
    }
}
//---------------------------------------------------------------------------

