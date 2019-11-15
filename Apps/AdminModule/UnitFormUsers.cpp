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
	TmpUsers=*Users;

    // ��������� �� ������
	for ( auto &user: TmpUsers )
	{
		TListItem *item;
		item=ListViewUsers->Items->Add();
        item->Data=&user;
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
	EditLogin->Text=L"";
    EditName->Text=L"";
    // ������ �����
    TmpUsers.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ListViewUsersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
	Item->SubItems->Add(L"");
    Item->SubItems->Add(L"");
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ListViewUsersSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( dynamic_cast<TListView&>(*Sender).SelCount!=1 )
    {
        SetEdit(false);
        return;
    } else
        SetEdit(true);

	auto *user=reinterpret_cast<MUsersItem*>(
		ListViewUsers->Selected->Data);
    EditLogin->Text=user->Login.c_str();
    EditName->Text=user->Name.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::EditLoginExit(TObject *Sender)
{
    if ( ListViewUsers->Selected==nullptr ) return;

    EditLogin->Text=EditLogin->Text.Trim();
	auto *user=reinterpret_cast<MUsersItem*>(
		ListViewUsers->Selected->Data);
    user->Login=EditLogin->Text.c_str();
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
        reinterpret_cast<MUsersItem*>(item->Data)->Active=Active;
        SetListViewUsersLine(item);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::EditNameExit(TObject *Sender)
{
    if ( ListViewUsers->Selected==nullptr ) return;

    EditName->Text=EditName->Text.Trim();
	auto *user=reinterpret_cast<MUsersItem*>(
		ListViewUsers->Selected->Data);
    user->Name=EditName->Text.c_str();
    SetListViewUsersLine(ListViewUsers->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormUsers::ButtonPasswordClick(TObject *Sender)
{
	auto *user=reinterpret_cast<MUsersItem*>(
		ListViewUsers->Selected->Data);
    // �������������� ����������
    TPoint dialog_coord;
    dialog_coord.x=ButtonPassword->Left+10;
    dialog_coord.y=ButtonPassword->Top+10;
    dialog_coord=ClientToScreen(dialog_coord);
    // ��������� ���� ����� ������
    try
    {
		std::unique_ptr <TFormUserPass> form;
		form.reset(new TFormUserPass(0));
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
	MUsersItem& user=TmpUsers.Add();
	user.Login=L"NewUser";
    user.Name=L"����� ������������";
    // �������� ������ � ������ � ������� � ���
    TListItem *item=ListViewUsers->Items->Add();
    item->Data=&user;
    SetListViewUsersLine(item);
    // �������� ���������
    ListViewUsers->ItemFocused=item;
    ListViewUsers->Selected=nullptr;
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
		TmpUsers.Del(
			MUsers::const_iterator(
			reinterpret_cast<MUsersItem*>(item->Data)
			));
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
    *Users=std::move(TmpUsers);
    // ������ ID-������ ��� �����
    Users->SetUUIDs();                      /// ��������� ��� ����� �������
    // ��������� � �����
    if ( !Users->Save() )
    {
        ShellState->State|=mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,3,MB_APPLMODAL|MB_OK|MB_ICONERROR,Users->gLastErr());
        return;
    }
    // ������ � �����
    if ( !Log->AddUsers(Users.get()) )
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
    auto *user=reinterpret_cast<MUsersItem*>(Item_->Data);
//    Item_->ImageIndex=user->Active?15:16;
    Item_->ImageIndex=user->Active?-1:16;
    Item_->Caption=user->Login.c_str();
    Item_->SubItems->Strings[0]=user->Name.c_str();
}
//---------------------------------------------------------------------------

