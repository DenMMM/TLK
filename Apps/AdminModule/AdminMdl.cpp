//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "UnitCommon.h"
//---------------------------------------------------------------------------
USEFORM("UnitFormFine.cpp", FormFine);
USEFORM("UnitFormFines.cpp", FormFines);
USEFORM("UnitFormLogIn.cpp", FormLogIn);
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormNewPass.cpp", FormNewPass);
USEFORM("UnitFormComputers.cpp", FormComputers);
USEFORM("UnitFormAuth.cpp", FormAuth);
USEFORM("UnitFormClient.cpp", FormClient);
USEFORM("UnitFormUserPass.cpp", FormUserPass);
USEFORM("UnitFormUsers.cpp", FormUsers);
USEFORM("UnitFormTariffTimes.cpp", FormTariffTimes);
USEFORM("UnitFormOpenConfig.cpp", FormOpenConfig);
USEFORM("UnitFormOptions.cpp", FormOptions);
USEFORM("UnitFormOptionsPass.cpp", FormOptionsPass);
USEFORM("UnitFormRun.cpp", FormRun);
USEFORM("UnitFormTariffs.cpp", FormTariffs);
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
    // Инициализируем генератор случайных чисел
    randomize();

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

