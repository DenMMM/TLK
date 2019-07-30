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
//---------------------------------------------------------------------------
#include "UnitClOptions.h"
#include "UnitStates.h"
#include "UnitSync.h"
#include "UnitSend.h"
#include "UnitAuth.h"
#include "UnitMessage.h"
#include "UnitLockDsk.h"
#include <Menus.hpp>
//---------------------------------------------------------------------------
// Сообщения (картинки)
#define mimNone             0       // Скрыть картинку с сообщением
#define mimEndTime          1       // Время закончилось
#define mimLocked           2       // Компьютер закрыт
#define mimPaused           3       // Компьютер прикрыт адмиинстратором или аварийно
#define mimFine             4       // Применен штраф
#define mimTimePaused       5       // Время приостановлено
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
    int TimeMsgEndTime;
    int TimeToReboot;
    bool MessageAreShowed;
    bool MyClose;
    MLockDsk LockDesk;
    void SetState(MStateInfo *Info_);
    void SetTransp(bool Transp_);
    void ShowGames(unsigned Pages_);
    void ShowImageMessage(int Message_);
public:		// User declarations
    bool ConfigMode;
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
extern char win_dir[];
extern Mptr <MClOptions> Options;
extern Mptr <MStateCl> State;
extern Mptr <MSyncCl> Sync;
extern Mptr <MSendCl> Send;
extern Mptr <MAuth> Auth;
extern Mptr <MMessage> Message;
//---------------------------------------------------------------------------
#endif

