//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormEvents.cpp", FormEvents);
USEFORM("UnitFormUsersUpTime.cpp", FormUsersUpTime);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->Title = "TLK Log Viewer";
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
