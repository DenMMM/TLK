//---------------------------------------------------------------------------
#ifndef UnitFormMainH
#define UnitFormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <NMMSG.hpp>
#include <Psock.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
    TTimer *TimerRefreshMessage;
    TNMMSGServ *NMMSGServ;
    TNMMsg *NMMsg;
    TTimer *TimerTimes;
    TPanel *PanelBottom;
    TImageList *ImageListIcons;
    TPanel *PanelGames;
    TListView *ListViewGames;
    TPanel *PanelBottomBorder;
    TImage *ImageBottomLeftBorder;
    TImage *ImageBottomRightBorder;
    TImage *ImageBottomBorder;
    TPanel *PanelTop;
    TMemo *MemoBuffer;
    TImage *ImageGameIcon;
    TPanel *PanelBorder;
    TBevel *Bevel4;
    TGroupBox *GroupBoxPasswordChange;
    TLabel *Label7;
    TLabel *Label8;
    TBevel *Bevel5;
    TBitBtn *BitBtnPasswordChangeOk;
    TBitBtn *BitBtnPasswordChangeCancel;
    TEdit *EditPasswordOptions;
    TEdit *EditConfirmPasswordOptions;
    TGroupBox *GroupBoxNetOptions;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label9;
    TBevel *Bevel2;
    TBevel *Bevel3;
    TBitBtn *BitBtnNetOptionsOk;
    TBitBtn *BitBtnNetOptionsCancel;
    TEdit *EditServerAddress;
    TEdit *EditNumberComputer;
    TEdit *EditPasswordToConnect;
    TGroupBox *GroupBoxOptions;
    TBevel *Bevel6;
    TCheckBox *CheckBoxNotLock;
    TBitBtn *BitBtnPasswordChange;
    TBitBtn *BitBtnNetOptions;
    TBitBtn *BitBtnShutdown;
    TBitBtn *BitBtnOptionsCancel;
    TBitBtn *BitBtnOptionsOk;
    TPanel *PanelGeneral;
    TImage *ImageFace;
    TLabel *LabelTime;
    TLabel *LabelTimeToEndTime;
    TLabel *LabelNumberComputer;
    TLabel *LabelMessages;
    TSpeedButton *SpeedButtonReboot;
    TSpeedButton *SpeedButtonOptions;
    TSpeedButton *SpeedButtonAbout;
    TEdit *EditMessage;
    TCheckBox *CheckBoxAutoServer;
    TEdit *EditNikName;
    TImage *ImageLeftBorder;
    TPanel *PanelTopBorder;
    TImage *ImageTopLeftBorder;
    TImage *ImageTopRightBorder;
    TImage *ImageTopBorder;
    TImage *ImageRightBorder;
    TPanel *PanelGameDescription;
    TMemo *MemoGameDescription;
    TImage *Image2;
    TImage *Image4;
    TImage *Image8;
    TImage *Image1;
    TImage *Image3;
    TImage *Image5;
    TImage *Image6;
    TImage *Image7;
    TPanel *PanelImage;
    TImage *ImageMessage;
    TPanel *PanelAbout;
    TMemo *MemoInfo;
    TImage *ImageAbout;
    TSpeedButton *SpeedButtonAboutOk;
    TPanel *PanelEnterPassword;
    TImage *ImageEnterPassword;
    TEdit *EditEnterPassword;
    TLabel *LabelEnterPasswordRetry;
    TSpeedButton *SpeedButtonPasswordOk;
    TSpeedButton *SpeedButtonPasswordCancel;
    void __fastcall TimerRefreshMessageTimer(TObject *Sender);
    void __fastcall NMMSGServMSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall TimerTimesTimer(TObject *Sender);
    void __fastcall BitBtnNetOptionsOkClick(TObject *Sender);
    void __fastcall BitBtnNetOptionsCancelClick(TObject *Sender);
    void __fastcall EditNumberComputerEnter(TObject *Sender);
    void __fastcall EditNumberComputerExit(TObject *Sender);
    void __fastcall EditMessageKeyPress(TObject *Sender, char &Key);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall BitBtnShutdownClick(TObject *Sender);
    void __fastcall BitBtnNetOptionsClick(TObject *Sender);
    void __fastcall BitBtnOptionsOkClick(TObject *Sender);
    void __fastcall BitBtnOptionsCancelClick(TObject *Sender);
    void __fastcall BitBtnPasswordChangeCancelClick(TObject *Sender);
    void __fastcall BitBtnPasswordChangeOkClick(TObject *Sender);
    void __fastcall BitBtnPasswordChangeClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall SpeedButtonRebootClick(TObject *Sender);
    void __fastcall SpeedButtonOptionsClick(TObject *Sender);
    void __fastcall SpeedButtonAboutClick(TObject *Sender);
    void __fastcall ListViewGamesChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
    void __fastcall ListViewGamesDblClick(TObject *Sender);
    void __fastcall ListViewGamesMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
    void __fastcall CheckBoxAutoServerClick(TObject *Sender);
    
    
    
    
    
    void __fastcall SpeedButtonAboutOkClick(TObject *Sender);
    void __fastcall SpeedButtonPasswordOkClick(TObject *Sender);
    void __fastcall SpeedButtonPasswordCancelClick(TObject *Sender);
    void __fastcall EditEnterPasswordKeyPress(TObject *Sender, char &Key);
    
private:	// User declarations
public:		// User declarations
    void __fastcall MessageImage(int Message);
    void __fastcall SetMode(int Mode);
    int __fastcall SendNetMessage(int Type, AnsiString Message);
    void __fastcall SwitchTo (TWinControl *From, TWinControl *To, TWinControl *Active);
    void __fastcall AddLogMessage(AnsiString Message);
    void __fastcall AddBossLog(int Type);
    int __fastcall LoadGamesMenu();
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern AnsiString WinDir;
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif

