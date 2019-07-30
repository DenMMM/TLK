//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
char win_dir[MAX_PATH];
double SystemTime;
//---------------------------------------------------------------------------
void BasicCode(char *Data_, int DataSize_, unsigned int Code_)
{
    for ( int i=0, j=DataSize_-sizeof(unsigned int); i<=j; i++ ) *((unsigned int*)(Data_+i))+=Code_;
}
//---------------------------------------------------------------------------
void BasicEncode(char *Data_, int DataSize_, unsigned int Code_)
{
    for ( int i=DataSize_-sizeof(unsigned int); i>=0; i-- ) *((unsigned int*)(Data_+i))-=Code_;
}
//---------------------------------------------------------------------------
bool Exit(UINT uFlags)
{
    OSVERSIONINFO vi;
    HANDLE Process, Token=NULL;
    LUID Luid;
    TOKEN_PRIVILEGES Privileges;

    // Запрашиваем информацию о версии ОС
    vi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
    if ( !GetVersionEx(&vi) ) goto error;
    // Проверяем не WinNT-система ли и требуются ли дополнительные права на выполнение операции
    if ( (vi.dwPlatformId==VER_PLATFORM_WIN32_NT)&&
        (uFlags&(EWX_POWEROFF|EWX_REBOOT|EWX_SHUTDOWN)) )
    {
        // Берем описатель процесса
        Process=::GetCurrentProcess();
        // Берем токен процесса
        if ( !::OpenProcessToken(Process,TOKEN_ADJUST_PRIVILEGES,&Token) ) goto error;
        // Запрашиваем LUID для SE_SHUTDOWN_NAME
        if ( !::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&Luid) ) goto error;
        // Устанавливаем привелегию для выполнения выхода из системы
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

