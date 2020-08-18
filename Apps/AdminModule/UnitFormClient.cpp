//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdexcept>
#include <cwchar>
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
	Send.NetInit(Handle, WM_USER+0, ENC_Net, Auth.get());

    PageControl->ActivePage=TabSheetGames;
    PageControlChange(nullptr);

	// Заполняем список компьютеров
	for ( auto &Computer: *Computers )
	{
		TListItem *Item;

		Item=ListViewComputers->Items->Add();
		Item->Data=&Computer;
		// Цвет группы и номер компьютера
		Item->SubItems->Add(IntToStr(Computer.Number));
		Item->SubItems->Add(L"");
		Item->SubItemImages[0]=FormMain->GetCompColorIcon(&Computer);
    }
    //
    EditName->MaxLength=MAX_PrgNameLength;
    EditCmd->MaxLength=MAX_PrgCmdLength;
    EditIcon->MaxLength=MAX_PrgIconLength;
    ButtonCancel->Visible=false;
    ButtonAddChild->Enabled=false;
    SetNet(false,true);
    //
    CreateGamesTree(TmpGames);

    EditShellUser->MaxLength=MAX_ClUNameLen;
	ComboBoxToEndTime->Items->Add(L"Не показывать");
	ComboBoxMsgEndTime->Items->Add(L"Не показывать");
	ComboBoxRebootWait->Items->Add(L"Не перезагружать");
	ComboBoxAutoLockTime->Items->Add(L"Не блокировать");
	for ( int i=1; i<=30; i++ )
	{
		UnicodeString line=IntToStr(i);
		ComboBoxToEndTime->Items->Add(line);
		ComboBoxMessageTime->Items->Add(line);
        ComboBoxMsgEndTime->Items->Add(line);
        ComboBoxRebootWait->Items->Add(line);
        ComboBoxAutoLockTime->Items->Add(line);
    }
    OptionsToShell(TmpOptions);
    ComboBoxToEndTimeClick(nullptr);
    ComboBoxMsgEndTimeClick(nullptr);
    ComboBoxRebootWaitClick(nullptr);

    ActiveControl=ListViewComputers;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // Отменяем сетевые операции и освобождаем ресурсы WinSock
    ButtonCancelClick(nullptr);
    Send.NetFree();
    // Очищаем память
    SendComps.clear();
    TmpGames.Clear();
    //
    TreeViewGames->Items->Clear();
    ListViewComputers->Items->Clear();
	EditName->Text=L""; EditName->ClearUndo();
	EditCmd->Text=L""; EditCmd->ClearUndo();
	EditIcon->Text=L""; EditIcon->ClearUndo();
    EditShellUser->Text=L""; EditShellUser->ClearUndo();
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
			L"Список игр TLK (*.gms)|*.gms|"
			L"Все файлы (*.*)|*.*";
		OpenDialog->DefaultExt=L"GMS";
	} else if ( PageControl->ActivePage==TabSheetOptions )
	{
		OpenDialog->Filter=
			L"Настройки для клиента (*.cfg)|*.cfg|"
			L"Все файлы (*.*)|*.*";
		OpenDialog->DefaultExt=L"CFG";
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
        TreeViewGames->Selected=nullptr;
        if ( !TmpGames.LoadFrom(OpenDialog->FileName.c_str(),ENC_Code) )
            ResMessageBox(Handle,1,9,MB_APPLMODAL|MB_OK|MB_ICONERROR,TmpGames.gLastErr());
        else
            CreateGamesTree(TmpGames);
        TmpGames.Clear();
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        if ( !TmpOptions.LoadFrom(OpenDialog->FileName.c_str(),ENC_Code) )
            ResMessageBox(Handle,1,11,MB_APPLMODAL|MB_OK|MB_ICONERROR,TmpOptions.gLastErr());
        else
			OptionsToShell(TmpOptions);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::NSaveClick(TObject *Sender)
{
    if ( !SaveDialog->Execute() ) return;
    if ( PageControl->ActivePage==TabSheetGames )
    {
        CreateGamesFromTree(TmpGames);
        if ( !TmpGames.SaveTo(SaveDialog->FileName.c_str(),ENC_Code) )
            ResMessageBox(Handle,1,10,MB_APPLMODAL|MB_OK|MB_ICONERROR,TmpGames.gLastErr());
        TmpGames.Clear();
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        ShellToOptions(TmpOptions);
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
        SendComps.resize(ListViewComputers->SelCount);
        // Заполним его
        TItemStates is=TItemStates()<<isSelected;
        TListItem *item=ListViewComputers->Selected;
        for ( unsigned i=0; item; i++ )
        {
            SendComps[i]=reinterpret_cast<MComputersItem*>(item->Data);
            item->SubItemImages[1]=13;
            item=ListViewComputers->GetNextItem(item,sdAll,is);
        }
    }

    if ( PageControl->ActivePage==TabSheetGames )
    {
        // Заполняем объект для отправки
        CreateGamesFromTree(TmpGames);
        // Запускаем отправку данных
        SetNet(true,true);
        if ( !Send.Send(&SendComps,&TmpGames,nullptr) ) goto error;
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        // Заполняем объект для отправки
        ShellToOptions(TmpOptions);
        // Запускаем отправку данных
        SetNet(true,true);
        if ( !Send.Send(&SendComps,nullptr,&TmpOptions) ) goto error;
    } else goto error;

    return;
error:
    // Удаляем мусор
    TmpGames.Clear();
    SendComps.clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::NLoadClick(TObject *Sender)
{
	if ( ListViewComputers->SelCount!=1 ) return;

	auto *Computer=reinterpret_cast<MComputersItem*>(
		ListViewComputers->Selected->Data);
	if ( Computer==nullptr ) return;

    if ( PageControl->ActivePage==TabSheetGames )
    {
        // Запускаем поток загрузки списка игр в Games
        SetNet(true,false);
        Send.Get(Computer,&TmpGames,nullptr);
    } else if ( PageControl->ActivePage==TabSheetOptions )
    {
        // Запускаем поток загрузки настроек
        SetNet(true,false);
        Send.Get(Computer,nullptr,&TmpOptions);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::TreeViewGamesChange(TObject *Sender,
      TTreeNode *Node)
{
	TTreeNode *Select=TreeViewGames->Selected;
	if ( Select==nullptr )
	{
		SetEdit(false,false);
		return;
	}

	auto *Game=reinterpret_cast<MGamesItem*>(Select->Data);
	if ( Game==nullptr ) return;

    EditName->Text=Game->Name.c_str();
    EditCmd->Text=Game->Command.c_str();
    EditIcon->Text=Game->Icon.c_str();
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
    auto *Game=reinterpret_cast<MGamesItem*>(Node->Data);
    delete Game;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::EditNameExit(TObject *Sender)
{
    if ( TreeViewGames->Selected==nullptr ) return;
    EditName->Text=EditName->Text.Trim();
    reinterpret_cast<MGamesItem*>(TreeViewGames->Selected->Data)->Name=EditName->Text.c_str();
    SetTreeViewGamesLine(TreeViewGames->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::EditCmdExit(TObject *Sender)
{
    if ( TreeViewGames->Selected==nullptr ) return;
    EditCmd->Text=EditCmd->Text.Trim();
    reinterpret_cast<MGamesItem*>(TreeViewGames->Selected->Data)->Command=EditCmd->Text.c_str();
    SetTreeViewGamesLine(TreeViewGames->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::EditIconExit(TObject *Sender)
{
    if ( TreeViewGames->Selected==nullptr ) return;
    EditIcon->Text=EditIcon->Text.Trim();
    reinterpret_cast<MGamesItem*>(TreeViewGames->Selected->Data)->Icon=EditIcon->Text.c_str();
    SetTreeViewGamesLine(TreeViewGames->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ButtonAddClick(TObject *Sender)
{
    TTreeNode *Select, *New;
    if ( (Select=TreeViewGames->Selected)==nullptr ) return;
    if ( Select->Level==0 ) { ButtonAddChildClick(nullptr); return; }
    New=TreeViewGames->Items->Add(Select, L"");
    New->Data=static_cast<void*>(new MGamesItem);
    SetTreeViewGamesLine(New);
    TreeViewGames->Selected=New;
    if ( EditName->Enabled ) EditName->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::ButtonDelClick(TObject *Sender)
{
    TTreeNode *Select, *Parent, *Prev;
    if ( ((Select=TreeViewGames->Selected)==nullptr)||(Select->Level==0) ) return;
    if ( Select->HasChildren&&
        ResMessageBox(Handle,27,28,
            MB_APPLMODAL|MB_YESNO|MB_ICONQUESTION)!=IDYES ) return;
    // Запоминаем родительский и предыдущий элементы
    Parent=Select->Parent;
    Prev=Parent->GetLastChild()==Select? Select->GetPrev(): nullptr;
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
    if ( (Select=TreeViewGames->Selected)==nullptr ) return;
    if ( Select->Level>=MAX_PrgLevel ) return;   
    New=TreeViewGames->Items->AddChild(Select, L"");
    New->Data=static_cast<void*>(new MGamesItem);
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
    if ( ((Select=TreeViewGames->Selected)==nullptr)||
        (Select->Level==0) ) return;

    if ( (Prev=Select->getPrevSibling())!=nullptr )
        Select->MoveTo(Prev,Prev->HasChildren?naAddChild:naInsert);
    else
    {
        Prev=Select->Parent;
        if ( Prev->Level ) Select->MoveTo(Prev,naInsert);
        else if ( (Prev=Prev->getPrevSibling())!=nullptr )
            Select->MoveTo(Prev,naAddChild);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::BitBtnDownClick(TObject *Sender)
{
    TTreeNode *Select, *Next;
    if ( ((Select=TreeViewGames->Selected)==nullptr)||
        (Select->Level==0) ) return;

    if ( (Next=Select->getNextSibling())!=nullptr )
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
        } else if ( (Next=Next->getNextSibling())!=nullptr )
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
//    UnicodeString line;
    auto *Game=reinterpret_cast<MGamesItem*>(Node_->Data);
	if ( Game==nullptr ) return;

/*    line=Game->Name.c_str();
    if ( Node_->Parent&&(!Node_->HasChildren) )
    {
		line+=L"   { ";
		line+=Game->Command.c_str();
		line+=L" , ";
		line+=Game->Icon.c_str();
        line+=L" }";
    }
    Node_->Text=line; */

    Node_->Text=Game->Name.c_str();
}
//---------------------------------------------------------------------------
void TFormClient::AddGamesToTree(TTreeNode *Node_, const MGames &Games_)
{
	for ( const auto &sGame: Games_ )
	{
		// Добавляем строку в дерево
		TTreeNode *Node=TreeViewGames->Items->AddChild(Node_, L"");
		MGamesItem *dGame=new MGamesItem;
		Node->Data=dGame;

		// Зададим имя программы/узла
		dGame->Name=sGame.Name;
		// Зададим остальные параметры или добавим элементы нижних уровней
		if ( !sGame.upSubGames )
		{
			dGame->Command=sGame.Command;
			dGame->Icon=sGame.Icon;
        } else AddGamesToTree(Node,*sGame.upSubGames);
        //
        SetTreeViewGamesLine(Node);
    }
}
//---------------------------------------------------------------------------
void TFormClient::CreateGamesTree(const MGames &Games_)
{
	TreeViewGames->Items->Clear();

	for ( int i=0; i<8; i++ )
	{
		TTreeNode *Node=TreeViewGames->Items->Add(nullptr, L"");
		MGamesItem *dGame=new MGamesItem;
		Node->Data=dGame;

		try
		{
			const MGamesItem& sGame=Games_.GetItem(i);

			dGame->Name=sGame.Name;
			// Добавим элементы нижних уровней
			if ( sGame.upSubGames ) AddGamesToTree(Node,*sGame.upSubGames);
		} catch (std::out_of_range &e)
		{
			// Если узла верхнего уровня нет, зададим имя по-умолчанию
			wchar_t str[5+1];
			swprintf(
				str, sizeof(str)/sizeof(str[0]),
				L"Page%i", i+1);
			dGame->Name=str;
		}
		//
        SetTreeViewGamesLine(Node);
    }
}
//---------------------------------------------------------------------------
void TFormClient::AddGamesFromTree(MGames &Games_, TTreeNode *Node_)
{
	Games_.Clear();

	for ( int i=0; i<Node_->Count; i++ )
	{
		TTreeNode *Node=Node_->Item[i];
		MGamesItem& dGame=Games_.Add();
		auto *sGame=reinterpret_cast<MGamesItem*>(Node->Data);

		// Задаем имя программы/узла
		dGame.Name=sGame->Name;
		// Если это узловой элемент, добавляем дочерние
		if ( Node->HasChildren )
		{
			dGame.upSubGames.reset(new MGames);
			AddGamesFromTree(*dGame.upSubGames,Node);
        } else
        {
            // Иначе можно задать команду и путь к иконке
			dGame.Command=sGame->Command;
			dGame.Icon=sGame->Icon;
		}
    }
}
//---------------------------------------------------------------------------
void TFormClient::CreateGamesFromTree(MGames &Games_)
{
    TTreeNodes *items=TreeViewGames->Items;

	Games_.Clear();

	for ( int i=0; i<items->Count; i++ )
	{
		TTreeNode *node=items->Item[i];
		if ( node->Level ) continue;

		MGamesItem& dGame=Games_.Add();
		auto *sGame=reinterpret_cast<MGamesItem*>(node->Data);

		//
		dGame.Name=sGame->Name;
		// Добавляем вложенные элементы
		if ( node->HasChildren )
		{
			dGame.upSubGames.reset(new MGames);
			AddGamesFromTree(*dGame.upSubGames,node);
        }
    }
}
//---------------------------------------------------------------------------
void TFormClient::OptionsToShell(const MClOptions &Options_)
{
	EditShellUser->Text=Options_.ShellUser.c_str();
	ComboBoxToEndTime->ItemIndex=Options_.ToEndTime;
	ComboBoxMessageTime->ItemIndex=Options_.MessageTime-1;
	ComboBoxMsgEndTime->ItemIndex=Options_.MsgEndTime;
	ComboBoxRebootWait->ItemIndex=Options_.RebootWait;
	ComboBoxAutoLockTime->ItemIndex=Options_.AutoLockTime;
	ComboBoxToEndTimeClick(nullptr);
	CheckBoxTransp->Checked=Options_.Flags&mcoTransp;
	CheckBoxRoute->Checked=Options_.Flags&mcoAddRoute;
	CheckBoxAutoRun->Checked=Options_.Flags&mcoAutoRun;
}
//---------------------------------------------------------------------------
void TFormClient::ShellToOptions(MClOptions &Options_)
{
	Options_.SetShellUser(EditShellUser->Text.c_str());
	Options_.ToEndTime=ComboBoxToEndTime->ItemIndex;
	Options_.MessageTime=ComboBoxMessageTime->ItemIndex+1;
	Options_.MsgEndTime=ComboBoxMsgEndTime->ItemIndex;
	Options_.RebootWait=ComboBoxRebootWait->ItemIndex;
	Options_.AutoLockTime=ComboBoxAutoLockTime->ItemIndex;
	Options_.Flags=
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
            for ( unsigned int i=0; i<SendComps.size(); i++ )
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
    PanelProcess->Visible=Begin_; if ( !Begin_ ) PanelProcess->Caption=L"";
    PageControl->Enabled=!Begin_;
    ButtonOperations->Enabled=!Begin_;
}
//---------------------------------------------------------------------------
void __fastcall TFormClient::Dispatch(void *Message)
{
    TListItem *Item;

	switch(reinterpret_cast<TMessage*>(Message)->Msg)
    {
        case WM_USER+mseConnecting:
/*
			wchar_t line[12+MAX_ComputerAddressLength+1];
			line[0]=0; wcscpy(line, L"Соединение: ");
			// Проверяем что Message содержит правильный указатель на MComputer
			Item=ListViewComputers->FindData(0,
				(void*)(((TMessage*)Message)->WParam),true,false);
			// И добавляем из него IP-адрес компьютера
			if ( Item ) wcscat(line,((MComputersItem*)(Item->Data))->Address);
			PanelProcess->Caption=line;
*/
			PanelProcess->Caption=L"Соединение";
			break;

		case WM_USER+mseSending:
            PanelProcess->Caption=L"Отправка";
            break;

        case WM_USER+mseReceiving:
			PanelProcess->Caption=L"Загрузка";
			break;

		case WM_USER+mseDisconnecting:
            PanelProcess->Caption=L"Разъединение";
            break;

        case WM_USER+mseExecute:
            // Сбрасываем иконку, отображающую сетевой процесс
            Item=ListViewComputers->FindData(0,
				reinterpret_cast<void*>(
					reinterpret_cast<TMessage*>(Message)->WParam
				),true,false);
			if ( Item ) Item->SubItemImages[1]=-1;
			// Если данные загружали, обновляем интерфейс
			if ( Sending ) break;
			if ( PageControl->ActivePage==TabSheetGames )
				CreateGamesTree(TmpGames);
			else if ( PageControl->ActivePage==TabSheetOptions )
				OptionsToShell(TmpOptions);
			break;

		case WM_USER+mseNotConnect:
		case WM_USER+mseProtError:
			Item=ListViewComputers->FindData(0,
				reinterpret_cast<void*>(
					reinterpret_cast<TMessage*>(Message)->WParam
				),true,false);
            if ( Item ) Item->SubItemImages[1]=14;
            PanelProcess->Caption=L"Ошибка";
            break;

		case WM_USER+mseFreeParam:
			//
            Send.Stop();
            // Удаляем мусор
            SendComps.clear();
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

