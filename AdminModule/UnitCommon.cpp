//---------------------------------------------------------------------------
//#include <stddef.h>
#pragma hdrstop

#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool SystemTimeToInt64(SYSTEMTIME *lpSystemTime, __int64 *lpInt64)
{
    FILETIME FileTime;
    LARGE_INTEGER LargeInteger;

    if ( !::SystemTimeToFileTime(lpSystemTime,&FileTime) ) return false;
    memcpy(&LargeInteger,&FileTime,sizeof(LargeInteger));
    *lpInt64=LargeInteger.QuadPart;

    return true;
}

bool Int64ToSystemTime(__int64 *lpInt64, SYSTEMTIME *lpSystemTime)
{
    LARGE_INTEGER LargeInteger;
    FILETIME FileTime;

    LargeInteger.QuadPart=*lpInt64;
    memcpy(&FileTime,&LargeInteger,sizeof(FileTime));

    return ::FileTimeToSystemTime(&FileTime,lpSystemTime);
}

int ExtractHoursMin(__int64 Int64)
{
    SYSTEMTIME ssTime;
    if ( !Int64ToSystemTime(&Int64,&ssTime) ) return -1;
    return ssTime.wHour*60+ssTime.wMinute;
}

bool GetLocalTimeInt64(__int64 *lpInt64)
{
    SYSTEMTIME ssTime;
    ::GetLocalTime(&ssTime);
    return SystemTimeToInt64(&ssTime,lpInt64);
}

bool SetLocalTimeInt64(__int64 *lpInt64)
{
    SYSTEMTIME ssTime;
    return Int64ToSystemTime(lpInt64,&ssTime)&&
        ::SetLocalTime(&ssTime);
}

//---------------------------------------------------------------------------

void BasicCode(char *Data_, unsigned DataSize_, unsigned Code_)
{
    while(DataSize_>=sizeof(unsigned))
    {
        *((unsigned*)Data_)+=Code_;
        Data_++; DataSize_--;
    }
}

void BasicEncode(char *Data_, unsigned DataSize_, unsigned Code_)
{
    Data_+=DataSize_-sizeof(unsigned);

    while(DataSize_>=sizeof(unsigned))
    {
        *((unsigned*)Data_)-=Code_;
        Data_--; DataSize_--;
    }
}

//---------------------------------------------------------------------------

bool GeneratePassword(char *Line_, unsigned Length_,
    bool Capital_, bool Small_, bool Numbers_)
{
    int AllSymbols;
    char *Buffer;

    // Сразу выделяем буфер максимального размера
    Buffer=new char[26+26+10]; if ( Buffer==NULL ) goto error;

    // Считаем сколько символов в буфере будет использовано и попутно заполняем его
    AllSymbols=0;
    if ( Capital_ ) { memcpy(Buffer+AllSymbols,"ABCDEFGHIJKLMNOPQRSTUVWXYZ",26); AllSymbols+=26; }
    if ( Small_ ) { memcpy(Buffer+AllSymbols,"abcdefghijklmnopqrstuvwxyz",26); AllSymbols+=26; }
    if ( Numbers_ ) { memcpy(Buffer+AllSymbols,"0123456789",10); AllSymbols+=10; }
    if ( AllSymbols==0 ) goto error;

    // Заполняем строку с паролем случайными символами из буфера
    for ( unsigned i=0; i<Length_; i++ )
        Line_[i]=Buffer[random(AllSymbols)];
    *(Line_+Length_)=0;

    delete[] Buffer;
    return true;
error:
    delete[] Buffer;
    *Line_=0;
    return false;
}

//---------------------------------------------------------------------------

char *MemSetCLine(char *Mem_, char *Line_)
{
    while ( (*(Mem_++)=*(Line_++))!=0 );
    return Mem_;
}

char *MemGetCLine(char *Mem_, char *Line_, unsigned MaxLength_, char *Limit_)
{
    if ( (Limit_-Mem_)>(++MaxLength_) ) Limit_=Mem_+MaxLength_;

    do { if ( Mem_>=Limit_ ) return NULL; }
    while( (*(Line_++)=*(Mem_++))!=0 );

    return Mem_;
}

//---------------------------------------------------------------------------

