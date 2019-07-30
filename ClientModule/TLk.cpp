//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("TLk.res");
USEUNIT("UnitGames.cpp");
USEUNIT("UnitOptionsLoadSave.cpp");
USEUNIT("UnitBaseClassMList.cpp");
USEUNIT("UnitCommon.cpp");
USEUNIT("UnitState.cpp");
USEFORM("UnitFormMain.cpp", FormMain);
USEUNIT("UnitThreadNetSync.cpp");
//---------------------------------------------------------------------------
typedef DWORD RegServProc(DWORD,DWORD);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // Скрываем программу из CTRL+ALT+DEL
    HINSTANCE Library;
    RegServProc *RegServProcFunc;
    if ( (Library=::LoadLibrary("KERNEL32.DLL"))!=NULL )
    {
        RegServProcFunc=(RegServProc*)::GetProcAddress(Library,"RegisterServiceProcess");
        if ( RegServProcFunc!=NULL ) RegServProcFunc(NULL,1);
        ::FreeLibrary(Library);
    }
    //
    HANDLE mutex=::CreateMutex(NULL,true,"TLKCLRunMutex");
    if ( (mutex==NULL)||(::GetLastError()==ERROR_ALREADY_EXISTS) )
    {
        ::CloseHandle(mutex); return 0;
    }
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
    //
    ::CloseHandle(mutex);
    //
    return 0;
}
//---------------------------------------------------------------------------

