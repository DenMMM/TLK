#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <stdio.h>

#include "UnitSend.h"
#include "UnitAuth.h"
#include "UnitGames.h"
#include "UnitClOptions.h"


int _tmain(int argc, _TCHAR* argv[]) 
{
	MSendSrv SendSrv;                      		// TCP-рассыльщик объектов
	MSendCl SendCl;
	MAuth AuthSrv;
	MAuth AuthCl;
	MStateCl StateCl;

	MGames TmpGames;                    		// Буфер для списка игр
	MGames TmpGamesCl;
	MClOptions TmpOptions;              		// Буфер для настроек клиента
	MClOptions TmpOptionsCl;


	AuthSrv.SetDefaultKey(
		HKEY_LOCAL_MACHINE,
		L"Software\\MMM Groups\\TLK\\3.0\\Admin",
		L"Auth",
		ENC_Code);
	AuthSrv.Load();

	StateCl.SetDefault(
		HKEY_LOCAL_MACHINE, L"Software\\MMM Groups\\TLK\\3.0\\Client", L"State",    //path//
		HKEY_LOCAL_MACHINE, L"Software\\MMM Groups\\TLK\\3.0\\Client", L"Options",  //path//
		L"",ENC_Code);
	AuthCl.SetDefaultKey(
		HKEY_LOCAL_MACHINE, L"Software\\MMM Groups\\TLK\\3.0\\Client", L"Auth",ENC_Code);    //path//
	AuthCl.Load();


	SendSrv.NetInit(nullptr, WM_USER+0, ENC_Net, &AuthSrv);
	SendCl.NetInit(&StateCl,ENC_Net,&AuthCl);



//	SendSrv.Send(&SendComps,&TmpGames,nullptr);
//	SendSrv.Send(&SendComps,nullptr,&TmpOptions);
//	SendSrv.Get(Computer,&TmpGames,nullptr);
//	SendSrv.Get(Computer,nullptr,&TmpOptions);





	SendSrv.NetFree();
	SendCl.NetFree();

	return 0;
}
