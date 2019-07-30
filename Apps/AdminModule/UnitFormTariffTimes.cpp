//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormTariffTimes.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormTariffTimes::TFormTariffTimes(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
bool TFormTariffTimes::Execute(MTariffTimes *Times_, char *Name_, int Left_, int Top_)
{
    if ( Times_==NULL ) return false;

    char line[MAX_TariffNameLen+19+1];
    strcpy(line,"����� ��� ������ \'");
    strcat(line,Name_);
    strcat(line,"\'");
    Caption=line;

    Left=Left_;
    Top=Top_;

    // �������� ������ � �����
    TmpTimes.Copy(Times_);
    // ��������� ����
    if ( ShowModal()!=mrOk ) return false;
    // �������� �������� �� ������ �������
    Times_->Move(&TmpTimes);

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::FormShow(TObject *Sender)
{
    // ��������� ������ ������ ������� (����)
    for ( int i=0; i<24; i++ )
    {
        AnsiString Line;
        Line=IntToStr(i);
        ComboBoxBeginH->Items->Add(Line);
        ComboBoxEndH->Items->Add(Line);
        ComboBoxSizeH->Items->Add(Line);
    }
    // ��������� ������ ������ ������� (������)
    for ( int i=0; i<60; i+=5 )
    {
        AnsiString Line;
        Line=IntToStr(i/10)+IntToStr(i%10);
        ComboBoxBeginM->Items->Add(Line);
        ComboBoxEndM->Items->Add(Line);
        ComboBoxSizeM->Items->Add(Line);
    }
    // ������� ����� � ���� � ������
    for ( MTariffTime *time=(MTariffTime*)TmpTimes.gFirst();
        time; time=(MTariffTime*)time->gNext() )
    {
        TListItem *item;
        item=ListViewTimes->Items->Add();
        item->Data=time;
        SetListViewTimesLine(item);
    }

    SetEdit(false);
    ActiveControl=ListViewTimes;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    // ������ ������������ ��������
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
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
    Item->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ListViewTimesSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( ((TListView*)Sender)->SelCount!=1 )
    {
        SetEdit(false);
        return;
    } else
        SetEdit(true);

    MTariffTime *time=(MTariffTime*)ListViewTimes->Selected->Data;
    // ����� ������ ��������
    ComboBoxBeginH->ItemIndex=time->BeginTime/60;
    ComboBoxBeginM->ItemIndex=time->BeginTime%60;
    // ����� ��������� ��������
    int EndTime=
        time->EndTime>=(24*60)?
        time->EndTime-(24*60):
        time->EndTime;
    ComboBoxEndH->ItemIndex=EndTime/60;
    ComboBoxEndM->ItemIndex=EndTime%60;
    // ����� ��������
    switch ( time->Type )
    {
        case mttHours:
            ComboBoxType->ItemIndex=0;
            LabelSizeText->Enabled=false;
            ComboBoxSizeH->Enabled=false;
            ComboBoxSizeH->ItemIndex=1;
            ComboBoxSizeH->Color=clWindow;
            ComboBoxSizeM->Enabled=false;
            ComboBoxSizeM->ItemIndex=0;
            ComboBoxSizeM->Color=clWindow;
            break;
        case mttFlyPacket:
            ComboBoxType->ItemIndex=1;
            LabelSizeText->Enabled=true;
            ComboBoxSizeH->Enabled=true;
            ComboBoxSizeH->ItemIndex=time->SizeTime/60;
            ComboBoxSizeH->Color=clWindow;
            ComboBoxSizeM->Enabled=true;
            ComboBoxSizeM->ItemIndex=(time->SizeTime%60)/5;
            ComboBoxSizeM->Color=clWindow;
            break;
        case mttPacket:
            ComboBoxType->ItemIndex=2;
            LabelSizeText->Enabled=false;
            ComboBoxSizeH->Enabled=false;
            ComboBoxSizeH->ItemIndex=-1;
            ComboBoxSizeH->Color=clBtnFace;
            ComboBoxSizeM->Enabled=false;
            ComboBoxSizeM->ItemIndex=-1;
            ComboBoxSizeM->Color=clBtnFace;
            break;
        default: break;
    }
    // ���������
    EditCost->Text=FloatToStrF(time->Cost,ffFixed,8,2);
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

    MTariffTime *time=(MTariffTime*)ListViewTimes->Selected->Data;
    switch ( ((TComboBox*)Sender)->ItemIndex )
    {
        case 0: time->Type=mttHours; break;
        case 1: time->Type=mttFlyPacket; break;
        case 2: time->Type=mttPacket; break;
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

    MTariffTime *time=(MTariffTime*)ListViewTimes->Selected->Data;
    time->BeginTime=
        ComboBoxBeginH->ItemIndex*60+
        ComboBoxBeginM->ItemIndex*5;
    SetListViewTimesLine(ListViewTimes->Selected);
    ListViewTimes->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxEndHClick(TObject *Sender)
{
    if ( ListViewTimes->Selected==NULL ) return;

    MTariffTime *time=(MTariffTime*)ListViewTimes->Selected->Data;
    time->EndTime=
        ComboBoxEndH->ItemIndex*60+
        ComboBoxEndM->ItemIndex*5;
    if ( time->BeginTime>=time->EndTime ) time->EndTime+=24*60;
    SetListViewTimesLine(ListViewTimes->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxSizeHClick(TObject *Sender)
{
    if ( ListViewTimes->Selected==NULL ) return;

    MTariffTime *time=(MTariffTime*)ListViewTimes->Selected->Data;
    time->SizeTime=
        ComboBoxSizeH->ItemIndex*60+
        ComboBoxSizeM->ItemIndex*5;
    SetListViewTimesLine(ListViewTimes->Selected);
    ListViewTimes->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::EditCostExit(TObject *Sender)
{
    if ( ListViewTimes->Selected==NULL ) return;

    MTariffTime *time=(MTariffTime*)ListViewTimes->Selected->Data;
    double Cost;

    try { Cost=StrToFloat(EditCost->Text); }
    catch ( EConvertError *Error ) { goto error; }
    if ( (Cost>=0)&&(Cost<=MAX_TariffTimeCost) )
        time->Cost=((int)(Cost*100.))/100.;
error:
    EditCost->Text=FloatToStrF(time->Cost,ffFixed,8,2);
    SetListViewTimesLine(ListViewTimes->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ButtonAddClick(TObject *Sender)
{
    if ( ListViewTimes->Items->Count>=MAX_TariffTimes )
    {
        ResMessageBox(Handle,0,21,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    // ��������� ����� � �����
    MTariffTime *time=(MTariffTime*)TmpTimes.Add();
    // ��������� ������ � ������ � ��������� �� � �������
    TListItem *item=ListViewTimes->Items->Add();
    item->Data=time;
    // ��������� ���������
    SetListViewTimesLine(item);
    ListViewTimes->AlphaSort();
    ListViewTimes->ItemFocused=item;
    ListViewTimes->Selected=NULL;
    ListViewTimes->Selected=item;
    ActiveControl=ComboBoxType;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ButtonDelClick(TObject *Sender)
{
    TItemStates is=TItemStates()<<isSelected;
    TListItem *item=ListViewTimes->Selected, *next;
    while(item)
    {
        // ������� ����� �� ������
        TmpTimes.Del((MTariffTime*)item->Data);
        // ������� ������ �� ������
        next=ListViewTimes->GetNextItem(item,sdAll,is);
        item->Delete();
        item=next;
    }
    //
    ListViewTimes->Selected=ListViewTimes->ItemFocused;
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
    MTariffTime *time=(MTariffTime*)Item_->Data;

    Item_->SubItems->Strings[0]=((TDate)(time->BeginTime/(24.*60))).FormatString("hh:nn");

    int EndTime=
        time->EndTime>=(24*60)?
        time->EndTime-24*60:
        time->EndTime;
    Item_->SubItems->Strings[1]=((TDate)(EndTime/(24.*60))).FormatString("hh:nn");

    int h, m;
    AnsiString line;
    switch ( time->Type )
    {
        case mttHours: Item_->SubItems->Strings[2]="�� ������ ���"; break;
        case mttFlyPacket:
            h=time->SizeTime/60;
            m=time->SizeTime%60;
            if ( h ) line=IntToStr(h)+" ���. ";
            line+=IntToStr(m/10)+IntToStr(m%10)+" ���.";
            Item_->SubItems->Strings[2]=line;
            break;
        case mttPacket: Item_->SubItems->Strings[2]=""; break;
        default: break;
    }
    Item_->SubItems->Strings[3]=FloatToStrF(time->Cost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------



