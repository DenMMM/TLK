//---------------------------------------------------------------------------
#include <vcl.h>
#include "UnitCommon.h"
#pragma hdrstop
USERES("TLk.res");
USEUNIT("UnitGames.cpp");
USEUNIT("UnitOptionsLoadSave.cpp");
USEUNIT("UnitBaseClassMList.cpp");
USEUNIT("UnitCommon.cpp");
USEUNIT("UnitState.cpp");
USEUNIT("UnitThreadNetSync.cpp");
USEFORM("UnitFormMain.cpp", FormMain);
//---------------------------------------------------------------------------
typedef DWORD RegServProc(DWORD,DWORD);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // �������� ��������� �� CTRL+ALT+DEL
    HINSTANCE Library;
    RegServProc *RegServProcFunc;
    if ( (Library=::LoadLibrary("KERNEL32.DLL"))!=NULL )
    {
        RegServProcFunc=(RegServProc*)::GetProcAddress(Library,"RegisterServiceProcess");
        if ( RegServProcFunc!=NULL ) RegServProcFunc(NULL,1);
        ::FreeLibrary(Library);
    }
    // ������� ���������� ������ ������� TLK � �������, ���� �� ��� ����������
    HANDLE mutex=::CreateMutex(NULL,true,"TLKCLRunMutex");
    if ( (mutex==NULL)||(::GetLastError()==ERROR_ALREADY_EXISTS) )
    {
        ::CloseHandle(mutex); return 0;
    }
    // ��������� ��������� �� ������� �����������
    RegExecList(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
//    RegExecList(HKEY_LOCAL_MACHINE,"SOFTWARE\\MMM Groups\\Time Locker\\3.0\\Client\\Run");
    //
    try
    {
         Application->Initialize();
         Application->Title = "TimeLocker";
         Application->CreateForm(__classid(TFormMain), &FormMain);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    // ������� ���������� ������
    ::CloseHandle(mutex);
    //
    return 0;
}
//---------------------------------------------------------------------------

