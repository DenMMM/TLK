//---------------------------------------------------------------------------
#ifndef UnitFormMainH
#define UnitFormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <NMMSG.hpp>
#include <Psock.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
// Режимы работы клиентских компьютеров
#define ClientModeNotLock 0
#define ClientModeWork 1
#define ClientModeWorkPause 2
#define ClientModeLock 3
#define ClientModeRequestMode 4
#define ClientModeNotResponse 5
#define ClientModeNotUsed 6
// Режимы работы сервера
#define ServerModeLock 0
#define ServerModeNotLock 1
// Сетевые сообщения для клиентов
#define NetMessageAdminMessage 0
#define NetMessageRequestMode 1
#define NetMessageWork 2
#define NetMessageWorkPause 3
#define NetMessageLock 4
#define NetMessageReboot 5
#define NetMessageShutdown 6
#define NetMessageAddTime 7
#define NetMessageNotLock 8
#define NetMessageNormal 9
// События для записи в лог администратора
#define AdminLogAdminMessage 0
#define AdminLogClientMessage 1
#define AdminLogClientWarning 2
#define AdminLogProgramStart 3
#define AdminLogProgramClose 4
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
    TPanel *PanelButtons;
    TBitBtn *BitBtnRefreshAll;
    TBitBtn *BitBtnPause;
    TBitBtn *BitBtnStop;
    TPanel *PanelWorkSpace;
    TListView *ListViewComputers;
    TRichEdit *RichEditAdminLog;
    TPanel *PanelStatistics;
    TSplitter *SplitterLogMessages;
    TTimer *TimerTime;
    TBevel *Bevel2;
    TBevel *Bevel3;
    TBitBtn *BitBtnExit;
    TComboBox *ComboBoxSetTime;
    TLabel *Label6;
    TLabel *LabelFreeAre;
    TBevel *Bevel5;
    TBitBtn *BitBtnAbout;
    TBitBtn *BitBtnHelp;
    TBitBtn *BitBtnRun;
    TPanel *PanelMessage;
    TEdit *EditMessage;
    TProgressBar *ProgressBarRefresh;
    TBevel *Bevel1;
    TProgressBar *ProgressBarRun;
    TBevel *Bevel4;
    TProgressBar *ProgressBarStop;
    TBevel *Bevel7;
    TPanel *PanelTime;
    TLabel *LabelHours;
    TLabel *Label1;
    TLabel *LabelMinutes;
    TLabel *LabelSeconds;
    TBitBtn *BitBtnAdditionals;
    TBitBtn *BitBtnAddTime;
    TBitBtn *BitBtnRefresh;
    TPopupMenu *PopupMenuAdditionals;
    TMenuItem *NReboot;
    TMenuItem *NShutdown;
    TMenuItem *N1;
    TMenuItem *NUnchecked;
    TMenuItem *NChecked;
    TMenuItem *N2;
    TProgressBar *ProgressBarAdditionals;
    TBevel *Bevel10;
    TMenuItem *NMainOptions;
    TStatusBar *StatusBar;
    TMenuItem *NClientConfig;
    TMenuItem *NGamesList;
    TMenuItem *N3;
    TMenuItem *NPasswords;
    TMenuItem *N4;
    TComboBox *ComboBoxNeedFreeComputers;
    TLabel *LabelFreeAfter;
    TEdit *EditNikName;
    TMenuItem *NNotLock;
    TMenuItem *NNormal;
    TMenuItem *N5;
    TNMMSGServ *NMMSGServ;
    TNMMsg *NMMsg;
    TBevel *Bevel6;
    TLabel *LabelFreeNumbers;
    TBevel *Bevel8;
    TMenuItem *NOpenClose;
    TMenuItem *N6;
    void __fastcall TimerTimeTimer(TObject *Sender);
    void __fastcall BitBtnAboutClick(TObject *Sender);
    void __fastcall BitBtnExitClick(TObject *Sender);
    void __fastcall BitBtnRunClick(TObject *Sender);
    void __fastcall BitBtnRefreshAllClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall BitBtnPauseClick(TObject *Sender);
    void __fastcall BitBtnStopClick(TObject *Sender);
    void __fastcall NMMSGServMSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall EditMessageKeyPress(TObject *Sender, char &Key);
    void __fastcall PanelMessageResize(TObject *Sender);
    
    void __fastcall BitBtnAdditionalsClick(TObject *Sender);
    void __fastcall BitBtnAddTimeClick(TObject *Sender);
    void __fastcall BitBtnRefreshClick(TObject *Sender);
    void __fastcall NRebootClick(TObject *Sender);
    void __fastcall NUncheckedClick(TObject *Sender);
    void __fastcall NCheckedClick(TObject *Sender);
    void __fastcall NMainOptionsClick(TObject *Sender);
    
    void __fastcall FormResize(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall BitBtnHelpClick(TObject *Sender);
    void __fastcall NClientConfigClick(TObject *Sender);
    void __fastcall NGamesListClick(TObject *Sender);
    void __fastcall NPasswordsClick(TObject *Sender);
    void __fastcall ListViewComputersChange(TObject *Sender,
          TListItem *Item, TItemChange Change);
    
    void __fastcall ComboBoxNeedFreeComputersChange(TObject *Sender);
    
    
    void __fastcall NOpenCloseClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
public:		// User declarations
    __fastcall TFormMain(TComponent* Owner);
    void __fastcall ShowHint(TObject *Sender);
    int __fastcall SendNetMessage(int ClientNumber, int Type, AnsiString Message);
    void __fastcall CreateComputerList();
    void __fastcall CreateTariffsList();
    void __fastcall SetProgressBars();
    void __fastcall SetListViewClientMode(int ClientNumber);
    void __fastcall AddAdminLog(int Type, AnsiString Message, int Computer);
    void __fastcall RefreshStatistic();
    void __fastcall SetMode(int Mode);
};
//---------------------------------------------------------------------------
extern AnsiString WinDir;
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
