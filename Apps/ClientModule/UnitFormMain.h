//---------------------------------------------------------------------------
#ifndef UnitFormMainH
#define UnitFormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ImgList.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
#include "UnitMessage.h"
#include "UnitLockDsk.h"
#include "UnitShared.h"
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
    TPanel *PanelMenu;
    TImage *ImageMenu;
    TSpeedButton *SpeedButtonReboot;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *LabelWorkTime;
    TLabel *LabelSysTime;
    TTimer *Timer;
    TLabel *LabelCompNum;
    TSpeedButton *SpeedButtonOptions;
    TLabel *LabelCompNumShad;
    TPopupMenu *PopupMenuConfig;
    TMenuItem *NLogOff;
    TMenuItem *NShutdown;
    TMenuItem *N2;
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall SpeedButtonRebootClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall SpeedButtonOptionsClick(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall NLogOffClick(TObject *Sender);
    void __fastcall NShutdownClick(TObject *Sender);
private:	// User declarations
    MMessage WarnMessage;
    MLockDsk LockDsk;
    bool Transp;
    unsigned CompNumVer;
    unsigned SysTimeVer;
    unsigned WorkTimeVer;
    unsigned WarnMsgVer;
    unsigned ImageMsgVer;
    unsigned GamesVer;
    unsigned TranspVer;
    bool ConfigMode;
    unsigned ConfigModeVer;
    UnicodeString ExePath;

    void SetTransp(bool Transp_);
    void SharedProcess();
    void __fastcall MQueryEndSession(TMessage &Msg);
    BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER(WM_QUERYENDSESSION,TMessage,MQueryEndSession);
    END_MESSAGE_MAP(TForm);
public:		// User declarations
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
extern MShared Shared;
//---------------------------------------------------------------------------
#endif

