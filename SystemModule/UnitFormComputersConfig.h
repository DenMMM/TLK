//---------------------------------------------------------------------------
#ifndef UnitFormComputersConfigH
#define UnitFormComputersConfigH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "UnitGames.h"
//---------------------------------------------------------------------------
class TFormComputersConfig : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewComputers;
    TPageControl *PageControlOptions;
    TTabSheet *TabSheetGames;
    TTreeView *TreeViewGames;
    TEdit *EditGameName;
    TEdit *EditGameCommandLine;
    TEdit *EditGameIconFile;
    TButton *ButtonBrowseGameFile;
    TButton *ButtonBrowseGameIcon;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TButton *ButtonAddGame;
    TButton *ButtonDeleteGame;
    TButton *ButtonChangeGame;
    TBitBtn *BitBtnClose;
    TBitBtn *BitBtnHelp;
    TBevel *Bevel1;
    TOpenDialog *OpenDialog;
    TButton *ButtonSetComputerParam;
    TButton *ButtonGetComputerParam;
    void __fastcall TreeViewGamesDeletion(TObject *Sender,
          TTreeNode *Node);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall ButtonGetComputerParamClick(TObject *Sender);
    void __fastcall ButtonSetComputerParamClick(TObject *Sender);
    void __fastcall ButtonBrowseGameFileClick(TObject *Sender);
    void __fastcall ButtonBrowseGameIconClick(TObject *Sender);
    void __fastcall EditGameNameExit(TObject *Sender);
    void __fastcall EditGameCommandLineExit(TObject *Sender);
    void __fastcall EditGameIconFileExit(TObject *Sender);
    void __fastcall ButtonAddGameClick(TObject *Sender);
    void __fastcall ButtonDeleteGameClick(TObject *Sender);
    void __fastcall ButtonChangeGameClick(TObject *Sender);
    void __fastcall TreeViewGamesChange(TObject *Sender, TTreeNode *Node);
    void __fastcall ListViewComputersInsert(TObject *Sender,
          TListItem *Item);
private:	// User declarations
    MGame *TMPGame;
    void __fastcall SetTreeViewGamesLine(TTreeNode *TreeNode_);
    void __fastcall AddGamesToTree(MGames *Games_, TTreeNode *TreeNode_);
    void __fastcall AddGamesFromTree(MGames *Games_, TTreeNode *TreeNode_);
public:		// User declarations
    void __fastcall SetListViewComputersLine(TListItem *Item_);
    __fastcall TFormComputersConfig(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormComputersConfig *FormComputersConfig;
//---------------------------------------------------------------------------
#endif

