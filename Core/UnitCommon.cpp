//---------------------------------------------------------------------------
#include <stdio.h>
#pragma hdrstop

#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
/// ������ ��� ��������
void *MemSetLine(void *Mem__, const std::string &Line__)
{
	char *Mem_=static_cast<char*>(Mem__);
	const char *Line_=Line__.c_str();           /// ����� ������������ UTF8

	while ( (*(Mem_++)=*(Line_++))!='\0' );
	return Mem_;
}

void *MemSetLine(void *Mem__, const std::wstring &Line__)
{
	wchar_t *Mem_=static_cast<wchar_t*>(Mem__);
	const wchar_t *Line_=Line__.c_str();        /// ����� ������������ UTF8

	while ( (*(Mem_++)=*(Line_++))!=L'\0' );
	return Mem_;
}

const void *MemGetLine(const void *Mem__, std::string &Line_,
	size_t MaxLength_, const void *Limit__)
{
	const char *Mem_=static_cast<const char*>(Mem__);
	const char *Limit_=static_cast<const char*>(Limit__);

	if ( (size_t)(Limit_-Mem_)>(++MaxLength_) ) Limit_=Mem_+MaxLength_;
	Line_.clear();
	Line_.reserve(MaxLength_);      			// �����������

	do
	{
		if ( Mem_>=Limit_ ) return nullptr;     /// throw ?
		if ( *Mem_=='\0' ) break;
		Line_.push_back(*Mem_);
		Mem_++;
	} while(true);
	Mem_++;

	return Mem_;
}

const void *MemGetLine(const void *Mem__, std::wstring &Line_,
	size_t MaxLength_, const void *Limit__)
{
	const wchar_t *Mem_=static_cast<const wchar_t*>(Mem__);
	const wchar_t *Limit_=static_cast<const wchar_t*>(Limit__);

	if ( (size_t)(Limit_-Mem_)>(++MaxLength_) ) Limit_=Mem_+MaxLength_;
	Line_.clear();
	Line_.reserve(MaxLength_);      			// �����������

	do
	{
		if ( Mem_>=Limit_ ) return nullptr;     /// throw ?
		if ( *Mem_==L'\0' ) break;
		Line_.push_back(*Mem_);
		Mem_++;
	} while(true);
	Mem_++;

	return Mem_;
}
//---------------------------------------------------------------------------
void BasicEncode(void *Data__, size_t DataSize_, unsigned Code_, int Round_)
{
	unsigned char *Data_=static_cast<unsigned char*>(Data__);
	unsigned char *limit;
	unsigned blk;

	if ( DataSize_<sizeof(blk) ) return;

	limit=Data_+DataSize_-sizeof(blk);
	// ������� ��������� ���� �������
	blk=*((unsigned*)Data_);
	goto begin;

	do
	{
		// �������� � ����� ������� ���� �����
		Data_[0]=blk;
		// ������� ���� ������ � �������� ������� ���� �� �����
		blk>>=8;
		blk|=( (unsigned)Data_[sizeof(blk)] )<<(sizeof(blk)*8-8);
		++Data_;
begin:
		// ���������� ���� �����
		for ( int i=Round_; i; i-- )
			blk=BasicEncodeRound(blk,Code_);
	} while(Data_!=limit);

	// �������� � ����� ��������� ���� �������
	*((unsigned*)Data_)=blk;
}

void BasicDecode(void *Data__, size_t DataSize_, unsigned Code_, int Round_)
{
	unsigned char *Data_=static_cast<unsigned char*>(Data__);
	unsigned char *limit;
    unsigned blk;

    if ( DataSize_<sizeof(blk) ) return;

    limit=Data_;
    Data_+=DataSize_-sizeof(blk);

    // ������� ��������� ���� �������
    blk=*((unsigned*)Data_);
    goto begin;

    do
    {
        --Data_;
        // �������� � ����� ������� ���� �����
        Data_[sizeof(blk)]=blk>>(sizeof(blk)*8-8);
        // ������� ���� ����� � �������� ������� ���� �� �����
        blk<<=8;
        blk|=Data_[0];
begin:
        // ���������� ���� �����
        for ( int i=Round_; i; i-- )
            blk=BasicDecodeRound(blk,Code_);
    } while(Data_!=limit);

    // �������� � ����� ��������� ���� �������
    *((unsigned*)Data_)=blk;
}

unsigned BasicMix(unsigned Key1_, unsigned Key2_)
{
    unsigned k1=Key1_;
    unsigned k2=Key2_;

    // ������� ������ ���� ���� ������
    BasicEncode((char*)&Key2_,sizeof(Key2_),k1,sizeof(Key2_)*8);
    BasicEncode((char*)&Key1_,sizeof(Key1_),k2,sizeof(Key1_)*8);
	// XOR ����������� � ��������� �� ���������
//	return (Key1_^=Key2_)? Key1_: Key2_;
	return Key1_^=Key2_;
}

unsigned BasicRand()
{
    unsigned rnd=0;
    for ( int i=sizeof(rnd); i; i--)
        rnd=(rnd<<8)|random(256);
    return rnd;
}

bool TimeRand(void *Buff__, size_t Size_)
{
	unsigned char *Buff_=static_cast<unsigned char*>(Buff__);
	LARGE_INTEGER ent1;
    DWORD ent2;

    // ��������� ������� ������ ����������
    if ( !::QueryPerformanceCounter(&ent1) ) return false;  /// throw ?
    // ��������� ������ ���������� ��
    ent2=::GetTickCount();

    // �������� �������� ����� ���������������� ����������
	for ( size_t i=0; i<Size_; i++ ) Buff_[i]=random(256);
	// ������� ��������
	BasicEncode(Buff_, Size_, ent1.LowPart, 32);
	BasicEncode(Buff_, Size_, ent1.HighPart, 32);
	BasicEncode(Buff_, Size_, ent2, 32);

	return true;
}
//---------------------------------------------------------------------------
size_t ByteToHEX(const void *Bytes__, size_t BytesCount_,
	wchar_t *Line_, size_t LineSize_, wchar_t Delim_)
{
	static const wchar_t sym[]={
		L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
		L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F'};
	const unsigned char *Bytes_=static_cast<const unsigned char*>(Bytes__);

	size_t pin=0, pout=0;
	if ( Delim_==L'\0' )
	{
		while((pin<BytesCount_)&&(pout<(LineSize_-2)))
		{
			Line_[pout+0]=sym[Bytes_[pin]>>4];
			Line_[pout+1]=sym[Bytes_[pin]&0x0F];
			pout+=2;
			pin++;
		}
	} else
	{
		while((pin<BytesCount_)&&(pout<(LineSize_-3)))
		{
			Line_[pout+0]=sym[Bytes_[pin]>>4];
			Line_[pout+1]=sym[Bytes_[pin]&0x0F];
			Line_[pout+2]=Delim_;
			pout+=3;
			pin++;
		}
	}

	// ����� ��� ������ ���������� ������, ��� ������� ��� �����
	if ( pin!=BytesCount_ )
	{
		throw std::runtime_error (
			"ByteToHEX()\n"
			"�� ����������� ������ ������ ��� HEX-������.");
	}

	// ��������� ����������� ������� ������ ������
	Line_[Delim_==L'\0'? pout: --pout]=0;

	return pout;
}

size_t HEXToByte(const wchar_t *Line_, void *Buff__, size_t BuffSize_)
{
	static const wchar_t sym[]={
		L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
		L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F',
		L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
		L'8', L'9', L'a', L'b', L'c', L'd', L'e', L'f'};
	unsigned char *Buff_=static_cast<unsigned char*>(Buff__);

    size_t pin=0, pout=0;
    const wchar_t *pos;
    unsigned char byte;

    while(pout<BuffSize_)
    {
        // ���� ������� ������� ��������� �����
        do
        {
            if ( Line_[pin]==0 ) return pout;
			pos=static_cast<const wchar_t*>(
				memchr(sym, Line_[pin++], sizeof(sym)));
		} while(pos==nullptr);
		byte=((pos-sym)%16)<<4;

		// ���� ������� ������� ��������� �����
		do
		{
			if ( Line_[pin]==0 ) return pout;
			pos=static_cast<const wchar_t*>(
				memchr(sym, Line_[pin++], sizeof(sym)));
        } while(pos==nullptr);
        byte+=(pos-sym)%16;

        // ��������� �������������� ���� � �������� ������
        Buff_[pout]=byte;
        pout++;
    }

    return pout;
}
//---------------------------------------------------------------------------
bool GeneratePassword(char *Line_, unsigned Len_,
    bool Cap_, bool Low_, bool Num_)
{
    static const char cap[]={'A','B','C','D','E','F','G','H','I','J','K','L','M',
                             'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    static const char low[]={'a','b','c','d','e','f','g','h','i','j','k','l','m',
                             'n','o','p','q','r','s','t','u','v','w','x','y','z'};
    static const char dig[]={'0','1','2','3','4','5','6','7','8','9'};
    char buff[sizeof(cap)+
             sizeof(low)+
             sizeof(dig)];
    unsigned use;

    // ������� ������� �������� � ������ ����� ������������ � ������� ��������� ���
    use=0;
    if ( Cap_ ) { memcpy(buff+use,cap,sizeof(cap)); use+=sizeof(cap); }
    if ( Low_ ) { memcpy(buff+use,low,sizeof(low)); use+=sizeof(low); }
    if ( Num_ ) { memcpy(buff+use,dig,sizeof(dig)); use+=sizeof(dig); }
    if ( use==0 ) goto error;

    // �������� ����� ���������� ����������
    TimeRand(Line_,Len_);
    // ������� �� ���������� ���������
    for (unsigned i=0; i<Len_; i++)
        Line_[i]=buff[((unsigned)Line_[i])%use];
    *(Line_+Len_)=0;

    return true;
error:
    *Line_=0;
    return false;
}
//---------------------------------------------------------------------------
int ResMessageBox(HWND Owner_, UINT uCaption_, UINT uMessage_, UINT Type_, DWORD LastErr_)
{
    wchar_t Caption[128], Message[128+2+128];
    HINSTANCE HInstance;
    DWORD Error;

    HInstance=::GetModuleHandle(nullptr);
    // ��������� ��������� ���������
	if ( ::LoadString(HInstance, uCaption_, Caption, 128)==0 )
		swprintf(Caption, sizeof(Caption), L"<< ����� ��������� �� ������ ! >>");
	// ��������� ���������
	if ( ::LoadString(HInstance,uMessage_,Message,128)==0 )
		swprintf(Message, sizeof(Caption), L"<< ����� ��������� �� ������ ! >>");

    if ( LastErr_!=0 )
    {
        // ��������� ���� ������ �����
		wcscat(Message, L"\n\n");
        // ��������� � ������ ��������� �� ������ Windows
        ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,nullptr,
            LastErr_,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
            (LPTSTR)(Message+wcslen(Message)),128,nullptr);
    }
    
    // ������� ���������/������
    return ::MessageBox(Owner_,Message,Caption,Type_);
}
//---------------------------------------------------------------------------
bool WinExit(UINT uFlags)
{
	OSVERSIONINFOW vi;
	HANDLE Process, Token=nullptr;
    LUID Luid;
    TOKEN_PRIVILEGES Privileges;

/*
	// ����������� ���������� � ������ ��
	vi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	if ( !GetVersionExW(&vi) ) goto error;
	// ��������� �� WinNT-������� �� � ��������� �� �������������� ����� �� ���������� ��������
	if ( (vi.dwPlatformId==VER_PLATFORM_WIN32_NT)&&
		(uFlags&(EWX_POWEROFF|EWX_REBOOT|EWX_SHUTDOWN)) )
*/
    {
        // ����� ��������� ��������
        Process=::GetCurrentProcess();
        // ����� ����� ��������
        if ( !::OpenProcessToken(Process,TOKEN_ADJUST_PRIVILEGES,&Token) ) goto error;
        // ����������� LUID ��� SE_SHUTDOWN_NAME
        if ( !::LookupPrivilegeValue(nullptr,SE_SHUTDOWN_NAME,&Luid) ) goto error;
        // ������������� ���������� ��� ���������� ������ �� �������
        Privileges.PrivilegeCount=1;
        Privileges.Privileges[0].Luid=Luid;
        Privileges.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
        if ( !::AdjustTokenPrivileges(Token,FALSE,&Privileges,0,nullptr,nullptr) ) goto error;
        //
        ::CloseHandle(Token); Token=nullptr;
    }

    if ( !::ExitWindowsEx(uFlags,0) ) goto error;

	return true;
error:
	if ( Token ) ::CloseHandle(Token);
	return false;
}
//---------------------------------------------------------------------------
void RegExecList(HKEY hKey_, LPCWSTR SubKey_, HANDLE hToken_)
{
	HKEY key;
	DWORD index, type, name_size, data_size;
	wchar_t name[256+1], value[MAX_PATH+1];
	LPVOID lpEnv=nullptr;

	// �������� ��������� ����� ������������
	if ( (hToken_!=INVALID_HANDLE_VALUE)&&
		(!::CreateEnvironmentBlock(&lpEnv,hToken_,FALSE)) ) return;

	// ������� ������ �������
	if ( ::RegOpenKeyExW(
			hKey_, SubKey_, 0,
			KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE,
			&key)!=ERROR_SUCCESS )
	{
		if ( lpEnv!=nullptr ) ::DestroyEnvironmentBlock(lpEnv);
		return;
	}

	index=0;
	while(true)
	{
		name_size=256;
		data_size=MAX_PATH;

		// ������� �������� ���������� ��������� �������
		if ( ::RegEnumValueW(key, index, name, &name_size, nullptr,
			&type, (BYTE*)value, &data_size)!=ERROR_SUCCESS ) break;

		if ( (data_size>1)&&(type==REG_SZ) )
		{
			bool result=false;
			STARTUPINFOW si;
			PROCESS_INFORMATION pi;

			memset(&si,0,sizeof(STARTUPINFO));
			si.cb=sizeof(STARTUPINFO);

			// �������� �������
			if ( hToken_==INVALID_HANDLE_VALUE )
			{
				result=::CreateProcessW(
					nullptr, value,
					nullptr, nullptr, FALSE,
					CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
					nullptr, nullptr, &si, &pi);
			} else
			{
				result=::CreateProcessAsUserW(
					hToken_,
					nullptr, value,
					nullptr, nullptr, FALSE,
					CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS|CREATE_UNICODE_ENVIRONMENT,
					lpEnv, nullptr, &si, &pi);
			}
			if (result)
			{
				::CloseHandle(pi.hThread);
				::CloseHandle(pi.hProcess);
			}
        }
        if ( (++index)>=100 ) break;
    }

    ::RegCloseKey(key);
    if ( lpEnv!=nullptr ) ::DestroyEnvironmentBlock(lpEnv);
}
//---------------------------------------------------------------------------

