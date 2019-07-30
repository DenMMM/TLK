//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdlib.h>
#include "UnitFormPasswords.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormPasswords *FormPasswords;
//---------------------------------------------------------------------------
__fastcall TFormPasswords::TFormPasswords(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormPasswords::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    EditPassword->Text="";
}
//---------------------------------------------------------------------------
void __fastcall TFormPasswords::FormDeactivate(TObject *Sender)
{
    WindowState=wsMinimized;
}
//---------------------------------------------------------------------------
void __fastcall TFormPasswords::FormShow(TObject *Sender)
{
    ComboBoxPasswordLength->ItemIndex=2;
    EditPassword->Text="";
    ActiveControl=ComboBoxPasswordLength;
    // Инициализируем генератор случайных чисел
    srand( ((int)(( (double)Time() )*24*60*60*10))%(7*24*60*60*10) );
}
//---------------------------------------------------------------------------
void __fastcall TFormPasswords::BitBtnGenerateClick(TObject *Sender)
{
    int PasswordLength;
    const char SymbolsString[]={"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
    const int SymbolsStringLength=strlen(SymbolsString);
    char Password[16+1];

    // Определяем заданную для пароля длину
    switch ( ComboBoxPasswordLength->ItemIndex )
    {
        case 0: PasswordLength=6; break;
        case 1: PasswordLength=7; break;
        case 2: PasswordLength=8; break;
        case 3: PasswordLength=10; break;
        case 4: PasswordLength=11; break;
        case 5: PasswordLength=12; break;
        case 6: PasswordLength=16; break;
        default: break;
    }
    // Генерируем пароль
    for ( int i=0; i<PasswordLength; i++ )
        Password[i]=SymbolsString[random(SymbolsStringLength)];
    Password[PasswordLength]='\0';

    EditPassword->Text=Password;

    // Реинициализируем генератор случайных чисел для последующих генераций
//    randomize();
}
//---------------------------------------------------------------------------
void __fastcall TFormPasswords::BitBtnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

