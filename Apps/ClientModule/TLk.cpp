//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormDesk.cpp", FormDesk);
USEFORM("UnitFormGames.cpp", FormGames);
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
    // Инициализируем генератор случайных чисел
    randomize();

    try
    {
         Application->Initialize();
         Application->Title = "TLK - Клиент";
         Application->CreateForm(__classid(TFormMain), &FormMain);
         Application->CreateForm(__classid(TFormGames), &FormGames);
         Application->CreateForm(__classid(TFormDesk), &FormDesk);
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

