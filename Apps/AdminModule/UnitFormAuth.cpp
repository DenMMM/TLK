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
    // Переводим BYTE в HEX-строку
	ByteToHEX(TmpKey, sizeof(TmpKey), HexBuffer, sizeof(HexBuffer), L' ');
	// Подаем на редактирование
    MemoHEX->Lines->SetText(HexBuffer);

    SaveDialog->Filter=
		L"Файлы реестра Windows(*.reg)|*.reg|"
		L"Все файлы (*.*)|*.*";
	SaveDialog->DefaultExt=L"REG";

	// Добавим энтропии
	BasicRand.event();
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::MemoHEXKeyPress(TObject *Sender, char &Key)
{
	TMemo *const Memo=&dynamic_cast<TMemo&>(*Sender);
	const wchar_t ku[]=L"ABCDEF0123456789";
	const wchar_t kl[]=L"abcdef";
	const wchar_t *pos, *text;
	unsigned curs;

	if ( Memo->SelLength>1 ) goto nokey;

	pos=wcschr(kl,Key);
	if ( pos!=nullptr ) Key=ku[pos-kl];
	else
	{
		pos=wcschr(ku,Key);
		if ( pos==nullptr ) goto nokey;
	}
	Memo->SelLength=1;

	curs=Memo->SelStart;
	text=Memo->Lines->GetText();
	if ( (text[curs]!=L' ')&&
		(text[curs]!=L'\r')&&
        (text[curs]!=L'\n') ) return;

    if ( (curs+1)<wcslen(text) ) Memo->SelStart++;

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
			"TFormAuth::MemoHEXExit()"
			"Длина ключа в байтах получилась меньше ожидаемого.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::ButtonOKClick(TObject *Sender)
{
    // Замещаем текущий ключ безопасности новым
    Sync->Stop();
    Auth->SetKey(TmpKey,sizeof(TmpKey));
    Sync->Start();
    // Сохраняем его в файл
    if ( !Auth->Save() )
    {
        ShellState->State|=mssErrorConfig; FormMain->SetShell();
        ResMessageBox(Handle,1,16,MB_APPLMODAL|MB_OK|MB_ICONERROR,Auth->gLastErr());
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::ButtonNewClick(TObject *Sender)
{
    // Генерируем ключ
	if ( !CngRand(TmpKey,sizeof(TmpKey)) ) TimeRand(TmpKey,sizeof(TmpKey));
    // Переводим BYTE в HEX-строку
    ByteToHEX(TmpKey,sizeof(TmpKey),HexBuffer,sizeof(HexBuffer),L' ');
    // Подаем на редактирование
    MemoHEX->Lines->SetText(HexBuffer);
}
//---------------------------------------------------------------------------
void __fastcall TFormAuth::ButtonSaveClick(TObject *Sender)
{
    if ( !SaveDialog->Execute() ) return;

	TmpAuth.SetKey(TmpKey, sizeof(TmpKey));
	if ( !TmpAuth.SaveAsReg(
			SaveDialog->FileName.c_str(),
			HKEY_LOCAL_MACHINE,
			L"Software\\MMM Groups\\TLK\\3.0\\Client",
			L"Auth",
			ENC_Code) )
		ResMessageBox(Handle, 1, 33, MB_APPLMODAL|MB_OK|MB_ICONERROR, Auth->gLastErr());
}
//---------------------------------------------------------------------------

