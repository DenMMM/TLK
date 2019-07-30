//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("TmLockSr.res");
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormMainOptions.cpp", FormMainOptions);
USEFORM("UnitFormAbout.cpp", FormAbout);
USEUNIT("UnitCommon.cpp");
USEFORM("UnitFormClientConfig.cpp", FormClientConfig);
USEFORM("UnitFormGamesList.cpp", FormGamesList);
USEFORM("UnitFormPasswords.cpp", FormPasswords);
USEFORM("..\..\..\Администратор\Рабочий стол\Server\UnitModeChange.cpp", FormModeChange);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->Title = "Time Locker Server";
         Application->HelpFile = "TmLocker.hlp";
        Application->CreateForm(__classid(TFormMain), &FormMain);
        Application->CreateForm(__classid(TFormAbout), &FormAbout);
        Application->CreateForm(__classid(TFormMainOptions), &FormMainOptions);
        Application->CreateForm(__classid(TFormClientConfig), &FormClientConfig);
        Application->CreateForm(__classid(TFormGamesList), &FormGamesList);
        Application->CreateForm(__classid(TFormPasswords), &FormPasswords);
        Application->CreateForm(__classid(TFormModeChange), &FormModeChange);
        Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
