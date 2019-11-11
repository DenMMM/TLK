//---------------------------------------------------------------------------
#ifndef UnitFormOptionsPassH
#define UnitFormOptionsPassH
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
class TFormOptionsPass : public TForm
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
	void __fastcall EditPasswordKeyPress(TObject *Sender, System::WideChar &Key);
private:	// User declarations
    MOptions *TmpOptions;
    void SetCoord(int Left_, int Top_, bool LeftTop_);
public:		// User declarations
    bool Execute(MOptions *Options_, int Left_, int Top_, bool LeftTop_);
    __fastcall TFormOptionsPass(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

