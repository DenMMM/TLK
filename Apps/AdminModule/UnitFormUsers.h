//---------------------------------------------------------------------------
#ifndef UnitFormUsersH
#define UnitFormUsersH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitUsers.h"
//---------------------------------------------------------------------------
class TFormUsers : public TForm
{
__published:	// IDE-managed Components
    TListView *ListViewUsers;
    TEdit *EditName;
    TEdit *EditLogin;
    TLabel *LabelFullName;
    TLabel *LabelLogin;
    TBevel *Bevel4;
    TBevel *Bevel1;
    TButton *ButtonSave;
    TButton *ButtonCancel;
    TButton *ButtonHelp;
    TButton *ButtonAdd;
    TButton *ButtonDel;
    TLabel *LabelActive;
    TBitBtn *BitBtnActive;
    TBitBtn *BitBtnInactive;
    TButton *ButtonPassword;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ListViewUsersInsert(TObject *Sender, TListItem *Item);
    void __fastcall EditLoginExit(TObject *Sender);
    void __fastcall EditNameExit(TObject *Sender);
    void __fastcall ListViewUsersSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDelClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ButtonPasswordClick(TObject *Sender);
    void __fastcall BitBtnActiveClick(TObject *Sender);
    void __fastcall ButtonSaveClick(TObject *Sender);
private:	// User declarations
    MUsers  TmpUsers;
    void SetEdit(bool Edit_);
    void SetListViewUsersLine(TListItem *Item_);
public:		// User declarations
    __fastcall TFormUsers(TComponent* Owner);
};
//---------------------------------------------------------------------------
//extern PACKAGE TFormUsers *FormUsers;
//---------------------------------------------------------------------------
#endif

