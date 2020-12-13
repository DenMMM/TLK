#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif


#include <stdio.h>
#include <memory>

#include "UnitComputers.h"
#include "UnitStates.h"
#include "UnitSync.h"

#include <iostream>
#include "fasthash.h"

int _tmain(int argc, _TCHAR* argv[]) 
{
/*
	uint64_t counter=0;

	do
	{
		auto hash=fasthash64(&counter, sizeof(counter), 0);
		std::cout << counter << "\t" << hash << "\r\n";

	} while((counter++)<10);

	int a;
	std::cin >> a;

	return 0;
*/

/*
	int a;
	do
	{
		std::uint64_t t1=std::chrono::system_clock:://steady_clock::
			now().time_since_epoch().count();
//		t1=fasthash64(&t1,sizeof(t1),0);

		auto t2=[]() -> std::uint64_t {
			LARGE_INTEGER cntr;
			cntr.QuadPart=0;
			::QueryPerformanceCounter(&cntr);
//			cntr.QuadPart=fasthash64(&cntr,sizeof(cntr),0);
			return cntr.QuadPart;
		}();

		std::uint64_t t3=t1;
		BasicEncode(&t3,sizeof(t3),t2,32);

		std::cout << "t1: " << t1 << "\r\n";
		std::cout << "t2: " << t2 << "\r\n";
		std::cout << "t3: " << t3 << "\r\n";

		std::cin >> a;
	} while (a);

	return 0;
*/


	std::unique_ptr <MComputers> Computers;
	std::unique_ptr <MStates> States;
	std::unique_ptr <MSync> Sync;
	std::unique_ptr <MAuth> Auth;

	Computers.reset(new MComputers);
	States.reset(new MStates);
	Sync.reset(new MSync);
	Auth.reset(new MAuth);

	Computers->Add().Address=L"192.168.99.101";
	States->Update(*Computers);
	Auth->SetDefaultKey(
		HKEY_LOCAL_MACHINE,
		L"Software\\MMM Groups\\TLK\\3.0\\Admin",
		L"Auth",
		ENC_Code);
	Auth->Load();

	bool result=false;
	// Подготавливаем синхронизацию по сети с клиентами
	result=Sync->NetInit(ENC_Net, Auth.get());
	Sync->Associate(States.get(), Computers.get());
	// Запускаем синхронизацию
	result=Sync->Start();
	//
	std::int64_t SystemTime;
	GetLocalTimeInt64(SystemTime);
	States->Timer(SystemTime);

	// Имитация задержки между моментами инициализации клиента и сервера
	::Sleep(12345);

	std::unique_ptr <MStateCl> ClState;
	std::unique_ptr <MAuth> ClAuth;
	std::unique_ptr <MSyncCl> ClSync;

	ClState.reset(new MStateCl);
	ClAuth.reset(new MAuth);
	ClSync.reset(new MSyncCl);

	ClState->SetDefault(
		HKEY_LOCAL_MACHINE, L"Software\\MMM Groups\\TLK\\3.0\\Client", L"State",    //path//
		HKEY_LOCAL_MACHINE, L"Software\\MMM Groups\\TLK\\3.0\\Client", L"Options",  //path//
		L"D:\\FTP",ENC_Code);
	ClAuth->SetDefaultKey(
		HKEY_LOCAL_MACHINE, L"Software\\MMM Groups\\TLK\\3.0\\Client", L"Auth",ENC_Code);    //path//
	ClAuth->Load();

	ClSync->NetInit(ENC_Net,ClAuth.get());
	ClSync->Associate(ClState.get());
	ClSync->Start();



	int a;
	std::cin >> a;



	Sync->Stop();
	Sync->NetFree();
	ClSync->Stop();
	ClSync->NetFree();

	return 0;
}
