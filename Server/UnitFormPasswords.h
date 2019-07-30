//---------------------------------------------------------------------------
#ifndef UnitFormPasswordsH
#define UnitFormPasswordsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TFormPasswords : public TForm
{
__published:	// IDE-managed Components
    TBitBtn *BitBtnClose;
    TGroupBox *GroupBox1;
    TLabel *Label1;
    TComboBox *ComboBoxPasswordLength;
    TLabel *Label2;
    TEdit *EditPassword;
    TBitBtn *BitBtnGenerate;
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    
    void __fastcall BitBtnCloseClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall BitBtnGenerateClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormPasswords(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPasswords *FormPasswords;
//---------------------------------------------------------------------------
#endif
