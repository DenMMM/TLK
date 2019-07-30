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
    TButton *ButtonSave;
    TButton *ButtonCancel;
    TButton *ButtonHelp;
    TButton *ButtonAdd;
    TButton *ButtonDel;
    TListView *ListViewFines;
    TBevel *Bevel12;
    TLabel *LabelDescription;
    TEdit *EditDescription;
    TBevel *BevelBorder;
    TComboBox *ComboBoxTime;
    TLabel *LabelTime;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ButtonSaveClick(TObject *Sender);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDelClick(TObject *Sender);
    void __fastcall EditDescriptionExit(TObject *Sender);
    void __fastcall ListViewFinesInsert(TObject *Sender, TListItem *Item);
    void __fastcall ListViewFinesDeletion(TObject *Sender,
          TListItem *Item);
    void __fastcall ListViewFinesSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall ComboBoxTimeClick(TObject *Sender);
private:	// User declarations
    void SetEdit(bool Edit_);
    void SetListViewFinesLine(TListItem *Item_);
public:		// User declarations
    __fastcall TFormFines(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormFines *FormFines;
//---------------------------------------------------------------------------
#endif

