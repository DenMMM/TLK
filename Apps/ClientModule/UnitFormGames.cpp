//---------------------------------------------------------------------------
#include <vcl.h>
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
    MyClose=true;
}
//---------------------------------------------------------------------------
void TFormGames::ShowGames(unsigned Pages_)
{
    // Отфильтруем список программ от других флагов
    Pages_&=mgpAll;

    // Если не заданы группы программ скроем окно
    if ( Pages_==mgpNone )
    {
        MyClose=true; Close();
    } else
    {
        UpdateGames(Pages_);
        MyClose=false; Show();
    }
}
//---------------------------------------------------------------------------
void TFormGames::UpdateGames(unsigned Pages_)
{
    // Очистим список программ и иконок
    TreeViewGames->Items->Clear();
    ImageListGamesIcons->Clear();
    Games.Clear();

    // Загрузим и добавим новые
    if ( State->GetGames(&Games) )
    {
        MGame *Game;
        
        if ( (Pages_&mgp1)&&((Game=(MGame*)Games.Item(0))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp2)&&((Game=(MGame*)Games.Item(1))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp3)&&((Game=(MGame*)Games.Item(2))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp4)&&((Game=(MGame*)Games.Item(3))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp5)&&((Game=(MGame*)Games.Item(4))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp6)&&((Game=(MGame*)Games.Item(5))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp7)&&((Game=(MGame*)Games.Item(6))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
        if ( (Pages_&mgp8)&&((Game=(MGame*)Games.Item(7))!=NULL) )
            AddGamesToTree(Game->SubGames,NULL,ImageListGamesIcons);
    }
}
//---------------------------------------------------------------------------
void TFormGames::AddGamesToTree(MGames *Games_, TTreeNode *TreeNode_, TImageList *ImageList_)
{
    TTreeNode *NewTreeNode;
    HICON icon;
    char icon_file[MAX_PrgIconLength+1];    /// надо: [max(MAX_PrgCmdLength,MAX_PrgIconLength)+1]
    char *pos1, *pos2;

    if ( Games_==NULL ) return;

    for ( MGame *Game=(MGame*)Games_->gFirst(); Game;
        Game=(MGame*)Game->gNext() )
    {
        NewTreeNode=TreeViewGames->Items->AddChild(TreeNode_,Game->Name);

        // Извлечем иконку
        strcpy(icon_file,
            strlen(Game->Icon)? Game->Icon: Game->Command);
        pos1=strchr(icon_file,'\"');
        if ( pos1==NULL ) icon=::ExtractIcon(NULL,icon_file,0);
        else if ( (pos2=strchr(pos1+1,'\"'))==NULL ) icon=NULL;
        else
        {
            *pos2=0;
            icon=::ExtractIcon(NULL,pos1+1,0);
        }

        // Добавим ее в дерево со списком игр
        if ( (icon==NULL)||(((int)icon)==1) )
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

        // Командная строка для запуска
        NewTreeNode->Data=Game->Command;
        // Подуровни дерева
        AddGamesToTree(Game->SubGames,NewTreeNode,ImageList_);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGames::FormCreate(TObject *Sender)
{
    this->DoubleBuffered=true;
    TreeViewGames->DoubleBuffered=true;
    TreeViewGames->Font->Color=(TColor)0x02D2C66F;
}
//---------------------------------------------------------------------------
void __fastcall TFormGames::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose=MyClose;
}
//---------------------------------------------------------------------------
void __fastcall TFormGames::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // Очистим список программ и иконок
    TreeViewGames->Items->Clear();
    ImageListGamesIcons->Clear();
    Games.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormGames::TreeViewGamesDblClick(TObject *Sender)
{
    TTreeNode *TreeNode=TreeViewGames->Selected;
    if ( (TreeNode==NULL)||TreeNode->HasChildren ) return;

/*
    char buff[MAX_PrgCmdLength+1];
    char *cmd=(char*)TreeNode->Data;

    // Проверим спец-команды shell'а
    if ( strncmp(cmd,CMD_VPlay,strlen(CMD_VPlay))==0 )
    {
        Munique_ptr <TFormVPlay> form;

        try
        {
    /// От интеграции плейера в оболочку пока отказался ради стабильности
            // Открываем плеер
            form(new TFormVPlay(0));        /// new TFormVPlay(this) ?
            form.get()->ShowModal();
        }
        catch (Exception &ex)
        {
            Application->ShowException(&ex);
        }
        return;
    }
*/

    // Запустим обычное приложение
    AnsiString path, command=(char*)TreeNode->Data;
    int pos;
    //
    STARTUPINFO si;
    memset(&si,0,sizeof(STARTUPINFO));
    si.cb=sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;
    // Запускаем игру
    if ( (pos=command.AnsiPos("\""))==NULL ) return;
    path=command.SubString(pos+1,command.Length()-pos);
    if ( (pos=path.AnsiPos("\""))==NULL ) return;
    path.Delete(pos,path.Length()-pos+1);
    path=ExtractFilePath(path);
    ::CreateProcess(NULL,command.c_str(),NULL,NULL,false,
        CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,NULL,
        path.c_str(),&si,&pi);
}
//---------------------------------------------------------------------------
void __fastcall TFormGames::TreeViewGamesKeyPress(TObject *Sender,
      char &Key)
{
//    if ( Key==VK_RETURN ) TreeViewGamesDblClick(NULL);
}
//---------------------------------------------------------------------------

