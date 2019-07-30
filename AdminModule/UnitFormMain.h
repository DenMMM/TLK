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
#include <AppEvnts.hpp>
#include <Graphics.hpp>
#include "Trayicon.h"
//---------------------------------------------------------------------------
#include "UnitComputers.h"
#include "UnitTariffs.h"
#include "UnitFines.h"
#include "UnitState.h"
//---------------------------------------------------------------------------
#define mcfAll ((int)0)  // Все компьютеры
#define mcfUsed ((int)1)  // Используемые в работе админом компьютеры
#define mcfFree ((int)2)  // Свободные компьютеры
#define mcfOccupied ((int)3)  // Занятые компьютеры (для которых поставлено время)
//---------------------------------------------------------------------------
#define mlcNumber ((unsigned int)1)  // Номер компьютера и цвет его группы
#define mlcNet ((unsigned int)2)  // Состояние сети
#define mlcState ((unsigned int)4)  // Режим работы компьютера
#define mlcTariff ((unsigned int)8)  // Сведения о тарифе
#define mlcWorkTime ((unsigned int)16)  // Время работы
#define mlcWorkTimeToEnd ((unsigned int)32)  // Оставшееся время работы
#define mlcFineTime ((unsigned int)64)  // Время штрафа
#define mlcFineTimeToEnd ((unsigned int)128)  // Оставшееся время штрафа
#define mlcAll ((unsigned int)255)  // Все столбцы
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
    TStatusBar *StatusBar;
    TApplicationEvents *ApplicationEvents;
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
    TMenuItem *NClose;
    TMenuItem *N14;
    TTrayIcon *TrayIcon;
    TMenuItem *NToTray;
    TMenuItem *NLock;
    TMenuItem *N12;
    TMenuItem *NLogIn;
    TMenuItem *N19;
    TPanel *PanelButtons;
    TBevel *Bevel3;
    TBevel *Bevel5;
    TBevel *Bevel6;
    TBevel *Bevel2;
    TBitBtn *BitBtnNewOrder;
    TBitBtn *BitBtnAddTime;
    TBitBtn *BitBtnFine;
    TBitBtn *BitBtnAdditionals;
    TBitBtn *BitBtnLockServer;
    TBitBtn *BitBtnToTray;
    TPanel *PanelGeneral;
    TListView *ListViewComputers;
    TBevel *Bevel1;
    TProgressBar *ProgressBarNetProcess;
    TBevel *Bevel4;
    TBevel *Bevel7;
    TBitBtn *BitBtnLock;
    TBitBtn *BitBtnUnLock;
    TPopupMenu *PopupMenuViewFilter;
    TMenuItem *NFilterUsed;
    TMenuItem *NFilterFree;
    TMenuItem *NFilterOccupied;
    TMenuItem *NFilterAll;
    TBitBtn *BitBtnViewFilter;
    TImageList *ImageListIcons;
    TPopupMenu *PopupMenuAdditionals;
    TMenuItem *NReboot;
    TMenuItem *NShutdown;
    TMenuItem *N27;
    TMenuItem *N28;
    TMenuItem *NNotInUse;
    TMenuItem *NInUse;
    TMenuItem *NWithoutLocker;
    TMenuItem *NWithLocker;
    TMenuItem *N32;
    TBitBtn *BitBtnDescription;
    TBevel *Bevel9;
    TBevel *Bevel10;
    TMenuItem *NViewingAndMessages;
    TMenuItem *N34;
    TMenuItem *NAutoMode;
    TMenuItem *NNormalMode;
    TMenuItem *N37;
    TMenuItem *NPause;
    TMenuItem *NRun;
    TMenuItem *NUsers;
    TMenuItem *NLogOut;
    TMenuItem *NComputersConfig;
    TMenuItem *NPasswords;
    TMenuItem *NChangePassword;
    TLabel *LabelTimeHoursMins;
    TLabel *LabelTimeSecs;
    TBitBtn *BitBtnAlarm;
    TBevel *Bevel11;
    TBevel *Bevel12;
    TBitBtn *BitBtnExchange;
    TBevel *Bevel13;
    TBevel *Bevel8;
    TBitBtn *BitBtnClose;
    TMenuItem *N1;
    TPanel *Panel1;
    TLabel *Label1;
    TComboBox *ComboBoxNeedCompNum;
    TLabel *Label2;
    TLabel *LabelMinTime;
    TBevel *Bevel14;
    TLabel *Label4;
    TLabel *Label5;
    TBevel *Bevel15;
    TLabel *Label6;
    TLabel *LabelMaxTime;
    TBevel *Bevel16;
    TLabel *Label8;
    TLabel *LabelNeedComputers;
    TBevel *Bevel17;
    void __fastcall ApplicationEventsHint(TObject *Sender);
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall BitBtnNewOrderClick(TObject *Sender);
    void __fastcall NCloseClick(TObject *Sender);
    void __fastcall NToTrayClick(TObject *Sender);
    void __fastcall NLockClick(TObject *Sender);
    void __fastcall TrayIconMinimize(TObject *Sender);
    void __fastcall TrayIconRestore(TObject *Sender);
    void __fastcall NLogInClick(TObject *Sender);
    void __fastcall BitBtnViewFilterClick(TObject *Sender);
    void __fastcall NFilterUsedClick(TObject *Sender);
    void __fastcall BitBtnAdditionalsClick(TObject *Sender);
    void __fastcall NComputersClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall BitBtnLockClick(TObject *Sender);
    void __fastcall NAutoModeClick(TObject *Sender);
    void __fastcall NWithoutLockerClick(TObject *Sender);
    void __fastcall NPauseClick(TObject *Sender);
    void __fastcall NNotInUseClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ListViewComputersInsert(TObject *Sender,
          TListItem *Item);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall NLogOutClick(TObject *Sender);
    void __fastcall NAboutClick(TObject *Sender);
    void __fastcall NRebootClick(TObject *Sender);
    void __fastcall NShutdownClick(TObject *Sender);
    void __fastcall BitBtnExchangeClick(TObject *Sender);
private:	// User declarations
    CRITICAL_SECTION CS_ListViewComputers;
    int ComputersFilter;  // Режим фильтрации показываемых в списке компьютеров
    unsigned int __fastcall ComputersFilterMask();  // Определить маску для отсева компьютеров по фильтру
    void __fastcall SetListViewComputersLine(TListItem *Item_, unsigned int Columns_);
public:		// User declarations
    void __fastcall CreateListViewComputers();
    void __fastcall CorrectListViewComputers(MComputer *Computer_, unsigned int Columns_);
        __fastcall TFormMain(TComponent* Owner);
    void __fastcall SetAccess(MUser *User_);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
extern MComputers *Computers;
extern MTariffs *Tariffs;
extern MFines *Fines;
extern MUsers *Users;
extern MState *State;
//---------------------------------------------------------------------------
#endif

