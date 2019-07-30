//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormTariffTimes.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTariffTimes *FormTariffTimes;
//---------------------------------------------------------------------------
__fastcall TFormTariffTimes::TFormTariffTimes(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::Execute(MTariffTimes *TariffTimes_)
{
    TMPTariffTimes=new MTariffTimes;
    TMPTariffTime=new MTariffTime;
    *TMPTariffTimes=*TariffTimes_;
    if ( ShowModal()==mrOk ) *TariffTimes_=*TMPTariffTimes;
    delete TMPTariffTime;
    delete TMPTariffTimes;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::FormShow(TObject *Sender)
{
    //
    for ( int i=0; i<24; i++ )
    {
        AnsiString Line;
        Line=IntToStr(i);
        ComboBoxBeginTimeH->Items->Add(Line);
        ComboBoxEndTimeH->Items->Add(Line);
        ComboBoxSizeTimeH->Items->Add(Line);
    }
    //
    for ( int i=0; i<60; i+=5 )
    {
        AnsiString Line;
        Line=IntToStr(i/10)+IntToStr(i%10);
        ComboBoxBeginTimeM->Items->Add(Line);
        ComboBoxEndTimeM->Items->Add(Line);
        ComboBoxSizeTimeM->Items->Add(Line);
    }
    //
    ListViewTimesAndCosts->Items->Clear();
    for ( MTariffTime *TariffTime=(MTariffTime*)TMPTariffTimes->FirstItem;
        TariffTime; TariffTime=(MTariffTime*)TariffTime->NextItem )
    {
        //
        TListItem *NewItem;
        NewItem=ListViewTimesAndCosts->Items->Add();
        NewItem->SubItems->Add("");
        NewItem->SubItems->Add("");
        NewItem->SubItems->Add("");
        NewItem->SubItems->Add("");
        NewItem->SubItems->Add("");
        SetTimesAndCostsLine(NewItem,TariffTime);
    }
    //
    TMPTariffTime->BeginTime=0;
    TMPTariffTime->EndTime=0;
    TMPTariffTime->SizeTime=-1;
    TMPTariffTime->Cost=0.;
    TMPTariffTime->WeekDays=mwdAll;
    //
    SetTimeAndCostParam();
    //
    ActiveControl=ListViewTimesAndCosts;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ListViewTimesAndCosts->Items->Clear();
    ComboBoxBeginTimeH->Items->Clear();
    ComboBoxBeginTimeM->Items->Clear();
    ComboBoxEndTimeH->Items->Clear();
    ComboBoxEndTimeM->Items->Clear();
    ComboBoxSizeTimeH->Items->Clear();
    ComboBoxSizeTimeM->Items->Clear();
    EditCost->Text="";
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ListViewTimesAndCostsChange(
      TObject *Sender, TListItem *Item, TItemChange Change)
{
    if ( (Change!=ctState)||(!Item->Focused) ) return;
    //
    *TMPTariffTime=*((MTariffTime*)(TMPTariffTimes->Item(Item->Index)));
    if ( TMPTariffTime->EndTime>=(24*60) ) TMPTariffTime->EndTime-=24*60;
    //
    SetTimeAndCostParam();
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxTimeTypeChange(
      TObject *Sender)
{
    switch ( ((TComboBox*)Sender)->ItemIndex )
    {
        case 0:
            LabelSizeTimeText->Enabled=false;
            ComboBoxSizeTimeH->Enabled=false;
            ComboBoxSizeTimeH->ItemIndex=1;
            ComboBoxSizeTimeM->Enabled=false;
            ComboBoxSizeTimeM->ItemIndex=0;
            TMPTariffTime->SizeTime=-1;
            CheckBoxEnableDesc->Enabled=false;
            CheckBoxEnableDesc->Checked=false;
            break;
        case 1:
            LabelSizeTimeText->Enabled=true;
            ComboBoxSizeTimeH->Enabled=true;
            ComboBoxSizeTimeH->ItemIndex=1;
            ComboBoxSizeTimeM->Enabled=true;
            ComboBoxSizeTimeM->ItemIndex=0;
            TMPTariffTime->SizeTime=60;
            CheckBoxEnableDesc->Enabled=false;
            CheckBoxEnableDesc->Checked=false;
            break;
        case 2:
            LabelSizeTimeText->Enabled=false;
            ComboBoxSizeTimeH->Enabled=false;
            ComboBoxSizeTimeH->ItemIndex=-1;
            ComboBoxSizeTimeM->Enabled=false;
            ComboBoxSizeTimeM->ItemIndex=-1;
            TMPTariffTime->SizeTime=0;
            CheckBoxEnableDesc->Enabled=true;
            CheckBoxEnableDesc->Checked=TMPTariffTime->EnableDesc;
            break;
        default: break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxBeginTimeHChange(TObject *Sender)
{
    int ItemIndexH, ItemIndexM;
    if ( ((ItemIndexH=ComboBoxBeginTimeH->ItemIndex)<0)||
        ((ItemIndexM=ComboBoxBeginTimeM->ItemIndex)<0) ) return;
    TMPTariffTime->BeginTime=ItemIndexH*60+ItemIndexM*5;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxEndTimeHChange(TObject *Sender)
{
    int ItemIndexH, ItemIndexM;
    if ( ((ItemIndexH=ComboBoxEndTimeH->ItemIndex)<0)||
        ((ItemIndexM=ComboBoxEndTimeM->ItemIndex)<0) ) return;
    TMPTariffTime->EndTime=ItemIndexH*60+ItemIndexM*5;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxSizeTimeHChange(TObject *Sender)
{
    int ItemIndexH, ItemIndexM;
    if ( ((ItemIndexH=ComboBoxSizeTimeH->ItemIndex)<0)||
        ((ItemIndexM=ComboBoxSizeTimeM->ItemIndex)<0) ) return;
    if ( !(ItemIndexH||ItemIndexM) )
    {
        ComboBoxSizeTimeH->ItemIndex=TMPTariffTime->SizeTime/60;
        ComboBoxSizeTimeM->ItemIndex=(TMPTariffTime->SizeTime%60)/5;
    } else TMPTariffTime->SizeTime=ItemIndexH*60+ItemIndexM*5;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::EditCostExit(TObject *Sender)
{
    double a;
    try { a=StrToFloat(EditCost->Text); }
    catch ( EConvertError *Error ) { goto error; }
    if ( (a>=0)&&(a<1000000) ) TMPTariffTime->Cost=a;
error:
    EditCost->Text=FloatToStrF(TMPTariffTime->Cost,ffFixed,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::CheckBoxDayMoClick(
      TObject *Sender)
{
    unsigned int Day;

    if ( Sender==CheckBoxDayMo ) Day=mwdMo;
    else if ( Sender==CheckBoxDayTu ) Day=mwdTu;
    else if ( Sender==CheckBoxDayWe ) Day=mwdWe;
    else if ( Sender==CheckBoxDayTh ) Day=mwdTh;
    else if ( Sender==CheckBoxDayFr ) Day=mwdFr;
    else if ( Sender==CheckBoxDaySa ) Day=mwdSa;
    else if ( Sender==CheckBoxDaySu ) Day=mwdSu;
    else return;

    if ( ((TCheckBox*)Sender)->Checked ) TMPTariffTime->WeekDays|=Day;
    else TMPTariffTime->WeekDays&=~Day;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::CheckBoxEnableDescClick(TObject *Sender)
{
    TMPTariffTime->EnableDesc=CheckBoxEnableDesc->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::SetTimesAndCostsLine(TListItem *ListItem_,MTariffTime *TariffTime_)
{
    TStrings *Strings=ListItem_->SubItems;
    Strings->Strings[0]=((TDate)(TariffTime_->BeginTime/(24.*60))).FormatString("hh:nn");
    Strings->Strings[1]=((TDate)(TariffTime_->EndTime/(24.*60))).FormatString("hh:nn");
    //
    switch ( TariffTime_->SizeTime )
    {
        case -1: Strings->Strings[2]="Каждый час"; break;
        case 0:
            if ( TariffTime_->EnableDesc ) Strings->Strings[2]="(с записью)";
            else Strings->Strings[2]="";
            break;
        default:
            int h, m;
            AnsiString line;
            h=TariffTime_->SizeTime/60;
            m=TariffTime_->SizeTime%60;
            if ( h ) line=IntToStr(h)+" час. ";
            line+=IntToStr(m/10)+IntToStr(m%10)+" мин.";
            Strings->Strings[2]=line;
            break;
    }
    //
    int Days=TariffTime_->WeekDays;
    AnsiString DaysString;
    if ( Days&mwdMo ) DaysString+="Пн. ";
    if ( Days&mwdTu ) DaysString+="Вт. ";
    if ( Days&mwdWe ) DaysString+="Ср. ";
    if ( Days&mwdTh ) DaysString+="Чт. ";
    if ( Days&mwdFr ) DaysString+="Пт. ";
    if ( Days&mwdSa ) DaysString+="Сб. ";
    if ( Days&mwdSu ) DaysString+="Вс. ";
    Strings->Strings[3]=DaysString;
    Strings->Strings[4]=FloatToStrF(TariffTime_->Cost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::SetTimeAndCostParam()
{
    int ItemIndex;
    switch ( TMPTariffTime->SizeTime )
    {
        case -1: ItemIndex=0; break;
        case 0: ItemIndex=2; break;
        default: ItemIndex=1; break;
    }
    ComboBoxTimeType->ItemIndex=ItemIndex;
    ComboBoxTimeTypeChange(ComboBoxTimeType);
    //
    ComboBoxBeginTimeH->ItemIndex=TMPTariffTime->BeginTime/60;
    ComboBoxBeginTimeM->ItemIndex=TMPTariffTime->BeginTime%60;
    ComboBoxEndTimeH->ItemIndex=TMPTariffTime->EndTime/60;
    ComboBoxEndTimeM->ItemIndex=TMPTariffTime->EndTime%60;
    ComboBoxSizeTimeH->ItemIndex=TMPTariffTime->SizeTime/60;
    ComboBoxSizeTimeM->ItemIndex=(TMPTariffTime->SizeTime%60)/5;
    //
    EditCost->Text=""; EditCostExit(EditCost);
    //
    unsigned int Day=TMPTariffTime->WeekDays;
    CheckBoxDayMo->Checked=Day&mwdMo;
    CheckBoxDayTu->Checked=Day&mwdTu;
    CheckBoxDayWe->Checked=Day&mwdWe;
    CheckBoxDayTh->Checked=Day&mwdTh;
    CheckBoxDayFr->Checked=Day&mwdFr;
    CheckBoxDaySa->Checked=Day&mwdSa;
    CheckBoxDaySu->Checked=Day&mwdSu;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ButtonAddClick(TObject *Sender)
{
    MTariffTime *TariffTime;
    TariffTime=(MTariffTime*)TMPTariffTimes->Add();
    *TariffTime=*TMPTariffTime;
    if ( TariffTime->BeginTime>=TariffTime->EndTime ) TariffTime->EndTime+=24*60;
    TListItem *NewItem;
    NewItem=ListViewTimesAndCosts->Items->Add();
    NewItem->SubItems->Add("");
    NewItem->SubItems->Add("");
    NewItem->SubItems->Add("");
    NewItem->SubItems->Add("");
    NewItem->SubItems->Add("");
    SetTimesAndCostsLine(NewItem,TariffTime);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ButtonDeleteClick(TObject *Sender)
{
    TListItems *ListItems;
    ListItems=ListViewTimesAndCosts->Items;
    for ( int i=ListItems->Count-1; i>=0; i-- )
    {
        if ( !ListItems->Item[i]->Selected ) continue;
        ListItems->Item[i]->Delete();
        TMPTariffTimes->Delete(i);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ButtonChangeClick(TObject *Sender)
{
    TListItem *ListItem;
    if ( (ListItem=ListViewTimesAndCosts->ItemFocused)==NULL ) return;
    MTariffTime *TariffTime=(MTariffTime*)TMPTariffTimes->Item(ListItem->Index);
    *TariffTime=*TMPTariffTime;
    if ( TariffTime->BeginTime>=TariffTime->EndTime ) TariffTime->EndTime+=24*60;
    SetTimesAndCostsLine(ListItem,TariffTime);
}
//---------------------------------------------------------------------------

