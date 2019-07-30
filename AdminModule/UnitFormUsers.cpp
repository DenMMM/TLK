//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormUsers.h"
#include "UnitFormMain.h"
#include "UnitFormUserPassword.h"
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
    // Формируем список пользователей и попутно определяем
    // максимальное значение ID-кода, использованного с их описателями
    for ( MUser *User=(MUser*)Users->First;
        User; User=(MUser*)User->Next )
    {
        TListItem *NewItem;
        NewItem=ListViewUsers->Items->Add();
        ((MUser*)NewItem->Data)->Copy(User);
        SetListViewUsersLine(NewItem);
    }

    EditLogin->MaxLength=MAX_UserLoginLength;
    EditName->MaxLength=MAX_UserNameLength;

    SetEdit(false);
    ActiveControl=ListViewUsers;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ListViewUsers->Items->Clear();
    EditLogin->Text="";
    EditName->Text="";
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ListViewUsersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
    Item->Data=new MUser;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
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
    if ( ((TListView*)Sender)->SelCount!=1 ) { SetEdit(false); return; }
    else SetEdit(true);

    MUser *User=(MUser*)ListViewUsers->Selected->Data;
    //
    EditLogin->Text=User->Login;
    EditName->Text=User->Name;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::EditLoginExit(TObject *Sender)
{
    if ( ListViewUsers->Selected==NULL ) return;

    MUser *User=(MUser*)ListViewUsers->Selected->Data;
    User->SetLogin(EditLogin->Text.Trim().c_str());
    EditLogin->Text=User->Login;
    SetListViewUsersLine(ListViewUsers->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::BitBtnActiveClick(TObject *Sender)
{
    bool Active=Sender==BitBtnActive? true: false;
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewUsers->Selected; Item;
        Item=ListViewUsers->GetNextItem(Item,sdAll,is) )
    {
        ((MUser*)Item->Data)->Active=Active;
        SetListViewUsersLine(Item);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::EditNameExit(TObject *Sender)
{
    if ( ListViewUsers->Selected==NULL ) return;

    MUser *User=(MUser*)ListViewUsers->Selected->Data;
    User->SetName(EditName->Text.Trim().c_str());
    EditName->Text=User->Name;
    SetListViewUsersLine(ListViewUsers->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonPasswordClick(TObject *Sender)
{
    MUser *User=(MUser*)ListViewUsers->Selected->Data;

    TPoint dialog_coord;
    dialog_coord.x=ButtonPassword->Left+10;
    dialog_coord.y=ButtonPassword->Top+10;
    dialog_coord=ClientToScreen(dialog_coord);

    FormUserPassword->Execute(User,dialog_coord.x,dialog_coord.y,false);
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonAddClick(TObject *Sender)
{
    if ( ListViewUsers->Items->Count>=MAX_Users )
    {
        ::MessageBox(Handle,"Большее количество пользователей не поддерживается.",
            "Сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
        return;
    }

    TListItem *NewItem=ListViewUsers->Items->Add();
    MUser *NewUser=(MUser*)NewItem->Data;
    NewUser->SetLogin("NewUser");
    NewUser->SetName("Новый пользователь");
    SetListViewUsersLine(NewItem);
    //
    ListViewUsers->ItemFocused=NewItem;
    ListViewUsers->Selected=NULL;
    ListViewUsers->Selected=NewItem;
    ActiveControl=EditLogin;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonDelClick(TObject *Sender)
{
    // Удаляем пользователей из списка
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewUsers->Selected, *NewItem; Item; )
    {
        NewItem=ListViewUsers->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
    //
    ListViewUsers->Selected=ListViewUsers->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonSaveClick(TObject *Sender)
{
    Users->Clear();
    TListItems *Items=ListViewUsers->Items;
    for ( int i=0, j=Items->Count; i<j; i++ )
    {
        MUser *NewUser;
        NewUser=(MUser*)Users->Add();
        NewUser->Copy((MUser*)Items->Item[i]->Data);
    }
    //
    Users->SetIDs();
    // Запись в логах
    Log->AddUsers(Users);
    //
    Users->Save();
}
//---------------------------------------------------------------------------
void TFormUsers::SetEdit(bool Edit_)
{
    TColor Color=Edit_? clWindow: clBtnFace;

    LabelLogin->Enabled=Edit_;
    EditLogin->Enabled=Edit_;
    EditLogin->Color=Color;
    LabelFullName->Enabled=Edit_;
    EditName->Enabled=Edit_;
    EditName->Color=Color;
    ButtonPassword->Enabled=Edit_;
}
//---------------------------------------------------------------------------
void TFormUsers::SetListViewUsersLine(TListItem *Item_)
{
    MUser *User=(MUser*)Item_->Data;
//    Item_->ImageIndex=User->Active?15:16;
    Item_->ImageIndex=User->Active?-1:16;
    Item_->Caption=User->Login;
    Item_->SubItems->Strings[0]=User->Name;
}
//---------------------------------------------------------------------------

