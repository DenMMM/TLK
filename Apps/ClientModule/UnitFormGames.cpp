//---------------------------------------------------------------------------
#include <vcl.h>
#include <cwchar>
#pragma hdrstop

#include "UnitFormGames.h"
#include "UnitFormMain.h"
#include "UnitTariffs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormGames *FormGames;
//---------------------------------------------------------------------------
__fastcall TFormGames::TFormGames(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TFormGames::ShowGames(unsigned Pages_)
{
    // ����������� ������ �������� �� ������ ������
    Pages_&=mgpAll;

    // ���� �� ������ ������ �������� ������ ����
    if ( Pages_==mgpNone )
    {
        Tag=true; Close();
    } else
    {
        UpdateGames(Pages_);
        Tag=false; Show();
    }
}
//---------------------------------------------------------------------------
void TFormGames::UpdateGames(unsigned Pages_)
{
    // ������� ������ �������� � ������
    TreeViewGames->Items->Clear();
    ImageListGamesIcons->Clear();
    Games.Clear();

    // �������� � ������� �����
    if ( Games.Load() )
    {
		if ( Pages_&mgp1 ) AddGamesIndexToTree(Games,0,nullptr,ImageListGamesIcons);
		if ( Pages_&mgp2 ) AddGamesIndexToTree(Games,1,nullptr,ImageListGamesIcons);
		if ( Pages_&mgp3 ) AddGamesIndexToTree(Games,2,nullptr,ImageListGamesIcons);
		if ( Pages_&mgp4 ) AddGamesIndexToTree(Games,3,nullptr,ImageListGamesIcons);
		if ( Pages_&mgp5 ) AddGamesIndexToTree(Games,4,nullptr,ImageListGamesIcons);
		if ( Pages_&mgp6 ) AddGamesIndexToTree(Games,5,nullptr,ImageListGamesIcons);
		if ( Pages_&mgp7 ) AddGamesIndexToTree(Games,6,nullptr,ImageListGamesIcons);
		if ( Pages_&mgp8 ) AddGamesIndexToTree(Games,7,nullptr,ImageListGamesIcons);
	}
}
//---------------------------------------------------------------------------
void TFormGames::AddGamesToTree(const MGames &Games_,
	TTreeNode *TreeNode_, TImageList *ImageList_)
{
	TTreeNode *NewTreeNode;
	HICON icon;
	wchar_t icon_file[MAX_PrgIconLength+1];    /// ����: [max(MAX_PrgCmdLength,MAX_PrgIconLength)+1]
	wchar_t *pos1, *pos2;

	for ( const auto &Game: Games_ )
	{
		NewTreeNode=TreeViewGames->Items->AddChild(TreeNode_,Game.Name.c_str());

		// �������� ������
		wcscpy(
			icon_file,
			wcslen(Game.Icon.c_str())?
			Game.Icon.c_str():
			Game.Command.c_str());
		pos1=wcschr(icon_file, L'\"');
		if ( pos1==nullptr ) icon=::ExtractIcon(nullptr, icon_file, 0);
		else if ( (pos2=wcschr(pos1+1, L'\"'))==nullptr ) icon=nullptr;
		else
		{
			*pos2=L'\0';
			icon=::ExtractIcon(nullptr, pos1+1, 0);
		}

		// ������� �� � ������ �� ������� ���
		if ( (icon==nullptr)||(((int)icon)==1) )
		{
			NewTreeNode->ImageIndex=-1;
			NewTreeNode->SelectedIndex=-1;
		} else
		{
			NewTreeNode->ImageIndex=
				::ImageList_ReplaceIcon((HIMAGELIST)ImageList_->Handle,-1,icon);
			NewTreeNode->SelectedIndex=
				NewTreeNode->ImageIndex;
		}

		// ��������� ������ ��� �������
		NewTreeNode->Data=
			const_cast<wchar_t*>(Game.Command.c_str());     /// ��������� ???
		// ��������� ������
		AddGamesToTree(*Game.upSubGames,NewTreeNode,ImageList_);
	}
}
//---------------------------------------------------------------------------
void TFormGames::AddGamesIndexToTree(const MGames &Games_, size_t Index_,
	TTreeNode *TreeNode_, TImageList *ImageList_)
{
	const MGamesItem *Item=nullptr;
	// ���������� ����� ����� MGames �� ������ �������
	try  { Item=&Games_.GetItem(Index_); }
	catch (std::out_of_range &e) { return; }
	// ������� ���������� ��� SubGames � ������ ��������
	if ( !Item->upSubGames ) return;
	AddGamesToTree(*Item->upSubGames,TreeNode_,ImageList_);
}
//---------------------------------------------------------------------------
void __fastcall TFormGames::FormCreate(TObject *Sender)
{
	Tag=false;
	this->DoubleBuffered=true;
	TreeViewGames->DoubleBuffered=true;
	TreeViewGames->Font->Color=(TColor)0x02D2C66F;

	// ����������� ���� ��� ������
	UnicodeString GmsFile=
		ExtractFilePath(Application->ExeName)+L"\\TLK.GMS";
	Games.SetDefaultFile(GmsFile.c_str(),ENC_Code);
}
//---------------------------------------------------------------------------
void __fastcall TFormGames::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	CanClose=Tag;
}
//---------------------------------------------------------------------------
void __fastcall TFormGames::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // ������� ������ �������� � ������
    TreeViewGames->Items->Clear();
    ImageListGamesIcons->Clear();
    Games.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormGames::MQueryEndSession(TMessage &Msg)
{
    Msg.Result=TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TFormGames::TreeViewGamesDblClick(TObject *Sender)
{
    TTreeNode *TreeNode=TreeViewGames->Selected;
    if ( (TreeNode==nullptr)||TreeNode->HasChildren ) return;

	WideString path, command(reinterpret_cast<wchar_t*>(TreeNode->Data));
	int pos;

	// �������� �� ��������� ������ ����
	if ( (pos=command.Pos(L"\""))==0 ) return;
	path=command.SubString(pos+1,command.Length()-pos);
	if ( (pos=path.Pos(L"\""))==0 ) return;
	path.Delete(pos,path.Length()-pos+1);
	path=ExtractFilePath(path);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si,0,sizeof(STARTUPINFO));
	si.cb=sizeof(STARTUPINFO);

	// �������� ����������
	if ( ::CreateProcess(
		nullptr,
		command.c_bstr(),
		nullptr, nullptr, false,
		CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,nullptr,
		path.c_bstr(),
		&si, &pi) )
	{
		::CloseHandle(pi.hThread);
		::CloseHandle(pi.hProcess);
	}
}
//---------------------------------------------------------------------------

