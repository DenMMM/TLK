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
//---------------------------------------------------------------------------
#define ENC_Code        0x5BC935CF  // ��� ���������� ������
#define ENC_Net         0x9ABD5BAE  // ��� ���������� ������� ������
//---------------------------------------------------------------------------
// ��������� ���� �����: "0" - �����, "-1" - ������ ������ �������, "+1" - ��������
template <typename type>
//inline int DComp(type &D1_, type &D2_)
inline int DComp(type D1_, type D2_)
{
	return D1_==D2_? 0: (D1_<D2_? -1: 1);
}

// ��������� ����������� ����� �����
template <typename type>
inline type BitsLeft(type Bits_)
{
	return (Bits_<<1)|(Bits_>>(sizeof(type)*8-1));
}

// ��������� ����������� ����� ������
template <typename type>
inline type BitsRight(type Bits_)
{
    return (Bits_>>1)|(Bits_<<(sizeof(type)*8-1));
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

// ���������� ���������� � ������� ������ �� ������� ���������
template <typename type>
inline void *MemSet(void *Mem_, const type Data_)
{
	*static_cast<type*>(Mem_)=Data_;
	return static_cast<char*>(Mem_)+sizeof(type);
}

// ������ ���������� �� ������� ������ �� ������� ���������
// � ��������� ������ �� ������� �������
template <typename type>
inline const void *MemGet(const void *Mem_, type *Data_, const void *Limit_)
{
	const void *new_limit=
		static_cast<const char*>(Mem_)+sizeof(type);

	if ( new_limit>Limit_ ) return nullptr;     /// throw ?
	*Data_=*static_cast<const type*>(Mem_);

	return new_limit;
}

// ����� �������� � ������� ������ � ��������� ������ �� �������
template <typename type>
type *MemSrch(type *Mem_, const type *Limit_, type Data_)
{
    do { if ( Mem_>=Limit_ ) return nullptr; }
    while( *(Mem_++)!=Data_ );
    return Mem_;
}

// ����������� ����� �� ������� ���������, ��������� ����� � ������ �� �������
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
inline bool SystemTimeToInt64(const SYSTEMTIME *lpSystemTime, __int64 *lpInt64)
{
    FILETIME FileTime;
    LARGE_INTEGER LargeInteger;

    if ( !::SystemTimeToFileTime(lpSystemTime,&FileTime) ) return false;
    memcpy(&LargeInteger,&FileTime,sizeof(LargeInteger));
    *lpInt64=LargeInteger.QuadPart;

    return true;
}

inline bool Int64ToSystemTime(const __int64 *lpInt64, SYSTEMTIME *lpSystemTime)
{
    LARGE_INTEGER LargeInteger;
    FILETIME FileTime;

    LargeInteger.QuadPart=*lpInt64;
    memcpy(&FileTime,&LargeInteger,sizeof(FileTime));

    return ::FileTimeToSystemTime(&FileTime,lpSystemTime);
}

inline int ExtractHoursMin(const __int64 Int64)
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

inline bool SetLocalTimeInt64(const __int64 *lpInt64)
{
    SYSTEMTIME ssTime;
    return Int64ToSystemTime(lpInt64,&ssTime)&&
        ::SetLocalTime(&ssTime);
}
//---------------------------------------------------------------------------
// �������� ����� ����������
template <typename type>
inline type BasicEncodeRound(type Blk_, type Code_)
{
    Blk_+=Code_;                             // ���������� � ����������� ���������
    Blk_=BitsRight(Blk_);                    // ��������� ����������� ����� ������
    Blk_^=Code_;                             // ��������� ����������� "���"
    return Blk_;
}

// �������� ����� ������������
template <typename type>
inline type BasicDecodeRound(type Blk_, type Code_)
{
    Blk_^=Code_;                             // ��������� ����������� "���"
    Blk_=BitsLeft(Blk_);                     // ��������� ����������� ����� ������
    Blk_-=Code_;                             // ���������� � ����������� ���������
    return Blk_;
}

// ���������� ������-��������� ����������/������������
void BasicEncode(void *Data__, size_t DataSize_, unsigned Code_, int Round_=8);
void BasicDecode(void *Data__, size_t DataSize_, unsigned Code_, int Round_=8);
unsigned BasicMix(unsigned Key1_, unsigned Key2_);
unsigned BasicRand();
bool TimeRand(void *Buff__, size_t Size_);
//---------------------------------------------------------------------------
size_t ByteToHEX(const void *Bytes__, size_t BytesCount_,
	wchar_t *Line_, size_t LineSize_, wchar_t Delim_=L'\0');
size_t HEXToByte(const wchar_t *Line_, void *Buff__, size_t BuffSize_);
//---------------------------------------------------------------------------
bool GeneratePassword(char *Line_, unsigned Len_,
    bool Cap_, bool Low_, bool Num_);
//---------------------------------------------------------------------------
int ResMessageBox(HWND Owner_, UINT uCaption, UINT uMessage, UINT Type_, DWORD LastErr_=0);
//---------------------------------------------------------------------------
// ����� �� �������
bool WinExit(UINT uFlags);
//---------------------------------------------------------------------------
// ������ �������� �� ������ �� �������
void RegExecList(HKEY hKey_, LPCWSTR SubKey_, HANDLE hToken_=INVALID_HANDLE_VALUE);
//---------------------------------------------------------------------------
#endif

