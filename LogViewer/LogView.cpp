//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("LogView.res");
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormEvents.cpp", FormEvents);
USEFORM("UnitFormUsersUpTime.cpp", FormUsersUpTime);
USEUNIT("UnitAnalyse.cpp");
USEUNIT("UnitCommon.cpp");
USEUNIT("UnitComputers.cpp");
USEUNIT("UnitFines.cpp");
USEUNIT("UnitList.cpp");
USEUNIT("UnitLogRecords.cpp");
USEUNIT("UnitSLList.cpp");
USEUNIT("UnitStates.cpp");
USEUNIT("UnitTariffs.cpp");
USEUNIT("UnitUsers.cpp");
USEUNIT("UnitUsersUpTime.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
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
