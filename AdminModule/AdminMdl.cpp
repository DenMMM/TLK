//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("AdminMdl.res");
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormComputers.cpp", FormComputers);
USEFORM("UnitFormTariffs.cpp", FormTariffs);
USEFORM("UnitFormTariffTimes.cpp", FormTariffTimes);
USEFORM("UnitFormFines.cpp", FormFines);
USEFORM("UnitFormUsers.cpp", FormUsers);
USEFORM("UnitFormRun.cpp", FormRun);
USEFORM("UnitFormFine.cpp", FormFine);
USEUNIT("UnitCommon.cpp");
USEUNIT("UnitList.cpp");
USEUNIT("UnitSLList.cpp");
USEUNIT("UnitComputers.cpp");
USEUNIT("UnitTariffs.cpp");
USEUNIT("UnitFines.cpp");
USEUNIT("UnitUsers.cpp");
USEUNIT("UnitStates.cpp");
USEUNIT("UnitSync.cpp");
USELIB("IPHLPAPI.LIB");
USEFORM("UnitFormUserPassword.cpp", FormUserPassword);
USEFORM("UnitFormNewPassword.cpp", FormNewPassword);
USEUNIT("UnitLogRecords.cpp");
USEUNIT("UnitLog.cpp");
USEUNIT("UnitOptions.cpp");
USEFORM("UnitFormOptions.cpp", FormOptions);
USEUNIT("UnitShellState.cpp");
USEFORM("UnitFormLogIn.cpp", FormLogIn);
USERES("ErrorMessages.res");
USEFORM("UnitFormClient.cpp", FormClient);
USEUNIT("UnitGames.cpp");
USEUNIT("UnitSend.cpp");
USEUNIT("UnitClOptions.cpp");
USEFORM("UnitFormOptionsPassword.cpp", FormOptionsPassword);
USEFORM("UnitFormOpenConfig.cpp", FormOpenConfig);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    HANDLE mutex=::CreateMutex(NULL,true,"TLK30RunMutex");
    if ( (mutex==NULL)||(::GetLastError()==ERROR_ALREADY_EXISTS) )
    {
        ::CloseHandle(mutex);
        ::MessageBox(NULL,"TLK уже запущен или требуемый системный ресурс занят !",
            "TLK - ошибка",MB_OK|MB_ICONERROR|MB_APPLMODAL);
        return 0;
    }
        try
        {
         Application->Initialize();
         Application->Title = "TLK 3.0";
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

