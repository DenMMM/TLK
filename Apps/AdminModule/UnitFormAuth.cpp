//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitFormAuth.h"
#include "UnitFormMain.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormAuth::TFormAuth(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::FormShow(TObject *Sender)
{
    Auth->GetKey(TmpKey,sizeof(TmpKey));
    // ��������� BYTE � HEX-������
    ByteToHEX(TmpKey,sizeof(TmpKey),HexBuffer,sizeof(HexBuffer),' ');
    // ������ �� ��������������
    MemoHEX->Lines->SetText(HexBuffer);

    SaveDialog->Filter=
        TEXT("����� ������� Windows(*.reg)|*.reg|")
        TEXT("��� ����� (*.*)|*.*");
    SaveDialog->DefaultExt="REG";
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::MemoHEXKeyPress(TObject *Sender, char &Key)
{
    char ku[]="ABCDEF0123456789";
    char kl[]="abcdef";
    char *pos, *text;
    unsigned curs;
    TMemo *Memo=(TMemo*)Sender;

    if ( Memo->SelLength>1 ) goto nokey;

    pos=strchr(kl,Key);
    if ( pos!=NULL ) Key=ku[pos-kl];
    else
    {
        pos=strchr(ku,Key);
        if ( pos==NULL ) goto nokey;
    }
    Memo->SelLength=1;

    curs=Memo->SelStart;
    text=Memo->Lines->GetText();
    if ( (text[curs]!=' ')&&
        (text[curs]!='\r')&&
        (text[curs]!='\n') ) return;

    if ( (curs+1)<strlen(text) ) Memo->SelStart++;

nokey:
    Key=0;
    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::MemoHEXKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    switch(Key)
    {
        case VK_DELETE:
        case VK_BACK:
            Key=0;
        default:
            break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::MemoHEXExit(TObject *Sender)
{
    if ( HEXToByte(MemoHEX->Lines->GetText(),
        TmpKey,sizeof(TmpKey))!=sizeof(TmpKey) )
    {
        throw std::runtime_error (
        TEXT("TFormAuth::MemoHEXExit")
        TEXT("����� ����� � ������ ���������� ������ ����������.")
        );
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::ButtonOKClick(TObject *Sender)
{
    // �������� ������� ���� ������������ �����
    Sync->Stop();
    Auth->SetKey(TmpKey,sizeof(TmpKey));
    Sync->Start();
    // ��������� ��� � ����
    if ( !Auth->Save() )
    {
        ShellState->State|=mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,16,MB_APPLMODAL|MB_OK|MB_ICONERROR,Auth->gLastErr());
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::ButtonNewClick(TObject *Sender)
{
    // ���������� ����
    if ( !TimeRand(TmpKey,sizeof(TmpKey)) ) return;
    // ��������� BYTE � HEX-������
    ByteToHEX(TmpKey,sizeof(TmpKey),HexBuffer,sizeof(HexBuffer),' ');
    // ������ �� ��������������
    MemoHEX->Lines->SetText(HexBuffer);
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::ButtonSaveClick(TObject *Sender)
{
    if ( !SaveDialog->Execute() ) return;

    TmpAuth.SetKey(TmpKey,sizeof(TmpKey));
    if ( !TmpAuth.SaveAsReg(SaveDialog->FileName.c_str(),
            HKEY_LOCAL_MACHINE,
            "Software\\MMM Groups\\TLK\\3.0\\Client",
            "Auth",ENC_Code) )
        ResMessageBox(Handle,1,33,MB_APPLMODAL|MB_OK|MB_ICONERROR,Auth->gLastErr());
}
//---------------------------------------------------------------------------

