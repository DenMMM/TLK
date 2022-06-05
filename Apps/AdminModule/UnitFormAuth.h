//---------------------------------------------------------------------------
#ifndef UnitFormAuthH
#define UnitFormAuthH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitAuth.h"
#include <Dialogs.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFormAuth : public TForm
{
__published:	// IDE-managed Components
    TBevel *BevelBorder;
    TBevel *Bevel12;
    TLabel *LabelTime;
    TButton *ButtonOK;
    TButton *ButtonCancel;
    TButton *ButtonHelp;
    TButton *ButtonNew;
    TButton *ButtonSave;
    TMemo *MemoHEX;
    TSaveDialog *SaveDialog;
    TPopupMenu *PopupMenuHEX;
    void __fastcall MemoHEXKeyPress(TObject *Sender, char &Key);
    void __fastcall MemoHEXKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ButtonNewClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ButtonOKClick(TObject *Sender);
    void __fastcall MemoHEXExit(TObject *Sender);
    void __fastcall ButtonSaveClick(TObject *Sender);
private:	// User declarations
    MAuth TmpAuth;
    unsigned char TmpKey[MAC_KeySize];
    wchar_t HexBuffer[sizeof(TmpKey)*3+1];
public:		// User declarations
    __fastcall TFormAuth(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
