//---------------------------------------------------------------------------
#ifndef UnitFormFinesH
#define UnitFormFinesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitFines.h"
//---------------------------------------------------------------------------
class TFormFines : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewFines;
    TBitBtn *BitBtnAdd;
    TBitBtn *BitBtnDelete;
    TBitBtn *BitBtnChange;
    TLabel *LabelTimeText;
    TComboBox *ComboBoxTime;
    TLabel *LabelCommentText;
    TEdit *EditComment;
    TBevel *Bevel1;
    TBitBtn *BitBtnSave;
    TBitBtn *BitBtnCancel;
    TBitBtn *BitBtnHelp;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ComboBoxTimeChange(TObject *Sender);
    void __fastcall EditCommentExit(TObject *Sender);
    void __fastcall BitBtnAddClick(TObject *Sender);
    void __fastcall BitBtnDeleteClick(TObject *Sender);
    void __fastcall BitBtnChangeClick(TObject *Sender);
    void __fastcall BitBtnSaveClick(TObject *Sender);
    void __fastcall ListViewFinesChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
private:	// User declarations
    MFines *TMPFines;
    MFine *TMPFine;
    void __fastcall SetListViewFinesLine(int Number_);
public:		// User declarations
    __fastcall TFormFines(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormFines *FormFines;
//---------------------------------------------------------------------------
#endif

