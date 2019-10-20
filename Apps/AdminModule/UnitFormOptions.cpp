//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormOptions.h"
#include "UnitFormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormOptions::TFormOptions(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::FormShow(TObject *Sender)
{
    // Копируем настройки в буфер
	TmpOptions=*Options;

    {
        int Index;
        switch(TmpOptions.LogPeriod)
        {
            case mlpDay: Index=0; break;
            case mlpWeek: Index=1; break;
            case mlpMonth: Index=2; break;
            default: Index=-1; break;
        }
        ComboBoxLogPeriod->ItemIndex=Index;
    }
    EditCostDialogTime->Text=IntToStr(TmpOptions.CostDialogTime);
    EditCostPrecision->Text=FloatToStrF(TmpOptions.CostPrecision,ffFixed,8,2);
    EditFilterFreeTime->Text=IntToStr(TmpOptions.FilterFreeTime);
    CheckBoxRightPause->Checked=TmpOptions.UsersRights&murPause;

    EditCostDialogTime->MaxLength=5;
    EditCostPrecision->MaxLength=10;
    EditFilterFreeTime->MaxLength=5;
    ActiveControl=ComboBoxLogPeriod;
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	EditCostDialogTime->Text=L"";
	EditCostPrecision->Text=L"";
	EditFilterFreeTime->Text=L"";
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::ComboBoxLogPeriodExit(TObject *Sender)
{
	switch ( dynamic_cast<TComboBox&>(*Sender).ItemIndex )
	{
        case 0: TmpOptions.LogPeriod=mlpDay; break;
        case 1: TmpOptions.LogPeriod=mlpWeek; break;
        case 2: TmpOptions.LogPeriod=mlpMonth; break;
        default: break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::EditCostDialogTimeExit(TObject *Sender)
{
    int Time;

    try { Time=StrToInt(EditCostDialogTime->Text); }
    catch ( EConvertError *Error ) { goto error; }
    if ( (Time>=0)&&(Time<=MAX_DialogTime) ) TmpOptions.CostDialogTime=Time;
error:
    EditCostDialogTime->Text=IntToStr(TmpOptions.CostDialogTime);
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::EditCostPrecisionExit(TObject *Sender)
{
    double Precision;

    try { Precision=StrToFloat(EditCostPrecision->Text); }
    catch ( EConvertError *Error ) { goto error; }
    if ( (Precision>=0)&&(Precision<=MAX_TariffTimeCost) )
        TmpOptions.CostPrecision=((int)(Precision*100.))/100.;
error:
    EditCostPrecision->Text=FloatToStrF(TmpOptions.CostPrecision,ffFixed,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::EditFilterFreeTimeExit(TObject *Sender)
{
    int Time;

    try { Time=StrToInt(EditFilterFreeTime->Text); }
    catch ( EConvertError *Error ) { goto error; }
    if ( (Time>=0)&&(Time<=MAX_FilterFreeTime) ) TmpOptions.FilterFreeTime=Time;
error:
    EditFilterFreeTime->Text=IntToStr(TmpOptions.FilterFreeTime);
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::CheckBoxRightPauseExit(TObject *Sender)
{
	if ( dynamic_cast<TCheckBox&>(*Sender).Checked )
		TmpOptions.UsersRights|=murPause;
	else
		TmpOptions.UsersRights&=~murPause;
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::ButtonSaveClick(TObject *Sender)
{
    // Копируем из буфера назад
	*Options=TmpOptions;
    // Сохраняем в файле
    if ( !Options->Save() )
    {
        ShellState->State|=mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,31,MB_APPLMODAL|MB_OK|MB_ICONERROR,Options->gLastErr());
        return;
    }
    // Запись в логах
    if ( !Log->AddOptions() )
    {
        // Настройки сохранили, но без отметки об их изменении (надо ли ?) работать не дадим
        ShellState->State|=mssErrorLog|mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,5,MB_APPLMODAL|MB_OK|MB_ICONERROR,Log->gLastErr());
    }
}
//---------------------------------------------------------------------------

