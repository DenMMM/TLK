//---------------------------------------------------------------------------
#include <stdio.h>
#pragma hdrstop

#include "UnitLockDsk.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
DWORD WINAPI MLockDsk::ThreadF(LPVOID Data_)
{
    DWORD ExitCode;

    // Создаем очередь сообщений для потока
    ::PeekMessage(nullptr,(HWND)-1,0,0,PM_NOREMOVE);
    // Обработка тела
    ExitCode=reinterpret_cast<MLockDsk*>(Data_)->ThreadP();
    // Завершаем работу
    ::ExitThread(ExitCode);

    return 0;
}
//---------------------------------------------------------------------------
DWORD MLockDsk::ThreadP()
{
    DWORD LastError;

    // Запомним текущие параметры экрана
    dmPrev.dmSize=sizeof(dmPrev);
    dmPrev.dmDriverExtra=0;
    ::EnumDisplaySettings(nullptr,ENUM_CURRENT_SETTINGS,&dmPrev);
    // Запомним текущий рабочий стол и создадим новый
    hPrevDsk=::GetThreadDesktop(::GetCurrentThreadId());
    if ( hPrevDsk==nullptr ) goto api_error;
    hMainDsk=::CreateDesktop(LOCKDSK_Name,nullptr,nullptr,0,GENERIC_ALL,nullptr);
    if ( hMainDsk==nullptr ) goto api_error;
#ifndef _DEBUG
    // Выберем его для потока и переключимся
    if ( !::SetThreadDesktop(hMainDsk) ) goto api_error;
    ::SwitchDesktop(hMainDsk);
//    if ( !::SwitchDesktop(hMainDsk) ) goto api_error;
    // Зададим режим экрана по-умолчанию (как при входе пользователя)
    ::ChangeDisplaySettings(nullptr,0);
#endif


{
    wchar_t szClassName[]=L"TLK_LockWnd";
    WNDCLASSEXW wincl;

    hInst=::GetModuleHandle(nullptr);

    hbTransp=::CreateSolidBrush(TCOLOR_TRANSPARENT);
//    if ( hbTransp==nullptr ) goto error;
	hfCompNum=::CreateFont(
		-32,0,0,0,FW_NORMAL,
		FALSE,FALSE,FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FIXED_PITCH,
		L"BankGothic Md BT");
//    if ( hfCompNum==nullptr ) goto error;

    hfSysTime=::CreateFont(
        -13,0,0,0,FW_NORMAL,
        FALSE,FALSE,FALSE,
        RUSSIAN_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FIXED_PITCH,
        L"Lucida Console");

    hfFineTime=::CreateFont(
        -24,0,0,0,FW_BOLD,
        FALSE,FALSE,FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FIXED_PITCH,
        L"BankGothic Md BT");

    // Зарегистрируем класс основного окна
    wincl.hInstance = hInst;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = &WndProc;
    wincl.style = CS_HREDRAW|CS_VREDRAW;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = ::LoadIcon (nullptr, IDI_APPLICATION);
    wincl.hIconSm = ::LoadIcon (nullptr, IDI_APPLICATION);
    wincl.hCursor = ::LoadCursor (nullptr, IDC_ARROW);
    wincl.lpszMenuName = nullptr;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground=hbTransp;
    ::RegisterClassEx(&wincl);
//    if ( ::RegisterClassEx(&wincl)==0 ) goto error;

    // Узнаем размеры экрана
    HDC hDCScreen = ::GetDC(nullptr);
    int ScrWidth = ::GetDeviceCaps(hDCScreen, HORZRES);
    int ScrHeight = ::GetDeviceCaps(hDCScreen, VERTRES);
    ::ReleaseDC(nullptr, hDCScreen);

    // Создадим основное окно
	hwMain=::CreateWindowEx(
		0x02000000,  // WS_EX_COMPOSITED
		szClassName, nullptr,
		WS_POPUP,
		0, 0, ScrWidth,	ScrHeight,
		nullptr, nullptr, wincl.hInstance, nullptr);

    // Создадим элемент с для картинки-сообщения
	hwMsgImg=::CreateWindowEx(
			0, L"STATIC", nullptr,
			SS_BITMAP|SS_CENTERIMAGE|WS_CHILD|WS_VISIBLE,
			0, 0, ScrWidth, ScrHeight,
			hwMain, (HMENU)IDC_MSGIMG, hInst, nullptr);

    // Создадим элемент для картинки-подложки меню
	hwMenuImg=::CreateWindowEx(
			0, L"STATIC", L"LOCKDESKMENU",
			SS_BITMAP|WS_CHILD|WS_VISIBLE,
			0, 0, 0, 0,
			hwMain,(HMENU)IDC_MENUIMG,hInst,nullptr);

	// Создадим элемент-номер компьютера и его тень
	hwCompNumSh=CreateText(L"99", 0, 0, 0, 0, hwMain, IDC_COMPNUMSH);
	hwCompNum=CreateText(L"99",0,0,0,0,hwMain,IDC_COMPNUM);
    ::SendMessage(hwCompNumSh,WM_SETFONT,(WPARAM)hfCompNum,0);
    ::SendMessage(hwCompNum,WM_SETFONT,(WPARAM)hfCompNum,0);

    // Создадим элемент "Текущее время 00:00"
    hwSysTimeTxt=CreateText(L"Текущее время",0,0,0,0,hwMain,IDC_SYSTIMETXT);
    ::SendMessage(hwSysTimeTxt,WM_SETFONT,(WPARAM)hfSysTime,0);
    hwSysTime=CreateText(L"00:00",0,0,0,0,hwMain,IDC_SYSTIME);
    ::SendMessage(hwSysTime,WM_SETFONT,(WPARAM)hfSysTime,0);

    // Создадим элемент "Осталось 00:00"
	hwWorkTimeTxt=CreateText(L"Осталось",0,0,0,0,hwMain,IDC_WORKTIMETXT);
	::SendMessage(hwWorkTimeTxt,WM_SETFONT,(WPARAM)hfSysTime,0);
    hwWorkTime=CreateText(L"00:00",0,0,0,0,hwMain,IDC_WORKTIME);
    ::SendMessage(hwWorkTime,WM_SETFONT,(WPARAM)hfSysTime,0);

/*
	hwFineTimeSh=CreateText(L"00:00",0,0,0,0,hwMain,IDC_FINETIMESH);
    hwFineTime=CreateText(L"00:00",0,0,0,0,hwMain,IDC_FINETIME);
    ::SendMessage(hwFineTimeSh,WM_SETFONT,(WPARAM)hfFineTime,0);
    ::SendMessage(hwFineTime,WM_SETFONT,(WPARAM)hfFineTime,0);
*/

    // Зададим первоначальные настройки и заполним данные
    UpdTransp();
    LoadImg();
    UpdCompNum();
    UpdSysTime();
    UpdWorkTime();

    // Расставим элементы окна
    int Left=ScrWidth-320;
    ::MoveWindow(hwMsgImg,0,83,ScrWidth,ScrHeight-83,FALSE);
    ::MoveWindow(hwMenuImg,Left+0,0,320,83,FALSE);
    ::MoveWindow(hwCompNumSh,Left+250,15,52,34,FALSE);
    ::MoveWindow(hwCompNum,Left+249,13,52,34,FALSE);
    ::MoveWindow(hwSysTimeTxt,Left+62,19,104,13,FALSE);
    ::MoveWindow(hwSysTime,Left+184,19,40,13,FALSE);
    ::MoveWindow(hwWorkTimeTxt,Left+102,31,64,13,FALSE);
    ::MoveWindow(hwWorkTime,Left+184,31,40,13,FALSE);

    // Покажем его
    ::ShowWindow(hwMain,SW_SHOW);
    ::UpdateWindow(hwMain);
}

    uTimer=::SetTimer(nullptr,0,1000,nullptr);

    MSG Msg;
    while(::GetMessage(&Msg,nullptr,0,0))
    {
        // Обработаем сообщения GUI
        if ( Msg.hwnd!=nullptr )
        {
            ::DispatchMessage(&Msg);
            continue;
        }

        // Обработаем сообщения потоку
        if ( Msg.message==MSG_Exit ) break;
        else if ( Msg.message==MSG_UnLock )
        {
#ifndef _DEBUG
            // Вернем прежние настройки экрана
            ::ChangeDisplaySettings(&dmPrev,CDS_RESET);
            // Переключимся на прежний рабочий стол
            ::SwitchDesktop(hPrevDsk);
#endif
            // Завершим работу
            break;
        }

        switch(Msg.message)
        {
#ifndef _DEBUG
            case WM_TIMER: ::SwitchDesktop(hMainDsk); break;
#endif
            case MSG_UpdTransp: UpdTransp(); break;
            case MSG_UpdMsg: LoadImg(); break;
            case MSG_UpdCompNum: UpdCompNum(); break;
            case MSG_UpdSysTime: UpdSysTime(); break;
            case MSG_UpdWorkTime: UpdWorkTime(); break;
            default: break;
        }
    }

    ::KillTimer(nullptr,uTimer);

    // Уничтожим основное окно и все что в нем создано
    ::DestroyWindow(hwMain);
#ifndef _DEBUG
    // Уничтожим свой рабочий стол
    ::SetThreadDesktop(hPrevDsk);
#endif
    ::CloseDesktop(hMainDsk);

    return 0;
api_error:
    LastError=::GetLastError();
    if ( hMainDsk!=nullptr ) { ::CloseDesktop(hMainDsk); hMainDsk=nullptr; }
#ifdef _DEBUG
    ResMessageBox(nullptr,1,1,MB_APPLMODAL|MB_OK|MB_ICONERROR,LastError);
#endif
    return LastError;
}
//---------------------------------------------------------------------------
void MLockDsk::UpdBkg(HWND hBkg_, HWND hText_)
{
    RECT rect;
    POINT point={0,0};

    // Определим область экрана где STATIC
    ::GetWindowRect(hText_,&rect);
    // Определим координаты меню на экране
    ::ClientToScreen(hBkg_,&point);
    // Вычислим относительные коодринаты STATIC'а
    rect.left-=point.x;
    rect.top-=point.y;
    rect.right-=point.x;
    rect.bottom-=point.y;
    // Обновим картинку под STATIC'ом
//    ::RedrawWindow(hBkg_,&rect,nullptr,
//        RDW_INVALIDATE|RDW_NOCHILDREN|RDW_UPDATENOW);   /// что лучше ?
	::InvalidateRect(hBkg_,&rect,FALSE);
	::UpdateWindow(hBkg_);
}
//---------------------------------------------------------------------------
void MLockDsk::UpdTransp() const
{
	std::lock_guard <std::mutex> lckObj(mtxParam);

	// Выставим режим прозрачности окна и параметры цвета
	if ( Transp )
	{
		::SetWindowLong(hwMain,GWL_EXSTYLE,WS_EX_LAYERED);
		::SetLayeredWindowAttributes(hwMain,TCOLOR_TRANSPARENT,255,LWA_COLORKEY);
	} else
		::SetWindowLong(hwMain,GWL_EXSTYLE,0);
	// Обновим содержимое
	::InvalidateRect(hwMain,nullptr,TRUE);
	::UpdateWindow(hwMain);
}
//---------------------------------------------------------------------------
bool MLockDsk::LoadImg()
{
	{
		std::lock_guard <std::mutex> lckObj(mtxParam);
		hMsgImg=::LoadImage(nullptr, MsgFile.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	if ( hMsgImg==nullptr ) return false;
	::SendMessage(hwMsgImg,STM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hMsgImg);
	return true;
}
//---------------------------------------------------------------------------
void MLockDsk::UpdCompNum() const
{
	std::lock_guard <std::mutex> lckObj(mtxParam);

	// Номер компьютера
	wchar_t line[]=L"99";
	swprintf(
		line, sizeof(line)/sizeof(line[0]),
		L"%2i",
		CompNum);

	::SetWindowText(hwCompNumSh, line);
	::SetWindowText(hwCompNum, line);
}
//---------------------------------------------------------------------------
void MLockDsk::UpdSysTime() const
{
	std::lock_guard <std::mutex> lckObj(mtxParam);

	// Текущее время
	SYSTEMTIME st;
	if ( Int64ToSystemTime(SysTime,st) )
	{
		wchar_t line[]=L"--:--";
		swprintf(
			line, sizeof(line)/sizeof(line[0]),
			L"%.2i:%.2i",
			st.wHour, st.wMinute);

		::SetWindowText(hwSysTime, line);
	}
}
//---------------------------------------------------------------------------
void MLockDsk::UpdWorkTime() const
{
	std::lock_guard <std::mutex> lckObj(mtxParam);

	// Сколько осталось работать
	wchar_t line[]=L"--:--";
	if ( WorkTime==0 ) swprintf(
		line,  sizeof(line)/sizeof(line[0]),
		L"--:--");
	else swprintf(
		line, sizeof(line)/sizeof(line[0]),
		L"%.2i:%.2i",
		WorkTime/60, WorkTime%60);

	::SetWindowText(hwWorkTime, line);
}
//---------------------------------------------------------------------------
bool MLockDsk::ThreadMsg(UINT Msg_) const
{
    DWORD ExitCode;

    // Убедимся что поток создан и работает перед отправкой ему сообщения
    if ( (hThread!=nullptr)&&
        ::GetExitCodeThread(hThread,&ExitCode)&&
        (ExitCode==STILL_ACTIVE) ) return ::PostThreadMessage(ThreadID,Msg_,0,0);

    return false;
}
//---------------------------------------------------------------------------
bool MLockDsk::SetTransp(bool Transp_)
{
    return true;            /// с прозрачностью баг - отключена
/*
    CS_Param.Enter();
    Transp=Transp_;
    CS_Param.Leafe();

    return ThreadMsg(MSG_UpdTransp);
*/
}
//---------------------------------------------------------------------------
bool MLockDsk::Show(const wchar_t *File_)
{
	// Скопируем параметры в буфер потока
	{
		std::lock_guard <std::mutex> lckObj(mtxParam);
		if ( wcslen(File_)>MAX_PATH ) MsgFile.clear();
		else MsgFile=File_;     /// проверить bad_alloc или выделить память заранее
	}

    // Отправим сообщение работающему потоку
    if ( hThread!=nullptr ) return ThreadMsg(MSG_UpdMsg);
    // Или создадим его и рабочий стол блокировки
    hThread=::CreateThread(nullptr,0,
        &MLockDsk::ThreadF,
        (LPVOID)this,0,&ThreadID);

    return hThread!=nullptr;
}
//---------------------------------------------------------------------------
bool MLockDsk::UpdateCompNum(int Num_)
{
	{
		std::lock_guard <std::mutex> lckObj(mtxParam);
		CompNum=Num_;
	}

    return ThreadMsg(MSG_UpdCompNum);
}
//---------------------------------------------------------------------------
bool MLockDsk::UpdateSysTime(__int64 Time_)
{
	{
		std::lock_guard <std::mutex> lckObj(mtxParam);
		SysTime=Time_;
	}

    return ThreadMsg(MSG_UpdSysTime);
}
//---------------------------------------------------------------------------
bool MLockDsk::UpdateWorkTime(int Time_)
{
	{
		std::lock_guard <std::mutex> lckObj(mtxParam);
		WorkTime=Time_;
	}

    return ThreadMsg(MSG_UpdWorkTime);
}
//---------------------------------------------------------------------------
void MLockDsk::Hide(bool UnLock_)
{
   DWORD ExitCode;

    // Проверяем работает ли поток
    if ( hThread==nullptr ) return;

    if ( ::GetExitCodeThread(hThread,&ExitCode)&&(ExitCode==STILL_ACTIVE) )
    {
        // Посылаем потоку сообщение и ждем завершения его работы
        ::PostThreadMessage(
            ThreadID,
            UnLock_? MSG_UnLock: MSG_Exit,
            0,0);
        ::WaitForSingleObject(hThread,INFINITE);
    }
    // Удаляем объект потока
    ::CloseHandle(hThread);

    hThread=nullptr;
    ThreadID=0;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK MLockDsk::WndProc(HWND hWnd_,
    UINT Msg_, WPARAM wParam_, LPARAM lParam_)
{
    HDC hDC;
    PAINTSTRUCT ps;

    switch(Msg_)
    {
        case WM_CTLCOLORSTATIC:
            switch(::GetDlgCtrlID((HWND)lParam_))
            {
                case IDC_COMPNUM:
                    ::SetTextColor((HDC)wParam_,TCOLOR_COMPNUM);
                    UpdBkg(
                        ::GetDlgItem(hWnd_,IDC_COMPNUMSH),
                        ::GetDlgItem(hWnd_,IDC_COMPNUM));
                    break;

                case IDC_COMPNUMSH:
                    ::SetTextColor((HDC)wParam_,TCOLOR_SHADOW);
                    UpdBkg(
                        ::GetDlgItem(hWnd_,IDC_MENUIMG),
                        ::GetDlgItem(hWnd_,IDC_COMPNUMSH));
                    break;

                case IDC_SYSTIMETXT:
                    ::SetTextColor((HDC)wParam_,TCOLOR_SYSTIME);
                    break;

                case IDC_SYSTIME:
                    ::SetTextColor((HDC)wParam_,TCOLOR_SYSTIME);
                    UpdBkg(
                        ::GetDlgItem(hWnd_,IDC_MENUIMG),
                        ::GetDlgItem(hWnd_,IDC_SYSTIME));
                    break;

                case IDC_WORKTIMETXT:
                    ::SetTextColor((HDC)wParam_,TCOLOR_SYSTIME);
                    break;

                case IDC_WORKTIME:
                    ::SetTextColor((HDC)wParam_,TCOLOR_SYSTIME);
                    UpdBkg(
                        ::GetDlgItem(hWnd_,IDC_MENUIMG),
                        ::GetDlgItem(hWnd_,IDC_WORKTIME));
                    break;

                case IDC_FINETIME:
                    ::SetTextColor((HDC)wParam_,TCOLOR_FINETIME);
                    break;

                case IDC_FINETIMESH:
                    ::SetTextColor((HDC)wParam_,TCOLOR_SHADOW);
                    break;

                default: break;
            }
            // Все static-элементы c "прозрачной" заливкой
            ::SetBkMode((HDC)wParam_,TRANSPARENT);
            return (LRESULT)::GetStockObject(NULL_BRUSH);

        case WM_CLOSE:
            return 0;

        case WM_DESTROY:
            ::PostQuitMessage(0);
            break;

        default:
            return ::DefWindowProc(hWnd_,Msg_,wParam_,lParam_);
    }

    return 0;
}
//---------------------------------------------------------------------------

/*
        case WM_DISPLAYCHANGE:
wParam 	Новое число битов на пиксель
LOWORD(lParam) 	Новая ширина пикселя
HIWORD(lParam) 	Новая высота пикселя
            break;
*/

