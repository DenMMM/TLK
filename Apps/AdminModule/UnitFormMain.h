//---------------------------------------------------------------------------
#ifndef UnitFormMainH
#define UnitFormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Graphics.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
#include <memory>
#include "UnitComputers.h"
#include "UnitTariffs.h"
#include "UnitFines.h"
#include "UnitUsers.h"
#include "UnitOptions.h"
#include "UnitStates.h"
#include "UnitSync.h"
#include "UnitLog.h"
//---------------------------------------------------------------------------
// Какие компьютеры отображать в основном окне
#define mcfAll                  1       // Все компьютеры
#define mcfFree                 2       // Свободные и скоро освобождающиеся
#define mcfService              3       // Открытые для обслуживания
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
    TStatusBar *StatusBar;
    TTimer *Timer;
    TMainMenu *MainMenu;
    TMenuItem *NFile;
    TMenuItem *NOptions;
    TMenuItem *NHelp;
    TMenuItem *N6;
    TMenuItem *N7;
    TMenuItem *NAbout;
    TMenuItem *NComputers;
    TMenuItem *NTariffs;
    TMenuItem *NFines;
    TMenuItem *NClosePrg;
    TMenuItem *NAdmin;
    TMenuItem *NLogIn;
    TMenuItem *N19;
    TPanel *PanelButtons;
    TBevel *Bevel3;
    TBevel *Bevel5;
    TBevel *Bevel6;
    TBevel *Bevel2;
    TBitBtn *BitBtnRun;
    TBitBtn *BitBtnAdd;
    TBitBtn *BitBtnFine;
    TBitBtn *BitBtnAdditionals;
    TBevel *Bevel1;
    TProgressBar *ProgressBarNetProcess;
    TBevel *Bevel4;
    TBitBtn *BitBtnLock;
    TBitBtn *BitBtnUnLock;
    TPopupMenu *PopupMenuViewFilter;
    TMenuItem *NFilterAll;
    TMenuItem *NFilterFree;
    TBitBtn *BitBtnViewFilter;
    TImageList *ImageListIcons;
    TPopupMenu *PopupMenuAdditionals;
    TMenuItem *NReboot;
    TMenuItem *NShutdown;
    TMenuItem *N27;
    TBitBtn *BitBtnDescription;
    TBevel *Bevel9;
    TBevel *Bevel10;
    TMenuItem *NPause;
    TMenuItem *NRun;
    TMenuItem *NUsers;
    TMenuItem *NLogOut;
    TMenuItem *NPasswords;
    TMenuItem *NOptionsPassword;
    TLabel *LabelTimeHoursMins;
    TLabel *LabelTimeSecs;
    TBitBtn *BitBtnAlarm;
    TBevel *Bevel11;
    TBevel *Bevel12;
    TBitBtn *BitBtnExchange;
    TBevel *Bevel13;
    TBevel *Bevel8;
    TBitBtn *BitBtnClose;
    TPopupMenu *PopupMenuSystem;
    TMenuItem *NOpen;
    TMenuItem *NClose;
    TBitBtn *BitBtnSystem;
    TBevel *Bevel7;
    TMenuItem *NFilterService;
    TMenuItem *N2;
    TMenuItem *NPowerOn;
    TMenuItem *NUsersPasswords;
    TMenuItem *N1;
    TMenuItem *NConfigOpen;
    TMenuItem *N5;
    TMenuItem *NCommon;
    TMenuItem *N4;
    TMenuItem *NLogReset;
    TMenuItem *N3;
    TMenuItem *NClient;
    TMenuItem *NAuth;
    TMenuItem *N9;
    TMenuItem *N8;
    TListView *ListViewComputers;
    TTimer *TimerNet;
    TPopupMenu *PopupMenuCmp;
    TMenuItem *NCmpRun;
    TMenuItem *NCmpAdd;
    TMenuItem *NCmpFine;
    TMenuItem *N13;
    TMenuItem *NCmpExchange;
    TMenuItem *N15;
    TMenuItem *N16;
    TMenuItem *NCmpLock;
    TMenuItem *NCmpUnLock;
    TMenuItem *N10;
    TMenuItem *NCmpFilterAll;
    TMenuItem *NCmpFilterFree;
    TMenuItem *NCmpFilterService;
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall BitBtnRunClick(TObject *Sender);
    void __fastcall NClosePrgClick(TObject *Sender);
    void __fastcall NLogInClick(TObject *Sender);
    void __fastcall NFilterAllClick(TObject *Sender);
    void __fastcall BitBtnAdditionalsClick(TObject *Sender);
    void __fastcall NComputersClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall BitBtnLockClick(TObject *Sender);
    void __fastcall NOpenClick(TObject *Sender);
    void __fastcall NPauseClick(TObject *Sender);
    void __fastcall ListViewComputersInsert(TObject *Sender,
          TListItem *Item);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall NLogOutClick(TObject *Sender);
    void __fastcall NRebootClick(TObject *Sender);
    void __fastcall NShutdownClick(TObject *Sender);
    void __fastcall BitBtnExchangeClick(TObject *Sender);
    void __fastcall ListViewComputersCompare(TObject *Sender,
          TListItem *Item1, TListItem *Item2, int Data, int &Compare);
    void __fastcall NPowerOnClick(TObject *Sender);
    void __fastcall NUsersPasswordsClick(TObject *Sender);
    void __fastcall NConfigOpenClick(TObject *Sender);
    void __fastcall NLogResetClick(TObject *Sender);
    void __fastcall NOptionsPasswordClick(TObject *Sender);
    void __fastcall TimerNetTimer(TObject *Sender);
private:	// User declarations
    bool EnableConfig;          // Настройка модуля
//    bool EnableLogReSet;        // Ручное заведение нового файла лога
//    bool EnableCompCmd;         // Команды для компьютеров
//    bool EnableCommon;        ///
    bool ConfigMode;
    int Filter;                 // Режим фильтрации показываемых в списке компьютеров
    bool CheckFilter(MStatesInfo *Info_, int FreeTime_);
    void SetListViewComputersLine(TListItem *Item_, MStatesInfo *Info_);
public:		// User declarations
    void UpdateListViewComputers(bool Full_);
        __fastcall TFormMain(TComponent* Owner);
    void SetShell();
    int GetCompColorIcon(const MComputersItem *Comp_);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
extern std::unique_ptr <MShellState> ShellState;
extern std::unique_ptr <MOptions> Options;
extern std::unique_ptr <MComputers> Computers;
extern std::unique_ptr <MTariffs> Tariffs;
extern std::unique_ptr <MFines> Fines;
extern std::unique_ptr <MUsers> Users;
extern std::unique_ptr <MStates> States;
extern std::unique_ptr <MSync> Sync;
extern std::unique_ptr <MAuth> Auth;
extern std::unique_ptr <MLog> Log;
//---------------------------------------------------------------------------
#endif

