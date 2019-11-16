//---------------------------------------------------------------------------
#ifndef UnitFormMainH
#define UnitFormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ImgList.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
#include "UnitLogRecords.h"
//---------------------------------------------------------------------------
struct MLogFile
{
    std::wstring Name;
    MLogRecords Records;
};
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
    TMainMenu *MainMenu;
    TStatusBar *StatusBar1;
    TMenuItem *NOpen;
    TMenuItem *N2;
    TMenuItem *NClose;
    TOpenDialog *OpenDialog;
    TImageList *ImageList;
    TMenuItem *NWindows;
    TMenuItem *NFile;
    TMenuItem *NCascade;
    TMenuItem *NTile;
    TMenuItem *N4;
    void __fastcall NOpenClick(TObject *Sender);
    void __fastcall NCloseClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall NWindowsClick(TObject *Sender);
    void __fastcall NCascadeClick(TObject *Sender);
    void __fastcall NTileClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    void WindowOpen(const MLogFile &File_, TForm *Window_);
    void WindowCaption(TForm *Window_, char *Caption_);
    void WindowClose(TForm *Window_);
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif

