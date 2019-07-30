//---------------------------------------------------------------------------
#ifndef UnitFormClientConfigH
#define UnitFormClientConfigH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <NMMSG.hpp>
#include <Psock.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormClientConfig : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox3;
    TLabel *Label4;
    TEdit *EditOptionsPassword;
    TGroupBox *GroupBox2;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TBevel *Bevel1;
    TEdit *EditServerAddress;
    TEdit *EditNumberComputer;
    TEdit *EditPasswordToConnect;
    TBitBtn *BitBtnSave;
    TSaveDialog *SaveDialog;
    TBitBtn *BitBtnClose;
    TEdit *EditOptionsPasswordConfirm;
    TLabel *Label5;
    TBitBtn *BitBtnOpen;
    TBevel *Bevel2;
    TOpenDialog *OpenDialog;
    TBitBtn *BitBtnDefault;
    TCheckBox *CheckBoxServerAuto;
    void __fastcall EditNumberComputerEnter(TObject *Sender);
    void __fastcall EditNumberComputerExit(TObject *Sender);
    void __fastcall BitBtnSaveClick(TObject *Sender);
    void __fastcall BitBtnCloseClick(TObject *Sender);
    void __fastcall BitBtnOpenClick(TObject *Sender);
    
    void __fastcall FormDeactivate(TObject *Sender);
    
    void __fastcall FormShow(TObject *Sender);
    
    void __fastcall CheckBoxServerAutoClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormClientConfig(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern AnsiString WinDir;
extern PACKAGE TFormClientConfig *FormClientConfig;
//---------------------------------------------------------------------------
#endif

