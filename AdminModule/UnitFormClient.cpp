//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "UnitFormClient.h"
#include "UnitFormMain.h"
#include "UnitComputers.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormClient *FormClient;
//---------------------------------------------------------------------------
__fastcall TFormClient::TFormClient(TComponent* Owner)
    : TForm(Owner)
{
    SendComputers=NULL;
    Games=NULL;
    Options=NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::FormShow(TObject *Sender)
{
    PageControl->ActivePage=TabSheetGames;
    PageControlChange(NULL);

    Games=new MGames;
    Options=new MClOptions;
    // Заполняем список компьютеров
    for ( MComputer *Computer=(MComputer*)Computers->First; Computer;
        Computer=(MComputer*)Computer->Next )
    {
        TListItem *Item;
        Item=ListViewComputers->Items->Add();
        Item->Data=(void*)Computer;
        // Цвет группы и номер компьютера
        Item->SubItems->Add(IntToStr(Computer->Number));
        Item->SubItems->Add("");
        Item->SubItemImages[0]=Computer->GroupColor;
    }
    //
    EditName->MaxLength=MAX_PrgNameLength;
    EditCmd->MaxLength=MAX_PrgCmdLength;
    EditIcon->MaxLength=MAX_PrgIconLength;
    ButtonCancel->Visible=false;
    ButtonAddChild->Enabled=false;
    SetSend(false);
//    SetEdit(false,false);
    //
    CreateGamesTree(Games);

    ComboBoxToEndTime->Items->Add("Не показывать");
    ComboBoxRebootWait->Items->Add("Не перезагружать");
    ComboBoxAutoLockTime->Items->Add("Не блокировать");
    for ( int i=1; i<=30; i++ )
    {
        ComboBoxToEndTime->Items->Add(IntToStr(i));
        ComboBoxMessageTime->Items->Add(IntToStr(i));
        ComboBoxRebootWait->Items->Add(IntToStr(i));
        ComboBoxAutoLockTime->Items->Add(IntToStr(i));
    }
    ComboBoxToEndTime->ItemIndex=2;
    ComboBoxMessageTime->ItemIndex=10-1;
    ComboBoxRebootWait->ItemIndex=20;
    ComboBoxAutoLockTime->ItemIndex=15;
    ComboBoxToEndTimeClick(NULL);

    ActiveControl=ListViewComputers;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ButtonCancelClick(NULL);
    ListViewComputers->Items->Clear();
    TreeViewGames->Items->Clear();
    EditName->Text=""; EditName->ClearUndo();
    EditCmd->Text=""; EditCmd->ClearUndo();
    EditIcon->Text=""; EditIcon->ClearUndo();
    ComboBoxToEndTime->Clear();
    ComboBoxMessageTime->Clear();
    ComboBoxRebootWait->Clear();
    ComboBoxAutoLockTime->Clear();
    Games->First=Games->Last=NULL; Games->Count=0;
    delete Games;
    delete Options;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::PageControlChange(TObject *Sender)
{
    if ( PageControl->ActivePage==TabSheetGames )
    {
        OpenDialog->Filter=
            TEXT("Список игр TLK (*.gms)|*.gms|")
            TEXT("Все файлы (*.*)|*.*");
        OpenDialog->DefaultExt="GMS";
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        OpenDialog->Filter=
            TEXT("Настройки для клиента (*.cfg)|*.cfg|")
            TEXT("Все файлы (*.*)|*.*");
        OpenDialog->DefaultExt="CFG";
    }
    SaveDialog->Filter=OpenDialog->Filter;
    SaveDialog->DefaultExt=OpenDialog->DefaultExt;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ButtonOperationsClick(TObject *Sender)
{
    TPoint local_button_coord;
    local_button_coord.x=ButtonOperations->Left+4;
    local_button_coord.y=ButtonOperations->Top+
        ButtonOperations->Height-4;
    local_button_coord=ClientToScreen(local_button_coord);
    PopupMenuOperations->Popup(local_button_coord.x,local_button_coord.y);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::NOpenClick(TObject *Sender)
{
    if ( !OpenDialog->Execute() ) return;
    if ( PageControl->ActivePage==TabSheetGames )
    {
        Games->First=Games->Last=NULL; Games->Count=0;
        if ( Games->LoadFrom(OpenDialog->FileName.c_str(),0) ) CreateGamesTree(Games);
        else FormMain->MessageBoxError(1,7);
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        if ( Options->LoadFrom(OpenDialog->FileName.c_str(),0) ) OptionsToShell(Options);
        else FormMain->MessageBoxError(1,7);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::NSaveClick(TObject *Sender)
{
    if ( !SaveDialog->Execute() ) return;
    if ( PageControl->ActivePage==TabSheetGames )
    {
        Games->First=Games->Last=NULL; Games->Count=0;
        CreateGamesFromTree(Games);
        if ( !Games->SaveTo(SaveDialog->FileName.c_str(),0) )
            FormMain->MessageBoxError(1,7);
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        ShellToOptions(Options);
        if ( !Options->SaveTo(SaveDialog->FileName.c_str(),0) )
            FormMain->MessageBoxError(1,7);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::NSendClick(TObject *Sender)
{
    if ( ListViewComputers->SelCount<1 ) return;

    SendComputers=new MComputer*[ListViewComputers->SelCount+1];
    // Заполняем список указателей на компьютеры, куда требуется отправить данные
    TItemStates is=TItemStates()<<isSelected;
    MComputer **pComputer=SendComputers;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        *(pComputer++)=(MComputer*)Item->Data;
        Item->SubItemImages[1]=13;
    }
    *pComputer=NULL;
    //
    Send.SetShell(Handle,WM_USER+0);
    if ( PageControl->ActivePage==TabSheetGames )
    {
        //
        Games->First=Games->Last=NULL; Games->Count=0;
        CreateGamesFromTree(Games);
        //
        SetSend(true);
        // Запускаем отправку данных
        Send.Send(SendComputers,Games,NULL);
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        MClOptions Options;
        Options.ToEndTime=ComboBoxToEndTime->ItemIndex;
        Options.MessageTime=ComboBoxMessageTime->ItemIndex+1;
        Options.RebootWait=ComboBoxRebootWait->ItemIndex;
        Options.AutoLockTime=ComboBoxAutoLockTime->ItemIndex;
        //
        SetSend(true);
        // Запускаем отправку данных
        Send.Send(SendComputers,NULL,&Options);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::NLoadClick(TObject *Sender)
{
    if ( ListViewComputers->SelCount!=1 ) return;
    Send.SetShell(Handle,WM_USER+10);
    if ( PageControl->ActivePage==TabSheetGames )
    {
        SetSend(true);
        Send.Get((MComputer*)ListViewComputers->Selected->Data,Games,NULL);
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        SetSend(true);
        Send.Get((MComputer*)ListViewComputers->Selected->Data,NULL,Options);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::TreeViewGamesChange(TObject *Sender,
      TTreeNode *Node)
{
    TTreeNode *Select;
    MGame *Game;
    if ( (Select=TreeViewGames->Selected)==NULL ) { SetEdit(false,false); return; }
    Game=(MGame*)Select->Data;
    EditName->Text=Game->Name;
    EditCmd->Text=Game->Command;
    EditIcon->Text=Game->Icon;
    // Если элемент самого верхнего уровня или корень ветви, запрещаем делать его корнем
    ButtonAddChild->Enabled=!((Select->Level==0)||Select->HasChildren);
    SetEdit(true,(Select->Level!=0)&&(!Select->HasChildren));
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::TreeViewGamesDeletion(TObject *Sender,
      TTreeNode *Node)
{
    MGame *Game=(MGame*)Node->Data;
    Game->SubGames->First=Game->SubGames->Last=NULL;
    Game->SubGames->Count=0;
    delete Game;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::EditNameChange(TObject *Sender)
{
    if ( (!EditName->Modified)||(TreeViewGames->Selected==NULL) ) return;
    ((MGame*)TreeViewGames->Selected->Data)->SetName(EditName->Text.Trim().c_str());
    SetTreeViewGamesLine(TreeViewGames->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::EditCmdChange(TObject *Sender)
{
    if ( (!EditCmd->Modified)||(TreeViewGames->Selected==NULL) ) return;
    ((MGame*)TreeViewGames->Selected->Data)->SetCommand(EditCmd->Text.Trim().c_str());
    SetTreeViewGamesLine(TreeViewGames->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::EditIconChange(TObject *Sender)
{
    if ( (!EditIcon->Modified)||(TreeViewGames->Selected==NULL) ) return;
    ((MGame*)TreeViewGames->Selected->Data)->SetIcon(EditIcon->Text.Trim().c_str());
    SetTreeViewGamesLine(TreeViewGames->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ButtonAddClick(TObject *Sender)
{
    TTreeNode *Select, *New;
    if ( (Select=TreeViewGames->Selected)==NULL ) return;
    if ( Select->Level==0 ) { ButtonAddChildClick(NULL); return; }
    New=TreeViewGames->Items->Add(Select,"");
    New->Data=(void*)new MGame;
    SetTreeViewGamesLine(New);
    TreeViewGames->Selected=New;
    if ( EditName->Enabled ) EditName->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ButtonDelClick(TObject *Sender)
{
    TTreeNode *Select, *Parent, *Prev;
    MGame *Game;
    if ( ((Select=TreeViewGames->Selected)==NULL)||(Select->Level==0) ) return;
    if ( Select->HasChildren&&
        ::MessageBox(Application->Handle,
        TEXT("Удалить все дерево с подчиненными элементами ?\n"),
        "TLK - вопрос",MB_ICONQUESTION|MB_YESNO|MB_APPLMODAL)!=IDYES ) return;
    // Запоминаем родительский и предыдущий элементы
    Parent=Select->Parent;
    Prev=Parent->GetLastChild()==Select? Select->GetPrev(): NULL;
    Select->Delete();
    // Корректриуем вид строки, если элемент больше не корень дерева
    if ( Parent&&(!Parent->HasChildren) ) SetTreeViewGamesLine(Parent);
    // Назначем активной строку, предшествовавшую удаленной, если она была нижней в ветви
    if ( Prev ) TreeViewGames->Selected=Prev;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ButtonAddChildClick(TObject *Sender)
{
    TTreeNode *Select, *New;
    if ( (Select=TreeViewGames->Selected)==NULL ) return;
    New=TreeViewGames->Items->AddChild(Select,"");
    New->Data=(void*)new MGame;
    SetTreeViewGamesLine(Select);
    SetTreeViewGamesLine(New);
    Select->Expand(true);
    TreeViewGames->Selected=New;
    if ( EditName->Enabled ) EditName->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::BitBtnUpClick(TObject *Sender)
{
    TTreeNode *Select, *Prev;
    if ( ((Select=TreeViewGames->Selected)==NULL)||
        (Select->Level==0) ) return;

    if ( (Prev=Select->getPrevSibling())!=NULL )
        Select->MoveTo(Prev,Prev->HasChildren?naAddChild:naInsert);
    else
    {
        Prev=Select->Parent;
        if ( Prev->Level ) Select->MoveTo(Prev,naInsert);
        else if ( (Prev=Prev->getPrevSibling())!=NULL )
            Select->MoveTo(Prev,naAddChild);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::BitBtnDownClick(TObject *Sender)
{
    TTreeNode *Select, *Next;
    if ( ((Select=TreeViewGames->Selected)==NULL)||
        (Select->Level==0) ) return;

    if ( (Next=Select->getNextSibling())!=NULL )
    {
        if ( Next->HasChildren ) Select->MoveTo(Next,naAddChildFirst);
        else Next->MoveTo(Select,naInsert);
    } else
    {
        Next=Select->Parent;
        if ( Next->Level )
        {
            Select->MoveTo(Next,naInsert);
            Next->MoveTo(Select,naInsert);
        } else if ( (Next=Next->getNextSibling())!=NULL )
            Select->MoveTo(Next,naAddChildFirst);
    }
}
//---------------------------------------------------------------------------
void TFormClient::SetTreeViewGamesLine(TTreeNode *Node_)
{
    char line[MAX_PrgNameLength+MAX_PrgCmdLength+MAX_PrgIconLength+10+1];
    MGame *Game=(MGame*)Node_->Data;

    strcpy(line,Game->Name);
    if ( Node_->Parent&&(!Node_->HasChildren) )
    {
        strcat(line,"   { ");
        strcat(line,Game->Command);
        strcat(line," , ");
        strcat(line,Game->Icon);
        strcat(line," }");
    }
    Node_->Text=line;
}
//---------------------------------------------------------------------------
void TFormClient::AddGamesToTree(TTreeNode *Node_, MGames *Games_)
{
    MGame *Game;
    static TTreeNode *Node;

    for ( MGame *Game=(MGame*)Games_->First; Game;
        Game=(MGame*)Game->Next )
    {
        Node=TreeViewGames->Items->AddChild(Node_,"");
        Node->Data=(void*)Game;
        SetTreeViewGamesLine(Node);
        if ( Game->SubGames->Count ) AddGamesToTree(Node,Game->SubGames);
    }
    Games_->First=Games_->Last=NULL; Games_->Count=0;
}
//---------------------------------------------------------------------------
void TFormClient::CreateGamesTree(MGames *Games_)
{
    TTreeNode *Node;
    MGame *Game;
    char str[5+1];

    TreeViewGames->Items->Clear();
    for ( int i=0; i<8; i++ )
    {
        Node=TreeViewGames->Items->Add(NULL,"");
        if ( (Game=(MGame*)Games_->Item(i))==NULL )
        {
            Game=new MGame;
            sprintf(str,"Page%i",i+1);
            Game->SetName(str);
        }
        Node->Data=(void*)Game;
        SetTreeViewGamesLine(Node);
        AddGamesToTree(Node,Game->SubGames);
    }
    Games_->First=Games_->Last=NULL; Games_->Count=0;
}
//---------------------------------------------------------------------------
void TFormClient::AddGamesFromTree(MGames *Games_, TTreeNode *Node_)
{
    static TTreeNode *Node;

    for ( int i=0; i<Node_->Count; i++ )
    {
        Node=Node_->Item[i];
        Games_->attach((MListItem*)Node->Data);
        ((MGame*)Games_->Last)->SubGames->First=
            ((MGame*)Games_->Last)->SubGames->Last=NULL;
        ((MGame*)Games_->Last)->SubGames->Count=0;
        if ( Node->HasChildren )
            AddGamesFromTree(((MGame*)Games_->Last)->SubGames,Node);
    }
}
//---------------------------------------------------------------------------
void TFormClient::CreateGamesFromTree(MGames *Games_)
{
    TTreeNodes *Items=TreeViewGames->Items;
    static TTreeNode *Node;

    Games_->First=Games_->Last=NULL; Games_->Count=0;
    for ( int i=0; i<Items->Count; i++ )
    {
        Node=Items->Item[i]; if ( Node->Level ) continue;
        Games_->attach((MListItem*)Node->Data);
        ((MGame*)Games_->Last)->SubGames->First=
            ((MGame*)Games_->Last)->SubGames->Last=NULL;
        ((MGame*)Games_->Last)->SubGames->Count=0;
        if ( Node->HasChildren )
            AddGamesFromTree(((MGame*)Games_->Last)->SubGames,Node);
    }
}
//---------------------------------------------------------------------------
void TFormClient::OptionsToShell(MClOptions *Options_)
{
    ComboBoxToEndTime->ItemIndex=Options_->ToEndTime;
    ComboBoxMessageTime->ItemIndex=Options_->MessageTime-1;
    ComboBoxRebootWait->ItemIndex=Options_->RebootWait;
    ComboBoxAutoLockTime->ItemIndex=Options_->AutoLockTime;
    ComboBoxToEndTimeClick(NULL);
}
//---------------------------------------------------------------------------
void TFormClient::ShellToOptions(MClOptions *Options_)
{
    Options_->ToEndTime=ComboBoxToEndTime->ItemIndex;
    Options_->MessageTime=ComboBoxMessageTime->ItemIndex+1;
    Options_->RebootWait=ComboBoxRebootWait->ItemIndex;
    Options_->AutoLockTime=ComboBoxAutoLockTime->ItemIndex;
}
//---------------------------------------------------------------------------
void TFormClient::SetEdit(bool Edit_, bool Full_)
{
    TColor Color=Edit_? clWindow: clBtnFace;

    LabelName->Enabled=Edit_;
    EditName->Enabled=Edit_;
    EditName->Color=Color;
    if ( !Full_ ) { Edit_=false; Color=clBtnFace; }
    LabelCmd->Enabled=Edit_;
    EditCmd->Enabled=Edit_;
    EditCmd->Color=Color;
    LabelIcon->Enabled=Edit_;
    EditIcon->Enabled=Edit_;
    EditIcon->Color=Color;
}
//---------------------------------------------------------------------------
void TFormClient::SetSend(bool Send_)
{
    ButtonCancel->Visible=Send_;
    PanelProcess->Visible=Send_; if ( !Send_ ) PanelProcess->Caption="";
    PageControl->Enabled=!Send_;
    if ( !Send_ ) TreeViewGamesChange(TreeViewGames,TreeViewGames->Selected);
    ButtonOperations->Enabled=!Send_;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::Dispatch(void *Message)
{
    TListItem *Item;
    switch(((TMessage*)Message)->Msg)
    {
        case WM_USER+mseConnecting+00:
        case WM_USER+mseConnecting+10:
            PanelProcess->Caption="Соединение";
            break;

        case WM_USER+mseSending+00:
            PanelProcess->Caption="Отправка";
            break;
        case WM_USER+mseReceiving+10:
            PanelProcess->Caption="Загрузка";
            break;

        case WM_USER+mseDisconnecting+00:
        case WM_USER+mseDisconnecting+10:
            PanelProcess->Caption="Разъединение";
            break;

        case WM_USER+mseExecute+00:
            Item=ListViewComputers->FindData(0,
                (void*)(((TMessage*)Message)->WParam),true,false);
            if ( Item ) Item->SubItemImages[1]=-1;
            break;
        case WM_USER+mseExecute+10:
            Item=ListViewComputers->FindData(0,
                (void*)(((TMessage*)Message)->WParam),true,false);
            if ( Item ) Item->SubItemImages[1]=-1;
            if ( PageControl->ActivePage==TabSheetGames )
                CreateGamesTree(Games);
            else if ( PageControl->ActivePage==TabSheetOptions )
                OptionsToShell(Options);
            break;

        case WM_USER+mseNotConnect+00:
        case WM_USER+mseNotConnect+10:

        case WM_USER+msePrclError+00:
        case WM_USER+msePrclError+10:
            Item=ListViewComputers->FindData(0,
                (void*)(((TMessage*)Message)->WParam),true,false);
            if ( Item ) Item->SubItemImages[1]=14;
            PanelProcess->Caption="Ошибка";
            break;

        case WM_USER+mseFreeParam+00:
            delete[] SendComputers; SendComputers=NULL;
        case WM_USER+mseFreeParam+10:
            SetSend(false);
            break;

        default: break;
    }
    TForm::Dispatch(Message);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ButtonCancelClick(TObject *Sender)
{
    Send.Stop();
    for ( int i=ListViewComputers->Items->Count-1; i>=0; i-- )
        ListViewComputers->Items->Item[i]->SubItemImages[1]=-1;
    delete[] SendComputers; SendComputers=NULL;
    SetSend(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ComboBoxToEndTimeClick(TObject *Sender)
{
    ComboBoxMessageTime->Enabled=ComboBoxToEndTime->ItemIndex;
    ComboBoxMessageTime->Color=ComboBoxToEndTime->ItemIndex?clWindow:clBtnFace;
}
//---------------------------------------------------------------------------

