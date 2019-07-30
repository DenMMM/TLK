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
    TEdit *EditFullName;
    TEdit *EditLogin;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TComboBox *ComboBoxJobTitle;
    TButton *ButtonAdd;
    TButton *ButtonDelete;
    TButton *ButtonChange;
    TBitBtn *BitBtnSave;
    TBitBtn *BitBtnCancel;
    TBitBtn *BitBtnHelp;
    TLabel *Label4;
    TEdit *EditPassword;
    TComboBox *ComboBoxPasswordLength;
    TCheckBox *CheckBoxActive;
    TButton *ButtonAccessRights;
    TBevel *Bevel4;
    TBevel *Bevel1;
    TBevel *Bevel2;
    TButton *ButtonNewPassword;
    void __fastcall ButtonAccessRightsClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ListViewUsersInsert(TObject *Sender, TListItem *Item);
    void __fastcall ListViewUsersDeletion(TObject *Sender,
          TListItem *Item);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall EditLoginExit(TObject *Sender);
    void __fastcall EditPasswordExit(TObject *Sender);
    void __fastcall ComboBoxJobTitleExit(TObject *Sender);
    void __fastcall EditFullNameExit(TObject *Sender);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDeleteClick(TObject *Sender);
    void __fastcall ButtonChangeClick(TObject *Sender);
    void __fastcall ListViewUsersSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall BitBtnSaveClick(TObject *Sender);
    void __fastcall CheckBoxActiveClick(TObject *Sender);
private:	// User declarations
    unsigned int NewID;
    void __fastcall SetListViewUsersLine(TListItem *Item_);
public:		// User declarations
    MUser *TMPUser;
    __fastcall TFormUsers(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormUsers *FormUsers;
//---------------------------------------------------------------------------
#endif

