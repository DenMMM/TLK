//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
char win_dir[MAX_PATH];
double SystemTime;
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

    // ����� �������� ����� ������������� �������
    Buffer=new char[26+26+10]; if ( Buffer==NULL ) goto error;

    // ������� ������� �������� � ������ ����� ������������ � ������� ��������� ���
    AllSymbols=0;
    if ( Capital_ ) { memcpy(Buffer+AllSymbols,"ABCDEFGHIJKLMNOPQRSTUVWXYZ",26); AllSymbols+=26; }
    if ( Small_ ) { memcpy(Buffer+AllSymbols,"abcdefghijklmnopqrstuvwxyz",26); AllSymbols+=26; }
    if ( Numbers_ ) { memcpy(Buffer+AllSymbols,"0123456789",10); AllSymbols+=10; }
    if ( AllSymbols==0 ) goto error;

    // ��������� ������ � ������� ���������� ��������� �� ������
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
bool Exit(UINT uFlags)
{
    OSVERSIONINFO vi;
    HANDLE Process, Token=NULL;
    LUID Luid;
    TOKEN_PRIVILEGES Privileges;

    // ����������� ���������� � ������ ��
    vi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
    if ( !GetVersionEx(&vi) ) goto error;
    // ��������� �� WinNT-������� �� � ��������� �� �������������� ����� �� ���������� ��������
    if ( (vi.dwPlatformId==VER_PLATFORM_WIN32_NT)&&
        (uFlags&(EWX_POWEROFF|EWX_REBOOT|EWX_SHUTDOWN)) )
    {
        // ����� ��������� ��������
        Process=::GetCurrentProcess();
        // ����� ����� ��������
        if ( !::OpenProcessToken(Process,TOKEN_ADJUST_PRIVILEGES,&Token) ) goto error;
        // ����������� LUID ��� SE_SHUTDOWN_NAME
        if ( !::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&Luid) ) goto error;
        // ������������� ���������� ��� ���������� ������ �� �������
        Privileges.PrivilegeCount=1;
        Privileges.Privileges[0].Luid=Luid;
        Privileges.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
        if ( !::AdjustTokenPrivileges(Token,FALSE,&Privileges,0,NULL,NULL) ) goto error;
        //
        ::CloseHandle(Token); Token=NULL;
    }

    if ( !::ExitWindowsEx(uFlags,0) ) goto error;

    return true;
error:
    if ( Token ) ::CloseHandle(Token);
    return false;
}
//---------------------------------------------------------------------------
void RegExecList(HKEY hkey, LPCTSTR subkey)
{
    HKEY key;
    DWORD index, type, name_size, data_size;
    char name[256], value[256];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    if ( ::RegOpenKeyEx(hkey,subkey,0,
        KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS ) return;

    index=0;
    while(true)
    {
        name_size=data_size=256;
        if ( ::RegEnumValue(key,index,name,&name_size,NULL,
            &type,value,&data_size)!=ERROR_SUCCESS ) break;
        if ( (data_size>1)&&(type==REG_SZ) )
        {
            memset(&si,0,sizeof(STARTUPINFO));
            si.cb=sizeof(STARTUPINFO);
            ::CreateProcess(NULL,value,NULL,NULL,FALSE,
                CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
                NULL,NULL,&si,&pi);
        }
        if ( (++index)>=100 ) break;
    }

    ::RegCloseKey(key);
}
//---------------------------------------------------------------------------

