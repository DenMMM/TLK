//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormLogIn.h"
#include "UnitFormMain.h"
#include "UnitUsers.h"
#include "UnitLogs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormLogIn *FormLogIn;
//---------------------------------------------------------------------------
__fastcall TFormLogIn::TFormLogIn(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormLogIn::FormShow(TObject *Sender)
{
    Tag=false;
    // Заполняем список пользователей
    ComboBoxLogin->Items->Add("Сисадмин");
    for ( MUser *User=(MUser*)Users->FirstItem; User;
        User=(MUser*)User->NextItem ) ComboBoxLogin->Items->Add(User->Login);
    //
    ActiveControl=ComboBoxLogin;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogIn::FormHide(TObject *Sender)
{
    ComboBoxLogin->Items->Clear();
    EditPassword->Text=""; EditPassword->ClearUndo();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogIn::BitBtnOkClick(TObject *Sender)
{
    int a;
    if ( (a=ComboBoxLogin->ItemIndex)<0 )
    {
        ::MessageBox(Application->Handle,"Выберите пользователя.",
            "TimeLocker - сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
    } else if ( a==0 )
    {
        if ( EditPassword->Text!=State->AdminPassword )
        {
            EditPassword->Text=""; EditPassword->SetFocus();
        } else
        {
            State->CurrentUser=NULL; State->Save();
            if ( State->CurrentAdmin ) LogCompRun(NULL,(MTariff*)5,0,0.);
            FormMain->SetAccess(NULL); FormMain->Caption="TimeLocker - Сисадмин";
            Tag=true;
        }
    } else
    {
        MUser *User=(MUser*)Users->Item(a-1);
        if ( EditPassword->Text!=User->Password )
        {
            EditPassword->Text=""; EditPassword->SetFocus();
        } else if ( !User->Active )
        {
            ::MessageBox(Application->Handle,"Логин заблокирован ! За объяснениями обратитесь к сисадмину.",
                "TimeLocker - сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
        } else
        {
            if ( State->CurrentAdmin ) LogCompRun(NULL,(MTariff*)6,0,0.);
            State->CurrentUser=User;
            if ( (User!=State->CurrentAdmin)&&(User->Rights&murClientsServices) )
            {
                // Заканчиваем предыдущую смену
                if ( State->CurrentAdmin!=NULL ) LogClose();
                // Начинаем новую смену
                State->CurrentAdmin=User;
                State->LogFileNumber++; LogNewFile(User);
                FormMain->StatusBar->Panels->Items[0]->Text="Админ: "+User->Login;
                ::MessageBox(Application->Handle,"Начата новая смена.",
                    "TimeLocker - сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
            }
            State->Save(); FormMain->SetAccess(User);
            FormMain->Caption="TimeLocker - "+User->Login+" ("+User->JobTitle+")";
            Tag=true;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLogIn::BitBtnCancelClick(TObject *Sender)
{
    Tag=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogIn::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose=Tag;
}
//---------------------------------------------------------------------------

