//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitCommon.h"
#include "UnitFormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// Базовый код шифровки
#define ProtectCode ((unsigned int)0x4B527CF0)
//---------------------------------------------------------------------------
// Идентификационный код копии программы
const char ident[]={0xFC,0x18,0x43,0xAA,0x69,0x5F,0x9E,0x3B,0x44,0x10,0x55,0x4A,0x50,0xE7,0xBB,0xA3};
//---------------------------------------------------------------------------
MOptions Options;
//---------------------------------------------------------------------------

int __fastcall MOptions::Load()
{
    int DataSize, File;
    char *Data;

    // Загрузка зашифрованных опций в память
    File=FileOpen(WinDir+"\\TmLockSr.cfg",fmOpenRead);
    if ( File<0 ) return -1;
    DataSize=FileSeek(File,0,2); FileSeek(File,0,0);
    Data=new char[DataSize+1];
    if ( Data==NULL ) return -1;
    if ( FileRead(File,Data,DataSize)<0 ) goto error;
    FileClose(File);
    // Расшифровка опций
    for ( int i=DataSize-4; i>=0; i-- ) *((unsigned int*)(Data+i))-=ProtectCode;
    // Подготовка данных к интерпретации
    for ( int i=0; i<DataSize; i++ ) if ( (unsigned char)Data[i]<0x0A ) goto error;
    Data[DataSize]=0;
    // Разбор параметров
    try
    {
        char *ptr=Data, *pos;
        if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
        *pos=0; AdminPassword=ptr; ptr=pos+1;
        if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
        *pos=0; Mode=StrToInt(ptr); ptr=pos+1;
        if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
        *pos=0; PasswordToConnect=ptr; ptr=pos+1;
        if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
        *pos=0; ClientNumber=StrToInt(ptr); ptr=pos+1;
        if ( (ClientNumber<0)||(ClientNumber>MaxClients) ) goto error;
        for ( int i=0; i<ClientNumber; i++ )
        {
            if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
            *pos=0; Clients[i].Address=ptr; ptr=pos+1;
            if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
            *pos=0; Clients[i].Number=StrToInt(ptr); ptr=pos+1;
        }
        if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
        *pos=0; TariffNumber=StrToInt(ptr); ptr=pos+1;
        if ( (TariffNumber<0)||(TariffNumber>MaxTariffs) ) goto error;
        for ( int i=0; i<TariffNumber; i++ )
        {
            if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
            *pos=0; Tariffs[i].Begin=StrToInt(ptr); ptr=pos+1;
            if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
            *pos=0; Tariffs[i].Size=StrToInt(ptr); ptr=pos+1;
        }
    }
    catch ( EConvertError *Error ) { goto error; }

    delete Data;
    return 0;
error:
    delete Data;
    return -1;
}

int __fastcall MOptions::Save()
{
    AnsiString Line;
    char *Data, *Buffer;
    int DataSize, File;

    // Подготовка данных для сохранения
    Line+=AdminPassword+"\n";
    Line+=IntToStr(Mode)+"\n";
    Line+=PasswordToConnect+"\n";
    Line+=IntToStr(ClientNumber)+"\n";
    for ( int i=0; i<ClientNumber; i++ )
    {
        Line+=Clients[i].Address+"\n";
        Line+=IntToStr(Clients[i].Number)+"\n";
    }
    Line+=IntToStr(TariffNumber)+"\n";
    for ( int i=0; i<TariffNumber; i++ )
    {
        Line+=IntToStr(Tariffs[i].Begin)+"\n";
        Line+=IntToStr(Tariffs[i].Size)+"\n";
    }
    // Шифровка данных
    Data=Line.c_str(); DataSize=StrLen(Data);
    Buffer=new char[DataSize];
    if ( Buffer==NULL ) return -1;
    memcpy(Buffer,Data,DataSize);
    for ( int i=0; i<=(DataSize-4); i++ ) *((unsigned int*)(Buffer+i))+=ProtectCode;
    // Сохранение данных
    File=FileCreate(WinDir+"\\TmLockSr.cfg");
    if ( File<0 ) goto error;
    if ( FileWrite(File,Buffer,DataSize)<0 ) goto error;
    FileClose(File);

    delete Buffer;
    return 0;
error:
    delete Buffer;
    return -1;
}

int __fastcall MClientOptions::Load(AnsiString FileName)
{
    int DataSize, File;
    char *Data;

    // Загрузка зашифрованных опций в память
    File=FileOpen(FileName,fmOpenRead);
    if ( File<0 ) return -1;
    DataSize=FileSeek(File,0,2); FileSeek(File,0,0);
    Data=new char[DataSize+1];
    if ( Data==NULL ) { FileClose(File); return -1; }
    if ( FileRead(File,Data,DataSize)<0 ) goto error;
    FileClose(File);
    // Расшифровка опций
    for ( int i=DataSize-4; i>=0; i-- ) *((unsigned int*)(Data+i))-=ProtectCode;
    // Подготовка данных к интерпретации
    for ( int i=0; i<DataSize; i++ ) if ( (unsigned char)Data[i]<0x0A ) goto error;
    Data[DataSize]=0;
    // Разбор параметров
    try
    {
        char *ptr=Data, *pos;
        if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
        *pos=0; ServerAddress=ptr; ptr=pos+1;
        if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
        *pos=0; ServerAuto=(bool)StrToInt(ptr); ptr=pos+1;
        if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
        *pos=0; PasswordToConnect=ptr; ptr=pos+1;
        if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
        *pos=0; NumberComputer=StrToInt(ptr); ptr=pos+1;
        if ( (pos=strchr(ptr,'\n'))==NULL ) goto error;
        *pos=0; OptionsPassword=ptr;
    }
    catch ( EConvertError *Error ) { goto error; }

    DataSize=ServerAddress.Length();
    if ( (DataSize<1)||(DataSize>16) ) goto error;
    DataSize=PasswordToConnect.Length();
    if ( (DataSize<1)||(DataSize>16) ) goto error;
    DataSize=OptionsPassword.Length();
    if ( (DataSize<1)||(DataSize>16) ) goto error;

    delete Data;
    return 0;
error:
    delete Data;
    return -1;
}

int __fastcall MClientOptions::Save(AnsiString FileName)
{
    AnsiString Line;
    char *Data, *Buffer;
    int DataSize, File;

    // Подготовка данных для сохранения
    Line+=ServerAddress+"\n";
    if ( ServerAuto ) Line+="1\n"; else Line+="0\n";
    Line+=PasswordToConnect+"\n";
    Line+=IntToStr(NumberComputer)+"\n";
    Line+=OptionsPassword+"\n";
    // Шифровка данных
    Data=Line.c_str(); DataSize=StrLen(Data);
    Buffer=new char[DataSize];
    if ( Buffer==NULL ) return -1;
    memcpy(Buffer,Data,DataSize);
    for ( int i=0; i<=(DataSize-4); i++ ) *((unsigned int*)(Buffer+i))+=ProtectCode;
    // Сохранение данных
    File=FileCreate(FileName);
    if ( File<0 ) goto error;
    if ( FileWrite(File,Buffer,DataSize)<0 ) goto error;
    FileClose(File);

    delete Buffer;
    return 0;
error:
    delete Buffer;
    return -1;
}

//---------------------------------------------------------------------------

