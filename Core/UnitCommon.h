//---------------------------------------------------------------------------
#ifndef UnitCommonH
#define UnitCommonH
//---------------------------------------------------------------------------
#include <winsock2.h>
//#include <windows.h>
//---------------------------------------------------------------------------
#define ENC_Code        0x5BC935CF  // Код шифрования файлов
#define ENC_Net         0x9ABD5BAE  // Код шифрования сетевых пакетов
//---------------------------------------------------------------------------
template <typename type>
inline int DComp(type &D1_, type &D2_)
{
    return D1_==D2_? 0: (D1_<D2_? -1: 1);
}

// Побитовый циклический сдвиг влево
template <typename type>
inline type BitsLeft(type Bits_)
{
    return (Bits_<<1)|(Bits_>>(sizeof(type)*8-1));
}

// Побитовый циклический сдвиг вправо
template <typename type>
inline type BitsRight(type Bits_)
{
    return (Bits_>>1)|(Bits_<<(sizeof(type)*8-1));
}
//---------------------------------------------------------------------------
// Сохранение переменной в области памяти со сдвигом указателя
template <typename type>
inline char *MemSet(char *Mem_, type Data_)
{
    *((type*)Mem_)=Data_;
    return Mem_+sizeof(type);
}

// Чтение переменной из области памяти со сдвигом указателя
// и контролем выхода за границу области
template <typename type>
inline char *MemGet(char *Mem_, type *Data_, char *Limit_)
{
    if ( (Mem_+sizeof(type))>Limit_ ) return NULL;
    *Data_=*((type*)Mem_);
    return Mem_+sizeof(type);
}

template <typename type>
type *MemSrch(type *Mem_, type *Limit_, type Data_)
{
    do { if ( Mem_>=Limit_ ) return NULL; }
    while( *(Mem_++)!=Data_ );
    return Mem_;
}

char *MemSetCLine(char *Mem_, char *Line_);
char *MemGetCLine(char *Mem_, char *Line_, unsigned MaxLength_, char *Limit_);
//---------------------------------------------------------------------------
inline bool SystemTimeToInt64(SYSTEMTIME *lpSystemTime, __int64 *lpInt64)
{
    FILETIME FileTime;
    LARGE_INTEGER LargeInteger;

    if ( !::SystemTimeToFileTime(lpSystemTime,&FileTime) ) return false;
    memcpy(&LargeInteger,&FileTime,sizeof(LargeInteger));
    *lpInt64=LargeInteger.QuadPart;

    return true;
}

inline bool Int64ToSystemTime(__int64 *lpInt64, SYSTEMTIME *lpSystemTime)
{
    LARGE_INTEGER LargeInteger;
    FILETIME FileTime;

    LargeInteger.QuadPart=*lpInt64;
    memcpy(&FileTime,&LargeInteger,sizeof(FileTime));

    return ::FileTimeToSystemTime(&FileTime,lpSystemTime);
}

inline int ExtractHoursMin(__int64 Int64)
{
    SYSTEMTIME ssTime;
    if ( !Int64ToSystemTime(&Int64,&ssTime) ) return -1;
    return ssTime.wHour*60+ssTime.wMinute;
}

inline bool GetLocalTimeInt64(__int64 *lpInt64)
{
    SYSTEMTIME ssTime;
    ::GetLocalTime(&ssTime);
    return SystemTimeToInt64(&ssTime,lpInt64);
}

/*
bool GetLocalTimeInt64(__int64 *lpInt64)
{
    SYSTEMTIME ssTime;
    FILETIME FileTime;
    LARGE_INTEGER LargeInteger;
    ::GetLocalTime(&ssTime);
    if ( !::SystemTimeToFileTime(&ssTime,&FileTime) ) return false;
    memcpy(&LargeInteger,&FileTime,sizeof(LargeInteger));
    *lpInt64=LargeInteger.QuadPart;
    return true;
}
*/

inline bool SetLocalTimeInt64(__int64 *lpInt64)
{
    SYSTEMTIME ssTime;
    return Int64ToSystemTime(lpInt64,&ssTime)&&
        ::SetLocalTime(&ssTime);
}
//---------------------------------------------------------------------------
// Простейшее шифрование/дешифрование
void BasicEncode(char *Data_, unsigned DataSize_, unsigned Code_);
void BasicDecode(char *Data_, unsigned DataSize_, unsigned Code_);
//---------------------------------------------------------------------------
bool GeneratePassword(char *Line_, unsigned Length_,
    bool Capital_, bool Small_, bool Numbers_);
//---------------------------------------------------------------------------
// Выход из системы
bool WinExit(UINT uFlags);
//---------------------------------------------------------------------------
// Запуск программ по списку из реестра
void RegExecList(HKEY hkey, LPCTSTR subkey);
//---------------------------------------------------------------------------
#endif

