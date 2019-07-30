//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <math.h>
#include <shellapi.h>
#include "UnitFormMain.h"
#include "UnitFormMainOptions.h"
#include "UnitFormAbout.h"
#include "UnitCommon.h"
#include "UnitFormClientConfig.h"
#include "UnitFormGamesList.h"
#include "UnitFormPasswords.h"
#include "UnitModeChange.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
AnsiString WinDir;
int XScrSize=-1, YScrSize=-1;
typedef DWORD RegServProc(DWORD,DWORD);
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
    HANDLE process;
    HINSTANCE Library;
    RegServProc *RegServProcFunc;
    char windir[512];

    GetWindowsDirectory(windir,512);
    WinDir=windir;
    // ������������� ��������� ���������
    process=GetCurrentProcess();
    SetPriorityClass(process,HIGH_PRIORITY_CLASS);
    // �������� ��������� �� CTRL+ALT+DEL
    if ( (Library=LoadLibrary("KERNEL32.DLL"))>=(HINSTANCE)32 )
    {
        RegServProcFunc=(RegServProc*)GetProcAddress(Library,"RegisterServiceProcess");
        if ( RegServProcFunc!=NULL ) RegServProcFunc(NULL,1);
        FreeLibrary(Library);
    }
    // ��������� ���������
    if ( Options.Load() )
    {
        Application->MessageBox("����������� ���������� ���� �������� 'TmLockSr.cfg' ! ������������ ���� ���� ��� ������ �������� ��� � ������� �������� ��������� �������.",
            "�������� - Time Locker Server",MB_OK+MB_ICONWARNING);
        Options.ClientNumber=0;
        Options.TariffNumber=0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    // �������������� ������ �����������
    CreateComputerList();
    // ��������� ������ ������ �������
    CreateTariffsList();
    // ����������� ���������� ���� ���������� �������
    SetProgressBars();
    // ��������� ��� ��������������
    try
    {
        RichEditAdminLog->Lines->LoadFromFile(WinDir+"\\AdminLog.dat");
    } catch ( EFOpenError *Error ) {;}
    // ��� ������ ���������
    Application->OnHint=ShowHint;
    TimerTimeTimer(TimerTime);
    TimerTime->Enabled=true;
    RichEditAdminLog->Tag=0;
    AddAdminLog(AdminLogProgramStart,"",0);
    EditNikName->Text="�����";
    // ������������� ����� ������
    SetMode(Options.Mode);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormResize(TObject *Sender)
{
    if ( ClientWidth<680 ) ClientWidth=680;
    if ( ClientHeight<440 ) ClientHeight=440;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if ( !Tag ) CanClose=caNone;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    TimerTime->Enabled=false;
    ListViewComputers->Tag=0;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        try { NMMsg->Disconnect(); NMMsg->Abort(); } catch ( ESockError *Error ) {;}
        try { NMMSGServ->Disconnect(); NMMSGServ->Abort(); } catch ( ESockError *Error ) {;}
    }
    // ��������� ��� ������
    RichEditAdminLog->Tag=0;
    AddAdminLog(AdminLogProgramClose,"",0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ShowHint(TObject *Sender)
{
    StatusBar->SimpleText=Application->Hint;
    StatusBar->SimplePanel=(Application->Hint!="");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimeTimer(TObject *Sender)
{
    double time;
    int hours, minutes, seconds;

    time=(double)Time();
    hours=(int)(time*24.); time-=hours/24.;
    minutes=(int)(time*24*60); time-=minutes/(24.*60.);
    seconds=(int)(time*24*60*60);
    LabelHours->Caption=IntToStr(hours/10)+IntToStr(hours%10);
    LabelMinutes->Caption=IntToStr(minutes/10)+IntToStr(minutes%10);
    LabelSeconds->Caption=IntToStr(seconds/10)+IntToStr(seconds%10);
    // ������������� ������� ���� ���������
    if ( (XScrSize!=Screen->Width)||(YScrSize!=Screen->Height) )
    {
        XScrSize=Screen->Width; YScrSize=Screen->Height;
        Left=0; Top=0;
        Width=XScrSize; Height=YScrSize;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnRefreshAllClick(TObject *Sender)
{
    if ( BitBtnRefresh->Tag ) return;
    BitBtnRefresh->Tag=1;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        ProgressBarRefresh->Position=i+1;
        if ( Options.Clients[i].NotInUse ) continue;
        Options.Clients[i].Mode=ClientModeRequestMode;
        SetListViewClientMode(i);
        if ( SendNetMessage(i,NetMessageRequestMode,"")<0 )
        {
            Options.Clients[i].Mode=ClientModeNotResponse;
            SetListViewClientMode(i);
        }
    }
    ProgressBarRefresh->Position=0;
    BitBtnRefresh->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnRefreshClick(TObject *Sender)
{
    TListItem *ListItem;

    if ( BitBtnRefresh->Tag ) return;
    BitBtnRefresh->Tag=1;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        ProgressBarRefresh->Position=i+1;
        ListItem=ListViewComputers->Items->Item[i];
        if ( (!ListItem->Selected)||(Options.Clients[i].NotInUse) ) continue;
        Options.Clients[i].Mode=ClientModeRequestMode;
        SetListViewClientMode(i);
        if ( SendNetMessage(i,NetMessageRequestMode,"")<0 )
        {
            Options.Clients[i].Mode=ClientModeNotResponse;
            SetListViewClientMode(i);
        }
    }
    ProgressBarRefresh->Position=0;
    BitBtnRefresh->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnRunClick(TObject *Sender)
{
    int a, time1, time2;
    TListItem *ListItem;

    if ( BitBtnRefresh->Tag ) return;
    BitBtnRefresh->Tag=1;
    a=ComboBoxSetTime->ItemIndex-1;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        ProgressBarRun->Position=i+1;
        ListItem=ListViewComputers->Items->Item[i];
        if ( (!ListItem->Selected)||(Options.Clients[i].NotInUse) ) continue;
        if ( a<0 )
        {
            time2=0;
        } else
        {
            if ( Options.Tariffs[a].Begin>=0 )
            {
                time1=(int)((double)Time()*24*60);
                time2=Options.Tariffs[a].Begin+Options.Tariffs[a].Size;
                if ( time2>=(24*60) ) time2-=24*60;
                if ( (time1<Options.Tariffs[a].Begin)&&(time1>=time2) ) continue;
                time1-=Options.Tariffs[a].Begin;
                if ( time1<0 ) time1+=24*60;
                time2=Options.Tariffs[a].Size-time1;
            } else time2=Options.Tariffs[a].Size;
        }
        if ( SendNetMessage(i,NetMessageWork,IntToStr(time2))<0 )
        {
            Options.Clients[i].Mode=ClientModeNotResponse;
            SetListViewClientMode(i);
        }
    }
    ProgressBarRun->Position=0;
    ComboBoxSetTime->ItemIndex=0;
    BitBtnRefresh->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnAddTimeClick(TObject *Sender)
{
    int a;
    TListItem *ListItem;

    if ( BitBtnRefresh->Tag ) return;
    if ( (a=ComboBoxSetTime->ItemIndex-1)<0 ) return;
    BitBtnRefresh->Tag=1;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        ProgressBarRun->Position=i+1;
        ListItem=ListViewComputers->Items->Item[i];
        if ( (!ListItem->Selected)||(Options.Clients[i].NotInUse) ) continue;
        if ( Options.Tariffs[a].Begin>=0 ) continue;
        if ( SendNetMessage(i,NetMessageAddTime,IntToStr(Options.Tariffs[a].Size))<0 )
        {
            Options.Clients[i].Mode=ClientModeNotResponse;
            SetListViewClientMode(i);
        }
    }
    ProgressBarRun->Position=0;
    ComboBoxSetTime->ItemIndex=0;
    BitBtnRefresh->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnPauseClick(TObject *Sender)
{
    TListItem *ListItem;

    if ( BitBtnRefresh->Tag ) return;
    BitBtnRefresh->Tag=1;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        ProgressBarStop->Position=i+1;
        ListItem=ListViewComputers->Items->Item[i];
        if ( (!ListItem->Selected)||(Options.Clients[i].NotInUse) ) continue;
        if ( SendNetMessage(i,NetMessageWorkPause,"")<0 )
        {
            Options.Clients[i].Mode=ClientModeNotResponse;
            SetListViewClientMode(i);
        }
    }
    ProgressBarStop->Position=0;
    BitBtnRefresh->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnStopClick(TObject *Sender)
{
    TListItem *ListItem;

    if ( BitBtnRefresh->Tag ) return;
    BitBtnRefresh->Tag=1;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        ProgressBarStop->Position=i+1;
        ListItem=ListViewComputers->Items->Item[i];
        if ( (!ListItem->Selected)||(Options.Clients[i].NotInUse) ) continue;
        if ( SendNetMessage(i,NetMessageLock,"")<0 )
        {
            Options.Clients[i].Mode=ClientModeNotResponse;
            SetListViewClientMode(i);
        }
    }
    ProgressBarStop->Position=0;
    BitBtnRefresh->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnAdditionalsClick(TObject *Sender)
{
    PopupMenuAdditionals->Popup(Left+(Width-ClientWidth)+PanelButtons->Left+BitBtnAdditionals->Left+10,
        Top+(Height-ClientHeight)+PanelButtons->Top+BitBtnAdditionals->Top+10);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnHelpClick(TObject *Sender)
{
    Application->HelpCommand ( HELP_FINDER, 0 );
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnAboutClick(TObject *Sender)
{
    FormAbout->Left=Left+(Width-FormAbout->Width)/2;
    FormAbout->Top=Top+(Height-FormAbout->Height)/2;
    FormAbout->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BitBtnExitClick(TObject *Sender)
{
    Tag=1;
    Close();
    ExitWindowsEx(EWX_REBOOT,0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NMMSGServMSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
    char message[16+1+16+1+100+1], *curpos=message, *password;
    int size, passwsize, Computer;

    NMMSGServ->TimeOut=10000;

    if ( (sFrom.Length()>15)||(sMsg=="")||(sMsg.Length()>(16+1+16+1+100)) ) return;
    // ���� �� IP-������ ���������� ��������� ��������� �������
    Computer=-1;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
        if ( Options.Clients[i].Address==sFrom ) { Computer=i; break; }
    // ���� ������ ������� ���, �� �������
    if ( Computer<0 ) return;

    // �������������� ���������
    password=Options.PasswordToConnect.c_str();
    passwsize=Options.PasswordToConnect.Length();
    size=sMsg.Length(); memcpy(message,sMsg.c_str(),size); message[size]='\0';
    for ( int i=0, j=passwsize-1; i<size; i++, j=j?(passwsize-1):(j-1) )
    {
        char a;
        message[i]=(unsigned char)(a=(char)(message[i]-password[j]))>
            (unsigned char)message[i] ? (char)(a-1):a;
    }

    curpos=message;
    // ��������� �� ��������� ������ � ��������� ���
    if ( (password=strchr(curpos,'\n'))==NULL ) return;
    *password='\0';
    if ( strcmp(curpos,Options.PasswordToConnect.c_str()) ) return;
    curpos=password+1;

    // ��������� ���������
    if ( (password=strchr(curpos,'\n'))==NULL ) return;
    *password='\0';
    if ( !strcmp(curpos,"Mode") )
    {
        // ���������� � ��������� �������
        int a, b, c;
        curpos=password+1;
        if ( (password=strchr(curpos,'\n'))==NULL ) return;
        *password='\0';
        try { a=StrToInt(curpos); } catch ( EConvertError *Error ) { return; }
        if ( (a!=ClientModeNotLock)&&(a!=ClientModeWork)&&(a!=ClientModeWorkPause)&&(a!=ClientModeLock) ) return;
        curpos=password+1;
        if ( (password=strchr(curpos,'\n'))==NULL ) return;
        *password++='\0';
        try { b=StrToInt(curpos); } catch ( EConvertError *Error ) { return; }
        try { c=StrToInt(password); } catch ( EConvertError *Error ) { return; }
        if ( (c<0)||(c>(23*60+59)) ) return;
        Options.Clients[Computer].Mode=a;
        Options.Clients[Computer].TimeSize=b;
        Options.Clients[Computer].TimeToEnd=c;
        SetListViewClientMode(Computer);
        RefreshStatistic();
    } else if ( !strcmp(curpos,"Message") )
    {
        // ��������� �� �������
        AddAdminLog(AdminLogClientMessage,password+1,Computer);
    } else if ( !strcmp(curpos,"Warning") )
    {
        // ��������� �� ������� � ������� ������
        Beep(); AddAdminLog(AdminLogClientWarning,"",Computer);
    }
}
//---------------------------------------------------------------------------
int __fastcall TFormMain::SendNetMessage(int Computer, int Type, AnsiString Message)
{
    char message[16+1+2+1+16+1+100+1], *curpos=message, *password;
    bool addmsg=false;
    int size, passwsize;

    if ( (size=Message.Length())>100 ) return -1;
    // ��������� ���������
    password=Options.PasswordToConnect.c_str();
    passwsize=Options.PasswordToConnect.Length();
    *(curpos=(char*)memcpy(curpos,password,passwsize)+passwsize)++='\n';
    if ( Options.Clients[Computer].Number>=10 )
        curpos=(char*)memcpy(curpos,IntToStr(Options.Clients[Computer].Number).c_str(),2)+2;
    else curpos=(char*)memcpy(curpos,IntToStr(Options.Clients[Computer].Number).c_str(),1)+1;
    curpos=(char*)memcpy(curpos,"\n",1)+1;
    switch ( Type )
    {
        case NetMessageAdminMessage:
            curpos=(char*)memcpy(curpos,"Message\n",8)+8; addmsg=true;
            break;
        case NetMessageRequestMode:
            curpos=(char*)memcpy(curpos,"RequestMode\n",12)+12;
            break;
        case NetMessageWork:
            curpos=(char*)memcpy(curpos,"Work\n",5)+5; addmsg=true;
            break;
        case NetMessageWorkPause:
            curpos=(char*)memcpy(curpos,"WorkPause\n",10)+10;
            break;
        case NetMessageLock:
            curpos=(char*)memcpy(curpos,"Lock\n",5)+5;
            break;
        case NetMessageReboot:
            curpos=(char*)memcpy(curpos,"Reboot\n",7)+7;
            break;
        case NetMessageShutdown:
            curpos=(char*)memcpy(curpos,"Shutdown\n",9)+9;
            break;
        case NetMessageAddTime:
            curpos=(char*)memcpy(curpos,"AddTime\n",8)+8; addmsg=true;
            break;
        case NetMessageNotLock:
            curpos=(char*)memcpy(curpos,"NotLock\n",8)+8;
            break;
        case NetMessageNormal:
            curpos=(char*)memcpy(curpos,"Normal\n",7)+7;
            break;
        default: return -1;
    }
    if ( addmsg )
    {
        size=Message.Length();
        curpos=(char*)memcpy(curpos,Message.c_str(),size)+size;
    }
    *curpos='\0';
    size=curpos-message;

    // ������� ���������
    for ( int i=0, j=passwsize-1; i<size; i++, j=j?(passwsize-1):(j-1) )
    {
        char a;
        message[i]=(unsigned char)(a=(char)(message[i]+password[j]))<
            (unsigned char)message[i] ? (char)(a+1):a;
    }

    // ���������� ���������
    try
    {
        NMMsg->FromName=NMMsg->LocalIP;
        NMMsg->Host=Options.Clients[Computer].Address;
        NMMsg->TimeOut=5000;
        NMMsg->PostIt(message);
    } catch ( ESockError *Error ) { NMMsg->Disconnect(); NMMsg->Abort(); return -1; }

    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::CreateComputerList()
{
    TListItem *NewItem;

    ListViewComputers->Tag=0;
    ListViewComputers->Items->Clear();
    ComboBoxNeedFreeComputers->Items->Clear();
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        NewItem=ListViewComputers->Items->Add();
        NewItem->SubItems->Add ( IntToStr(Options.Clients[i].Number) );
        NewItem->SubItems->Add ( Options.Clients[i].Address );
        NewItem->SubItems->Add ( "" );
        NewItem->SubItems->Add ( "" );
        NewItem->SubItems->Add ( "" );
        NewItem->SubItems->Add ( "" );
        NewItem->Checked=!Options.Clients[i].NotInUse;
        ComboBoxNeedFreeComputers->Items->Add(IntToStr(i+1));
    }
    ComboBoxNeedFreeComputers->ItemIndex=0;
    ListViewComputers->Tag=1;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::CreateTariffsList()
{
    AnsiString String;
    int a, b;

    ComboBoxSetTime->Items->Clear();
    ComboBoxSetTime->Items->Add("����� �� ������");
    for ( int i=0, j=Options.TariffNumber; i<j; i++ )
    {
        a=Options.Tariffs[i].Begin;
        if ( a>=0 )
        {
            String="� "+IntToStr(a/60)+":";
            b=a%60; String+=IntToStr(b/10)+IntToStr(b%10);
            a+=Options.Tariffs[i].Size; if ( a>=(24*60) ) a-=24*60;
            String+=" �� "+IntToStr(a/60)+":";
            b=a%60; String+=IntToStr(b/10)+IntToStr(b%10);
        }
        else
        {
            a=Options.Tariffs[i].Size;
            String="��";
            if ( (b=a/60)!=0 ) String+=" "+IntToStr(b)+" ���.";
            if ( (b=a%60)!=0 ) String+=" "+IntToStr(b/10)+IntToStr(b%10)+" ���.";
        }
        ComboBoxSetTime->Items->Add(String);
    }
    ComboBoxSetTime->ItemIndex=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetProgressBars()
{
    ProgressBarRefresh->Max=Options.ClientNumber;
    ProgressBarRun->Max=Options.ClientNumber;
    ProgressBarStop->Max=Options.ClientNumber;
    ProgressBarAdditionals->Max=Options.ClientNumber;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::AddAdminLog(int Type, AnsiString Message, int Computer)
{
    int Size=8; TColor Color=clBlack;
    double time;
    int hours, minutes, seconds;

    // ������������� ��������� �������� ���������� ���������� ������
    RichEditAdminLog->SelLength=0;
    RichEditAdminLog->SelStart=RichEditAdminLog->Lines->Text.Length();
    RichEditAdminLog->SelAttributes->Style=RichEditAdminLog->SelAttributes->Style>>fsBold;
    // ��������� ���������
    switch ( Type )
    {
        case AdminLogAdminMessage:
            Size=9; Color=clMaroon;
            Message="��� �"+IntToStr(Options.Clients[Computer].Number)+": "+Message;
            break;
        case AdminLogClientMessage:
            Size=9; Color=clNavy;
            Message="�"+IntToStr(Options.Clients[Computer].Number)+": "+Message;
            break;
        case AdminLogClientWarning:
            RichEditAdminLog->SelAttributes->Style=RichEditAdminLog->Font->Style<<fsBold;
            Size=10; Color=clRed;
            Message="�"+IntToStr(Options.Clients[Computer].Number)+" ������� ������ !!!";
            break;
        case AdminLogProgramStart:
            RichEditAdminLog->SelAttributes->Style=RichEditAdminLog->SelAttributes->Style<<fsBold;
            Size=10; Color=clBlack;
            Message="������ �������.";
            break;
        case AdminLogProgramClose:
            RichEditAdminLog->SelAttributes->Style=RichEditAdminLog->SelAttributes->Style<<fsBold;
            Size=10; Color=clBlack;
            Message="������ ����������.";
            break;
    }
    time=Time();
    hours=(int)(time*24); minutes=(int)((time*24-hours)*60); seconds=(int)((time*24*60-hours*60-minutes)*60);
    Message="["+IntToStr(hours/10)+IntToStr(hours%10)+":"+IntToStr(minutes/10)+IntToStr(minutes%10)+
        ":"+IntToStr(seconds/10)+IntToStr(seconds%10)+"] "+Message;
    // ������������� ����� ��������� ���������� ������
    RichEditAdminLog->SelAttributes->Size=Size;
    RichEditAdminLog->SelAttributes->Color=Color;
    RichEditAdminLog->SelAttributes->Charset=RUSSIAN_CHARSET;
    // ������� �����
    RichEditAdminLog->Lines->Add(Message);
    // ������� ������ ������ �� ����
    while ( RichEditAdminLog->Lines->Count>60 ) RichEditAdminLog->Lines->Delete(0);
    // ������� ���������� ���� � ����� � ����� ������
    RichEditAdminLog->SelStart=RichEditAdminLog->Lines->Text.Length();
    ::SendMessage(RichEditAdminLog->Handle,WM_VSCROLL,SB_BOTTOM,0);
    ::SendMessage(RichEditAdminLog->Handle,WM_VSCROLL,SB_PAGEUP,0);
    // ��������� �� ���� �� �������� ��� � ����
    if ( !RichEditAdminLog->Tag )
    {
        RichEditAdminLog->Tag=10;
        try
        {
            RichEditAdminLog->Lines->SaveToFile(WinDir+"\\AdminLog.dat");
        } catch ( EFCreateError *Error ) {;}
    } else RichEditAdminLog->Tag--;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetListViewClientMode(int ClientNumber)
{
    TStrings *SubItems;
    AnsiString TextMode;
    MClient *Client;
    double EndTime;
    int Hours, Minutes;

    Client=&Options.Clients[ClientNumber];
    SubItems=ListViewComputers->Items->Item[ClientNumber]->SubItems;
    // ��������� ����������� ����������
    switch ( Client->Mode )
    {
        case ClientModeNotLock: TextMode="���������"; break;
        case ClientModeWork: TextMode="������"; break;
        case ClientModeWorkPause: TextMode="������������� !"; break;
        case ClientModeLock: TextMode="��������"; break;
        case ClientModeRequestMode: TextMode="����� ���������..."; break;
        case ClientModeNotResponse: TextMode="�� ��������..."; break;
        case ClientModeNotUsed: TextMode="=��� ������=";
        default: break;
    }
    if ( ListViewComputers->Items->Item[ClientNumber]->Checked ) SubItems->Strings[2]=TextMode;
    else SubItems->Strings[2]="[ "+TextMode+" ]";
    if ( (Client->Mode==ClientModeWork)||(Client->Mode==ClientModeWorkPause) )
    {
        // �����
        Minutes=Client->TimeSize;
        TextMode="��";
        if ( (Hours=Minutes/60)!=0 ) TextMode+=" "+IntToStr(Hours)+" ���.";
        if ( (Hours=Minutes%60)!=0 ) TextMode+=" "+IntToStr(Hours/10)+IntToStr(Hours%10)+" ���.";
        SubItems->Strings[3]=TextMode;
        // ����� ��������� ������
        EndTime=(double)Time()+Client->TimeToEnd/(24.*60.); EndTime-=(int)EndTime;
        Hours=(int)(EndTime*24.); Minutes=(int)((EndTime*24.-Hours)*60);
        SubItems->Strings[4]=IntToStr(Hours)+":"+IntToStr(Minutes/10)+IntToStr(Minutes%10);
        // ���������� �����
        Hours=Client->TimeToEnd/60; Minutes=Client->TimeToEnd%60;
        SubItems->Strings[5]=IntToStr(Hours/10)+IntToStr(Hours%10)+":"+
            IntToStr(Minutes/10)+IntToStr(Minutes%10);
    } else
    {
        SubItems->Strings[3]="";
        SubItems->Strings[4]="";
        SubItems->Strings[5]="";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::EditMessageKeyPress(TObject *Sender, char &Key)
{
    AnsiString message;
    TListItem *ListItem;
    bool send=false;

    if ( Key!=0x0D ) return;
    if ( EditMessage->Text=="" ) return;
    if ( BitBtnRefresh->Tag ) return;
    BitBtnRefresh->Tag=1;

    EditMessage->Enabled=false;
    EditNikName->Enabled=false;
    message="("+EditNikName->Text+") "+EditMessage->Text;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        ListItem=ListViewComputers->Items->Item[i];
        if ( (!ListItem->Selected)||(Options.Clients[i].NotInUse) ) continue;
        if ( SendNetMessage(i,NetMessageAdminMessage,message)<0 )
        {
            Options.Clients[i].Mode=ClientModeNotResponse;
            SetListViewClientMode(i);
        } else
        {
            AddAdminLog(AdminLogAdminMessage,message,i);
            send=true;
        }
    }
    if ( send ) EditMessage->Text="";
    EditNikName->Enabled=true;
    EditMessage->Enabled=true;
    if ( EditMessage->CanFocus() ) EditMessage->SetFocus();
    BitBtnRefresh->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PanelMessageResize(TObject *Sender)
{
    EditNikName->Left=PanelMessage->Width-4-77;
    EditMessage->Width=EditNikName->Left-4*2;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::RefreshStatistic()
{
    int FreeAre, LocalMin, LocalMinNumber;
    AnsiString String;

    // ������������ ���������� ��������� �����������
    FreeAre=0;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        if ( (Options.Clients[i].NotInUse)||
            (Options.Clients[i].Mode!=ClientModeLock) ) continue;
        FreeAre++;
    }
    // ������� ����������
    LabelFreeAre->Caption=IntToStr(FreeAre);
    // ���������� �����, ����� ������� ����� ����������� ���������� �����������
    for ( int i=0, j=Options.ClientNumber; i<j; i++ ) Options.Clients[i].StatUsed=false;
    FreeAre=0;
    for ( int i=0, j=ComboBoxNeedFreeComputers->ItemIndex+1; i<j; i++ )
    {
        LocalMin=24*60;
        for ( int k=0, l=Options.ClientNumber; k<l; k++ )
        {
            if ( (Options.Clients[k].StatUsed)||
                (Options.Clients[k].TimeToEnd>=LocalMin) ) continue;
            if ( (Options.Clients[k].Mode==ClientModeWork)||
                (Options.Clients[k].Mode==ClientModeWorkPause)||
                (Options.Clients[k].Mode==ClientModeLock) )
            {
                LocalMin=Options.Clients[k].TimeToEnd;
                LocalMinNumber=k;
            }
        }
        if ( LocalMin!=(24*60) ) Options.Clients[LocalMinNumber].StatUsed=true;
        if ( FreeAre<LocalMin ) FreeAre=LocalMin;
    }
    // ������� ���������
    if ( FreeAre<(24*60) )
    {
        if ( (LocalMin=FreeAre/60)!=0 ) String+=IntToStr(LocalMin)+" ���. ";
        LocalMin=FreeAre%60;
        String+=IntToStr(LocalMin/10)+IntToStr(LocalMin%10)+" ���.";
    }
    else String="???";
    LabelFreeAfter->Caption=String;
    String="";
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        if ( !Options.Clients[i].StatUsed ) continue;
        String+=IntToStr(Options.Clients[i].Number)+",";
    }
    LabelFreeNumbers->Caption=String;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NUncheckedClick(TObject *Sender)
{
    TListItem *ListItem;

    if ( BitBtnRefresh->Tag ) return;
    BitBtnRefresh->Tag=1;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        ListItem=ListViewComputers->Items->Item[i];
        if ( !(ListItem->Selected) ) continue;
        ListItem->Checked=false;
    }
    BitBtnRefresh->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NCheckedClick(TObject *Sender)
{
    TListItem *ListItem;

    if ( BitBtnRefresh->Tag ) return;
    BitBtnRefresh->Tag=1;
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        ListItem=ListViewComputers->Items->Item[i];
        if ( !(ListItem->Selected) ) continue;
        ListItem->Checked=true;
    }
    BitBtnRefresh->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NRebootClick(TObject *Sender)
{
    TListItem *ListItem;
    int command;

    if ( BitBtnRefresh->Tag ) return;
    BitBtnRefresh->Tag=1;
    // ���������� ����� ������� ���������
    if ( (TObject*)Sender==NReboot ) command=NetMessageReboot;
    else if ( (TObject*)Sender==NShutdown ) command=NetMessageShutdown;
    else if ( (TObject*)Sender==NNotLock ) command=NetMessageNotLock;
    else if ( (TObject*)Sender==NNormal ) command=NetMessageNormal;
    else { BitBtnRefresh->Tag=0; return; }
    // �������� �������� ��� ��������
    for ( int i=0, j=Options.ClientNumber; i<j; i++ )
    {
        ProgressBarAdditionals->Position=i+1;
        ListItem=ListViewComputers->Items->Item[i];
        if ( (!ListItem->Selected)||(Options.Clients[i].NotInUse) ) continue;
        if ( SendNetMessage(i,command,"")<0 )
        {
            Options.Clients[i].Mode=ClientModeNotResponse;
            SetListViewClientMode(i);
        }
    }
    ProgressBarAdditionals->Position=0;
    BitBtnRefresh->Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NClientConfigClick(TObject *Sender)
{
    FormClientConfig->Left=Left+(Width-FormClientConfig->Width)/2;
    FormClientConfig->Top=Top+(Height-FormClientConfig->Height)/2;
    FormClientConfig->WindowState=wsNormal;
    FormClientConfig->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NGamesListClick(TObject *Sender)
{
    FormGamesList->Left=Left+(Width-FormGamesList->Width)/2;
    FormGamesList->Top=Top+(Height-FormGamesList->Height)/2;
    FormGamesList->WindowState=wsNormal;
    FormGamesList->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NPasswordsClick(TObject *Sender)
{
    FormPasswords->Left=Left+(Width-FormPasswords->Width)/2;
    FormPasswords->Top=Top+(Height-FormPasswords->Height)/2;
    FormPasswords->WindowState=wsNormal;
    FormPasswords->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NMainOptionsClick(TObject *Sender)
{
    FormMainOptions->Left=Left+(Width-FormMainOptions->Width)/2;
    FormMainOptions->Top=Top+(Height-FormMainOptions->Height)/2;
    FormMainOptions->WindowState=wsNormal;
    FormMainOptions->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewComputersChange(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
    if ( (!ListViewComputers->Tag)||(Change!=ctState) ) return;
    Options.Clients[Item->Index].NotInUse=!Item->Checked;
    if ( Item->Checked )
    {
        if ( Options.Clients[Item->Index].Mode==ClientModeNotUsed )
           Options.Clients[Item->Index].Mode=-1;
        else return;
    } else
    {
        Options.Clients[Item->Index].Mode=ClientModeNotUsed;
        Options.Clients[Item->Index].TimeToEnd=24*60;
    }
    SetListViewClientMode(Item->Index);
    RefreshStatistic();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ComboBoxNeedFreeComputersChange(TObject *Sender)
{
    RefreshStatistic();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetMode(int Mode)
{
    static STARTUPINFO startupinfoprogram;
    static PROCESS_INFORMATION processinfoprogram;

    switch (Mode)
    {
        case ServerModeNotLock:
            NNotLock->Enabled=true;
            NClientConfig->Enabled=true;
            NGamesList->Enabled=true;
            NMainOptions->Enabled=true;
            BitBtnHelp->Enabled=true;
            // ��������� "���������"
            startupinfoprogram.lpReserved=NULL;
            CreateProcess(NULL,(WinDir+"\\explorer.exe").c_str(),NULL,NULL,false,
                CREATE_DEFAULT_ERROR_MODE+NORMAL_PRIORITY_CLASS,NULL,WinDir.c_str(),
                &startupinfoprogram,&processinfoprogram);
            break;
        case ServerModeLock:
            NNotLock->Enabled=false;
            NClientConfig->Enabled=false;
            NGamesList->Enabled=false;
            NMainOptions->Enabled=false;
            BitBtnHelp->Enabled=false;
            break;
        default: return;
    }
    Options.Mode=Mode;
    Options.Save();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NOpenCloseClick(TObject *Sender)
{
    FormModeChange->Left=Left+(Width-FormModeChange->Width)/2;
    FormModeChange->Top=Top+(Height-FormModeChange->Height)/2;
    FormModeChange->WindowState=wsNormal;
    FormModeChange->Show();
}
//---------------------------------------------------------------------------

