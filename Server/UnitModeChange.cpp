//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitModeChange.h"
#include "UnitCommon.h"
#include "UnitFormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormModeChange *FormModeChange;
//---------------------------------------------------------------------------
__fastcall TFormModeChange::TFormModeChange(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormModeChange::FormShow(TObject *Sender)
{
    EditCurrentPassword->Text="";
    EditNewPassword->Text="";
    EditConfirmPassword->Text="";
    ActiveControl=EditCurrentPassword;
}
//---------------------------------------------------------------------------
void __fastcall TFormModeChange::FormDeactivate(TObject *Sender)
{
    WindowState=wsMinimized;
}
//---------------------------------------------------------------------------
void __fastcall TFormModeChange::BitBtnOkClick(TObject *Sender)
{
    // ��������� ������������ �������� ������
    if ( EditCurrentPassword->Text!=Options.AdminPassword )
    {
        EditCurrentPassword->Text="";
        EditCurrentPassword->SetFocus();
        return;
    }
    // ��������� ������� ������ ������
    if ( (EditNewPassword->Text!="")||(EditConfirmPassword->Text!="") )
    {
        if ( EditNewPassword->Text=="" )
        {
            EditNewPassword->SetFocus();
            return;
        } else if ( EditNewPassword->Text!=EditConfirmPassword->Text )
        {
            EditConfirmPassword->Text="";
            EditConfirmPassword->SetFocus();
            return;
        }
        Options.AdminPassword=EditNewPassword->Text;
        Options.Save();
    } else
    {
        // �������� ����� ������ ���������
        if ( Options.Mode==ServerModeLock ) FormMain->SetMode(ServerModeNotLock);
        else if ( Options.Mode==ServerModeNotLock ) FormMain->SetMode(ServerModeLock);
    }
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormModeChange::BitBtnCancelClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

