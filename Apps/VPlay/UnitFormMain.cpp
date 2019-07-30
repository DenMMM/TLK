//---------------------------------------------------------------------------
#include <vcl.h>
#include <mem.h>
#include <stdio.h>
#pragma hdrstop

#include "UnitFormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TForm(Owner)
{
    DeviceID=0;
    PosChanging=false;
    InActiveTimer=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    // Извлекаем из командной строки имя файла для воспроизведения
    FileName=::GetCommandLine();
    FileName.Trim();
    FileName.Delete(1,Application->ExeName.Length()+2);
    FileName=FileName.Trim();

    // Меняем заголовок окна программы
    Caption=L"VPlay - "+ExtractFileName(FileName);

    // Отправляем сообщение для открытия устройства
    ::PostMessage(Handle,MOPEN,0,0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    // Закрываем MCI-устройство
    mcierror=::mciSendCommand(DeviceID, MCI_CLOSE,
        0, (DWORD)(LPMCI_GENERIC_PARMS)nullptr);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormResize(TObject *Sender)
{
    if ( !DeviceID ) return;

    // Определяем изначальный размер картинки
    MCI_DGV_RECT_PARMS dgv_rect_parms;
    setmem(&dgv_rect_parms,sizeof(dgv_rect_parms),0);
    mcierror=::mciSendCommand(DeviceID, MCI_WHERE,
        MCI_DGV_WHERE_SOURCE, (DWORD)(LPMCI_GENERIC_PARMS)&dgv_rect_parms);
    int MainWidth=ClientWidth,
        MainHeight=ClientHeight-PanelControl->Height;
    // Рассчитываем новый масштаб для вывода на экран
    float Scale, XScale, YScale;
    XScale=(float)MainWidth/dgv_rect_parms.rc.right;
    YScale=(float)MainHeight/dgv_rect_parms.rc.bottom;
    Scale= XScale<YScale? XScale: YScale;
    // Рассчитываем новые размеры для выводимой картинки
    int NewWidth=dgv_rect_parms.rc.right*Scale,
        NewHeight=dgv_rect_parms.rc.bottom*Scale;

    // Задаем новый размер и положение для кадра в окне
    MCI_DGV_PUT_PARMS dgv_put_parms;
    setmem(&dgv_put_parms,sizeof(dgv_put_parms),0);
    dgv_put_parms.rc.left=(MainWidth-NewWidth)/2;
    dgv_put_parms.rc.top=(MainHeight-NewHeight)/2;
    dgv_put_parms.rc.right=NewWidth;
    dgv_put_parms.rc.bottom=NewHeight;
    mcierror=::mciSendCommand(DeviceID, MCI_PUT,
        MCI_DGV_PUT_DESTINATION|MCI_DGV_RECT, (DWORD)(LPMCI_GENERIC_PARMS)&dgv_put_parms);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
    if ( !DeviceID ) return;

    // Запрашиваем текущее состояние устройства
    MCI_STATUS_PARMS status_parms;
    setmem(&status_parms,sizeof(status_parms),0);
    status_parms.dwItem=MCI_STATUS_MODE;
    ::mciSendCommand(DeviceID, MCI_STATUS,
        MCI_STATUS_ITEM, (DWORD)(LPMCI_STATUS_PARMS)&status_parms);
    switch(status_parms.dwReturn)
    {
		case MCI_MODE_NOT_READY: LabelState->Caption=L"Устройство не готово"; break;
		case MCI_MODE_PAUSE: LabelState->Caption=L"Пауза"; break;		case MCI_MODE_PLAY: LabelState->Caption=L"Воспроизведение"; break;		case MCI_MODE_STOP: LabelState->Caption=L"Остановлено"; break;		case MCI_MODE_OPEN: LabelState->Caption=L"Открыто"; break;        case MCI_MODE_SEEK: LabelState->Caption=L"Перемотка"; break;        default: break;
    }

    // Запрашиваем текущую позицию
    setmem(&status_parms,sizeof(status_parms),0);
    status_parms.dwItem=MCI_STATUS_POSITION;
    ::mciSendCommand(DeviceID, MCI_STATUS,
        MCI_STATUS_ITEM, (DWORD)(LPMCI_STATUS_PARMS)&status_parms);

    //
    char time[8+1];
    GetTimeString(status_parms.dwReturn,time);
    LabelTime->Caption=time;

    // Если возможно, выставляем новое положение индикатора перемотки
    if ( (!PosChanging)&&(PanelPos->Tag!=0) )
    {
        int MinX=BevelPos->Left-(PanelPos->Height-BevelPos->Height)/2;
        int MaxX=BevelPos->Left+BevelPos->Width-(PanelPos->Height+BevelPos->Height)/2;
        PanelPos->Left=MinX+(float)status_parms.dwReturn/PanelPos->Tag*(MaxX-MinX);
    }

    if ( Application->Active )
    {
        InActiveTimer=0;
    } else if ( InActiveTimer>=(15*1000/Timer->Interval) )
    {
        if ( InActiveTimer!=0xFF )
        {
            InActiveTimer=0xFF;
            SpeedButtonPause->Down=true; SpeedButtonPauseClick(nullptr);
        }
    } else InActiveTimer++;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonPlayClick(TObject *Sender)
{
    ::PostMessage(Handle,MPLAY,0,0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonPauseClick(TObject *Sender)
{
    ::PostMessage(Handle,MPAUSE,0,0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonStopClick(TObject *Sender)
{
    ::PostMessage(Handle,MSTOP,0,0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PanelPosMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    PosChanging=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PanelPosMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    static int OldX=0, OldY=0;

    if ( !PosChanging ) { OldX=X; OldY=Y; return; }

    int NewX=PanelPos->Left+X-OldX;
    int MinX=BevelPos->Left-(PanelPos->Height-BevelPos->Height)/2;
    int MaxX=BevelPos->Left+BevelPos->Width-(PanelPos->Height+BevelPos->Height)/2;
    if ( NewX<MinX ) NewX=MinX;
    if ( NewX>MaxX ) NewX=MaxX;
    if ( PanelPos->Left!=NewX ) PanelPos->Left=NewX;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PanelPosMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    int MinX=BevelPos->Left-(PanelPos->Height-BevelPos->Height)/2;
    int MaxX=BevelPos->Left+BevelPos->Width-(PanelPos->Height+BevelPos->Height)/2;
    int Pos=(float)(PanelPos->Left-MinX)/(MaxX-MinX)*PanelPos->Tag;
    ::PostMessage(Handle,MSEEK,Pos,0);

    PosChanging=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TrackBarVolumeChange(TObject *Sender)
{
    ::PostMessage(Handle,MVOLUME,TrackBarVolume->Position,0);
}
//---------------------------------------------------------------------------
void TFormMain::GetTimeString(int Time_, char *Line_)
{
    LARGE_INTEGER large_integer;
    large_integer.QuadPart=Time_*10000i64;
    FILETIME filetime;
    memcpy(&filetime,&large_integer,sizeof(filetime));
    SYSTEMTIME systemtime;
    setmem(&systemtime,sizeof(systemtime),0);
    ::FileTimeToSystemTime(&filetime,&systemtime);
    sprintf(Line_,"%.2i:%.2i:%.2i",systemtime.wHour,systemtime.wMinute,systemtime.wSecond);
}//---------------------------------------------------------------------------
void __fastcall TFormMain::MOpen(TMessage &Msg)
{
    // Помечаем текущую операцию
    LabelState->Caption=L"Загрузка данных..."; Application->ProcessMessages();

    // Открываем MCI-устройство
    MCI_OPEN_PARMS open_params;
    setmem(&open_params,sizeof(open_params),0);
    open_params.dwCallback=(DWORD)Handle;
    open_params.lpstrDeviceType=L"MPEGVideo";
	open_params.lpstrElementName=FileName.c_bstr();
    mcierror=::mciSendCommand(0, MCI_OPEN,
        MCI_OPEN_ELEMENT|MCI_OPEN_TYPE|MCI_DGV_OPEN_32BIT,
        (DWORD)(LPMCI_OPEN_PARMS)&open_params);
    if ( mcierror ) { LabelState->Caption=L"Не удалось открыть файл !"; return; }
    DeviceID=open_params.wDeviceID;

    // Определяем изначальный размер картинки
    MCI_DGV_RECT_PARMS dgv_rect_parms;
    setmem(&dgv_rect_parms,sizeof(dgv_rect_parms),0);
    mcierror=::mciSendCommand(DeviceID, MCI_WHERE,
        MCI_DGV_WHERE_SOURCE, (DWORD)(LPMCI_GENERIC_PARMS)&dgv_rect_parms);

    if ( !mcierror )
    {
        // Задаем размеры окна
        ClientWidth=dgv_rect_parms.rc.right;
        ClientHeight=dgv_rect_parms.rc.bottom+PanelControl->Height;
    }

    // Задаем в какое окно выводить изображение
    MCI_DGV_WINDOW_PARMS dgv_window_parms;
    setmem(&dgv_window_parms,sizeof(dgv_window_parms),0);
    dgv_window_parms.hWnd=Handle;
    mcierror=::mciSendCommand(DeviceID, MCI_WINDOW,
        MCI_DGV_WINDOW_HWND, (DWORD)(LPMCI_GENERIC_PARMS)&dgv_window_parms);

    Application->ProcessMessages();

    // Включаем звук
    mcierror=::mciSendCommand(DeviceID, MCI_SETAUDIO,
        MCI_SET_ON, (DWORD)(LPMCI_GENERIC_PARMS)nullptr);
    if ( mcierror ) return;

    // Устанавливаем громкость
    TrackBarVolume->Min=0; TrackBarVolume->Max=100;
    TrackBarVolume->PageSize=20; TrackBarVolume->Position=40;

    // Устанавливаем формат времени в миллисекундах
    MCI_SET_PARMS set_parms;
    setmem(&set_parms,sizeof(set_parms),0);
    set_parms.dwTimeFormat=MCI_FORMAT_MILLISECONDS;
    ::mciSendCommand(DeviceID, MCI_SET,
        MCI_SET_TIME_FORMAT, (DWORD)(LPMCI_SET_PARMS)&set_parms);    // Определяем длительность файла для перемотки    MCI_STATUS_PARMS status_parms;    setmem(&status_parms,sizeof(status_parms),0);    status_parms.dwItem=MCI_STATUS_LENGTH;
    ::mciSendCommand(DeviceID, MCI_STATUS,
        MCI_STATUS_ITEM, (DWORD)(LPMCI_STATUS_PARMS)&status_parms);    //    PanelPos->Tag=status_parms.dwReturn;    char length[8+1];    GetTimeString(status_parms.dwReturn,length);    LabelLength->Caption=length;    // Отправляем сообщение для запуска воспроизведения
    ::PostMessage(Handle,MPLAY,0,0);
}//---------------------------------------------------------------------------
void __fastcall TFormMain::MPlay(TMessage &Msg)
{
    if ( !DeviceID ) return;

    // Запускаем воспроизведение
    MCI_PLAY_PARMS play_params;
    setmem(&play_params,sizeof(play_params),0);
    play_params.dwCallback=(DWORD)Handle;
    play_params.dwFrom=MCI_FROM;
    play_params.dwTo=MCI_TO;

    mcierror=::mciSendCommand(DeviceID, MCI_PLAY,
        0, (DWORD)(LPMCI_PLAY_PARMS)&play_params);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MPause(TMessage &Msg)
{
    if ( !DeviceID ) return;

    MCI_GENERIC_PARMS generic_parms;
    setmem(&generic_parms,sizeof(generic_parms),0);
    generic_parms.dwCallback=(DWORD)Handle;

    mcierror=::mciSendCommand(DeviceID, MCI_PAUSE,
        0, (DWORD)(LPMCI_GENERIC_PARMS)&generic_parms);}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MStop(TMessage &Msg)
{
    if ( !DeviceID ) return;

    MCI_GENERIC_PARMS generic_parms;
    setmem(&generic_parms,sizeof(generic_parms),0);
    generic_parms.dwCallback=(DWORD)Handle;

    ::mciSendCommand(DeviceID, MCI_STOP,
        MCI_NOTIFY, (DWORD)(LPMCI_GENERIC_PARMS)&generic_parms);    if ( mcierror ) return;

    mcierror=::mciSendCommand(DeviceID, MCI_SEEK,
        MCI_SEEK_TO_START, (DWORD)(LPMCI_SEEK_PARMS)nullptr);}//---------------------------------------------------------------------------
void __fastcall TFormMain::MSeek(TMessage &Msg)
{
    if ( !DeviceID ) return;

    LabelState->Caption=L"Перемотка..."; Application->ProcessMessages();

    MCI_SEEK_PARMS seek_parms;
    setmem(&seek_parms,sizeof(seek_parms),0);
    seek_parms.dwCallback=(DWORD)Handle;
    seek_parms.dwTo=Msg.WParam;
    mcierror=::mciSendCommand(DeviceID, MCI_SEEK,
        MCI_TO, (DWORD)(LPMCI_SEEK_PARMS)&seek_parms);    // Если до перемотки было включено воспроизведение, то возобновляем его    if ( SpeedButtonPlay->Down ) ::PostMessage(Handle,MPLAY,0,0);
}//---------------------------------------------------------------------------
void __fastcall TFormMain::MVolume(TMessage &Msg)
{
    if ( !DeviceID ) return;

    // Устанавливаем громкость звука
    MCI_DGV_SETAUDIO_PARMS dgv_setaudio_parms;
    setmem(&dgv_setaudio_parms,sizeof(dgv_setaudio_parms),0);
    dgv_setaudio_parms.dwItem=MCI_DGV_SETAUDIO_VOLUME;
    dgv_setaudio_parms.dwValue=(float)Msg.WParam/100*0x3E8;
    mcierror=::mciSendCommand(DeviceID, MCI_SETAUDIO,
        MCI_DGV_SETAUDIO_ITEM|MCI_DGV_SETAUDIO_VALUE, (DWORD)(LPMCI_GENERIC_PARMS)&dgv_setaudio_parms);
}
//---------------------------------------------------------------------------

