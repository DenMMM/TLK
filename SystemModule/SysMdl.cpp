//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("SysMdl.res");
USEFORM("UnitFormMain.cpp", FormMain);
USEUNIT("UnitComputers.cpp");
USEUNIT("UnitTariffs.cpp");
USEFORM("UnitFormOrder.cpp", FormOrder);
USEFORM("UnitFormComputers.cpp", FormComputers);
USEUNIT("UnitBaseClassMList.cpp");
USEFORM("UnitFormTariffs.cpp", FormTariffs);
USEFORM("UnitFormFines.cpp", FormFines);
USEUNIT("UnitOrderComputers.cpp");
USEUNIT("UnitListOfComputers.cpp");
USEUNIT("UnitListOfTariffs.cpp");
USEUNIT("UnitListOfTariffTimes.cpp");
USEFORM("UnitFormTariffTimes.cpp", FormTariffTimes);
USEFORM("UnitFormComputersConfig.cpp", FormComputersConfig);
USEUNIT("UnitOptionsLoadSave.cpp");
USEUNIT("UnitCommon.cpp");
USEUNIT("UnitListOfIDs.cpp");
USEUNIT("UnitFines.cpp");
USEUNIT("UnitFineComputers.cpp");
USEFORM("UnitFormFine.cpp", FormFine);
USEFORM("UnitFormUsers.cpp", FormUsers);
USEFORM("UnitFormAccessRights.cpp", FormAccessRights);
USEUNIT("UnitGames.cpp");
USEFORM("UnitFormChangePassword.cpp", FormChangePassword);
USEUNIT("UnitUsers.cpp");
USEFORM("UnitFormLogIn.cpp", FormLogIn);
USEUNIT("UnitThreadNetSync.cpp");
USEUNIT("UnitLogs.cpp");
USEUNIT("UnitState.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    HANDLE mutex=::CreateMutex(NULL,true,"TLKRunMutex");
    if ( (mutex==NULL)||(::GetLastError()==ERROR_ALREADY_EXISTS) )
    {
        ::CloseHandle(mutex);
        ::MessageBox(NULL,"TimeLocker уже запущен или требуемый системный ресурс занят !",
            "TimeLocker - ошибка",MB_OK|MB_ICONERROR|MB_APPLMODAL);
        return 0;
    }
        try
        {
         Application->Initialize();
         Application->Title = "TimeLocker 3.0";
         Application->CreateForm(__classid(TFormMain), &FormMain);
         Application->CreateForm(__classid(TFormOrder), &FormOrder);
         Application->CreateForm(__classid(TFormComputers), &FormComputers);
         Application->CreateForm(__classid(TFormTariffs), &FormTariffs);
         Application->CreateForm(__classid(TFormFines), &FormFines);
         Application->CreateForm(__classid(TFormTariffTimes), &FormTariffTimes);
         Application->CreateForm(__classid(TFormComputersConfig), &FormComputersConfig);
         Application->CreateForm(__classid(TFormFine), &FormFine);
         Application->CreateForm(__classid(TFormUsers), &FormUsers);
         Application->CreateForm(__classid(TFormAccessRights), &FormAccessRights);
         Application->CreateForm(__classid(TFormChangePassword), &FormChangePassword);
         Application->CreateForm(__classid(TFormLogIn), &FormLogIn);
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

