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
#include <System.ImageList.hpp>
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
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
    MGames Games;

    void AddGamesToTree(MGames *Games_, TTreeNode *TreeNode_, TImageList *ImageList_);
    void UpdateGames(unsigned Pages_);
    void __fastcall MQueryEndSession(TMessage &Msg);
    BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER(WM_QUERYENDSESSION,TMessage,MQueryEndSession);
    END_MESSAGE_MAP(TForm);
public:		// User declarations
    void ShowGames(unsigned Pages_);
    __fastcall TFormGames(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormGames *FormGames;
//---------------------------------------------------------------------------
#endif
