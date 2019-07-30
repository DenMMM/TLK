//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormUsers.h"
#include "UnitFormMain.h"
#include "UnitFormUserPass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TFormUsers *FormUsers;
//---------------------------------------------------------------------------
__fastcall TFormUsers::TFormUsers(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::FormShow(TObject *Sender)
{
    // �������� ������������� � �����
    TmpUsers.Copy(Users);

    // ��������� �� ������
    for ( MUser *user=(MUser*)TmpUsers.gFirst();
        user; user=(MUser*)user->gNext() )
    {
        TListItem *item;
        item=ListViewUsers->Items->Add();
        item->Data=user;
        SetListViewUsersLine(item);
    }
    // ��������� ��� �������
    ListViewUsers->AlphaSort();

    EditLogin->MaxLength=MAX_UserLoginLen;
    EditName->MaxLength=MAX_UserNameLen;

    SetEdit(false);
    ActiveControl=ListViewUsers;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // ������ ������������ ��������
    ListViewUsers->Items->Clear();
    EditLogin->Text="";
    EditName->Text="";
    // ������ �����
    TmpUsers.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ListViewUsersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ListViewUsersSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( ((TListView*)Sender)->SelCount!=1 )
    {
        SetEdit(false);
        return;
    } else
        SetEdit(true);

    MUser *user=(MUser*)ListViewUsers->Selected->Data;
    EditLogin->Text=user->Login;
    EditName->Text=user->Name;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::EditLoginExit(TObject *Sender)
{
    if ( ListViewUsers->Selected==NULL ) return;

    EditLogin->Text=EditLogin->Text.Trim();
    MUser *user=(MUser*)ListViewUsers->Selected->Data;
    user->SetLogin(EditLogin->Text.c_str());
    SetListViewUsersLine(ListViewUsers->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::BitBtnActiveClick(TObject *Sender)
{
    bool Active=Sender==BitBtnActive? true: false;
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *item=ListViewUsers->Selected; item;
        item=ListViewUsers->GetNextItem(item,sdAll,is) )
    {
        ((MUser*)item->Data)->Active=Active;
        SetListViewUsersLine(item);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::EditNameExit(TObject *Sender)
{
    if ( ListViewUsers->Selected==NULL ) return;

    EditName->Text=EditName->Text.Trim();
    MUser *user=(MUser*)ListViewUsers->Selected->Data;
    user->SetName(EditName->Text.c_str());
    SetListViewUsersLine(ListViewUsers->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonPasswordClick(TObject *Sender)
{
    MUser *user=(MUser*)ListViewUsers->Selected->Data;
    // �������������� ����������
    TPoint dialog_coord;
    dialog_coord.x=ButtonPassword->Left+10;
    dialog_coord.y=ButtonPassword->Top+10;
    dialog_coord=ClientToScreen(dialog_coord);
    // ��������� ���� ����� ������
    try
    {
        Mptr <TFormUserPass> form;
        form(new TFormUserPass(0));
        form->Execute(user,dialog_coord.x,dialog_coord.y,false);
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonAddClick(TObject *Sender)
{
    if ( ListViewUsers->Items->Count>=MAX_Users )
    {
        ResMessageBox(Handle,0,23,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    // �������� � ����� ������ ������������
    MUser *user=(MUser*)TmpUsers.Add();
    user->SetLogin("NewUser");
    user->SetName("����� ������������");
    // �������� ������ � ������ � ������� � ���
    TListItem *item=ListViewUsers->Items->Add();
    item->Data=user;
    SetListViewUsersLine(item);
    // �������� ���������
    ListViewUsers->ItemFocused=item;
    ListViewUsers->Selected=NULL;
    ListViewUsers->Selected=item;
    ActiveControl=EditLogin;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonDelClick(TObject *Sender)
{
    // ������� ������������� �� ������
    TItemStates is=TItemStates()<<isSelected;
    TListItem *item=ListViewUsers->Selected, *next;
    while(item)
    {
        // ������� ������������ �� ������
        TmpUsers.Del((MUser*)item->Data);
        // ������� ������ �� ������
        next=ListViewUsers->GetNextItem(item,sdAll,is);
        item->Delete();
        item=next;
    }
    //
    ListViewUsers->Selected=ListViewUsers->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonSaveClick(TObject *Sender)
{
    // �������� ���������� �������������� �� ������
    Users->Move(&TmpUsers);
    // ������ ID-������ ��� �����
    Users->SetIDs();                        /// ��������� ��� ����� �������
    // ��������� � �����
    if ( !Users->Save() )
    {
        ShellState->State|=mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,3,MB_APPLMODAL|MB_OK|MB_ICONERROR,Users->gLastErr());
        return;
    }
    // ������ � �����
    if ( !Log->AddUsers(Users) )
    {
        // ��������� ���������, �� ��� ����������� �� � ���� �������� �� �����
        ShellState->State|=mssErrorLog|mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
    }
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
    MUser *user=(MUser*)Item_->Data;
//    Item_->ImageIndex=user->Active?15:16;
    Item_->ImageIndex=user->Active?-1:16;
    Item_->Caption=user->Login;
    Item_->SubItems->Strings[0]=user->Name;
}
//---------------------------------------------------------------------------

