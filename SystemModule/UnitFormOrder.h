//---------------------------------------------------------------------------
#ifndef UnitFormOrderH
#define UnitFormOrderH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
#include "UnitOrderComputers.h"
#include "UnitListOfTariffs.h"
#include "UnitListOfTariffTimes.h"
//---------------------------------------------------------------------------
class TFormOrder : public TForm
{
__published:	// IDE-managed Components
    TBevel *Bevel1;
    TLabel *LabelCostText;
    TLabel *LabelRemaindTimeText;
    TBevel *BevelBaseButtonsIn;
    TComboBox *ComboBoxTariffTime;
    TEdit *EditSubscriptionNumber;
    TListView *ListViewOrderComputers;
    TListView *ListViewComputersForOrder;
    TBitBtn *BitBtnClose;
    TComboBox *ComboBoxTariffName;
    TLabel *LabelOrderCost;
    TBevel *Bevel2;
    TProgressBar *ProgressBarDialogUseTime;
    TTimer *TimerDialogLock;
    TBevel *Bevel5;
    TLabel *LabelCost;
    TBevel *Bevel6;
    TLabel *LabelRemaindTime;
    TBevel *Bevel7;
    TLabel *LabelRealSizeTime;
    TRadioButton *RadioButtonCash;
    TRadioButton *RadioButtonDesc;
    TRadioButton *RadioButtonSubs;
    TBevel *Bevel8;
    TBevel *Bevel9;
    TComboBox *ComboBoxDescNumber;
    TBevel *Bevel10;
    TLabel *LabelSubscriptionNumberText;
    TLabel *Label1;
    TButton *ButtonAdd;
    TButton *ButtonDelete;
    TButton *ButtonChange;
    TButton *ButtonRun;
    TBevel *BevelBaseButtonsOut;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ComboBoxTariffNameChange(TObject *Sender);
    void __fastcall ComboBoxTariffTimeChange(TObject *Sender);
    void __fastcall TimerDialogLockTimer(TObject *Sender);
    void __fastcall RadioButtonCashClick(TObject *Sender);
    void __fastcall ListViewComputersForOrderEnter(TObject *Sender);
    void __fastcall ListViewOrderComputersEnter(TObject *Sender);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonChangeClick(TObject *Sender);
    void __fastcall ButtonDeleteClick(TObject *Sender);
    void __fastcall ButtonRunClick(TObject *Sender);
    void __fastcall ListViewComputersForOrderSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall ListViewOrderComputersSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall ListViewOrderComputersInsert(TObject *Sender,
          TListItem *Item);
    void __fastcall ListViewOrderComputersDeletion(TObject *Sender,
          TListItem *Item);
    void __fastcall FormHide(TObject *Sender);
private:	// User declarations
    MListOfTariffs *TariffsForComputers;  // Тарифы, подходящие для выбранных компьютеров
    MListOfTariffTimes *TimesForTariffHours;  // Типы тарифов по времени,
    MListOfTariffTimes *TimesForTariffPackets;  // подходящие для выбранного тарифа и компьютеров
    unsigned int DayOfWeek1, DayOfWeek2, DayOfWeek3;  // Дни недели
    int MinutesFromBeginDay;  // Минуты с начала суток
    MOrderComputer *CurrentOrder;  // Оформляемый в данный момент компьютер
    double OrderCost;  // Общая стоимость заказа без учета уже запущенных компьютеров
    void __fastcall SetGroupStateCash(bool State_);
    void __fastcall SetGroupStateDesc(bool State_);
    void __fastcall SetGroupStateSubs(bool State_);
    void __fastcall SetListViewComputersForOrderLine(TListItem *Item_);
    void __fastcall SetListViewOrderComputersLine(TListItem *Item_);
    void __fastcall CreateTariffsForComputersList(TListView *ListView_);
    void __fastcall CreateTariffsNamesList();
    void __fastcall CreateTimesForTariffList();
    void __fastcall CreateTariffsTimeTypesList();
public:		// User declarations
    __fastcall TFormOrder(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormOrder *FormOrder;
//---------------------------------------------------------------------------
#endif

