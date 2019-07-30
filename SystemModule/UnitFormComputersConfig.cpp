//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormComputersConfig.h"
#include "UnitFormMain.h"
#include "UnitOptionsLoadSave.h"
#include "UnitLogs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormComputersConfig *FormComputersConfig;
//---------------------------------------------------------------------------
__fastcall TFormComputersConfig::TFormComputersConfig(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::FormShow(TObject *Sender)
{
    TMPGame=new MGame;

    // Заносим компьютеры в список для настройки
    for ( MComputer *Computer=(MComputer*)Computers->FirstItem; Computer;
        Computer=(MComputer*)Computer->NextItem )
    {
        TListItem *NewItem;
        NewItem=ListViewComputers->Items->Add();
        NewItem->Data=(void*)Computer;
        SetListViewComputersLine(NewItem);
    }
    //
    MGames *Games=new MGames;
    for ( int i=1; i<=5; i++ )
    {
        MGame *NewGame;
        NewGame=(MGame*)Games->Add();
        NewGame->Name="Page"+IntToStr(i);
    }
    AddGamesToTree(Games,NULL);
    delete Games;

    ActiveControl=ListViewComputers;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::FormHide(TObject *Sender)
{
    ListViewComputers->Items->Clear();
    TreeViewGames->Items->Clear();
    EditGameName->Text="";
    EditGameCommandLine->Text="";
    EditGameIconFile->Text="";
    delete TMPGame;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::ListViewComputersInsert(
      TObject *Sender, TListItem *Item)
{
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::ButtonSetComputerParamClick(
      TObject *Sender)
{
    if ( ListViewComputers->SelCount<1 ) return;

    TTabSheet *ActivePage=PageControlOptions->ActivePage;
    if ( ActivePage==TabSheetGames )
    {
        MGames *Games=new MGames;
        // Формируем данные списка игр из отредактированного пользователем дерева
        TTreeNodes *Items=TreeViewGames->Items;
        for ( int i=0; i<Items->Count; i++ )
        {
            TTreeNode *Item;
            Item=Items->Item[i];
            if ( Item->Parent ) continue;
            MGame *NewGame;
            NewGame=(MGame*)Games->Add();
            *NewGame=*((MGame*)Item->Data);
            if ( !Item->HasChildren ) continue;
            NewGame->SubGames=new MGames;
            AddGamesFromTree(NewGame->SubGames,Item);
        }
        // Сохраняем новый список игр для выбранных компьютеров
        TItemStates is=TItemStates()<<isSelected;
        for ( TListItem *Item=ListViewComputers->Selected; Item;
            Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
        {
            if ( State->CurrentAdmin ) LogCompRun((MComputer*)Item->Data,(MTariff*)7,0,0.);
            ((MComputer*)Item->Data)->SetGames(Games);
        }
        delete Games;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::ButtonGetComputerParamClick(
      TObject *Sender)
{
    if ( !ListViewComputers->ItemFocused ) return;
    MComputer *Computer=(MComputer*)ListViewComputers->ItemFocused->Data;

    TTabSheet *ActivePage=PageControlOptions->ActivePage;
    if ( ActivePage==TabSheetGames )
    {
        TreeViewGames->Items->Clear();
        MGames *Games=new MGames;
        if ( !Computer->GetGames(Games) ) Games->Clear();
        while(Games->CountItems<5)
        {
            MGame *NewGame;
            NewGame=(MGame*)Games->Add(); NewGame->Name="Page"+IntToStr(Games->CountItems);
        }
        AddGamesToTree(Games,NULL);
        delete Games;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::TreeViewGamesChange(TObject *Sender,
      TTreeNode *Node)
{
    TTreeNode *TreeNode;
    if ( (TreeNode=TreeViewGames->Selected)==NULL ) return;
    *TMPGame=*((MGame*)TreeNode->Data);
    EditGameName->Text=TMPGame->Name;
    EditGameCommandLine->Text=TMPGame->CommandLine;
    EditGameIconFile->Text=TMPGame->IconFile;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::TreeViewGamesDeletion(
      TObject *Sender, TTreeNode *Node)
{
    delete (MGame*)Node->Data;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::EditGameNameExit(TObject *Sender)
{
    TMPGame->Name=EditGameName->Text.Trim();
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::EditGameCommandLineExit(
      TObject *Sender)
{
    TMPGame->CommandLine=EditGameCommandLine->Text.Trim();
    EditGameCommandLine->Text=TMPGame->CommandLine;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::EditGameIconFileExit(TObject *Sender)
{
    TMPGame->IconFile=EditGameIconFile->Text.Trim();
    EditGameIconFile->Text=TMPGame->IconFile;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::ButtonBrowseGameFileClick(
      TObject *Sender)
{
    OpenDialog->Title="Файл для запуска ("+TMPGame->Name+")";
    OpenDialog->Filter="Исполнимые файлы|*.EXE";
    if ( !OpenDialog->Execute() ) return;
    EditGameCommandLine->Text="\""+OpenDialog->FileName+"\"";
    EditGameCommandLine->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::ButtonBrowseGameIconClick(
      TObject *Sender)
{
    OpenDialog->Title="Файл с иконкой ("+TMPGame->Name+")";
    OpenDialog->Filter="Исполнимые файлы|*.EXE|Файлы иконок|*.ICO";
    if ( !OpenDialog->Execute() ) return;
    EditGameIconFile->Text=OpenDialog->FileName;
    EditGameIconFile->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::ButtonAddGameClick(TObject *Sender)
{
    TTreeNode *TreeNode, *NewTreeNode;
    if ( (TreeNode=TreeViewGames->Selected)==NULL ) return;
    NewTreeNode=TreeViewGames->Items->AddChild(TreeNode,"");
    NewTreeNode->Data=(void*)new MGame;
    *((MGame*)NewTreeNode->Data)=*TMPGame;
    SetTreeViewGamesLine(NewTreeNode);
    SetTreeViewGamesLine(TreeNode);
    TreeNode->Expand(true);
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::ButtonDeleteGameClick(
      TObject *Sender)
{
    TTreeNode *TreeNode, *Parent;
    if ( (TreeNode=TreeViewGames->Selected)==NULL ) return;
    if ( (Parent=TreeNode->Parent)==NULL ) return;
    TreeNode->Delete();
    if ( !Parent->HasChildren ) SetTreeViewGamesLine(Parent);
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::ButtonChangeGameClick(
      TObject *Sender)
{
    TTreeNode *TreeNode;
    if ( (TreeNode=TreeViewGames->Selected)==NULL ) return;
    *((MGame*)TreeNode->Data)=*TMPGame;
    SetTreeViewGamesLine(TreeNode);
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::SetListViewComputersLine(TListItem *Item_)
{
    Item_->SubItems->Strings[0]=IntToStr(((MComputer*)(Item_->Data))->Number);
    Item_->SubItemImages[0]=((MComputer*)(Item_->Data))->GroupColor;

    MComputer *Computer=(MComputer*)Item_->Data;
    unsigned int NetSync=Computer->NetSync|Computer->NotSync;
    int icon_number;
    if ( NetSync&mnsConfig ) icon_number=13;
    else if ( NetSync&mnsGames ) icon_number=14;
    else if ( Computer->NetPresent ) icon_number=-1;
    else icon_number=15;
    Item_->SubItemImages[1]=icon_number;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::SetTreeViewGamesLine(TTreeNode *TreeNode_)
{
    MGame *Game=(MGame*)TreeNode_->Data;
    AnsiString line=Game->Name;
    if ( (!TreeNode_->HasChildren)&&TreeNode_->Parent )
        line+="    { "+Game->CommandLine+" } { "+Game->IconFile+" }";
    TreeNode_->Text=line;
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::AddGamesToTree(MGames *Games_, TTreeNode *TreeNode_)
{
    static TTreeNode *NewTreeNode;

    for ( MGame *Game=(MGame*)Games_->FirstItem; Game; Game=(MGame*)Game->NextItem )
    {
        NewTreeNode=TreeViewGames->Items->AddChild(TreeNode_,"");
        NewTreeNode->Data=(void*)new MGame;
        ((MGame*)NewTreeNode->Data)->Name=Game->Name;
        ((MGame*)NewTreeNode->Data)->CommandLine=Game->CommandLine;
        ((MGame*)NewTreeNode->Data)->IconFile=Game->IconFile;
        SetTreeViewGamesLine(NewTreeNode);
        if ( Game->SubGames ) AddGamesToTree(Game->SubGames,NewTreeNode);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormComputersConfig::AddGamesFromTree(MGames *Games_, TTreeNode *TreeNode_)
{
    static MGame *NewGame;
    static TTreeNode *TreeNode;

    for ( int i=0; i<TreeNode_->Count; i++ )
    {
        TreeNode=TreeNode_->Item[i];
        NewGame=(MGame*)Games_->Add();
        *NewGame=*((MGame*)TreeNode->Data);
        if ( !TreeNode->HasChildren ) continue;
        NewGame->SubGames=new MGames;
        AddGamesFromTree(NewGame->SubGames,TreeNode);
    }
}
//---------------------------------------------------------------------------

