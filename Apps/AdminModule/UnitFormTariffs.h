//---------------------------------------------------------------------------
#ifndef UnitFormTariffsH
#define UnitFormTariffsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include <CheckLst.hpp>
//---------------------------------------------------------------------------
#include "UnitTariffs.h"
//---------------------------------------------------------------------------
class TFormTariffs : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewNames;
    TBevel *BevelBorder;
    TEdit *EditName;
    TOpenPictureDialog *OpenPictureDialog;
    TLabel *Label1;
    TListView *ListViewComputers;
    TButton *ButtonAdd;
    TButton *ButtonDel;
    TButton *ButtonTimes;
    TCheckBox *CheckBoxReboot;
    TButton *ButtonSave;
    TButton *ButtonCancel;
    TButton *ButtonHelp;
    TBevel *Bevel7;
    TLabel *LabelTariffName;
    TBevel *Bevel10;
    TLabel *LabelTariffApps;
    TCheckListBox *CheckListBoxApps;
    TLabel *LabelTariffComputers;
    TButton *ButtonSetSelComp;
    TButton *ButtonSetAllComp;
    TButton *ButtonResAllComp;
    TButton *ButtonResSelComp;
    TCheckBox *CheckBoxRoute;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ListViewComputersKeyPress(TObject *Sender, char &Key);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDelClick(TObject *Sender);
    void __fastcall ButtonTimesClick(TObject *Sender);
    void __fastcall ListViewNamesInsert(TObject *Sender, TListItem *Item);
    void __fastcall ListViewNamesSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall ListViewComputersExit(TObject *Sender);
    void __fastcall CheckListBoxAppsExit(TObject *Sender);
    void __fastcall ButtonSetSelCompClick(TObject *Sender);
    void __fastcall ButtonSetAllCompClick(TObject *Sender);
    void __fastcall CheckBoxRebootExit(TObject *Sender);
    void __fastcall EditNameExit(TObject *Sender);
    void __fastcall CheckBoxRouteExit(TObject *Sender);
    void __fastcall ButtonSaveClick(TObject *Sender);
private:	// User declarations
    MTariffs TmpTariffs;               // Буфер для списка тарифов
    char TmpComps[MAX_Comps];          // Буфер для списка компьютеров редактируемого тарифа
    void SetEdit(bool Edit_);
    void SetListViewNamesLine(TListItem *Item_);
public:		// User declarations
    __fastcall TFormTariffs(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

