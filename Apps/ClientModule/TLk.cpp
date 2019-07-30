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
    // �������������� ��������� ��������� �����
    randomize();

    try
    {
         Application->Initialize();
         Application->Title = "TLK - ������";
         Application->CreateForm(__classid(TFormMain), &FormMain);
         Application->CreateForm(__classid(TFormGames), &FormGames);
         Application->CreateForm(__classid(TFormDesk), &FormDesk);
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

