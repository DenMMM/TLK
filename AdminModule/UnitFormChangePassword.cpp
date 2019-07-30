//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormChangePassword.h"
#include "UnitFormMain.h"
#include "UnitOptionsLoadSave.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormChangePassword *FormChangePassword;
//---------------------------------------------------------------------------
__fastcall TFormChangePassword::TFormChangePassword(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormChangePassword::FormShow(TObject *Sender)
{
    Tag=false;
    // Заполняем список пользователей
    ComboBoxLogin->Items->Add("Сисадмин");
    for ( MUser *User=(MUser*)Users->FirstItem; User;
        User=(MUser*)User->NextItem ) ComboBoxLogin->Items->Add(User->Login);
    // Формируем список для выбора длины пароля
    for ( int i=6; i<17; i++ ) ComboBoxPasswordLength->Items->Add(IntToStr(i));
    //
    ActiveControl=ComboBoxLogin;
}
//---------------------------------------------------------------------------
void __fastcall TFormChangePassword::FormHide(TObject *Sender)
{
    ComboBoxLogin->Items->Clear();
    ComboBoxPasswordLength->Items->Clear();
    EditPassword->Text=""; EditPassword->ClearUndo();
    EditNewPassword->Text=""; EditNewPassword->ClearUndo();
    EditConfirmPassword->Text=""; EditConfirmPassword->ClearUndo();
}
//---------------------------------------------------------------------------
void __fastcall TFormChangePassword::BitBtnOkClick(TObject *Sender)
{
    int a;
    MUser *User;
    if ( (a=ComboBoxLogin->ItemIndex)<0 )
    {
        ::MessageBox(Application->Handle,"Выберите пользователя.",
            "TimeLocker - сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
        return;
    }
    // Проверяем текущий пароль
    if ( a==0 )
    {
        if ( EditPassword->Text!=State->AdminPassword )
        {
            ::MessageBox(Application->Handle,"Пароль неверный !",
                "TimeLocker - сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
            EditPassword->Text=""; EditPassword->SetFocus(); return;
        }
    } else
    {
        User=(MUser*)Users->Item(a-1);
        if ( EditPassword->Text!=User->Password )
        {
            ::MessageBox(Application->Handle,"Пароль неверный !\nВ крайнем случае обратитесь к сисадмину...",
                "TimeLocker - сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
            EditPassword->Text=""; EditPassword->SetFocus(); return;
        }
    }
    // Проверяем совпадение нового пароля и его подтверждения
    if ( EditNewPassword->Text!=EditConfirmPassword->Text )
    {
        ::MessageBox(Application->Handle,"Новый пароль и его подтверждение не совпадают !",
            "TimeLocker - сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
        EditNewPassword->Text=""; EditConfirmPassword->Text="";
        EditNewPassword->SetFocus(); return;
    }
    // Изменяем пароль
    if ( a==0 ) { State->AdminPassword=EditNewPassword->Text; State->Save(); }
    else { User->Password=EditNewPassword->Text; UsersSave(Users); }
    Tag=true;

    ::MessageBox(Application->Handle,"Пароль успешно изменен.",
        "TimeLocker - сообщение",MB_OK|MB_ICONINFORMATION|MB_APPLMODAL);
}
//---------------------------------------------------------------------------
void __fastcall TFormChangePassword::BitBtnCancelClick(TObject *Sender)
{
    Tag=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormChangePassword::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
    CanClose=Tag;
}
//---------------------------------------------------------------------------

