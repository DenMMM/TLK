//---------------------------------------------------------------------------
#include <windows.h>
#include <string.h>
#pragma hdrstop

#include "UnitService.h"
//---------------------------------------------------------------------------
#pragma argsused
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    char file_name[MAX_PATH];
    SC_HANDLE sc_manager_handle;
    SC_HANDLE service_handle;

    if ( strcmpi(lpCmdLine,"/install")==0 )
    {
        // Берем имя файла с путем к нему
        ::GetModuleFileName(NULL,file_name,MAX_PATH-strlen(" /run"));
        // Добавляем к названию файла службы ключ "/run"
        strcat(file_name," /run");
        // Открываем менеджер служб
        sc_manager_handle=::OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
        // Регистрируем службу
        service_handle=::CreateService(sc_manager_handle,"TLKRoute",
            "TLK Route Support",0,SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START,SERVICE_ERROR_NORMAL,file_name,
//            SERVICE_DEMAND_START,SERVICE_ERROR_NORMAL,file_name,
            NULL,NULL,NULL,NULL,NULL);
        //
        ::CloseServiceHandle(service_handle);
        ::CloseServiceHandle(sc_manager_handle);
    } else if ( strcmpi(lpCmdLine,"/uninstall")==0 )
    {
        // Открываем менеджер служб
        sc_manager_handle=::OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
        // Удаляем службу
        service_handle=::OpenService(sc_manager_handle,"TLKRoute",DELETE);
        ::DeleteService(service_handle);
        //
        ::CloseServiceHandle(service_handle);
        ::CloseServiceHandle(sc_manager_handle);
    } else if ( strcmpi(lpCmdLine,"/run")==0 )
    {
        SERVICE_TABLE_ENTRY service_table_entry[]={"TLKRoute",&ServiceMain,NULL,NULL};
        ::StartServiceCtrlDispatcher(service_table_entry);
    }/* else if ( strcmpi(lpCmdLine,"/debug")==0 )
    {
        ServiceMain(0,NULL);
    } */

    return 0;
}
//---------------------------------------------------------------------------

