//---------------------------------------------------------------------------
#ifndef UnitFormOpenConfigH
#define UnitFormOpenConfigH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitOptions.h"
//---------------------------------------------------------------------------
class TFormOpenConfig : public TForm
{
__published:	// IDE-managed Components
    TBevel *Bevel1;
    TLabel *LabelPassword;
    TEdit *EditPassword;
    TButton *ButtonApply;
    TButton *ButtonCancel;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
    MOptions *Options;
    void SetCoord(int Left_, int Top_, bool LeftTop_);
public:		// User declarations
    __fastcall TFormOpenConfig(TComponent* Owner);
    bool Execute(MOptions *Options_, int Left_, int Top_, bool LeftTop_);
};
//---------------------------------------------------------------------------
#endif
