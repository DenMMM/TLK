//---------------------------------------------------------------------------
#ifndef UnitFormGamesListH
#define UnitFormGamesListH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormGamesList : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewGames;
    TGroupBox *GroupBox1;
    TLabel *Label1;
    TEdit *EditGameName;
    TLabel *Label2;
    TEdit *EditCommandLine;
    TLabel *Label3;
    TEdit *EditIconFile;
    TLabel *Label4;
    TMemo *MemoGameDesciption;
    TBitBtn *BitBtnAdd;
    TBitBtn *BitBtnDelete;
    TBitBtn *BitBtnClose;
    TBitBtn *BitBtnLoad;
    TBitBtn *BitBtnSave;
    TMemo *MemoBuffer;
    TOpenDialog *OpenDialog;
    TSaveDialog *SaveDialog;
    TBitBtn *BitBtnDefault;
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall BitBtnCloseClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ListViewGamesChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
    void __fastcall BitBtnAddClick(TObject *Sender);
    void __fastcall BitBtnDeleteClick(TObject *Sender);
    void __fastcall BitBtnLoadClick(TObject *Sender);
    void __fastcall BitBtnSaveClick(TObject *Sender);
    void __fastcall EditGameNameExit(TObject *Sender);
    void __fastcall EditCommandLineExit(TObject *Sender);
    void __fastcall EditIconFileExit(TObject *Sender);
    void __fastcall MemoGameDesciptionExit(TObject *Sender);
private:	// User declarations
public:		// User declarations
    int __fastcall LoadGamesList(AnsiString FileName);
    int __fastcall SaveGamesList(AnsiString FileName);
    __fastcall TFormGamesList(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormGamesList *FormGamesList;
//---------------------------------------------------------------------------
#endif
