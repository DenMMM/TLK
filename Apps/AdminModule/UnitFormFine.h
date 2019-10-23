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
#include <vector>
#include "UnitStates.h"
#include "UnitComputers.h"
#include "UnitFines.h"
#include "UnitLog.h"
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
    void __fastcall BitBtnFineClick(TObject *Sender);
    void __fastcall ListViewFinesInsert(TObject *Sender, TListItem *Item);
    void __fastcall ComboBoxTimeClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
    struct MApplyFine
    {
		MStatesItem *State;
        MFinesItem *Fine;
        int Number;
        bool Wait;
        bool Warn;
    };
    
	std::vector <MApplyFine> ApplyFines;
    void __fastcall SetListViewFinesLine(TListItem *Item_);
public:		// User declarations
    __fastcall TFormFine(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormFine *FormFine;
//---------------------------------------------------------------------------
#endif

