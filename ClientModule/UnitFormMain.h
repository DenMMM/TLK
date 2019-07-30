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
#include "UnitGames.h"
#include "UnitState.h"
#include "UnitThreadNetSync.h"
//---------------------------------------------------------------------------
#define Max_EnterPasswordTime ((int)30)
#define WaitRebootTime ((int)20)
#define MessageTime ((int)10)
//---------------------------------------------------------------------------
#define mimNone ((int)0)
#define mimEndTime ((int)1)
#define mimLocked ((int)2)
#define mimPaused ((int)3)
#define mimFine ((int)4)
#define mimTimePaused ((int)5)
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
    int MessageTimer;
    void __fastcall AddGamesToTree(MGames *Games_, TTreeNode *TreeNode_, TImageList *ImageList_);
    void __fastcall ShowGames(unsigned int Pages_);
    void __fastcall ShowImageMessage(int Message_);
    void __fastcall HideWindows(bool Hide_);
    void __fastcall DrawMessage(HANDLE message_bitmap, BITMAP &bitmap);
public:		// User declarations
    void __fastcall SetState();
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
extern MState *State;
extern MThreadNetSync *ThreadNetSync;
//---------------------------------------------------------------------------
#endif

