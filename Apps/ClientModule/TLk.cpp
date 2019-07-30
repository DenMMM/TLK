//---------------------------------------------------------------------------
#include <vcl.h>
#include <string.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "UnitService.h"
#include "UnitFormMain.h"
//---------------------------------------------------------------------------
USEFORM("UnitFormMain.cpp", FormMain);
USEFORM("UnitFormGames.cpp", FormGames);
//---------------------------------------------------------------------------
void ServiceInstall();
void ServiceUninstall();
void ServiceStart();
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char cmd_shell[]="/shell:";
    char *cmd_HEX=NULL;

    // Инициализируем генератор случайных чисел
    randomize();

    if ( strcmpi(lpCmdLine,"/install")==0 )
    {
        ServiceInstall();
        return 0;
    } else if ( strcmpi(lpCmdLine,"/uninstall")==0 )
    {
        ServiceUninstall();
        return 0;
    } else if ( strcmpi(lpCmdLine,"/run")==0 )
    {
        ServiceStart();
        return 0;
    } else if ( strncmpi(lpCmdLine,cmd_shell,strlen(cmd_shell))==0 )
    {
        cmd_HEX=&lpCmdLine[strlen(cmd_shell)];
    } else
    {
        // Выведем подсказку по использованию tlk.exe
        ResMessageBox(NULL,0,5,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return 0;
    }

    if ( !Shared.Open(cmd_HEX) )
    {
        Shared.Close();
        // Добавим задержку
        ::Sleep(500);           /// смысл есть ?
        return 1;
    }

    // Зададим завершать процесс последним
    ::SetProcessShutdownParameters(1,0);
    {
        // Обновим фон рабочего стола (часть функций shell'а)
        char wlp[MAX_PATH+1];
        ::SystemParametersInfo(SPI_GETDESKWALLPAPER,MAX_PATH,wlp,0);
        ::SystemParametersInfo(SPI_SETDESKWALLPAPER,0,wlp,0);   /// ,SPIF_SENDCHANGE);
    }

    try
    {
         Application->Initialize();
         Application->Title = "TLK - Клиент";
         Application->CreateForm(__classid(TFormMain), &FormMain);
         Application->CreateForm(__classid(TFormGames), &FormGames);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }

    Shared.Close();

    return 0;
}
//---------------------------------------------------------------------------
void ServiceInstall()
{
    SC_HANDLE sc_manager_handle=NULL;
    SC_HANDLE service_handle=NULL;
    const char cmd_run[]=" /run";
    char file_name[MAX_PATH+1];

    // Берем имя файла с путем к нему
    if ( ::GetModuleFileName(NULL,file_name,
        MAX_PATH-strlen(cmd_run))==0 ) goto error;
    // Добавляем к названию файла службы ключ " /run"
    strcat(file_name,cmd_run);

    // Открываем менеджер служб
    sc_manager_handle=::OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
    if ( sc_manager_handle==NULL ) goto error;
    // Регистрируем службу
    service_handle=::CreateService(
        sc_manager_handle,
        SVC_Name,
        SVC_Desc,
        0,          // SERVICE_START
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        file_name,
        "UIGroup",      /// "Base" ?
        NULL,NULL,NULL,NULL);
    if ( service_handle==NULL ) goto error;

/*
    SERVICE_FAILURE_ACTIONS sfa;
    SC_ACTION sca[3];

    sca[0].Type=SC_ACTION_RESTART;      // перезапустить службу
    sca[0].Delay=60*1000;               // через 1 минуту
    sca[1]=sca[0];
    sca[2]=sca[0];

    sfa.dwResetPeriod=24*60*60;         // сброс счетчика ошибок через сутки
    sfa.lpRebootMsg="";
    sfa.lpCommand="";
    sfa.cActions=3;
    sfa.lpsaActions=sca;

    // Задаем параметры восстановления после сбоев
    if ( !::ChangeServiceConfig2(
        service_handle,
        SERVICE_CONFIG_FAILURE_ACTIONS,
        &sfa) ) goto error;
*/

    //
    ::CloseServiceHandle(service_handle);
    ::CloseServiceHandle(sc_manager_handle);

    ResMessageBox(NULL,0,8,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
    return;

error:
    DWORD LastError=::GetLastError();
    if ( service_handle!=NULL ) ::CloseServiceHandle(service_handle);
    if ( sc_manager_handle!=NULL ) ::CloseServiceHandle(sc_manager_handle);
    ResMessageBox(NULL,1,6,MB_APPLMODAL|MB_OK|MB_ICONERROR,LastError);
    return;
}
//---------------------------------------------------------------------------
void ServiceUninstall()
{
    SC_HANDLE sc_manager_handle=NULL;
    SC_HANDLE service_handle=NULL;

    // Открываем менеджер служб
    sc_manager_handle=::OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
    if ( sc_manager_handle==NULL ) goto error;
    // Удаляем службу
    service_handle=::OpenService(sc_manager_handle,SVC_Name,DELETE);
    if ( service_handle==NULL ) goto error;
    if ( !::DeleteService(service_handle) ) goto error;
    //
    ::CloseServiceHandle(service_handle);
    ::CloseServiceHandle(sc_manager_handle);

    ResMessageBox(NULL,0,9,MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
    return;

error:
    DWORD LastError=::GetLastError();
    if ( service_handle!=NULL ) ::CloseServiceHandle(service_handle);
    if ( sc_manager_handle!=NULL ) ::CloseServiceHandle(sc_manager_handle);
    ResMessageBox(NULL,1,7,MB_APPLMODAL|MB_OK|MB_ICONERROR,LastError);
    return;
}
//---------------------------------------------------------------------------
void ServiceStart()
{
    SERVICE_TABLE_ENTRY service_table_entry[]={SVC_Name,&ServiceMain,NULL,NULL};
    ::StartServiceCtrlDispatcher(service_table_entry);
}
//---------------------------------------------------------------------------

