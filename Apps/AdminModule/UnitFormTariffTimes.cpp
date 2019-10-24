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
bool TFormTariffTimes::Execute(MTariffTimes &Times_,
	const UnicodeString &Name_, int Left_, int Top_)
{
    Left=Left_;
	Top=Top_;
	Caption=L"Время для тарифа \'"+Name_+L"\'";

    // Копируем пакеты в буфер
	TmpTimes=Times_;
    // Открываем окно
    if ( ShowModal()!=mrOk ) return false;
    // Замещаем пакетами из буфера текущие
    Times_.Move(TmpTimes);

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::FormShow(TObject *Sender)
{
    // Формируем списки выбора времени (часы)
    for ( int i=0; i<24; i++ )
    {
        UnicodeString Line;
        Line=IntToStr(i);
        ComboBoxBeginH->Items->Add(Line);
        ComboBoxEndH->Items->Add(Line);
        ComboBoxSizeH->Items->Add(Line);
    }
    // Формируем списки выбора времени (минуты)
    for ( int i=0; i<60; i+=5 )
    {
        UnicodeString Line;
        Line=IntToStr(i/10)+IntToStr(i%10);
        ComboBoxBeginM->Items->Add(Line);
        ComboBoxEndM->Items->Add(Line);
        ComboBoxSizeM->Items->Add(Line);
    }
    // Заносим время и цены в список
	for ( auto &time: TmpTimes )
	{
		TListItem *item;
		item=ListViewTimes->Items->Add();
        item->Data=&time;
        SetListViewTimesLine(item);
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
    EditCost->Text=L"";
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ListViewTimesInsert(TObject *Sender,
      TListItem *Item)
{
    Item->ImageIndex=-1;
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
	Item->SubItems->Add(L"");
    Item->SubItems->Add(L"");
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ListViewTimesSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if ( dynamic_cast<TListView&>(*Sender).SelCount!=1 )
    {
        SetEdit(false);
        return;
    } else
        SetEdit(true);

	MTariffTimesItem *time=reinterpret_cast<MTariffTimesItem*>(
		ListViewTimes->Selected->Data);
    // Время начала действия
    ComboBoxBeginH->ItemIndex=time->BeginTime/60;
    ComboBoxBeginM->ItemIndex=time->BeginTime%60;
    // Время окончания действия
    int EndTime=
        time->EndTime>=(24*60)?
        time->EndTime-(24*60):
        time->EndTime;
    ComboBoxEndH->ItemIndex=EndTime/60;
    ComboBoxEndM->ItemIndex=EndTime%60;
    // Время действия
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
    // Стоимость
    EditCost->Text=FloatToStrF(time->Cost,ffFixed,8,2);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ListViewTimesCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
	auto *Time1=reinterpret_cast<MTariffTimesItem*>(Item1->Data);
	auto *Time2=reinterpret_cast<MTariffTimesItem*>(Item2->Data);

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
	if ( ListViewTimes->Selected==nullptr ) return;

	auto *time=reinterpret_cast<MTariffTimesItem*>(
		ListViewTimes->Selected->Data);
    switch ( dynamic_cast<TComboBox&>(*Sender).ItemIndex )
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
    if ( ListViewTimes->Selected==nullptr ) return;

	auto *time=reinterpret_cast<MTariffTimesItem*>(
		ListViewTimes->Selected->Data);
    time->BeginTime=
        ComboBoxBeginH->ItemIndex*60+
        ComboBoxBeginM->ItemIndex*5;
    SetListViewTimesLine(ListViewTimes->Selected);
    ListViewTimes->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxEndHClick(TObject *Sender)
{
    if ( ListViewTimes->Selected==nullptr ) return;

	auto *time=reinterpret_cast<MTariffTimesItem*>(
		ListViewTimes->Selected->Data);
    time->EndTime=
        ComboBoxEndH->ItemIndex*60+
        ComboBoxEndM->ItemIndex*5;
    if ( time->BeginTime>=time->EndTime ) time->EndTime+=24*60;
    SetListViewTimesLine(ListViewTimes->Selected);
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::ComboBoxSizeHClick(TObject *Sender)
{
    if ( ListViewTimes->Selected==nullptr ) return;

	auto *time=reinterpret_cast<MTariffTimesItem*>(
		ListViewTimes->Selected->Data);
    time->SizeTime=
        ComboBoxSizeH->ItemIndex*60+
        ComboBoxSizeM->ItemIndex*5;
    SetListViewTimesLine(ListViewTimes->Selected);
    ListViewTimes->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TFormTariffTimes::EditCostExit(TObject *Sender)
{
    if ( ListViewTimes->Selected==nullptr ) return;

	auto *time=reinterpret_cast<MTariffTimesItem*>(
		ListViewTimes->Selected->Data);
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

    // Добавляем пакет в буфер
	MTariffTimesItem& time=TmpTimes.Add();
	// Добавляем строку в список и связываем ее с пакетом
	TListItem *item=ListViewTimes->Items->Add();
    item->Data=&time;
    // Обновляем интерфейс
    SetListViewTimesLine(item);
    ListViewTimes->AlphaSort();
    ListViewTimes->ItemFocused=item;
    ListViewTimes->Selected=nullptr;
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
        // Удаляем пакет из буфера
		TmpTimes.Del(
			MTariffTimes::const_iterator(
			reinterpret_cast<MTariffTimesItem*>(item->Data)
			));
        // Удаляем строку из списка
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
    auto *time=reinterpret_cast<MTariffTimesItem*>(Item_->Data);

    Item_->SubItems->Strings[0]=((TDate)(time->BeginTime/(24.*60))).FormatString(L"hh:nn");

    int EndTime=
        time->EndTime>=(24*60)?
        time->EndTime-24*60:
        time->EndTime;
	Item_->SubItems->Strings[1]=((TDate)(EndTime/(24.*60))).FormatString(L"hh:nn");

    int h, m;
	UnicodeString line;
    switch ( time->Type )
    {
		case mttHours: Item_->SubItems->Strings[2]=L"За каждый час"; break;
		case mttFlyPacket:
			h=time->SizeTime/60;
			m=time->SizeTime%60;
            if ( h ) line=IntToStr(h)+L" час. ";
            line+=IntToStr(m/10)+IntToStr(m%10)+L" мин.";
            Item_->SubItems->Strings[2]=line;
            break;
        case mttPacket: Item_->SubItems->Strings[2]=L""; break;
        default: break;
    }
    Item_->SubItems->Strings[3]=FloatToStrF(time->Cost,ffCurrency,8,2);
}
//---------------------------------------------------------------------------

