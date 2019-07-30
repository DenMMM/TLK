//---------------------------------------------------------------------------
#ifndef UnitFormMainH
#define UnitFormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
#include <mmsystem.h>
#include <digitalv.h>
#include <AppEvnts.hpp>
//---------------------------------------------------------------------------
#define MOPEN (WM_USER+1)
#define MPLAY (WM_USER+2)
#define MPAUSE (WM_USER+3)
#define MSTOP (WM_USER+4)
#define MSEEK (WM_USER+5)
#define MVOLUME (WM_USER+6)
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
    TTimer *Timer;
    TPanel *PanelControl;
    TSpeedButton *SpeedButtonPlay;
    TSpeedButton *SpeedButtonPause;
    TSpeedButton *SpeedButtonStop;
    TBevel *BevelPos;
    TPanel *PanelPos;
    TTrackBar *TrackBarVolume;
    TBevel *BevelState;
    TLabel *LabelState;
    TLabel *LabelTime;
    TLabel *LabelSeparator;
    TLabel *LabelLength;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall SpeedButtonPlayClick(TObject *Sender);
    void __fastcall SpeedButtonPauseClick(TObject *Sender);
    void __fastcall SpeedButtonStopClick(TObject *Sender);
    void __fastcall PanelPosMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall PanelPosMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall PanelPosMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall TrackBarVolumeChange(TObject *Sender);
private:	// User declarations
    MCIERROR mcierror;
    MCIDEVICEID DeviceID;
    AnsiString FileName;
    bool PosChanging;
    unsigned int InActiveTimer;
    void GetTimeString(int Time_, char *Line_);
    void __fastcall MOpen(TMessage &Msg);
    void __fastcall MPlay(TMessage &Msg);
    void __fastcall MPause(TMessage &Msg);
    void __fastcall MStop(TMessage &Msg);
    void __fastcall MSeek(TMessage &Msg);
    void __fastcall MVolume(TMessage &Msg);
    BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER(MOPEN,TMessage,MOpen);
    MESSAGE_HANDLER(MPLAY,TMessage,MPlay);
    MESSAGE_HANDLER(MPAUSE,TMessage,MPause);
    MESSAGE_HANDLER(MSTOP,TMessage,MStop);
    MESSAGE_HANDLER(MSEEK,TMessage,MSeek);
    MESSAGE_HANDLER(MVOLUME,TMessage,MVolume);
    END_MESSAGE_MAP(TForm);
public:		// User declarations
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif

