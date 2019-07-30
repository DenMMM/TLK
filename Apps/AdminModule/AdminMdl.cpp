//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "UnitCommon.h"
//---------------------------------------------------------------------------
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormComputers.cpp", FormComputers);
USEFORM("UnitFormTariffs.cpp", FormTariffs);
USEFORM("UnitFormTariffTimes.cpp", FormTariffTimes);
USEFORM("UnitFormFines.cpp", FormFines);
USEFORM("UnitFormUsers.cpp", FormUsers);
USEFORM("UnitFormRun.cpp", FormRun);
USEFORM("UnitFormFine.cpp", FormFine);
USEFORM("UnitFormUserPass.cpp", FormUserPass);
USEFORM("UnitFormNewPass.cpp", FormNewPass);
USEFORM("UnitFormOptions.cpp", FormOptions);
USEFORM("UnitFormLogIn.cpp", FormLogIn);
USEFORM("UnitFormClient.cpp", FormClient);
USEFORM("UnitFormOptionsPass.cpp", FormOptionsPass);
USEFORM("UnitFormOpenConfig.cpp", FormOpenConfig);
USEFORM("UnitFormAuth.cpp", FormAuth);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    HANDLE mutex=::CreateMutex(NULL,true,"TLK31RunMutex");
    if ( (mutex==NULL)||(::GetLastError()==ERROR_ALREADY_EXISTS) )
    {            
        ::CloseHandle(mutex);
        ResMessageBox(NULL,1,24,MB_APPLMODAL|MB_OK|MB_ICONERROR);
        return 0;
    }
    // »нициализируем генератор случайных чисел
    randomize();

        try
        {
         Application->Initialize();
         Application->Title = "TLK 3.10";
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

