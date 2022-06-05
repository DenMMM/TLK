//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "UnitCommon.h"
//---------------------------------------------------------------------------
USEFORM("UnitFormLogIn.cpp", FormLogIn);
USEFORM("UnitFormFines.cpp", FormFines);
USEFORM("UnitFormFine.cpp", FormFine);
USEFORM("UnitFormOpenConfig.cpp", FormOpenConfig);
USEFORM("UnitFormNewPass.cpp", FormNewPass);
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormComputers.cpp", FormComputers);
USEFORM("UnitFormClient.cpp", FormClient);
USEFORM("UnitFormAuth.cpp", FormAuth);
USEFORM("UnitFormOptions.cpp", FormOptions);
USEFORM("UnitFormTariffs.cpp", FormTariffs);
USEFORM("UnitFormRun.cpp", FormRun);
USEFORM("UnitFormOptionsPass.cpp", FormOptionsPass);
USEFORM("UnitFormUsers.cpp", FormUsers);
USEFORM("UnitFormUserPass.cpp", FormUserPass);
USEFORM("UnitFormTariffTimes.cpp", FormTariffTimes);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR lpCmdLine, int)
{
    HANDLE mutex=::CreateMutex(nullptr, true, L"TLK31RunMutex");
	if (
		(mutex==nullptr)||
		(::GetLastError()==ERROR_ALREADY_EXISTS) )
	{
        ::CloseHandle(mutex);
        ResMessageBox(nullptr,1,24,MB_APPLMODAL|MB_OK|MB_ICONERROR);
        return 0;
    }

	try
	{
		Application->Initialize();
		Application->Title = L"TLK - Модуль управления";
		Application->CreateForm(__classid(TFormMain), &FormMain);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}

	::CloseHandle(mutex);
	return 0;
}
//---------------------------------------------------------------------------

