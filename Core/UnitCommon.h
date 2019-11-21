//---------------------------------------------------------------------------
#ifndef UnitCommonH
#define UnitCommonH
//---------------------------------------------------------------------------
#include <string>
#include <winsock2.h>
//#include <windows.h>
#include <userenv.h>
#include <mem.h>
#include <stdexcept>

#include "UnitTimeRand.h"
//---------------------------------------------------------------------------
#define ENC_Code        0x5BC935CF  // Код шифрования файлов
#define ENC_Net         0x9ABD5BAE  // Код шифрования сетевых данных
//---------------------------------------------------------------------------
// Сравнение двух чисел: "0" - равны, "-1" - первое меньше второго, "+1" - наоборот
template <typename type>
//inline int DComp(type &D1_, type &D2_)
inline int DComp(type D1_, type D2_)
{
	return D1_==D2_? 0: (D1_<D2_? -1: 1);
}
//---------------------------------------------------------------------------
size_t sizeofLine(const std::wstring &Line_)
{
	return (Line_.length()+1)*sizeof(wchar_t);
}

size_t sizeofLine(const std::string &Line_)
{
	return (Line_.length()+1)*sizeof(char);
}

// Сохранение переменной в области памяти со сдвигом указателя
template <typename type>
inline void *MemSet(void *Mem_, const type Data_)
{
	*static_cast<type*>(Mem_)=Data_;
	return static_cast<char*>(Mem_)+sizeof(type);
}

// Чтение переменной из области памяти со сдвигом указателя
// и контролем выхода за границу области
template <typename type>
inline const void *MemGet(const void *Mem_, type *Data_, const void *Limit_)
{
	const void *new_limit=
		static_cast<const char*>(Mem_)+sizeof(type);

	if ( new_limit>Limit_ ) return nullptr;     /// throw ?
	*Data_=*static_cast<const type*>(Mem_);

	return new_limit;
}

// Поиск значения в области памяти с контролем выхода за границу
template <typename type>
type *MemSrch(type *Mem_, const type *Limit_, type Data_)
{
    do { if ( Mem_>=Limit_ ) return nullptr; }
    while( *(Mem_++)!=Data_ );
    return Mem_;
}

// Копирование строк со сдвигом указателя, контролем длины и выхода за границу
void *MemSetLine(void *Mem_, const std::string &Line__);
void *MemSetLine(void *Mem__, const std::wstring &Line__);
const void *MemGetLine(const void *Mem__, std::string &Line_,
	size_t MaxLength_, const void *Limit__);
const void *MemGetLine(const void *Mem__, std::wstring &Line_,
	size_t MaxLength_, const void *Limit_);

inline void *MemSetBin(void *Mem_, const void *Line_, unsigned Size_)
{
    memcpy(Mem_,Line_,Size_);
	return static_cast<char*>(Mem_)+Size_;
}

inline const void *MemGetBin(const void *Mem_, void *Line_, unsigned Size_, const void *Limit_)
{
	const void *new_limit=
		static_cast<const char*>(Mem_)+Size_;

	if ( new_limit>Limit_) return nullptr;      /// throw ?
	memcpy(Line_,Mem_,Size_);

	return new_limit;
}

inline bool MemSlowCmp(const void *Mem1__, const void *Mem2__, size_t Size_)
{
	const char *Mem1_=static_cast<const char*>(Mem1__);
	const char *Mem2_=static_cast<const char*>(Mem2__);
	size_t diff_cnt=0;

	while(Size_)
	{
		diff_cnt += (size_t)(*Mem1_ != *Mem2_);
		Mem1_++; Mem2_++; Size_--;
	}

	return diff_cnt==0;
}
//---------------------------------------------------------------------------
inline bool SystemTimeToInt64(const SYSTEMTIME& rSystemTime, __int64& rInt64)
{
	union
	{
		FILETIME FileTime;
		LARGE_INTEGER LargeInteger;
	};

	if ( !::SystemTimeToFileTime(&rSystemTime,&FileTime) ) return false;    /// throw ?
	rInt64=LargeInteger.QuadPart;

	return true;
}

inline bool Int64ToSystemTime(const __int64 Int64, SYSTEMTIME& rSystemTime)
{
	union
	{
		LARGE_INTEGER LargeInteger;
		FILETIME FileTime;
	};

	LargeInteger.QuadPart=Int64;
//    memcpy(&FileTime,&LargeInteger,sizeof(FileTime));

	return ::FileTimeToSystemTime(&FileTime,&rSystemTime);
}

inline int ExtractHoursMin(const __int64 Int64)
{
	SYSTEMTIME ssTime;
	if ( !Int64ToSystemTime(Int64,ssTime) ) return -1;
	return ssTime.wHour*60+ssTime.wMinute;
}

inline bool GetLocalTimeInt64(__int64& rInt64)
{
	SYSTEMTIME ssTime;
	::GetLocalTime(&ssTime);
	return SystemTimeToInt64(ssTime,rInt64);
}

inline bool SetLocalTimeInt64(const __int64 Int64)
{
	SYSTEMTIME ssTime;
	return Int64ToSystemTime(Int64,ssTime)&&
        ::SetLocalTime(&ssTime);
}
//---------------------------------------------------------------------------
extern MTimeRand BasicRand;
void TimeRand(void *Buff__, size_t Size_);
bool CngRand(void *Buff_, size_t Size_);
//---------------------------------------------------------------------------
size_t ByteToHEX(const void *Bytes__, size_t BytesCount_,
	wchar_t *Line_, size_t LineSize_, wchar_t Delim_=L'\0');
size_t HEXToByte(const wchar_t *Line_, void *Buff__, size_t BuffSize_);
//---------------------------------------------------------------------------
int ResMessageBox(HWND Owner_, UINT uCaption, UINT uMessage, UINT Type_, DWORD LastErr_=0);
//---------------------------------------------------------------------------
// Выход из системы
bool WinExit(UINT uFlags);
//---------------------------------------------------------------------------
// Запуск программ по списку из реестра
void RegExecList(HKEY hKey_, LPCWSTR SubKey_, HANDLE hToken_=INVALID_HANDLE_VALUE);
//---------------------------------------------------------------------------
// Вычисляет хэш MAC-адресов всех физических сетевых адаптеров
uint32_t CalcHwMacHash();    		/// uint64_t ?
//---------------------------------------------------------------------------
#endif

