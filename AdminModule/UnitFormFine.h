//---------------------------------------------------------------------------
#ifndef UnitFormFineH
#define UnitFormFineH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
#include "UnitComputers.h"
#include "UnitFines.h"
//---------------------------------------------------------------------------
struct MApplyFine
{
    int Number;
    MFine *Fine;
    bool Wait;
    bool Warn;
};
//---------------------------------------------------------------------------
class TFormFine : public TForm
{
__published:	// IDE-managed Components
    TBevel *Bevel2;
    TBevel *Bevel4;
    TBevel *Bevel8;
    TLabel *Label1;
    TLabel *LabelAllReturn;
    TLabel *Label2;
    TCheckBox *CheckBoxWarn;
    TListView *ListViewFines;
    TBitBtn *BitBtn1;
    TBitBtn *BitBtnFine;
    TCheckBox *CheckBoxWait;
    TComboBox *ComboBoxTime;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall BitBtnFineClick(TObject *Sender);
    void __fastcall ListViewFinesInsert(TObject *Sender, TListItem *Item);
    void __fastcall ListViewFinesDeletion(TObject *Sender,
          TListItem *Item);
    void __fastcall ComboBoxTimeClick(TObject *Sender);
private:	// User declarations
    void __fastcall SetListViewFinesLine(TListItem *Item_);
public:		// User declarations
    __fastcall TFormFine(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormFine *FormFine;
//---------------------------------------------------------------------------
#endif

