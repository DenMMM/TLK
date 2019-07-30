//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormClientConfig.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TFormClientConfig *FormClientConfig;
//---------------------------------------------------------------------------
__fastcall TFormClientConfig::TFormClientConfig(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormClientConfig::EditNumberComputerEnter(TObject *Sender)
{
    EditNumberComputer->Tag=StrToInt(EditNumberComputer->Text);
}
//---------------------------------------------------------------------------
void __fastcall TFormClientConfig::EditNumberComputerExit(TObject *Sender)
{
    int a;
    try { a=StrToInt(EditNumberComputer->Text); }
    catch ( EConvertError *Error ) { EditNumberComputer->Text=IntToStr(EditNumberComputer->Tag); }
    if ( (a<1)||(a>99) ) EditNumberComputer->Text=IntToStr(EditNumberComputer->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TFormClientConfig::BitBtnOpenClick(TObject *Sender)
{
    MClientOptions ClientOptions;

    if ( !OpenDialog->Execute() ) return;
    if ( ClientOptions.Load(OpenDialog->FileName)<0 )
    {
        Application->MessageBox("Не удалось считать файл настроек или его содержимое некорректно!",
            "Ошибка - Time Locker Server",MB_OK+MB_ICONERROR);
        return;
    }
    EditServerAddress->Text=ClientOptions.ServerAddress;
    EditPasswordToConnect->Text=ClientOptions.PasswordToConnect;
    EditNumberComputer->Text=IntToStr(ClientOptions.NumberComputer);
    EditOptionsPassword->Text=ClientOptions.OptionsPassword;
    EditOptionsPasswordConfirm->Text=ClientOptions.OptionsPassword;
}
//---------------------------------------------------------------------------
void __fastcall TFormClientConfig::BitBtnSaveClick(TObject *Sender)
{
    MClientOptions ClientOptions;

    if ( (!CheckBoxServerAuto->Checked)&&(EditServerAddress->Text=="") )
    {
        EditServerAddress->SetFocus(); return;
    }
    if ( EditPasswordToConnect->Text=="" )
    {
        EditPasswordToConnect->SetFocus(); return;
    }
    if ( (EditOptionsPassword->Text!=EditOptionsPasswordConfirm->Text)||
        (EditOptionsPassword->Text=="") )
    {
        EditOptionsPasswordConfirm->Text="";
        EditOptionsPassword->SetFocus();
        return;
    }
    ClientOptions.ServerAddress=EditServerAddress->Text;
    ClientOptions.ServerAuto=CheckBoxServerAuto->Checked;
    ClientOptions.PasswordToConnect=EditPasswordToConnect->Text;
    ClientOptions.NumberComputer=StrToInt(EditNumberComputer->Text);
    ClientOptions.OptionsPassword=EditOptionsPassword->Text;
    if ( ((TObject*)Sender)==BitBtnSave )
    {
        if ( !SaveDialog->Execute() ) return;
        if ( ClientOptions.Save(SaveDialog->FileName)<0 )
        {
            Application->MessageBox("Не удалось сохранить файл настроек !",
                "Ошибка - Time Locker Server",MB_OK+MB_ICONERROR);
        }
    } else
    {
        if ( ClientOptions.Save(WinDir+"\\TmLockCl.cfg")<0 )
        {
            Application->MessageBox("Не удалось сохранить файл настроек по умолчанию !",
                "Ошибка - Time Locker Server",MB_OK+MB_ICONERROR);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormClientConfig::BitBtnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormClientConfig::FormDeactivate(TObject *Sender)
{
    WindowState=wsMinimized;
}
//---------------------------------------------------------------------------
void __fastcall TFormClientConfig::FormShow(TObject *Sender)
{
    MClientOptions ClientOptions;

    if ( ClientOptions.Load(WinDir+"\\TmLockCl.cfg")<0 )
    {
        ClientOptions.ServerAddress="127.0.0.1";
        ClientOptions.ServerAuto=true;
        ClientOptions.PasswordToConnect="ABCDEFGHIJKLMNOP";
        ClientOptions.NumberComputer=99;
        ClientOptions.OptionsPassword="admin";
    }
    EditServerAddress->Text=ClientOptions.ServerAddress;
    CheckBoxServerAuto->Checked=ClientOptions.ServerAuto;
    EditPasswordToConnect->Text=ClientOptions.PasswordToConnect;
    EditNumberComputer->Text=IntToStr(ClientOptions.NumberComputer);
    EditOptionsPassword->Text=ClientOptions.OptionsPassword;
    EditOptionsPasswordConfirm->Text=ClientOptions.OptionsPassword;

    if ( EditServerAddress->Enabled ) ActiveControl=EditServerAddress;
    else ActiveControl=EditPasswordToConnect;
}
//---------------------------------------------------------------------------
void __fastcall TFormClientConfig::CheckBoxServerAutoClick(TObject *Sender)
{
    if ( CheckBoxServerAuto->Checked )
    {
        Label1->Enabled=false;
        EditServerAddress->Enabled=false;
        EditServerAddress->Text="127.0.0.1";
        Label2->Enabled=false;
        EditNumberComputer->Enabled=false;
        EditNumberComputer->Text="99";
    } else
    {
        Label1->Enabled=true;
        EditServerAddress->Enabled=true;
        Label2->Enabled=true;
        EditNumberComputer->Enabled=true;
    }
}
//---------------------------------------------------------------------------

