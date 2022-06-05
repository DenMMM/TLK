//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormEvents.cpp", FormEvents);
USEFORM("UnitFormUsersUpTime.cpp", FormUsersUpTime);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR lpCmdLine, int)
{
	try
	{
		 Application->Initialize();
         Application->Title = L"TLK - Просмотр логов";
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
