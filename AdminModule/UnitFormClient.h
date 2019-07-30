//---------------------------------------------------------------------------
#ifndef UnitFormClientH
#define UnitFormClientH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "UnitSend.h"
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TFormClient : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewComputers;
    TPageControl *PageControl;
    TTabSheet *TabSheetGames;
    TTabSheet *TabSheetOptions;
    TTreeView *TreeViewGames;
    TLabel *LabelName;
    TEdit *EditName;
    TLabel *LabelCmd;
    TEdit *EditCmd;
    TLabel *LabelIcon;
    TEdit *EditIcon;
    TButton *ButtonAdd;
    TButton *ButtonDel;
    TBevel *Bevel1;
    TLabel *Label1;
    TComboBox *ComboBoxToEndTime;
    TLabel *Label4;
    TComboBox *ComboBoxMessageTime;
    TLabel *Label5;
    TComboBox *ComboBoxRebootWait;
    TLabel *Label6;
    TComboBox *ComboBoxAutoLockTime;
    TBevel *Bevel2;
    TButton *ButtonOperations;
    TButton *ButtonClose;
    TButton *ButtonHelp;
    TPopupMenu *PopupMenuOperations;
    TMenuItem *NSend;
    TMenuItem *N2;
    TMenuItem *NOpen;
    TMenuItem *NSave;
    TMenuItem *N5;
    TMenuItem *NLoad;
    TOpenDialog *OpenDialog;
    TSaveDialog *SaveDialog;
    TButton *ButtonAddChild;
    TBitBtn *BitBtnUp;
    TBitBtn *BitBtnDown;
    TButton *ButtonCancel;
    TBevel *Bevel3;
    TBevel *Bevel4;
    TBevel *Bevel5;
    TPanel *PanelProcess;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ButtonOperationsClick(TObject *Sender);
    void __fastcall NOpenClick(TObject *Sender);
    void __fastcall NSaveClick(TObject *Sender);
    void __fastcall NSendClick(TObject *Sender);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDelClick(TObject *Sender);
    void __fastcall ButtonAddChildClick(TObject *Sender);
    void __fastcall BitBtnUpClick(TObject *Sender);
    void __fastcall BitBtnDownClick(TObject *Sender);
    void __fastcall TreeViewGamesChange(TObject *Sender, TTreeNode *Node);
    void __fastcall EditNameChange(TObject *Sender);
    void __fastcall EditCmdChange(TObject *Sender);
    void __fastcall EditIconChange(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall TreeViewGamesDeletion(TObject *Sender,
          TTreeNode *Node);
    void __fastcall ButtonCancelClick(TObject *Sender);
    void __fastcall ComboBoxToEndTimeClick(TObject *Sender);
    void __fastcall PageControlChange(TObject *Sender);
    void __fastcall NLoadClick(TObject *Sender);
private:	// User declarations
    MSend Send;
    MComputer **SendComputers;
    MGames *Games;
    MClOptions *Options;
    void SetTreeViewGamesLine(TTreeNode *Node_);
    void AddGamesToTree(TTreeNode *Node_, MGames *Games_);
    void CreateGamesTree(MGames *Games_);
    void AddGamesFromTree(MGames *Games_, TTreeNode *Node_);
    void CreateGamesFromTree(MGames *Games_);
    void OptionsToShell(MClOptions *Options_);
    void ShellToOptions(MClOptions *Options_);
    void SetEdit(bool Edit_, bool Full_);
    void SetSend(bool Send_);
    void __fastcall Dispatch(void *Message);
public:		// User declarations
    __fastcall TFormClient(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormClient *FormClient;
//---------------------------------------------------------------------------
#endif

