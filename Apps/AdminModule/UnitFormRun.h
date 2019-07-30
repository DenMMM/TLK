//---------------------------------------------------------------------------
#ifndef UnitFormRunH
#define UnitFormRunH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
#include "UnitStates.h"
#include "UnitComputers.h"
#include "UnitTariffs.h"
#include "UnitLog.h"
//---------------------------------------------------------------------------
class TFormRun : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewComputers;
    TComboBox *ComboBoxTariff;
    TComboBox *ComboBoxTime;
    TLabel *Label1;
    TLabel *LabelAllCost;
    TBevel *Bevel2;
    TBitBtn *BitBtnClose;
    TRadioButton *RadioButtonCash;
    TRadioButton *RadioButtonDesc;
    TComboBox *ComboBoxDesc;
    TRadioButton *RadioButtonSubs;
    TLabel *LabelRemaindTimeText;
    TLabel *LabelRemaindTime;
    TBevel *Bevel6;
    TComboBox *ComboBoxSubs;
    TBevel *Bevel1;
    TProgressBar *ProgressBarTime;
    TBitBtn *BitBtnRun;
    TBevel *Bevel3;
    TBevel *Bevel4;
    TTimer *TimerDialogLock;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ListViewComputersInsert(TObject *Sender,
          TListItem *Item);
    void __fastcall ComboBoxTariffClick(TObject *Sender);
    void __fastcall ComboBoxTimeClick(TObject *Sender);
    void __fastcall ComboBoxTimeChange(TObject *Sender);
    void __fastcall BitBtnRunClick(TObject *Sender);
    void __fastcall TimerDialogLockTimer(TObject *Sender);
    void __fastcall ListViewComputersSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
private:	// User declarations
    __int64 OpenDialogTime;
	MTariffsInfo UseTariffs;		// Список тарифов, доступных на время открытия диалога
	MTariffRunTimes UseTimes;	// Список пакетов для выбранного тарифа
	MTariffRunTimes CompTimes;	// Буфер для хранения времени и стоимости по каждому компьютеру
	unsigned SelTariffID;
    void SetListViewComputersLine(TListItem *Item_);
    void UpdateFullCost();
public:		// User declarations
    bool RunMode;               // Диалог открыт в режиме "запуска", а не добавления времени
    __fastcall TFormRun(TComponent* Owner);
};
//---------------------------------------------------------------------------
//extern PACKAGE TFormRun *FormRun;
//---------------------------------------------------------------------------
#endif

