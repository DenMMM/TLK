//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormUsers.h"
#include "UnitFormMain.h"
#include "UnitFormAccessRights.h"
#include "UnitOptionsLoadSave.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormUsers *FormUsers;
//---------------------------------------------------------------------------
__fastcall TFormUsers::TFormUsers(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::FormShow(TObject *Sender)
{
    TMPUser=new MUser;

    NewID=0;
    for ( MUser *User=(MUser*)Users->FirstItem; User; User=(MUser*)User->NextItem )
    {
        TListItem *NewListItem;
        NewListItem=ListViewUsers->Items->Add();
        *((MUser*)NewListItem->Data)=*User;
        SetListViewUsersLine(NewListItem);
        if ( NewID<User->ID ) NewID=User->ID;
    }
    NewID++;
    //
    for ( int i=6; i<17; i++ ) ComboBoxPasswordLength->Items->Add(IntToStr(i));
    //
    CheckBoxActive->Checked=true;
    ActiveControl=ListViewUsers;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::FormHide(TObject *Sender)
{
    ListViewUsers->Items->Clear();
    ComboBoxPasswordLength->Items->Clear();
    EditLogin->Text="";
    EditPassword->Text="";
    ComboBoxJobTitle->Text="";
    EditFullName->Text="";
    delete TMPUser;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ListViewUsersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->Data=(void*)new MUser;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ListViewUsersDeletion(TObject *Sender,
      TListItem *Item)
{
    delete (MUser*)Item->Data;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ListViewUsersSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( !Selected ) return;
    //
    *TMPUser=*((MUser*)Item->Data);
    //
    EditLogin->Text=TMPUser->Login;
    EditPassword->Text="";
    CheckBoxActive->Checked=TMPUser->Active;
    ComboBoxJobTitle->Text=TMPUser->JobTitle;
    EditFullName->Text=TMPUser->FullName;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::EditLoginExit(TObject *Sender)
{
    TMPUser->Login=EditLogin->Text.Trim();
    EditLogin->Text=TMPUser->Login;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::EditPasswordExit(TObject *Sender)
{
    AnsiString line;
    if ( (line=EditPassword->Text.Trim())=="" ) return;
    TMPUser->Password=line; EditPassword->Text=line;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ComboBoxJobTitleExit(TObject *Sender)
{
    TMPUser->JobTitle=ComboBoxJobTitle->Text.Trim();
    ComboBoxJobTitle->Text=TMPUser->JobTitle;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::EditFullNameExit(TObject *Sender)
{
    TMPUser->FullName=EditFullName->Text.Trim();
    EditFullName->Text=TMPUser->FullName;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonAccessRightsClick(TObject *Sender)
{
    FormAccessRights->Left=Left+30;
    FormAccessRights->Top=Top+30;
    FormAccessRights->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonAddClick(TObject *Sender)
{
    TListItem *NewListItem;
    NewListItem=ListViewUsers->Items->Add();
    TMPUser->ID=NewID; NewID++;
    *((MUser*)NewListItem->Data)=*TMPUser;
    SetListViewUsersLine(NewListItem);
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonDeleteClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewUsers->Selected, *NewItem; Item; )
    {
        NewItem=ListViewUsers->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonChangeClick(TObject *Sender)
{
    TListItem *ListItem;
    if ( (ListItem=ListViewUsers->ItemFocused)==NULL ) return;
    *((MUser*)ListItem->Data)=*TMPUser;
    SetListViewUsersLine(ListItem);
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::BitBtnSaveClick(TObject *Sender)
{
    Users->Clear();
    TListItems *Items=ListViewUsers->Items;
    for ( int i=0, j=Items->Count; i<j; i++ )
    {
        MUser *NewUser;
        NewUser=(MUser*)Users->Add();
        *NewUser=*((MUser*)Items->Item[i]->Data);
    }
    UsersSave(Users);
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::SetListViewUsersLine(TListItem *Item_)
{
    MUser *User=(MUser*)Item_->Data;
    TStrings *SubItems=Item_->SubItems;
    SubItems->Strings[0]=User->Login;
    SubItems->Strings[1]=User->JobTitle;
    SubItems->Strings[2]=User->FullName;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::CheckBoxActiveClick(TObject *Sender)
{
    TMPUser->Active=CheckBoxActive->Checked;
}
//---------------------------------------------------------------------------

