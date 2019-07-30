//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "UnitFormMain.h"
#include "UnitFormGames.h"
#include "UnitTariffs.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
MShared Shared;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TForm(Owner)
{
    Transp=false;
    CompNumVer=0;
    SysTimeVer=0;
    WorkTimeVer=0;
    WarnMsgVer=0;
    ImageMsgVer=0;
    GamesVer=0;
    TranspVer=0;
    ConfigMode=false;
    ConfigModeVer=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
    Tag=false;
    PanelMenu->DoubleBuffered=true;

    // Настраиваем пути для файлов
    ExePath=ExtractFilePath(Application->ExeName);
    UnicodeString MsgFile=ExePath+L"\\MSGTIMEWARN.BMP";
    WarnMessage.File=MsgFile.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    // Скроемся из панели задач
    ::ShowWindow(Application->Handle,SW_HIDE);

    Timer->Interval=250;
    TimerTimer(nullptr);
    Timer->Enabled=true;
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
    WarnMessage.Stop();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MQueryEndSession(TMessage &Msg)
{
    Msg.Result=TRUE;
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
    // Завершим работу, если управляющая служба "упала"
    if ( !Shared.IsLive() )
    {
        FormMain->Tag=true;
        FormGames->Tag=true;
        Close();
        return;
    }

    // Попытаемся занять shared-секцию
    if ( Shared.Lock() )
    {
        try
        {
            // Отработаем команды службы
            SharedProcess();
            Shared.UnLock();
        }
        catch(Exception &exception)
        {
            // Защитимся от блокировки shared-секции на случай исключения
            Shared.UnLock();
            Application->ShowException(&exception);
        }
    }

    // Позиционируем окно программы
    if ( Transp )
    {
        if ( Left!=(Screen->Width-Width) ) Left=Screen->Width-Width;
        if ( Top!=0 ) Top=0;
        OnResize(nullptr);
    } else
    {
        if ( Width!=Screen->Width ) { Left=0; Width=Screen->Width; }
        if ( Height!=Screen->Height ) { Top=0; Height=Screen->Height; }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonRebootClick(TObject *Sender)
{
#ifdef _DEBUG
    FormMain->Tag=true;
    FormGames->Tag=true;
    Close();
#else
    WinExit(EWX_REBOOT|EWX_FORCE);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonOptionsClick(TObject *Sender)
{
    TPoint coord;

    coord.x=PanelMenu->Left+SpeedButtonOptions->Left+4;
    coord.y=PanelMenu->Top+SpeedButtonOptions->Top+SpeedButtonOptions->Height-4;
    coord=ClientToScreen(coord);
    PopupMenuConfig->Popup(coord.x,coord.y);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogOffClick(TObject *Sender)
{
    // Доп. проверка для большей безопасности
    if ( !ConfigMode ) return;

#ifdef _DEBUG
    FormMain->Tag=true;
    FormGames->Tag=true;
    Close();
#else
    WinExit(EWX_LOGOFF|EWX_FORCE);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NShutdownClick(TObject *Sender)
{
    // Доп. проверка для большей безопасности
    if ( !ConfigMode ) return;

#ifdef _DEBUG
    FormMain->Tag=true;
    FormGames->Tag=true;
    Close();
#else
    WinExit(EWX_POWEROFF|EWX_FORCE);
#endif
}
//---------------------------------------------------------------------------
void TFormMain::SetTransp(bool Transp_)
{
    Transp=Transp_;
    FormMain->Color=clBlack;
    FormMain->PanelMenu->Color=clBlack;
    FormMain->AutoSize=Transp_;
    if ( Transp_ )      /// прозрачность глючит
    {
		FormGames->Parent=nullptr;
//        FormGames->TransparentColor=true;
//        FormGames->TransparentColorValue=clFuchsia;
//        FormGames->Color=clFuchsia;
//        FormGames->AlphaBlend=true;
//        FormGames->AlphaBlendValue=220;
        FormGames->TreeViewGames->Color=(TColor)0x00000000;
    } else
    {
        FormGames->Parent=FormMain;
//        FormGames->TransparentColor=false;
//        FormGames->Color=clBlack;
//        FormGames->AlphaBlend=false;
        FormGames->TreeViewGames->Color=(TColor)0x0248422C;
    }
}
//---------------------------------------------------------------------------
void TFormMain::SharedProcess()
{
    // Обновляем номер компьютера
    int CompNum;
    if ( Shared.CheckCompNum(&CompNum,&CompNumVer) )
    {
        LabelCompNum->Caption=IntToStr(CompNum);
        LabelCompNumShad->Caption=LabelCompNum->Caption;
        LockDsk.UpdateCompNum(CompNum);
    }
    // Обновляем текущее время
    __int64 SysTime;
    if ( Shared.CheckSysTime(&SysTime,&SysTimeVer) )
    {
        wchar_t line[]=L"--:--";
        SYSTEMTIME st;

        if ( Int64ToSystemTime(&SysTime,&st) )
        {
			swprintf(
				line, sizeof(line),
				L"%.2i:%.2i",
				st.wHour, st.wMinute);
            LabelSysTime->Caption=line;
        }
        LockDsk.UpdateSysTime(SysTime);
    }
    // Обновляем оставшееся время работы
    int WorkTime;
    if ( Shared.CheckWorkTime(&WorkTime,&WorkTimeVer) )
    {
		wchar_t line[]=L"--:--";

		if ( WorkTime!=0 ) swprintf(
			line, sizeof(line),
			L"%.2i:%.2i",
			WorkTime/60, WorkTime%60);
		LabelWorkTime->Caption=line;
		LockDsk.UpdateWorkTime(WorkTime);
	}
	// Запускаем показ предупреждения об окончании времени
	bool WarnMsg;
	if ( Shared.CheckWarnMsg(&WarnMsg,&WarnMsgVer) )
	{
        if ( WarnMsg ) WarnMessage.Show();
        else WarnMessage.Stop();
    }
    // Обновляем экран блокировки
    int ImageMsg;
    if ( Shared.CheckImageMessage(&ImageMsg,&ImageMsgVer) )
    {
        if ( ImageMsg==mimNone ) LockDsk.Hide();
        else
        {
			UnicodeString FileName=ExePath;
            switch(ImageMsg)
            {
				case mimEndTime: FileName+=L"\\MSGTIMEOUT.BMP"; break;
				case mimLocked: FileName+=L"\\MSGLOCK.BMP"; break;
				case mimPaused: FileName+=L"\\MSGADMNLOCK.BMP"; break;
				case mimFine: FileName+=L"\\MSGFINE.BMP"; break;
				case mimTimePaused: FileName+=L"\\MSGPAUSE.BMP"; break;
                default: FileName=L""; break;
            }
            if ( FileName!=L"" ) LockDsk.Show(FileName.c_str());
        }
    }
    // Обновляем список программ
    unsigned Games;
    if ( Shared.CheckGames(&Games,&GamesVer) ) FormGames->ShowGames(Games);
    // Обновляем режим прозрачности
    bool Transp;
    if ( Shared.CheckTransp(&Transp,&TranspVer) )
    {
        SetTransp(Transp);
        LockDsk.SetTransp(Transp);
    }
    // Обновляем режим настройки
    if ( Shared.CheckConfigMode(&ConfigMode,&ConfigModeVer) )
        SpeedButtonOptions->Enabled=ConfigMode;
}
//---------------------------------------------------------------------------

