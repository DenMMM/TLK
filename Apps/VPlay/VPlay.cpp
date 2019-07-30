//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("VPlay.res");
USEFORM("UnitFormMain.cpp", FormMain);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    HANDLE mutex=::CreateMutex(NULL,true,"VPlayRunMutex");
    if ( (mutex==NULL)||(::GetLastError()==ERROR_ALREADY_EXISTS) )
    {
        ::CloseHandle(mutex);
        ::MessageBox(NULL,"������ ��� ������� !\n����������� ALT+TAB ��� ������������ ����� �����������.",
            "VPlay - ������",MB_OK|MB_ICONERROR|MB_APPLMODAL);
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

