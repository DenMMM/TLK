//---------------------------------------------------------------------------
#ifndef UnitFormOptionsPasswordH
#define UnitFormOptionsPasswordH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitUsers.h"
#include "UnitOptions.h"
//---------------------------------------------------------------------------
class TFormOptionsPassword : public TForm
{
__published:	// IDE-managed Components
    TBevel *Bevel1;
    TLabel *LabelPassword;
    TLabel *LabelNew;
    TLabel *LabelConfirm;
    TBevel *Bevel2;
    TLabel *Label5;
    TEdit *EditPassword;
    TEdit *EditNew;
    TEdit *EditConfirm;
    TButton *ButtonGenerate;
    TButton *ButtonChange;
    TButton *ButtonCancel;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ButtonGenerateClick(TObject *Sender);
private:	// User declarations
    MOptions *Options;
    void SetCoord(int Left_, int Top_, bool LeftTop_);
public:		// User declarations
    __fastcall TFormOptionsPassword(TComponent* Owner);
    bool Execute(MOptions *Options_, int Left_, int Top_, bool LeftTop_);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormOptionsPassword *FormOptionsPassword;
//---------------------------------------------------------------------------
#endif

