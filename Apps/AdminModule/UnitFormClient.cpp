//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdexcept.h>
#pragma hdrstop

#include "UnitFormClient.h"
#include "UnitFormMain.h"
#include "UnitComputers.h"
#include "UnitGames.h"
#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormClient::TFormClient(TComponent* Owner)
    : TForm(Owner)
{
    Sending=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::FormShow(TObject *Sender)
{
    Send.NetInit(Handle,WM_USER+0,ENC_Net,Auth);

    PageControl->ActivePage=TabSheetGames;
    PageControlChange(NULL);

    // Заполняем список компьютеров
    for ( MComputer *Computer=(MComputer*)Computers->gFirst(); Computer;
        Computer=(MComputer*)Computer->gNext() )
    {
        TListItem *Item;

        Item=ListViewComputers->Items->Add();
        Item->Data=(void*)Computer;
        // Цвет группы и номер компьютера
        Item->SubItems->Add(IntToStr(Computer->Number));
        Item->SubItems->Add("");
        Item->SubItemImages[0]=FormMain->GetCompColorIcon(Computer);
    }
    //
    EditName->MaxLength=MAX_PrgNameLength;
    EditCmd->MaxLength=MAX_PrgCmdLength;
    EditIcon->MaxLength=MAX_PrgIconLength;
    ButtonCancel->Visible=false;
    ButtonAddChild->Enabled=false;
    SetNet(false,true);
    //
    CreateGamesTree(&TmpGames);

    EditShellUser->MaxLength=MAX_ClUNameLen;
    ComboBoxToEndTime->Items->Add("Не показывать");
    ComboBoxMsgEndTime->Items->Add("Не показывать");
    ComboBoxRebootWait->Items->Add("Не перезагружать");
    ComboBoxAutoLockTime->Items->Add("Не блокировать");
    for ( int i=1; i<=30; i++ )
    {
        AnsiString line=IntToStr(i);
        ComboBoxToEndTime->Items->Add(line);
        ComboBoxMessageTime->Items->Add(line);
        ComboBoxMsgEndTime->Items->Add(line);
        ComboBoxRebootWait->Items->Add(line);
        ComboBoxAutoLockTime->Items->Add(line);
    }
    OptionsToShell(&TmpOptions);
    ComboBoxToEndTimeClick(NULL);
    ComboBoxMsgEndTimeClick(NULL);
    ComboBoxRebootWaitClick(NULL);

    ActiveControl=ListViewComputers;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // Отменяем сетевые операции и освобождаем ресурсы WinSock
    ButtonCancelClick(NULL);
    Send.NetFree();
    // Очищаем память
    SendComps.Alloc(0);
    TmpGames.Clear();
    //
    TreeViewGames->Items->Clear();
    ListViewComputers->Items->Clear();
    EditName->Text=""; EditName->ClearUndo();
    EditCmd->Text=""; EditCmd->ClearUndo();
    EditIcon->Text=""; EditIcon->ClearUndo();
    EditShellUser->Text=""; EditShellUser->ClearUndo();
    ComboBoxToEndTime->Clear();
    ComboBoxMessageTime->Clear();
    ComboBoxMsgEndTime->Clear();
    ComboBoxRebootWait->Clear();
    ComboBoxAutoLockTime->Clear();
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
        TreeViewGames->Selected=NULL;
        if ( !TmpGames.LoadFrom(OpenDialog->FileName.c_str(),ENC_Code) )
            ResMessageBox(Handle,1,9,MB_APPLMODAL|MB_OK|MB_ICONERROR,TmpGames.gLastErr());
        else
            CreateGamesTree(&TmpGames);
        TmpGames.Clear();
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        if ( !TmpOptions.LoadFrom(OpenDialog->FileName.c_str(),ENC_Code) )
            ResMessageBox(Handle,1,11,MB_APPLMODAL|MB_OK|MB_ICONERROR,TmpOptions.gLastErr());
        else
            OptionsToShell(&TmpOptions);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::NSaveClick(TObject *Sender)
{
    if ( !SaveDialog->Execute() ) return;
    if ( PageControl->ActivePage==TabSheetGames )
    {
        CreateGamesFromTree(&TmpGames);
        if ( !TmpGames.SaveTo(SaveDialog->FileName.c_str(),ENC_Code) )
            ResMessageBox(Handle,1,10,MB_APPLMODAL|MB_OK|MB_ICONERROR,TmpGames.gLastErr());
        TmpGames.Clear();
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        ShellToOptions(&TmpOptions);
        if ( !TmpOptions.SaveTo(SaveDialog->FileName.c_str(),ENC_Code) )
            ResMessageBox(Handle,1,12,MB_APPLMODAL|MB_OK|MB_ICONERROR,TmpOptions.gLastErr());
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::NSendClick(TObject *Sender)
{
    if ( ListViewComputers->SelCount<1 ) return;

    {
        // Создадим массив указателей на компьютеры, куда будем рассылать
        SendComps.Alloc(ListViewComputers->SelCount);
        // Заполним его
        TItemStates is=TItemStates()<<isSelected;
        TListItem *item=ListViewComputers->Selected;
        for ( unsigned i=0; item; i++ )
        {
            SendComps[i]=(MComputer*)item->Data;
            item->SubItemImages[1]=13;
            item=ListViewComputers->GetNextItem(item,sdAll,is);
        }
    }

    if ( PageControl->ActivePage==TabSheetGames )
    {
        // Заполняем объект для отправки
        CreateGamesFromTree(&TmpGames);
        // Запускаем отправку данных
        SetNet(true,true);
        if ( !Send.Send(&SendComps,&TmpGames,NULL) ) goto error;
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        // Заполняем объект для отправки
        ShellToOptions(&TmpOptions);
        // Запускаем отправку данных
        SetNet(true,true);
        if ( !Send.Send(&SendComps,NULL,&TmpOptions) ) goto error;
    } else goto error;

    return;
error:
    // Удаляем мусор
    TmpGames.Clear();
    SendComps.Alloc(0);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::NLoadClick(TObject *Sender)
{
    MComputer *Computer;

    if ( ListViewComputers->SelCount!=1 ) return;
    Computer=(MComputer*)ListViewComputers->Selected->Data;
    if ( Computer==NULL ) return;
    if ( PageControl->ActivePage==TabSheetGames )
    {
        // Запускаем поток загрузки списка игр в Games
        SetNet(true,false);
        Send.Get(Computer,&TmpGames,NULL);
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        // Запускаем поток загрузки настроек
        SetNet(true,false);
        Send.Get(Computer,NULL,&TmpOptions);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::TreeViewGamesChange(TObject *Sender,
      TTreeNode *Node)
{
    TTreeNode *Select;
    MGame *Game;

    Select=TreeViewGames->Selected;
    if ( Select==NULL )
    {
        SetEdit(false,false);
        return;
    }
    Game=(MGame*)Select->Data;
    if ( Game==NULL ) return;
    EditName->Text=Game->Name;
    EditCmd->Text=Game->Command;
    EditIcon->Text=Game->Icon;
    // Если элемент верхнего уровня (группы программ) или слишком вложенный,
    // корень ветви - запрещаем делать его новым корнем
    ButtonAddChild->Enabled=!(
        (Select->Level==0)||
        (Select->Level>=MAX_PrgLevel)||
        Select->HasChildren);
    SetEdit(true,(Select->Level!=0)&&(!Select->HasChildren));
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::TreeViewGamesDeletion(TObject *Sender,
      TTreeNode *Node)
{
    MGame *Game=(MGame*)Node->Data;
    delete Game;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::EditNameExit(TObject *Sender)
{
    if ( TreeViewGames->Selected==NULL ) return;
    EditName->Text=EditName->Text.Trim();
    ((MGame*)TreeViewGames->Selected->Data)->SetName(EditName->Text.c_str());
    SetTreeViewGamesLine(TreeViewGames->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::EditCmdExit(TObject *Sender)
{
    if ( TreeViewGames->Selected==NULL ) return;
    EditCmd->Text=EditCmd->Text.Trim();
    ((MGame*)TreeViewGames->Selected->Data)->SetCommand(EditCmd->Text.c_str());
    SetTreeViewGamesLine(TreeViewGames->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::EditIconExit(TObject *Sender)
{
    if ( TreeViewGames->Selected==NULL ) return;
    EditIcon->Text=EditIcon->Text.Trim();
    ((MGame*)TreeViewGames->Selected->Data)->SetIcon(EditIcon->Text.c_str());
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
    if ( ((Select=TreeViewGames->Selected)==NULL)||(Select->Level==0) ) return;
    if ( Select->HasChildren&&
        ResMessageBox(Handle,27,28,
            MB_APPLMODAL|MB_YESNO|MB_ICONQUESTION)!=IDYES ) return;
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
    if ( Select->Level>=MAX_PrgLevel ) return;   
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
void __fastcall TFormClient::ButtonCancelClick(TObject *Sender)
{
    // Останавливаем сетевые операции
    Send.Stop();
}
//---------------------------------------------------------------------------
void TFormClient::SetTreeViewGamesLine(TTreeNode *Node_)
{
//    char line[MAX_PrgNameLength+MAX_PrgCmdLength+MAX_PrgIconLength+10+1];
    MGame *Game=(MGame*)Node_->Data;

    if ( Game==NULL ) return;
/*    strcpy(line,Game->Name);
    if ( Node_->Parent&&(!Node_->HasChildren) )
    {
        strcat(line,"   { ");
        strcat(line,Game->Command);
        strcat(line," , ");
        strcat(line,Game->Icon);
        strcat(line," }");
    }
    Node_->Text=line; */
    Node_->Text=Game->Name;
}
//---------------------------------------------------------------------------
void TFormClient::AddGamesToTree(TTreeNode *Node_, MGames *Games_)
{
    TTreeNode *Node;
    MGame *sGame, *dGame;

    for ( sGame=(MGame*)Games_->gFirst(); sGame;
        sGame=(MGame*)sGame->gNext() )
    {
        // Добавляем строку в дерево
        Node=TreeViewGames->Items->AddChild(Node_,"");
        dGame=new MGame;
        Node->Data=(void*)dGame;
        // Зададим имя программы/узла
        dGame->SetName(sGame->Name);
        // Зададим остальные параметры или добавим элементы нижних уровней
        if ( sGame->SubGames==NULL )
        {
            dGame->SetCommand(sGame->Command);
            dGame->SetIcon(sGame->Icon);
        } else AddGamesToTree(Node,sGame->SubGames);
        //
        SetTreeViewGamesLine(Node);
    }
}
//---------------------------------------------------------------------------
void TFormClient::CreateGamesTree(MGames *Games_)
{
    TTreeNode *Node;
    MGame *sGame, *dGame;

    TreeViewGames->Items->Clear();
    for ( int i=0; i<8; i++ )
    {
        Node=TreeViewGames->Items->Add(NULL,"");
        dGame=new MGame;
        Node->Data=(void*)dGame;

        sGame=(MGame*)Games_->Item(i);
        // Если узла верхнего уровня нет, зададим имя по-умолчанию
        if ( sGame==NULL )
        {
            char str[5+1];
            sprintf(str,"Page%i",i+1);
            dGame->SetName(str);
        } else
        {
            dGame->SetName(sGame->Name);
            // Добавим элементы нижних уровней
            if ( sGame->SubGames!=NULL ) AddGamesToTree(Node,sGame->SubGames);
        }
        //
        SetTreeViewGamesLine(Node);
    }
}
//---------------------------------------------------------------------------
void TFormClient::AddGamesFromTree(MGames *Games_, TTreeNode *Node_)
{
    TTreeNode *Node;
    MGame *sGame, *dGame;
    MGames *SubGames;

    Games_->Clear();
    for ( int i=0; i<Node_->Count; i++ )
    {
        Node=Node_->Item[i];
        dGame=(MGame*)Games_->Add();
        sGame=(MGame*)Node->Data;
        // Задаем имя программы/узла
        dGame->SetName(sGame->Name);
        // Если это узловой элемент, добавляем дочерние
        if ( Node->HasChildren )
        {
            SubGames=dGame->AddSubGames();
            if ( SubGames!=NULL ) AddGamesFromTree(SubGames,Node);
        } else
        {
            // Иначе можно задать команду и путь к иконке
            dGame->SetCommand(sGame->Command);
            dGame->SetIcon(sGame->Icon);
        }
    }
}
//---------------------------------------------------------------------------
void TFormClient::CreateGamesFromTree(MGames *Games_)
{
    TTreeNodes *items=TreeViewGames->Items;
    MGame *sGame, *dGame;
    MGames *SubGames;

    Games_->Clear();
    for ( int i=0; i<items->Count; i++ )
    {
        TTreeNode *node=items->Item[i];
        if ( node->Level ) continue;
        dGame=(MGame*)Games_->Add();
        sGame=(MGame*)node->Data;
        //
        dGame->SetName(sGame->Name);
        // Добавляем вложенные элементы
        if ( node->HasChildren )
        {
            SubGames=dGame->AddSubGames();
            if ( SubGames!=NULL ) AddGamesFromTree(SubGames,node);
        }
    }
}
//---------------------------------------------------------------------------
void TFormClient::OptionsToShell(MClOptions *Options_)
{
    EditShellUser->Text=Options_->ShellUser;
    ComboBoxToEndTime->ItemIndex=Options_->ToEndTime;
    ComboBoxMessageTime->ItemIndex=Options_->MessageTime-1;
    ComboBoxMsgEndTime->ItemIndex=Options_->MsgEndTime;
    ComboBoxRebootWait->ItemIndex=Options_->RebootWait;
    ComboBoxAutoLockTime->ItemIndex=Options_->AutoLockTime;
    ComboBoxToEndTimeClick(NULL);
    CheckBoxTransp->Checked=Options_->Flags&mcoTransp;
    CheckBoxRoute->Checked=Options_->Flags&mcoAddRoute;
    CheckBoxAutoRun->Checked=Options_->Flags&mcoAutoRun;
}
//---------------------------------------------------------------------------
void TFormClient::ShellToOptions(MClOptions *Options_)
{
    Options_->SetShellUser(EditShellUser->Text.c_str());
    Options_->ToEndTime=ComboBoxToEndTime->ItemIndex;
    Options_->MessageTime=ComboBoxMessageTime->ItemIndex+1;
    Options_->MsgEndTime=ComboBoxMsgEndTime->ItemIndex;
    Options_->RebootWait=ComboBoxRebootWait->ItemIndex;
    Options_->AutoLockTime=ComboBoxAutoLockTime->ItemIndex;
    Options_->Flags=
        (CheckBoxTransp->Checked?mcoTransp:0)|
        (CheckBoxRoute->Checked?mcoAddRoute:0)|
        (CheckBoxAutoRun->Checked?mcoAutoRun:0);
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
void TFormClient::SetNet(bool Begin_, bool Sending_)
{
    Sending=Sending_;
    if ( Begin_ )
    {
        // Сбросим иконки у всех компьютеров
        for ( int i=0, j=ListViewComputers->Items->Count; i<j; i++ )
            ListViewComputers->Items->Item[i]->SubItemImages[1]=-1;
        // Выставим начальные у компьютеров, с которыми будем обмениваться данными.
        // Обновляться будут в TFormClient::Dispatch().
        if ( Sending_ )
        {
            for ( unsigned int i=0; i<SendComps.Count(); i++ )
            {
                TListItem *item=ListViewComputers->FindData(0,SendComps[i],true,false);
                if ( item ) item->SubItemImages[1]=13;
            }
        } else
        {
            TListItem *item=ListViewComputers->Selected;
            if ( item ) item->SubItemImages[1]=13;
        }
        SetEdit(false,false);
    } else
        TreeViewGamesChange(TreeViewGames,TreeViewGames->Selected);
    ButtonCancel->Visible=Begin_;
    PanelProcess->Visible=Begin_; if ( !Begin_ ) PanelProcess->Caption="";
    PageControl->Enabled=!Begin_;
    ButtonOperations->Enabled=!Begin_;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::Dispatch(void *Message)
{
    TListItem *Item;

    switch(((TMessage*)Message)->Msg)
    {
        case WM_USER+mseConnecting:
/*            char line[12+MAX_ComputerAddressLength+1];
            line[0]=0; strcpy(line,"Соединение: ");
            // Проверяем что Message содержит правильный указатель на MComputer
            Item=ListViewComputers->FindData(0,
                (void*)(((TMessage*)Message)->WParam),true,false);
            // И добавляем из него IP-адрес компьютера
            if ( Item ) strcat(line,((MComputer*)(Item->Data))->Address);
            PanelProcess->Caption=line;*/
            PanelProcess->Caption="Соединение";
            break;

        case WM_USER+mseSending:
            PanelProcess->Caption="Отправка";
            break;

        case WM_USER+mseReceiving:
            PanelProcess->Caption="Загрузка";
            break;

        case WM_USER+mseDisconnecting:
            PanelProcess->Caption="Разъединение";
            break;

        case WM_USER+mseExecute:
            // Сбрасываем иконку, отображающую сетевой процесс
            Item=ListViewComputers->FindData(0,
                (void*)(((TMessage*)Message)->WParam),true,false);
            if ( Item ) Item->SubItemImages[1]=-1;
            // Если данные загружали, обновляем интерфейс
            if ( Sending ) break;
            if ( PageControl->ActivePage==TabSheetGames )
                CreateGamesTree(&TmpGames);
            else if ( PageControl->ActivePage==TabSheetOptions )
                OptionsToShell(&TmpOptions);
            break;

        case WM_USER+mseNotConnect:
        case WM_USER+mseProtError:
            Item=ListViewComputers->FindData(0,
                (void*)(((TMessage*)Message)->WParam),true,false);
            if ( Item ) Item->SubItemImages[1]=14;
            PanelProcess->Caption="Ошибка";
            break;

        case WM_USER+mseFreeParam:
            // Удаляем мусор
            SendComps.Alloc(0);
            TmpGames.Clear();
            //
            SetNet(false,false);
            break;

        default: break;
    }
    TForm::Dispatch(Message);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::EditShellUserExit(TObject *Sender)
{
    EditShellUser->Text=EditShellUser->Text.Trim();
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ComboBoxToEndTimeClick(TObject *Sender)
{
    ComboBoxMessageTime->Enabled=ComboBoxToEndTime->ItemIndex;
    ComboBoxMessageTime->Color=ComboBoxToEndTime->ItemIndex?clWindow:clBtnFace;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ComboBoxMsgEndTimeClick(TObject *Sender)
{
    int time1=ComboBoxMsgEndTime->ItemIndex;
    int time2=ComboBoxRebootWait->ItemIndex;
    if ( time2==0 ) return;
    if ( time1>time2 ) ComboBoxRebootWait->ItemIndex=time1;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ComboBoxRebootWaitClick(TObject *Sender)
{
    int time1=ComboBoxMsgEndTime->ItemIndex;
    int time2=ComboBoxRebootWait->ItemIndex;
    if ( time2==0 ) return;
    if ( time2<time1 ) ComboBoxMsgEndTime->ItemIndex=time2;
}
//---------------------------------------------------------------------------

