//---------------------------------------------------------------------------
#ifndef UnitModeChangeH
#define UnitModeChangeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TFormModeChange : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TBevel *Bevel1;
    TBevel *Bevel2;
    TLabel *Label2;
    TEdit *EditCurrentPassword;
    TLabel *Label3;
    TEdit *EditNewPassword;
    TLabel *Label4;
    TEdit *EditConfirmPassword;
    TBitBtn *BitBtnOk;
    TBitBtn *BitBtnCancel;
    void __fastcall FormShow(TObject *Sender);
    
    
    
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall BitBtnOkClick(TObject *Sender);
    
    void __fastcall BitBtnCancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormModeChange(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormModeChange *FormModeChange;
//---------------------------------------------------------------------------
#endif
