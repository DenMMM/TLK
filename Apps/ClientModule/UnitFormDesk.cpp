//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormDesk.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormDesk *FormDesk;
//---------------------------------------------------------------------------
__fastcall TFormDesk::TFormDesk(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormDesk::FormShow(TObject *Sender)
{
    Left=Screen->WorkAreaWidth-Width;
    Top=Screen->WorkAreaHeight-Height;
//    Left=Screen->DesktopWidth-Width;
//    Top=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormDesk::LabelHideMouseEnter(TObject *Sender)
{
    if ( TimerHide->Enabled ) return;

    TimerHide->Interval=250;
    TimerHide->Enabled=true;
    Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFormDesk::TimerHideTimer(TObject *Sender)
{
    POINT mouse_pos;
    if ( !::GetCursorPos(&mouse_pos) ) return;

    if ( mouse_pos.x>=Left &&
        mouse_pos.x<(Left+Width) &&
        mouse_pos.y>=Top &&
        mouse_pos.y<(Top+Height) ) return;

    TimerHide->Enabled=false;
    Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormDesk::CreateParams(TCreateParams &Params)
{
    TForm::CreateParams(Params);
//    Params.Style&=~WS_CAPTION;
    Params.Style&=~WS_DLGFRAME;
    Params.Style|=WS_POPUP;
    Params.ExStyle|=0x02000000;     // WS_EX_COMPOSITED
}
//---------------------------------------------------------------------------

