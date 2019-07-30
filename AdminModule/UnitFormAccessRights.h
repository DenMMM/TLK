//---------------------------------------------------------------------------
#ifndef UnitFormAccessRightsH
#define UnitFormAccessRightsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormAccessRights : public TForm
{
__published:	// IDE-managed Components
    TBitBtn *BitBtnSave;
    TBitBtn *BitBtnCancel;
    TBitBtn *BitBtnHelp;
    TCheckBox *CheckBox1;
    TCheckBox *CheckBoxSetAutoMode;
    TCheckBox *CheckBoxSetUseMode;
    TCheckBox *CheckBox4;
    TCheckBox *CheckBox5;
    TCheckBox *CheckBox6;
    TCheckBox *CheckBoxRemoveAllTime;
    TCheckBox *CheckBox9;
    TCheckBox *CheckBoxClientsServices;
    TCheckBox *CheckBoxComputersServices;
    TBevel *Bevel1;
    TCheckBox *CheckBoxWorkWithLogs;
    TBevel *Bevel4;
    TBevel *Bevel5;
    void __fastcall CheckBoxClientsServicesClick(TObject *Sender);
    void __fastcall CheckBoxComputersServicesClick(TObject *Sender);
    void __fastcall BitBtnSaveClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
    unsigned int TMPRights;
    __fastcall TFormAccessRights(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAccessRights *FormAccessRights;
//---------------------------------------------------------------------------
#endif

