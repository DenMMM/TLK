//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormAccessRights.h"
#include "UnitFormUsers.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAccessRights *FormAccessRights;
//---------------------------------------------------------------------------
__fastcall TFormAccessRights::TFormAccessRights(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormAccessRights::FormShow(TObject *Sender)
{
    TMPRights=FormUsers->TMPUser->Rights;

    CheckBoxClientsServices->Checked=TMPRights&murClientsServices;
    CheckBoxComputersServices->Checked=TMPRights&murComputersServices;
    CheckBoxWorkWithLogs->Checked=TMPRights&murWorkWithLogs;

    CheckBoxClientsServicesClick(NULL);
    CheckBoxComputersServicesClick(NULL);

    Caption="Настройка прав для пользователя '"+FormUsers->TMPUser->Login+"'";
    ActiveControl=CheckBoxClientsServices;
}
//---------------------------------------------------------------------------
void __fastcall TFormAccessRights::CheckBoxClientsServicesClick(
      TObject *Sender)
{
//    bool enabled=CheckBoxClientsServices->Checked;
//    CheckBoxRemoveAllTime->Enabled=enabled;
}
//---------------------------------------------------------------------------
void __fastcall TFormAccessRights::CheckBoxComputersServicesClick(
      TObject *Sender)
{
//    bool enabled=CheckBoxComputersServices->Checked;
//    CheckBoxSetAutoMode->Enabled=enabled;
//    CheckBoxSetUseMode->Enabled=enabled;
}
//---------------------------------------------------------------------------
void __fastcall TFormAccessRights::BitBtnSaveClick(TObject *Sender)
{
    TMPRights=0;
    if ( CheckBoxClientsServices->Checked ) TMPRights|=murClientsServices;
    if ( CheckBoxComputersServices->Checked ) TMPRights|=murComputersServices;
    if ( CheckBoxWorkWithLogs->Checked ) TMPRights|=murWorkWithLogs;
    FormUsers->TMPUser->Rights=TMPRights;
}
//---------------------------------------------------------------------------

