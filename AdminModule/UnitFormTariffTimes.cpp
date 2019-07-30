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
void TFormTariffTimes::Execute(MTariffTimes *Times_, char *Name_)
{
    char line[MAX_TariffNameLength+19+1];
    strcpy(line,"Время для тарифа \'");
    strcat(line,Name_);
    strcat(line,"\'");
    Caption=line;
    TariffTimes=Times_;
    ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::FormShow(TObject *Sender)
{
    // Формируем списки выбора времени (часы)
    for ( int i=0; i<24; i++ )
    {
        AnsiString Line;
        Line=IntToStr(i);
        ComboBoxBeginH->Items->Add(Line);
        ComboBoxEndH->Items->Add(Line);
        ComboBoxSizeH->Items->Add(Line);
    }
    // Формируем списки выбора времени (минуты)
    for ( int i=0; i<60; i+=5 )
    {
        AnsiString Line;
        Line=IntToStr(i/10)+IntToStr(i%10);
        ComboBoxBeginM->Items->Add(Line);
        ComboBoxEndM->Items->Add(Line);
        ComboBoxSizeM->Items->Add(Line);
    }
    // Заносим время и цены в список
    for ( MTariffTime *TariffTime=(MTariffTime*)TariffTimes->First;
        TariffTime; TariffTime=(MTariffTime*)TariffTime->Next )
    {
        TListItem *NewItem;
        NewItem=ListViewTimes->Items->Add();
        ((MTariffTime*)NewItem->Data)->Copy(TariffTime);
        SetListViewTimesLine(NewItem);
    }

    SetEdit(false);
    ActiveControl=ListViewTimes;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // Чистим интерфейсные элементы
    ListViewTimes->Items->Clear();
    ComboBoxBeginH->Items->Clear();
    ComboBoxBeginM->Items->Clear();
    ComboBoxEndH->Items->Clear();
    ComboBoxEndM->Items->Clear();
    ComboBoxSizeH->Items->Clear();
    ComboBoxSizeM->Items->Clear();
    EditCost->Text="";
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ListViewTimesInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
    Item->Data=new MTariffTime;
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ListViewTimesDeletion(TObject *Sender,
      TListItem *Item)
{
    delete (MTariffTime*)Item->Data;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ListViewTimesSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( ((TListView*)Sender)->SelCount!=1 ) { SetEdit(false); return; }
    else SetEdit(true);

    MTariffTime *SelTime=(MTariffTime*)ListViewTimes->Selected->Data;
    // Время начала действия
    ComboBoxBeginH->ItemIndex=SelTime->BeginTime/60;
    ComboBoxBeginM->ItemIndex=SelTime->BeginTime%60;
    // Время окончания действия
    int EndTime=SelTime->EndTime>=(24*60)? SelTime->EndTime-(24*60): SelTime->EndTime;
    ComboBoxEndH->ItemIndex=EndTime/60;
    ComboBoxEndM->ItemIndex=EndTime%60;
    // Время действия
    switch ( SelTime->Type )
    {
        case mttHours:
            ComboBoxType->ItemIndex=0;
            LabelSizeText->Enabled=false;
            ComboBoxSizeH->Enabled=false; ComboBoxSizeH->ItemIndex=1;
            ComboBoxSizeH->Color=clWindow;
            ComboBoxSizeM->Enabled=false; ComboBoxSizeM->ItemIndex=0;
            ComboBoxSizeM->Color=clWindow;
            break;
        case mttFlyPacket:
            ComboBoxType->ItemIndex=1;
            LabelSizeText->Enabled=true;
            ComboBoxSizeH->Enabled=true; ComboBoxSizeH->ItemIndex=SelTime->SizeTime/60;
            ComboBoxSizeH->Color=clWindow;
            ComboBoxSizeM->Enabled=true; ComboBoxSizeM->ItemIndex=(SelTime->SizeTime%60)/5;
            ComboBoxSizeM->Color=clWindow;
            break;
        case mttPacket:
            ComboBoxType->ItemIndex=2;
            LabelSizeText->Enabled=false;
            ComboBoxSizeH->Enabled=false; ComboBoxSizeH->ItemIndex=-1;
            ComboBoxSizeH->Color=clBtnFace;
            ComboBoxSizeM->Enabled=false; ComboBoxSizeM->ItemIndex=-1;
            ComboBoxSizeM->Color=clBtnFace;
            break;
        default: break;
    }
    // Стоимость
    EditCost->Text=FloatToStrF(SelTime->Cost,ffFixed,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ListViewTimesCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
    MTariffTime *Time1=(MTariffTime*)Item1->Data,
        *Time2=(MTariffTime*)Item2->Data;

    if ( Time1->BeginTime<Time2->BeginTime ) Compare=-1;
    else if ( Time1->BeginTime>Time2->BeginTime ) Compare=1;
    else
    {
        if ( Time1->Type<Time2->Type ) Compare=-1;
        else if ( Time1->Type>Time2->Type ) Compare=1;
        else
        {
            if ( Time1->Type==mttFlyPacket )
            {
                if ( Time1->SizeTime<Time2->SizeTime ) Compare=-1;
                else if ( Time1->SizeTime>Time2->SizeTime ) Compare=1;
                else Compare=0;
            } else Compare=0;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxTypeClick(TObject *Sender)
{
    if ( ListViewTimes->Selected==NULL ) return;

    MTariffTime *SelTime=(MTariffTime*)ListViewTimes->Selected->Data;
    switch ( ((TComboBox*)Sender)->ItemIndex )
    {
        case 0: SelTime->Type=mttHours; break;
        case 1: SelTime->Type=mttFlyPacket; break;
        case 2: SelTime->Type=mttPacket; break;
        default: break;
    }
    SetListViewTimesLine(ListViewTimes->Selected);
    ListViewTimes->AlphaSort();
    ListViewTimesSelectItem(ListViewTimes,ListViewTimes->Selected,true);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxBeginHClick(TObject *Sender)
{
    if ( ListViewTimes->Selected==NULL ) return;

    MTariffTime *Time=(MTariffTime*)ListViewTimes->Selected->Data;
    Time->BeginTime=ComboBoxBeginH->ItemIndex*60+ComboBoxBeginM->ItemIndex*5;
    SetListViewTimesLine(ListViewTimes->Selected);
    ListViewTimes->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxEndHClick(TObject *Sender)
{
    if ( ListViewTimes->Selected==NULL ) return;

    MTariffTime *Time=(MTariffTime*)ListViewTimes->Selected->Data;
    Time->EndTime=ComboBoxEndH->ItemIndex*60+ComboBoxEndM->ItemIndex*5;
    if ( Time->BeginTime>=Time->EndTime ) Time->EndTime+=24*60;
    SetListViewTimesLine(ListViewTimes->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxSizeHClick(TObject *Sender)
{
    if ( ListViewTimes->Selected==NULL ) return;

    MTariffTime *Time=(MTariffTime*)ListViewTimes->Selected->Data;
    Time->SizeTime=ComboBoxSizeH->ItemIndex*60+ComboBoxSizeM->ItemIndex*5;
    SetListViewTimesLine(ListViewTimes->Selected);
    ListViewTimes->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::EditCostExit(TObject *Sender)
{
    if ( ListViewTimes->Selected==NULL ) return;

    MTariffTime *SelTime=(MTariffTime*)ListViewTimes->Selected->Data;
    double Cost;

    try { Cost=StrToFloat(EditCost->Text); }
    catch ( EConvertError *Error ) { goto error; }
    if ( (Cost>=0)&&(Cost<=MAX_TariffTimeCost) )
        SelTime->Cost=((int)(Cost*100.))/100.;
error:
    EditCost->Text=FloatToStrF(SelTime->Cost,ffFixed,8,2);
    SetListViewTimesLine(ListViewTimes->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ButtonAddClick(TObject *Sender)
{
    if ( ListViewTimes->Items->Count>=MAX_TariffTimes )
    {
        ::MessageBox(Handle,"Большее количество пакетов не поддерживается.",
            "Сообщение",MB_OK|MB_ICONWARNING|MB_APPLMODAL);
        return;
    }

    TListItem *NewItem=ListViewTimes->Items->Add();
    SetListViewTimesLine(NewItem);
    ListViewTimes->AlphaSort();
    //
    ListViewTimes->ItemFocused=NewItem;
    ListViewTimes->Selected=NULL;
    ListViewTimes->Selected=NewItem;
    ActiveControl=ComboBoxType;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ButtonDelClick(TObject *Sender)
{
    // Удаляем времена и цены из списка
    TItemStates is=TItemStates()<<isSelected;
    for ( TListItem *Item=ListViewTimes->Selected, *NewItem; Item; )
    {
        NewItem=ListViewTimes->GetNextItem(Item,sdAll,is);
        Item->Delete(); Item=NewItem;
    }
    //
    ListViewTimes->Selected=ListViewTimes->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ButtonSaveClick(TObject *Sender)
{
    TariffTimes->Clear();

    TListItems *Items=ListViewTimes->Items;
    for ( int i=0; i<Items->Count; i++ )
    {
        MTariffTime *NewTime;
        NewTime=(MTariffTime*)TariffTimes->Add();
        NewTime->Copy((MTariffTime*)Items->Item[i]->Data);
    }
}
//---------------------------------------------------------------------------
void TFormTariffTimes::SetEdit(bool Edit_)
{
    TColor Color=Edit_? clWindow: clBtnFace;

    LabelTimeTypeText->Enabled=Edit_;
    ComboBoxType->Enabled=Edit_;
    ComboBoxType->Color=Color;
    LabelBeginText->Enabled=Edit_;
    ComboBoxBeginH->Enabled=Edit_;
    ComboBoxBeginH->Color=Color;
    ComboBoxBeginM->Enabled=Edit_;
    ComboBoxBeginM->Color=Color;
    LabelEndText->Enabled=Edit_;
    ComboBoxEndH->Enabled=Edit_;
    ComboBoxEndH->Color=Color;
    ComboBoxEndM->Enabled=Edit_;
    ComboBoxEndM->Color=Color;
    LabelSizeText->Enabled=Edit_;
    ComboBoxSizeH->Enabled=Edit_;
    ComboBoxSizeH->Color=Color;
    ComboBoxSizeM->Enabled=Edit_;
    ComboBoxSizeM->Color=Color;
    LabelCostText->Enabled=Edit_;
    EditCost->Enabled=Edit_;
    EditCost->Color=Color;
}
//---------------------------------------------------------------------------
void TFormTariffTimes::SetListViewTimesLine(TListItem *Item_)
{
    MTariffTime *SelTime=(MTariffTime*)Item_->Data;

    Item_->SubItems->Strings[0]=((TDate)(SelTime->BeginTime/(24.*60))).FormatString("hh:nn");
    int EndTime=SelTime->EndTime>=(24*60)? SelTime->EndTime-24*60: SelTime->EndTime;
    Item_->SubItems->Strings[1]=((TDate)(EndTime/(24.*60))).FormatString("hh:nn");

    int h, m;
    AnsiString line;
    switch ( SelTime->Type )
    {
        case mttHours: Item_->SubItems->Strings[2]="За каждый час"; break;
        case mttFlyPacket:
            h=SelTime->SizeTime/60;
            m=SelTime->SizeTime%60;
            if ( h ) line=IntToStr(h)+" час. ";
            line+=IntToStr(m/10)+IntToStr(m%10)+" мин.";
            Item_->SubItems->Strings[2]=line;
            break;
        case mttPacket: Item_->SubItems->Strings[2]=""; break;
        default: break;
    }
    Item_->SubItems->Strings[3]=FloatToStrF(SelTime->Cost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------

