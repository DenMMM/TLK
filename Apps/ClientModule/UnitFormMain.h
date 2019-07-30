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
#include "UnitGames.h"
#include "UnitMessage.h"
//---------------------------------------------------------------------------
#define Max_EnterPasswordTime ((int)30)
//---------------------------------------------------------------------------
#define mimNone ((int)0)        // Скрыть картинку с сообщением
#define mimEndTime ((int)1)     // Время закончилось
#define mimLocked ((int)2)      // Компьютер закрыт
#define mimPaused ((int)3)      // Компьютер прикрыт адмиинстратором или аварийно
#define mimFine ((int)4)        // Применен штраф
#define mimTimePaused ((int)5)  // Время приостановлено
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
    TPanel *PanelMenu;
    TImage *ImageFace;
    TImage *ImageLogo;
    TSpeedButton *SpeedButtonReboot;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *LabelWorkTime;
    TLabel *LabelSysTime;
    TTimer *Timer;
    TImageList *ImageListGamesIcons;
    TPanel *PanelGames;
    TImage *Image1;
    TImage *Image2;
    TImage *Image3;
    TImage *Image4;
    TImage *Image5;
    TImage *Image6;
    TImage *Image7;
    TImage *Image8;
    TTreeView *TreeViewGames;
    TImage *ImageMessage;
    TLabel *LabelComputerNumber;
    TSpeedButton *SpeedButtonOptions;
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall TreeViewGamesDeletion(TObject *Sender,
          TTreeNode *Node);
    void __fastcall SpeedButtonRebootClick(TObject *Sender);
    void __fastcall TreeViewGamesDblClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall SpeedButtonOptionsClick(TObject *Sender);
private:	// User declarations
    int TimeToReboot;
    bool MessageAreShowed;
    void __fastcall SetState(MStateInfo *Info_);
    void __fastcall ShowGames(unsigned int Pages_);
    void __fastcall ShowImageMessage(int Message_);
    void __fastcall HideWindows(bool Hide_);
    void __fastcall AddGamesToTree(MGames *Games_, TTreeNode *TreeNode_, TImageList *ImageList_);
public:		// User declarations
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
extern char win_dir[];
extern MClOptions *Options;
extern MStateCl *State;
extern MSyncCl *Sync;
extern MSendCl *Send;
extern MMessage *Message;
//---------------------------------------------------------------------------
#endif

