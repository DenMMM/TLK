//---------------------------------------------------------------------------

#ifndef UnitFormDeskH
#define UnitFormDeskH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFormDesk : public TForm
{
__published:	// IDE-managed Components
    TTimer *TimerHide;
    TLabel *LabelHide;
    TLabel *LabelWorkTime;
    TLabel *LabelCompNum;
    TBevel *Bevel2;
    TBevel *Bevel3;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall TimerHideTimer(TObject *Sender);
    void __fastcall LabelHideMouseEnter(TObject *Sender);
private:	// User declarations
    void __fastcall CreateParams(TCreateParams &Params);
 public:		// User declarations
    __fastcall TFormDesk(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormDesk *FormDesk;
//---------------------------------------------------------------------------
#endif
