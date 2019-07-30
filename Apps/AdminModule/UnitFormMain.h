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
//---------------------------------------------------------------------------
#include "UnitComputers.h"
#include "UnitTariffs.h"
#include "UnitFines.h"
#include "UnitUsers.h"
#include "UnitOptions.h"
#include "UnitStates.h"
#include "UnitSync.h"
#include "UnitLog.h"
//---------------------------------------------------------------------------
// Main Computers Filter
#define mcfAll ((int)1)     // ��� ����������
#define mcfFree ((int)2)    // ��������� ����������
#define mcfService ((int)3) // �������� ��� ������������
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
    TPanel *PanelGeneral;
    TListView *ListViewComputers;
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
    TLabel *Label4;
    TLabel *Label5;
    TBevel *Bevel15;
    TPopupMenu *PopupMenuSystem;
    TMenuItem *NOpen;
    TMenuItem *NClose;
    TMenuItem *MenuItem9;
    TMenuItem *NWithoutLocker;
    TMenuItem *NWithLocker;
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
    TMenuItem *N8;
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
    void __fastcall NWithoutLockerClick(TObject *Sender);
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
private:	// User declarations
    bool EnableConfig;          // ��������� ������
    bool EnableLogReSet;        // ������ ��������� ������ ����� ����
    bool EnableCompCmd;         // ������� ��� �����������
    bool EnableCommon;
    int Filter;  // ����� ���������� ������������ � ������ �����������
    bool CheckFilter(MStateInfo *Info_, int FreeTime_);
    void SetListViewComputersLine(TListItem *Item_, MStateInfo *Info_);
public:		// User declarations
    void UpdateListViewComputers(bool Full_);
        __fastcall TFormMain(TComponent* Owner);
    void SetShell();
    bool MessageBoxError(UINT uCaption, UINT uMessage);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
extern MShellState *ShellState;
extern MOptions *Options;
extern MComputers *Computers;
extern MTariffs *Tariffs;
extern MFines *Fines;
extern MUsers *Users;
extern MStates *States;
extern MSync *Sync;
extern MLog *Log;
//---------------------------------------------------------------------------
#endif
