//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("TLk.res");
USEUNIT("UnitGames.cpp");
USEUNIT("UnitCommon.cpp");
USEFORM("UnitFormMain.cpp", FormMain);
USEUNIT("UnitState.cpp");
USEUNIT("UnitSync.cpp");
USEUNIT("UnitSLList.cpp");
USEUNIT("UnitList.cpp");
USEUNIT("UnitSend.cpp");
USEUNIT("UnitMessage.cpp");
USEUNIT("UnitClOptions.cpp");
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
    // Создаем глобальный маркер запуска TLK и выходим, если он уже существует
    HANDLE mutex=::CreateMutex(NULL,true,"TLKCLRunMutex");
    if ( (mutex==NULL)||(::GetLastError()==ERROR_ALREADY_EXISTS) )
    {
        ::CloseHandle(mutex); return 0;
    }
    //
    try
    {
         Application->Initialize();
         Application->Title = "";
         Application->CreateForm(__classid(TFormMain), &FormMain);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    // Удаляем глобальный маркер
    ::CloseHandle(mutex);
    //
    return 0;
}
//---------------------------------------------------------------------------

