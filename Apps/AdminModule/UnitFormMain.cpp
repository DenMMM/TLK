//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#pragma hdrstop

#include "UnitFormMain.h"
#include "UnitFormRun.h"
#include "UnitFormFine.h"
#include "UnitFormLogIn.h"
#include "UnitFormOpenConfig.h"
#include "UnitFormUserPass.h"
#include "UnitFormOptionsPass.h"
#include "UnitFormComputers.h"
#include "UnitFormTariffs.h"
#include "UnitFormFines.h"
#include "UnitFormUsers.h"
#include "UnitFormOptions.h"
#include "UnitFormClient.h"
#include "UnitFormAuth.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
std::unique_ptr <MShellState> ShellState;
std::unique_ptr <MOptions> Options;
std::unique_ptr <MComputers> Computers;
std::unique_ptr <MTariffs> Tariffs;
std::unique_ptr <MFines> Fines;
std::unique_ptr <MUsers> Users;
std::unique_ptr <MStates> States;
std::unique_ptr <MSync> Sync;
std::unique_ptr <MAuth> Auth;
std::unique_ptr <MLog> Log;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
        : TForm(Owner)
{
    EnableConfig=false;
    Tag=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
	try
	{

	// �������� ������� ����������� �����������
	ShellState.reset(new MShellState);
	Options.reset(new MOptions);
	Computers.reset(new MComputers);
	Tariffs.reset(new MTariffs);
	Fines.reset(new MFines);
	Users.reset(new MUsers);
	States.reset(new MStates);
	Sync.reset(new MSync);
	Auth.reset(new MAuth);
	Log.reset(new MLog);

	// �������� ���� �������� ������ � ���� ����������
	Options->SetDefaultKey(
		HKEY_LOCAL_MACHINE,
		L"Software\\MMM Groups\\TLK\\3.0\\Admin",
		L"Options",
		ENC_Code);
	Auth->SetDefaultKey(
		HKEY_LOCAL_MACHINE,
		L"Software\\MMM Groups\\TLK\\3.0\\Admin",
		L"Auth",
		ENC_Code);

	UnicodeString path=ExtractFilePath(Application->ExeName), file;

	file=path+L"COMPUTERS.DAT";
	Computers->SetDefaultFile(file.c_str(),ENC_Code);
	file=path+L"TARIFFS.DAT";
	Tariffs->SetDefaultFile(file.c_str(),ENC_Code);
	file=path+L"FINES.DAT";
	Fines->SetDefaultFile(file.c_str(),ENC_Code);
	file=path+L"USERS.DAT";
	Users->SetDefaultFile(file.c_str(),ENC_Code);
	file=path+L"STATES.DAT";
	States->SetDefaultFile(file.c_str(),ENC_Code);
	file=path+L"ARP.DAT";
	Sync->SetARPFile(file.c_str(),ENC_Code,true);
	file=path+L"LOGS";
	Log->SetDefault(file.c_str(),ENC_Code);

#ifdef _DEBUG
	if ( !Options->Load() )         /// ����� ������
	{
		Options->Pass.Set(L"");
		Options->Save();
	}
#else
	// �������� �������� ���������
	if ( !Options->Load() )
	{
		ResMessageBox(Handle,1,30,MB_APPLMODAL|MB_OK|MB_ICONERROR);
		// ��� ������ �� ��������� ����������� �� �����
		Tag=true; Close();
		return;
	}
#endif

	// �������� ��������� ���������, �������� �� ���
	bool all_load=true;
	all_load=Auth->Load()&&all_load;
	all_load=Computers->Load()&&all_load;
	all_load=Tariffs->Load()&&all_load;
	all_load=Fines->Load()&&all_load;
	all_load=Users->Load()&&all_load;
	if ( !all_load )
	{
		ShellState->State|=mssErrorConfig; SetShell();
		ResMessageBox(Handle,1,2,MB_APPLMODAL|MB_OK|MB_ICONERROR);
	}

	// �������� ��������� �����������
	if ( !States->Load() )
	{
		States->Clear();
		ShellState->State|=mssErrorState; SetShell();
		ResMessageBox(Handle,1,7,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
	} else
		// ���� ��������� � �����������, ������ �� � ���������� ������� �����������
		States->Update(*Computers);			/// ������� �������� ?

	// �������� ARP-���
	if ( !Sync->LoadARP() )
		ResMessageBox(Handle,0,29,MB_APPLMODAL|MB_OK|MB_ICONWARNING,Sync->gLastErr());

	// �������������� ������������� �� ���� � ���������
	Sync->NetInit(ENC_Net, Auth.get());
	Sync->Associate(States.get(), Computers.get());
	ProgressBarNetProcess->Max=Sync->gPCountMax();

	// ������ ����� �� �����, �.�. �� ����� ��������� �����
	Timer->Interval=1000;
	Timer->Enabled=true;
	TimerTimer(nullptr);

	// ��������� ���
	if ( Log->Open() )
	{
		// ���������� ������������, ���������� �����
		ShellState->User=Log->LastUser();
		// ��������� ���� �� �� ��� � ������ ������������� � ������� �� ��� �����
		auto iUser=Users->SrchUUID(ShellState->User);
		if ( (iUser==Users->end()) || (!iUser->Active) ) ShellState->User=0;
		// �������� � ���� ��������� ���������, ����� ����� ����� ��������� ��� ������
		if ( !Log->AddStart(
			*ShellState,
			*States,
			*Tariffs,
			*Fines,
			*Users) )
		{
			ShellState->State|=mssErrorLog; SetShell();
			ResMessageBox(Handle,1,4,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
		}
	} else
	{
		// ���� ������ ���� ������� � ���������� ��� �������,
		// ���������� ������ ����� ����
		if ( (Log->gLastErr()!=0)||
			(!Log->Begin(
				*ShellState,
				*States,
				*Tariffs,
				*Fines,
				*Users)) )
		{
			ShellState->State|=mssErrorLog; SetShell();
			ResMessageBox(Handle,1,4,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
		} else
			ResMessageBox(Handle,0,32,MB_APPLMODAL|MB_OK|MB_ICONWARNING);
	}

	NFilterAllClick(NFilterAll);
	StatusBar->SimplePanel=true;
	SetShell();

	TimerNet->Interval=250;
	TimerNet->Enabled=true;
	// ��������� �������������
	Sync->Start();

	// ������� ����������� ���������, ����� �� �������
	ListViewComputers->DoubleBuffered=true;
	ProgressBarNetProcess->DoubleBuffered=true;

	}
	catch (std::bad_alloc &e)
	{
		ResMessageBox(Handle,1,13,MB_APPLMODAL|MB_OK|MB_ICONERROR);
		Tag=true; Close();
		return;
	}
	catch (std::exception &e)
	{
		::MessageBoxA(Handle, e.what(),
			"TLK - ������",MB_APPLMODAL|MB_OK|MB_ICONERROR);
		Tag=true; Close();
		return;
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
		Tag=true; Close();
		return;
	}

	Tag=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	TimerNet->Enabled=false;
    Timer->Enabled=false;
    Sync->Stop();
    Sync->NetFree();

    // ������� ������ � ��� � ���������� ���������� ������
    if ( !Tag ) Log->AddStop();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
	std::int64_t SystemTime;

    // ����� ��������� �����
    GetLocalTimeInt64(SystemTime);
    // ������ ��� ������ ����
    Log->Timer(SystemTime);
    // ��������� ��������� ����������� � ��������� � ������ ���������
    if ( States->Timer(SystemTime) )
        if ( !States->Save() ) { ShellState->State|=mssErrorState; SetShell(); }
    // ��������� ��������� �����
    TDate CurrentTime=Time();
	LabelTimeSecs->Caption=CurrentTime.FormatString(L"ss");
    LabelTimeHoursMins->Caption=CurrentTime.FormatString(L"hh : mm");
    // � ������ �����������
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerNetTimer(TObject *Sender)
{
    ProgressBarNetProcess->Position=Sync->gPCount();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogInClick(TObject *Sender)
{
    std::unique_ptr <TFormLogIn> form;
    unsigned User;

    try
    {
        // ��������� ������ �����������
		form.reset(new TFormLogIn(0));
		User=form->Execute(*Users);
        if ( User==0 ) return;
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
        return;
    }
    
    // ���������� � ��� �������
    if ( !Log->AddLogIn(User) )
    {
        ShellState->State|=mssErrorLog; SetShell();
        ResMessageBox(Handle,1,17,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
        return;
    }
    ShellState->User=User;
    SetShell();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogOutClick(TObject *Sender)
{
    // ����������� ������������� �� �������� �����
    if ( ResMessageBox(Handle,25,26,
        MB_APPLMODAL|MB_YESNO|MB_ICONQUESTION)!=IDYES ) return;

    // ���������� � ��� �������
    if ( !Log->AddLogOut() )
    {
        ShellState->State|=mssErrorLog; SetShell();
        ResMessageBox(Handle,1,18,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
        return;
    }

    ShellState->User=0; SetShell();

    // ��������� �� ���� �� ������ ����� ����
    if ( !Log->CheckPeriod(Options->LogPeriod) ) return;

    // ��������� ������� ��� � �������� �����
	if ( !(Log->End()&&
		Log->Begin(
			*ShellState,
			*States,
			*Tariffs,
			*Fines,
			*Users)) )
    {
        ShellState->State|=mssErrorLog; SetShell();
        ResMessageBox(Handle,1,6,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NClosePrgClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NConfigOpenClick(TObject *Sender)
{
    std::unique_ptr <TFormOpenConfig> form;

    if ( !(ShellState->State&mssConfig) )
    {
        try
        {
			form.reset(new TFormOpenConfig(0));
			if ( !form->Execute(*Options, Left+20, Top+50, true) ) return;
		}
        catch (Exception &ex)
        {
            Application->ShowException(&ex);
            // �� ����� ����� ���������� �������� ��������� ������ ���������
            return;
        }
    }
    ShellState->State^=mssConfig; SetShell();
    Log->AddConfig(ShellState->State&mssConfig);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NComputersClick(TObject *Sender)
{
    std::unique_ptr <TForm> form;

    // ���. �������� ��� ������� ������������
    if ( !EnableConfig ) return;

    try
    {
		if ( Sender==NComputers ) form.reset(new TFormComputers(0));
		else if ( Sender==NTariffs ) form.reset(new TFormTariffs(0));
		else if ( Sender==NFines ) form.reset(new TFormFines(0));
		else if ( Sender==NUsers ) form.reset(new TFormUsers(0));
		else if ( Sender==NCommon ) form.reset(new TFormOptions(0));
		else if ( Sender==NClient ) form.reset(new TFormClient(0));
		else if ( Sender==NAuth ) form.reset(new TFormAuth(0));
		else return;

        form->Left=Left+30;
        form->Top=Top+50;
        form->ShowModal();
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NOptionsPasswordClick(TObject *Sender)
{
    std::unique_ptr <TFormOptionsPass> form;

    try
    {
		form.reset(new TFormOptionsPass(0));
		if ( !form->Execute(Options.get(), Left+100, Top+50, true) ) return;
		if ( Options->Save() ) return;
//        ShellState->State|=mssErrorConfig; SetShell();  /// �� ����� ������ � ������
        ResMessageBox(Handle,1,31,MB_APPLMODAL|MB_OK|MB_ICONERROR,Options->gLastErr());
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NUsersPasswordsClick(TObject *Sender)
{
    std::unique_ptr <TFormUserPass> form;

    try
    {
		form.reset(new TFormUserPass(0));
		if ( !form->Execute(*Users, Left+100, Top+50, true) ) return;
		if ( Users->Save() ) return;
//        ShellState->State|=mssErrorConfig; SetShell();  /// �� ����� ������ � ������
		ResMessageBox(Handle,1,3,MB_APPLMODAL|MB_OK|MB_ICONERROR,Users->gLastErr());
	}
	catch (Exception &ex)
	{
		Application->ShowException(&ex);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnRunClick(TObject *Sender)
{
	TPoint local_button_coord;
	std::unique_ptr <TForm> form;

	try
	{
		// ������� ������ �����
		if ( (Sender==BitBtnRun)||(Sender==NCmpRun) )
		{
			form.reset(new TFormRun(0));
			static_cast<TFormRun*>(form.get())->RunMode=true;	/// cast
		} else if ( (Sender==BitBtnAdd)||(Sender==NCmpAdd) )
		{
			form.reset(new TFormRun(0));
			static_cast<TFormRun*>(form.get())->RunMode=false;	/// cast
		} else if ( (Sender==BitBtnFine)||(Sender==NCmpFine) )
		{
			form.reset(new TFormFine(0));
		} else return;

        // ������ ������� ����������
        local_button_coord.x=PanelButtons->Left+BitBtnRun->Left-form->Width-2;
        local_button_coord.y=PanelButtons->Top+BitBtnRun->Top-40;
        local_button_coord=ClientToScreen(local_button_coord);
        form->Left=local_button_coord.x;
        form->Top=local_button_coord.y;
        // �������
        form->ShowModal();
    }
    catch (Exception &ex)
    {
        Application->ShowException(&ex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnExchangeClick(TObject *Sender)
{
    if ( ListViewComputers->SelCount!=2 )
    {
        ResMessageBox(Handle,0,15,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

	// ���� ��������� ��������� �����������
	TListItem *item=ListViewComputers->Selected;
	auto *state1=reinterpret_cast<MStatesItem*>(item->Data);
	item=ListViewComputers->GetNextItem(item,sdAll,TItemStates()<<isSelected);
	auto *state2=reinterpret_cast<MStatesItem*>(item->Data);

	// ��������� �������� �� ��������� �������
	// � �� ����� �� �������� ������� ����������
	if ( !state1->CmdExchange(*state2,true) )
	{
		if ( !state2->CmdExchange(*state1,true) )
		{
			ResMessageBox(Handle,0,34,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
			return;
		}
		std::swap(state1,state2);
	}

	MTariffRunTimesItem time=state1->GetRunParam();
	// ��������� �������� �� ��� �� ����� �� ������� ����������
	auto iTariff=Tariffs->SrchUUID(time.TariffID);
	if ( !((iTariff!=Tariffs->end()) && iTariff->CheckForComp(state2->Associated())) )
	{
        ResMessageBox(Handle,0,14,MB_APPLMODAL|MB_OK|MB_ICONWARNING);
        return;
    }

    // ��������� ������ � ���
    if ( !Log->AddExchange(state1->Associated(),
                           state2->Associated()) )
    {
        ShellState->State|=mssErrorLog; SetShell();
        ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
        return;
    }

    // ��������� �������
    state1->CmdExchange(*state2,false);
    // ��������� ����� ��������� � �����
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // ��������� ������ �����������
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnLockClick(TObject *Sender)
{
    bool lock;
    if ( (Sender==BitBtnLock)||(Sender==NCmpLock) ) lock=true;
    else if ( (Sender==BitBtnUnLock)||(Sender==NCmpUnLock) ) lock=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        auto *State=reinterpret_cast<MStatesItem*>(Item->Data);
        // ��������� �������� �� ��������� �������
        if ( !State->CmdLock(lock,true) ) continue;
        // ��������� ������ � ���
        if ( !Log->AddLock(State->Associated(),lock) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // ��������� �������
        State->CmdLock(lock,false);
    }
    // ��������� ����� ��������� � �����
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // ��������� ������ �����������
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnAdditionalsClick(TObject *Sender)
{
    TPopupMenu *PopupMenu;
    TBitBtn *BitBtn;
    TPoint local_button_coord;

    if ( Sender==BitBtnAdditionals ) { BitBtn=BitBtnAdditionals; PopupMenu=PopupMenuAdditionals; }
    else if ( Sender==BitBtnSystem ) { BitBtn=BitBtnSystem; PopupMenu=PopupMenuSystem; }
    else if ( Sender==BitBtnViewFilter ) { BitBtn=BitBtnViewFilter; PopupMenu=PopupMenuViewFilter; }
    else return;

    local_button_coord.x=PanelButtons->Left+BitBtn->Left+4;
    local_button_coord.y=PanelButtons->Top+BitBtn->Top+BitBtn->Height-4;
    local_button_coord=ClientToScreen(local_button_coord);

    PopupMenu->Popup(local_button_coord.x,local_button_coord.y);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NRebootClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        auto *State=reinterpret_cast<MStatesItem*>(Item->Data);
        // ��������� �������� �� ��������� �������
        if ( !State->CmdReboot(true) ) continue;
        // ��������� ������ � ���
        if ( !Log->AddReboot(State->Associated()) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // ��������� �������
        State->CmdReboot(false);
    }
    // ��������� ����� ��������� � �����
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // ��������� ������ �����������
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NPowerOnClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        auto *State=reinterpret_cast<MStatesItem*>(Item->Data);
        // ��������� �������� �� ��������� �������
        if ( !State->CmdPowerOn(true) ) continue;
        // ��������� ������ � ���
        if ( !Log->AddPowerOn(State->Associated()) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // ��������� �������
        State->CmdPowerOn(false);
    }
    // ��������� ����� ��������� � �����
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // ��������� ������ �����������
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NShutdownClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        auto *State=reinterpret_cast<MStatesItem*>(Item->Data);
        // ��������� �������� �� ��������� �������
        if ( !State->CmdShutdown(true) ) continue;
        // ��������� ������ � ���
        if ( !Log->AddShutdown(State->Associated()) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // ��������� �������
        State->CmdShutdown(false);
    }
    // ��������� ����� ��������� � �����
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // ��������� ������ �����������
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NPauseClick(TObject *Sender)
{
    bool pause;
    if ( Sender==NPause ) pause=true;
    else if ( Sender==NRun ) pause=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        auto *State=reinterpret_cast<MStatesItem*>(Item->Data);
        // ��������� �������� �� ��������� �������
        if ( !State->CmdPause(pause,true) ) continue;
        // ��������� ������ � ���
        if ( !Log->AddPause(State->Associated(),pause) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // ��������� �������
        State->CmdPause(pause,false);
    }
    // ��������� ����� ��������� � �����
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // ��������� ������ �����������
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NOpenClick(TObject *Sender)
{
    // ���. �������� ��� ������� ������������
    if ( !EnableConfig ) return;

    bool open;
    if ( Sender==NOpen ) open=true;
    else if ( Sender==NClose ) open=false;
    else return;

    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewComputers->Selected; Item;
        Item=ListViewComputers->GetNextItem(Item,sdAll,is) )
    {
        auto *State=reinterpret_cast<MStatesItem*>(Item->Data);
        // ��������� �������� �� ��������� �������
        if ( !State->CmdOpen(open,true) ) continue;
        // ��������� ������ � ���
        if ( !Log->AddOpen(State->Associated(),open) )
        {
            ShellState->State|=mssErrorLog; SetShell();
            ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
            break;
        }
        // ��������� �������
        State->CmdOpen(open,false);
    }
    // ��������� ����� ��������� � �����
    if ( !States->Save() )
    {
        ShellState->State|=mssErrorState; SetShell();
        ResMessageBox(Handle,1,8,MB_APPLMODAL|MB_OK|MB_ICONERROR,States->gLastErr());
    }
    // ��������� ������ �����������
    UpdateListViewComputers(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NFilterAllClick(TObject *Sender)
{
    if ( (Sender==NFilterAll)||(Sender==NCmpFilterAll) )
    {
        Filter=mcfAll;
        NFilterAll->Default=true;
        NFilterAll->Checked=true;
        NCmpFilterAll->Default=true;
        NCmpFilterAll->Checked=true;
    } else if ( (Sender==NFilterFree)||(Sender==NCmpFilterFree) )
    {
        Filter=mcfFree;
        NFilterFree->Default=true;
        NFilterFree->Checked=true;
        NCmpFilterFree->Default=true;
        NCmpFilterFree->Checked=true;
    } else if ( (Sender==NFilterService)||(Sender==NCmpFilterService) )
    {
        Filter=mcfService;
        NFilterService->Default=true;
        NFilterService->Checked=true;
        NCmpFilterService->Default=true;
        NCmpFilterService->Checked=true;
    } else return;
    // ��������� ������ �����������
    UpdateListViewComputers(true);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewComputersInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
    Item->SubItems->Add(L"");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewComputersCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
	int num1=reinterpret_cast<MStatesItem*>(Item1->Data)->Associated();
    int num2=reinterpret_cast<MStatesItem*>(Item2->Data)->Associated();
    Compare = num1==num2? 0: (num1<num2? -1: +1);
}
//---------------------------------------------------------------------------
bool TFormMain::CheckFilter(MStatesInfo *Info_, int FreeTime_)
{
    bool result=false;
    switch(Filter)
    {
        case mcfAll:
            result=true;
            break;
        case mcfFree:
            if ( (Info_->State==mcsFree)||
                ((Info_->State&mcsWork)&&(Info_->ToEndWork<FreeTime_)) ) result=true;
            break;
        case mcfService:
            if ( Info_->State&mcsOpen ) result=true;
            break;
        default: break;
    }
    return result;
}
//---------------------------------------------------------------------------
void TFormMain::SetListViewComputersLine(TListItem *Item_, MStatesInfo *Info_)
{
    TStrings *SubItems=Item_->SubItems;
    wchar_t line[50];
	int icon;

	// ����� ����������
	if ( Info_->Changes&mdcNumber )
	{
		auto iComp=Computers->Search(Info_->Number);
		Item_->SubItemImages[0]=GetCompColorIcon(&*iComp);
		swprintf(
			line, sizeof(line)/sizeof(line[0]),
			L"%i", Info_->Number);
        SubItems->Strings[0]=line;
    }
    // ��������� ����
    if ( Info_->Changes&mdcNetState )
    {
        unsigned int NetState=Info_->NetState;
        if ( NetState&mnsSyncNeed ) icon=12;
        else if ( NetState&mnsPresent ) icon=-1;
        else icon=14;
        Item_->SubItemImages[1]=icon;
    }
    // ����� ������
    if ( Info_->Changes&mdcState )
    {
        unsigned int State=Info_->State;
		if ( State&mcsOpen ) { icon=9; wcscpy(line, L"���������"); }
		else if ( State&mcsPause ) { icon=8; wcscpy(line, L"�������������"); }
		else if ( State&mcsLock ) { icon=7; wcscpy(line, L"������� !"); }
		else if ( State&mcsFine ) { icon=6; wcscpy(line, L"����� !!!"); }
		else if ( State&mcsWork ) { icon=5; wcscpy(line, L"������"); }
		else if ( State&mcsFree ) { icon=4; wcscpy(line, L"��������"); }
		else { icon=-1; wcscpy(line, L""); }
        Item_->SubItemImages[2]=icon;
        SubItems->Strings[2]=line;
    }
    // �������� ������
    if ( Info_->Changes&mdcTariff )
    {
		auto iTariff=Tariffs->SrchUUID(Info_->TariffID);
		if ( iTariff!=Tariffs->end() ) SubItems->Strings[3]=iTariff->Name.c_str();
        else SubItems->Strings[3]=L"";
    }
    // ����� ������
    if ( Info_->Changes&mdcWorkTime )
    {
        if ( Info_->State&mcsWork )
        {
			swprintf(
				line, sizeof(line)/sizeof(line[0]),
				L"%i ���. %.2i ���.",
				Info_->WorkTime/60,
				Info_->WorkTime%60);
            SubItems->Strings[4]=line;
        } else
        {
			SubItems->Strings[4]=L"";
			SubItems->Strings[5]=L"";
            SubItems->Strings[6]=L"";
        }
    }
    // ������� ������� �������� �������� � �� �������
    if ( Info_->State&mcsWork )
    {
		swprintf(
			line, sizeof(line)/sizeof(line[0]),
			L"%.2i:%.2i",
			Info_->ToEndWork/60,
			Info_->ToEndWork%60);
		SubItems->Strings[5]=line;
		swprintf(
			line, L"%.2i:%.2i",
			Info_->EndWorkTime/60,
			Info_->EndWorkTime%60);
        SubItems->Strings[6]=line;
    }
    // ����� ������
    if ( Info_->Changes&mdcFineTime )
    {
        if ( Info_->State&mcsFine )
        {
			swprintf(
				line, sizeof(line)/sizeof(line[0]),
				L"%i ���.",
				Info_->FineTime);
			SubItems->Strings[7]=line;
        } else
        {
			SubItems->Strings[7]=L"";
			SubItems->Strings[8]=L"";
		}
	}
	// ������� ������� ������ ��������
	if ( Info_->State&mcsFine )
	{
		swprintf(
			line, sizeof(line)/sizeof(line[0]),
			L"%.2i:%.2i",
			Info_->ToEndFine/60,
			Info_->ToEndFine%60);
        SubItems->Strings[8]=line;
    }
}
//---------------------------------------------------------------------------
void TFormMain::UpdateListViewComputers(bool Full_)
{
    TListItem *Item;
    MStatesInfo Info;

    if ( Full_ ) ListViewComputers->Items->Clear();

    // ������� �� ������ ����������, �� ���������� ��� ������. ��������� �����.
	for ( auto &State: *States )
	{
		Info=State.GetInfo();
		Item=ListViewComputers->FindData(0,&State,true,false);
		// ��������� �������� �� ��������� ��� ������������ ������
		if ( CheckFilter(&Info,Options->FilterFreeTime) )
		{
			// ���� ��������� �� ������� � ������, �� ��������� ���
			if ( !Item )
			{
				Item=ListViewComputers->Items->Add();
				Item->Data=&State;
				Info.Changes=mdcAll;
			}
		} else
		{
			if ( Item ) Item->Delete();
			continue;
		}
		// ��������� ���������� � �������
		if ( Full_ ) Info.Changes=mdcAll;
		SetListViewComputersLine(Item,&Info);
	}
}
//---------------------------------------------------------------------------
void TFormMain::SetShell()
{

	// ����� ��������
	bool enable=(ShellState->State&mssConfig)||(ShellState->User!=0);
	enable=ShellState->State&(mssErrorState|mssErrorConfig)? false: enable;
	BitBtnRun->Enabled=enable;          NCmpRun->Enabled=enable;
	BitBtnAdd->Enabled=enable;          NCmpAdd->Enabled=enable;
	BitBtnFine->Enabled=enable;         NCmpFine->Enabled=enable;
	BitBtnExchange->Enabled=enable;     NCmpExchange->Enabled=enable;
	BitBtnLock->Enabled=enable;         NCmpLock->Enabled=enable;
	BitBtnUnLock->Enabled=enable;       NCmpUnLock->Enabled=enable;
	BitBtnAdditionals->Enabled=enable;
	// ������������ �����������
	NPause->Enabled=((ShellState->User!=0)&&(Options->UsersRights&murPause))||
		(ShellState->State&mssConfig);
	NRun->Enabled=NPause->Enabled;

	// ���������, ��������� ������ ���������
	NConfigOpen->Caption=ShellState->State&mssConfig? "�������": "�������...";
	enable=(ShellState->State&mssConfig)&&(!(ShellState->State&mssErrorLog));
	EnableConfig=enable;
	NAdmin->Enabled=enable;
	NComputers->Enabled=enable;
	NTariffs->Enabled=enable;
	NFines->Enabled=enable;
	NUsers->Enabled=enable;
	NCommon->Enabled=enable;
	NClient->Enabled=enable;
	NAuth->Enabled=enable;
	BitBtnSystem->Enabled=enable;
	// ������������� �������
	NLogIn->Enabled=ShellState->User==0;
	NLogOut->Enabled=ShellState->User!=0;
//    NOptionsPassword->Enabled=!error;
//    NUsersPasswords->Enabled=!error;
	// ��������� ����� ����
	NLogReset->Enabled=(ShellState->State&mssConfig)&&(ShellState->State&mssErrorLog);

	// ��������� ���� ���������
	UnicodeString line=L"TLK - ";
	auto iUser=Users->SrchUUID(ShellState->User);
	if ( iUser!=Users->end() ) line+=iUser->Login.c_str();
	else line+=L"����� �� ������";
    if ( ShellState->State&mssConfig ) line+=L" (������� ���������)";
    Caption=line;
    // ������ ���������
	line=L"";
	if ( ShellState->State&mssErrorConfig ) line+=L"������ ������ � �������������, ";
	if ( ShellState->State&mssErrorState ) line+=L"�� ������� ��������� ��������� ���������, ";
    if ( ShellState->State&mssErrorLog ) line+=L"������ ������ � �����, ";
    StatusBar->SimpleText=line;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NLogResetClick(TObject *Sender)
{
	if ( Log->Begin(
		*ShellState,
		*States,
		*Tariffs,
		*Fines,
		*Users) )
	{
		ShellState->State&=~mssErrorLog; SetShell();
	} else
		ResMessageBox(Handle,1,6,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
}
//---------------------------------------------------------------------------
int TFormMain::GetCompColorIcon(const MComputersItem *Comp_)
{
	int icon;

	if ( Comp_==nullptr ) return -1;
	switch(Comp_->Color)
	{
		case mgcAqua:   icon=0; break;
		case mgcGreen:  icon=1; break;
		case mgcRed:    icon=2; break;
		case mgcYellow: icon=3; break;
		default:        icon=-1; break;
	}

	return icon;
}
//---------------------------------------------------------------------------

