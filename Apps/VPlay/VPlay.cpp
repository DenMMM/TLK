//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("VPlay.res");
USEFORM("UnitFormMain.cpp", FormMain);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    HANDLE mutex=::CreateMutex(nullptr,true,L"VPlayRunMutex");
    if ( (mutex==nullptr)||(::GetLastError()==ERROR_ALREADY_EXISTS) )
    {
        ::CloseHandle(mutex);
        ::MessageBox(nullptr,L"������ ��� ������� !\n����������� ALT+TAB ��� ������������ ����� �����������.",
            L"VPlay - ������",MB_OK|MB_ICONERROR|MB_APPLMODAL);
        return 0;
    }
    try
    {
         Application->Initialize();
         Application->Title = "TLK - �����-������";
         Application->CreateForm(__classid(TFormMain), &FormMain);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------

