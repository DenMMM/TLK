//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("TmLockCl.res");
USEFORM("UnitFormMain.cpp", FormMain);
USEUNIT("UnitCommon.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->Title = "Time Locker Client";
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
