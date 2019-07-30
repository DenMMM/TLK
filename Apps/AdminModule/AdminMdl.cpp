//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormComputers.cpp", FormComputers);
USEFORM("UnitFormTariffs.cpp", FormTariffs);
USEFORM("UnitFormTariffTimes.cpp", FormTariffTimes);
USEFORM("UnitFormFines.cpp", FormFines);
USEFORM("UnitFormUsers.cpp", FormUsers);
USEFORM("UnitFormRun.cpp", FormRun);
USEFORM("UnitFormFine.cpp", FormFine);
USEFORM("UnitFormUserPassword.cpp", FormUserPassword);
USEFORM("UnitFormNewPassword.cpp", FormNewPassword);
USEFORM("UnitFormOptions.cpp", FormOptions);
USEFORM("UnitFormLogIn.cpp", FormLogIn);
USEFORM("UnitFormClient.cpp", FormClient);
USEFORM("UnitFormOptionsPassword.cpp", FormOptionsPassword);
USEFORM("UnitFormOpenConfig.cpp", FormOpenConfig);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    HANDLE mutex=::CreateMutex(NULL,true,"TLK31RunMutex");
    if ( (mutex==NULL)||(::GetLastError()==ERROR_ALREADY_EXISTS) )
    {
        ::CloseHandle(mutex);
        ::MessageBox(NULL,"TLK уже запущен или требуемый системный ресурс занят !",
            "TLK - ошибка",MB_OK|MB_ICONERROR|MB_APPLMODAL);
        return 0;
    }
    // Инициализируем генератор случайных чисел
    randomize();

        try
        {
         Application->Initialize();
         Application->Title = "TLK 3.10";
         Application->CreateForm(__classid(TFormMain), &FormMain);
         Application->CreateForm(__classid(TFormComputers), &FormComputers);
         Application->CreateForm(__classid(TFormTariffs), &FormTariffs);
         Application->CreateForm(__classid(TFormTariffTimes), &FormTariffTimes);
         Application->CreateForm(__classid(TFormFines), &FormFines);
         Application->CreateForm(__classid(TFormUsers), &FormUsers);
         Application->CreateForm(__classid(TFormRun), &FormRun);
         Application->CreateForm(__classid(TFormFine), &FormFine);
         Application->CreateForm(__classid(TFormUserPassword), &FormUserPassword);
         Application->CreateForm(__classid(TFormNewPassword), &FormNewPassword);
         Application->CreateForm(__classid(TFormOptions), &FormOptions);
         Application->CreateForm(__classid(TFormLogIn), &FormLogIn);
         Application->CreateForm(__classid(TFormClient), &FormClient);
         Application->CreateForm(__classid(TFormOptionsPassword), &FormOptionsPassword);
         Application->CreateForm(__classid(TFormOpenConfig), &FormOpenConfig);
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

