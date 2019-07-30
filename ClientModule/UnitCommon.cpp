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

