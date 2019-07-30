//---------------------------------------------------------------------------
#ifndef UnitFormMainOptionsH
#define UnitFormMainOptionsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormMainOptions : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TListView *ListViewClients;
    TEdit *EditPasswordToConnect;
    TEdit *EditClientAddress;
    TEdit *EditClientNumber;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TBevel *Bevel1;
    TGroupBox *GroupBox2;
    TRadioGroup *RadioGroupTariffType;
    TListView *ListViewTariffs;
    TEdit *EditBeginTimeHours;
    TEdit *EditSizeTimeHours;
    TLabel *Label4;
    TLabel *Label5;
    TBitBtn *BitBtnAddClient;
    TBitBtn *BitBtnDeleteClient;
    TBitBtn *BitBtnAddTariff;
    TBitBtn *BitBtnDeleteTariff;
    TBitBtn *BitBtnSave;
    TBitBtn *BitBtnCancel;
    TEdit *EditBeginTimeMinutes;
    TLabel *Label7;
    TEdit *EditSizeTimeMinutes;
    TLabel *Label8;
    void __fastcall RadioGroupTariffTypeClick(TObject *Sender);
    void __fastcall EditClientNumberEnter(TObject *Sender);
    void __fastcall EditClientNumberExit(TObject *Sender);
    void __fastcall BitBtnAddClientClick(TObject *Sender);
    void __fastcall BitBtnDeleteClientClick(TObject *Sender);
    void __fastcall BitBtnAddTariffClick(TObject *Sender);
    void __fastcall BitBtnDeleteTariffClick(TObject *Sender);
    void __fastcall BitBtnSaveClick(TObject *Sender);
    void __fastcall EditBeginTimeHoursEnter(TObject *Sender);
    void __fastcall EditBeginTimeHoursExit(TObject *Sender);
    
    void __fastcall EditBeginTimeMinutesExit(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormDeactivate(TObject *Sender);
    
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall BitBtnCancelClick(TObject *Sender);
    
    void __fastcall EditPasswordToConnectExit(TObject *Sender);
    
    void __fastcall ListViewClientsChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
    
private:	// User declarations
public:		// User declarations
    __fastcall TFormMainOptions(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMainOptions *FormMainOptions;
//---------------------------------------------------------------------------
#endif
