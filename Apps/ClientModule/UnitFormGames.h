//---------------------------------------------------------------------------
#ifndef UnitFormGamesH
#define UnitFormGamesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
#include "UnitGames.h"
//---------------------------------------------------------------------------
#define CMD_VPlay           "vplay:"
//---------------------------------------------------------------------------
class TFormGames : public TForm
{
__published:	// IDE-managed Components
    TImageList *ImageListGamesIcons;
    TTreeView *TreeViewGames;
    TImage *Image4;
    TImage *Image2;
    TImage *Image1;
    TImage *Image3;
    TImage *Image5;
    TImage *Image8;
    TImage *Image7;
    TImage *Image6;
    void __fastcall TreeViewGamesDblClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall TreeViewGamesKeyPress(TObject *Sender, char &Key);
private:	// User declarations
    MGames Games;
    void AddGamesToTree(MGames *Games_, TTreeNode *TreeNode_, TImageList *ImageList_);
public:		// User declarations
    void ShowGames(unsigned Pages_);
    __fastcall TFormGames(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormGames *FormGames;
//---------------------------------------------------------------------------
#endif
