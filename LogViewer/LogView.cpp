//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("LogView.res");
USEFORM("UnitFormMain.cpp", FormMain);
USEUNIT("UnitEvents.cpp");
USEUNIT("UnitBaseClassMList.cpp");
USEUNIT("UnitCommon.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->Title = "Log Viewer";
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
