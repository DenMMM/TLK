//---------------------------------------------------------------------------
#include <stdio.h>
#pragma hdrstop

#include "UnitLockDsk.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MLockDsk::MLockDsk()
{
    hThread=NULL;
    ThreadID=0;
    hPrevDsk=NULL;
    hMainDsk=NULL;
    Transp=false;
    SysTime=0;
    CompNum=0;
    WorkTime=0;
    *MsgFile=0;
    ::InitializeCriticalSection(&CS_Param);
}
//---------------------------------------------------------------------------
MLockDsk::~MLockDsk()
{
    Hide();         /// правильно ли вызывать в деструкторе ?
    ::DeleteCriticalSection(&CS_Param);
}
//---------------------------------------------------------------------------
DWORD WINAPI MLockDsk::ThreadF(LPVOID Data_)
{
    DWORD ExitCode;

    // Создаем очередь сообщений для потока
    ::PeekMessage(NULL,(HWND)-1,0,0,PM_NOREMOVE);
    // Обработка тела
    ExitCode=((MLockDsk*)Data_)->ThreadP();
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
    ::EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmPrev);
    // Запомним текущий рабочий стол и создадим новый
    hPrevDsk=::GetThreadDesktop(::GetCurrentThreadId());
    if ( hPrevDsk==NULL ) goto api_error;
    hMainDsk=::CreateDesktop(LOCKDSK_Name,NULL,NULL,0,GENERIC_ALL,NULL);
    if ( hMainDsk==NULL ) goto api_error;
    // Выберем его для потока и переключимся
#ifndef _DEBUG
    if ( !::SetThreadDesktop(hMainDsk) ) goto api_error;
//    if ( !::SwitchDesktop(hMainDsk) ) goto api_error;
    ::SwitchDesktop(hMainDsk);
#endif
    // Зададим режим экрана по-умолчанию (как при входе пользователя)
    ::ChangeDisplaySettings(NULL,0);


{
    char szClassName[]="TLK_LockWnd";
    WNDCLASSEX wincl;

    hInst=::GetModuleHandle(NULL);

    hbTransp=::CreateSolidBrush(TCOLOR_TRANSPARENT);
//    if ( hbTransp==NULL ) goto error;
    hfCompNum=::CreateFont(
        -32,0,0,0,FW_NORMAL,
        FALSE,FALSE,FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FIXED_PITCH,
        "BankGothic Md BT");
//    if ( hfCompNum==NULL ) goto error;

    hfSysTime=::CreateFont(
        -13,0,0,0,FW_NORMAL,
        FALSE,FALSE,FALSE,
        RUSSIAN_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FIXED_PITCH,
        "Lucida Console");

    hfFineTime=::CreateFont(
        -24,0,0,0,FW_BOLD,
        FALSE,FALSE,FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FIXED_PITCH,
        "BankGothic Md BT");

    // Зарегистрируем класс основного окна
    wincl.hInstance = hInst;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = &WndProc;
    wincl.style = CS_HREDRAW|CS_VREDRAW;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = ::LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = ::LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = ::LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground=hbTransp;
    ::RegisterClassEx(&wincl);
//    if ( ::RegisterClassEx(&wincl)==0 ) goto error;

    // Узнаем размеры экрана
    HDC hDCScreen = ::GetDC(NULL);
    int ScrWidth = ::GetDeviceCaps(hDCScreen, HORZRES);
    int ScrHeight = ::GetDeviceCaps(hDCScreen, VERTRES);
    ::ReleaseDC(NULL, hDCScreen);

    // Создадим основное окно
    hwMain=::CreateWindowEx(
        0x02000000,  // WS_EX_COMPOSITED
        szClassName,NULL,
        WS_POPUP,
        0,0,
        ScrWidth,
        ScrHeight,
        NULL,NULL,wincl.hInstance,NULL);

    // Создадим элемент с для картинки-сообщения
    hwMsgImg=::CreateWindowEx(
            0,"STATIC",NULL,
            SS_BITMAP|SS_CENTERIMAGE|WS_CHILD|WS_VISIBLE,
            0,0,ScrWidth,ScrHeight,
            hwMain,(HMENU)IDC_MSGIMG,hInst,NULL);

    // Создадим элемент для картинки-подложки меню
    hwMenuImg=::CreateWindowEx(
            0,"STATIC","LOCKDESKMENU",
            SS_BITMAP|WS_CHILD|WS_VISIBLE,
            0,0,0,0,
            hwMain,(HMENU)IDC_MENUIMG,hInst,NULL);

    // Создадим элемент-номер компьютера и его тень
    hwCompNumSh=CreateText("99",0,0,0,0,hwMain,IDC_COMPNUMSH);
    hwCompNum=CreateText("99",0,0,0,0,hwMain,IDC_COMPNUM);
    ::SendMessage(hwCompNumSh,WM_SETFONT,(WPARAM)hfCompNum,0);
    ::SendMessage(hwCompNum,WM_SETFONT,(WPARAM)hfCompNum,0);

    // Создадим элемент "Текущее время 00:00"
    hwSysTimeTxt=CreateText(TEXT("Текущее время"),0,0,0,0,hwMain,IDC_SYSTIMETXT);
    ::SendMessage(hwSysTimeTxt,WM_SETFONT,(WPARAM)hfSysTime,0);
    hwSysTime=CreateText("00:00",0,0,0,0,hwMain,IDC_SYSTIME);
    ::SendMessage(hwSysTime,WM_SETFONT,(WPARAM)hfSysTime,0);

    // Создадим элемент "Осталось 00:00"
    hwWorkTimeTxt=CreateText(TEXT("Осталось"),0,0,0,0,hwMain,IDC_WORKTIMETXT);
    ::SendMessage(hwWorkTimeTxt,WM_SETFONT,(WPARAM)hfSysTime,0);
    hwWorkTime=CreateText("00:00",0,0,0,0,hwMain,IDC_WORKTIME);
    ::SendMessage(hwWorkTime,WM_SETFONT,(WPARAM)hfSysTime,0);

/*
    hwFineTimeSh=CreateText("00:00",0,0,0,0,hwMain,IDC_FINETIMESH);
    hwFineTime=CreateText("00:00",0,0,0,0,hwMain,IDC_FINETIME);
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

    uTimer=::SetTimer(NULL,0,1000,NULL);

    MSG Msg;
    while(::GetMessage(&Msg,NULL,0,0))
    {
        // Обработаем сообщения GUI
        if ( Msg.hwnd!=NULL )
        {
            ::DispatchMessage(&Msg);
            continue;
        }

        // Обработаем сообщения потоку
        if ( Msg.message==MSG_Exit ) break;
        switch(Msg.message)
        {
            case WM_TIMER: ::SwitchDesktop(hMainDsk); break;
            case MSG_UpdTransp: UpdTransp(); break;
            case MSG_UpdMsg: LoadImg(); break;
            case MSG_UpdCompNum: UpdCompNum(); break;
            case MSG_UpdSysTime: UpdSysTime(); break;
            case MSG_UpdWorkTime: UpdWorkTime(); break;
            default: break;
        }
    }

    ::KillTimer(NULL,uTimer);

    // Уничтожим основное окно и все что в нем создано
    ::DestroyWindow(hwMain);

    // Вернем прежние настройки экрана
    ::ChangeDisplaySettings(&dmPrev,CDS_RESET);
    // Переключимся на прежний рабочий стол и уничтожим свой
#ifndef _DEBUG
    ::SwitchDesktop(hPrevDsk);
    ::SetThreadDesktop(hPrevDsk);
#endif    
    ::CloseDesktop(hMainDsk);

    return 0;
api_error:
    LastError=::GetLastError();
    if ( hMainDsk!=NULL ) { ::CloseDesktop(hMainDsk); hMainDsk=NULL; }
//    ResMessageBox(NULL,1,1,MB_APPLMODAL|MB_OK|MB_ICONERROR,LastError);  /// для отладки
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
//    ::RedrawWindow(hBkg_,&rect,NULL,
//        RDW_INVALIDATE|RDW_NOCHILDREN|RDW_UPDATENOW);   /// что лучше ?
    ::InvalidateRect(hBkg_,&rect,FALSE);
    ::UpdateWindow(hBkg_);
}
//---------------------------------------------------------------------------
void MLockDsk::UpdTransp() const
{
    Lock();
    // Выставим режим прозрачности окна и параметры цвета
    if ( Transp )
    {
        ::SetWindowLong(hwMain,GWL_EXSTYLE,WS_EX_LAYERED);
        ::SetLayeredWindowAttributes(hwMain,TCOLOR_TRANSPARENT,255,LWA_COLORKEY);
    } else
        ::SetWindowLong(hwMain,GWL_EXSTYLE,0);
    // Обновим содержимое
    ::InvalidateRect(hwMain,NULL,TRUE);
    ::UpdateWindow(hwMain);
    UnLock();
}
//---------------------------------------------------------------------------
bool MLockDsk::LoadImg()
{
    Lock();
    hMsgImg=::LoadImage(NULL,MsgFile,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
    UnLock();
    if ( hMsgImg==NULL ) return false;
    ::SendMessage(hwMsgImg,STM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hMsgImg);
    return true;
}
//---------------------------------------------------------------------------
void MLockDsk::UpdCompNum() const
{
    char line[]="99";

    Lock();
    // Номер компьютера
    sprintf(line,"%2i",CompNum);
    ::SetWindowText(hwCompNumSh,line);
    ::SetWindowText(hwCompNum,line);
    UnLock();
}
//---------------------------------------------------------------------------
void MLockDsk::UpdSysTime() const
{
    char line[]="--:--";
    SYSTEMTIME st;

    Lock();
    // Текущее время
    if ( Int64ToSystemTime(&SysTime,&st) )
    {
        sprintf(line,"%.2i:%.2i",st.wHour,st.wMinute);
        ::SetWindowText(hwSysTime,line);
    }
    UnLock();
}
//---------------------------------------------------------------------------
void MLockDsk::UpdWorkTime() const
{
    char line[]="--:--";

    Lock();
    // Сколько осталось работать
    if ( WorkTime==0 ) sprintf(line,"--:--");
    else sprintf(line,"%.2i:%.2i",WorkTime/60,WorkTime%60);
    ::SetWindowText(hwWorkTime,line);
    UnLock();
}
//---------------------------------------------------------------------------
bool MLockDsk::ThreadMsg(UINT Msg_) const
{
    DWORD ExitCode;

    // Убедимся что поток создан и работает перед отправкой ему сообщения
    if ( (hThread!=NULL)&&
        ::GetExitCodeThread(hThread,&ExitCode)&&
        (ExitCode==STILL_ACTIVE) ) return ::PostThreadMessage(ThreadID,Msg_,0,0);

    return false;
}
//---------------------------------------------------------------------------
bool MLockDsk::SetTransp(bool Transp_)
{
    Lock();
    Transp=Transp_;
    UnLock();
    return ThreadMsg(MSG_UpdTransp);
}
//---------------------------------------------------------------------------
bool MLockDsk::Show(const char *File_)
{
    // Скопируем параметры в буфер потока
    Lock();
    if ( strlen(File_)>MAX_PATH ) *MsgFile=0;
    else strcpy(MsgFile,File_);
    UnLock();

    // Отправим сообщение работающему потоку
    if ( hThread!=NULL ) return ThreadMsg(MSG_UpdMsg);
    // Или создадим его и рабочий стол блокировки
    hThread=::CreateThread(NULL,0,
        &MLockDsk::ThreadF,
        (LPVOID)this,0,&ThreadID);

    return hThread!=NULL;
}
//---------------------------------------------------------------------------
bool MLockDsk::UpdateCompNum(int Num_)
{
    Lock();
    CompNum=Num_;
    UnLock();
    return ThreadMsg(MSG_UpdCompNum);
}
//---------------------------------------------------------------------------
bool MLockDsk::UpdateSysTime(__int64 Time_)
{
    Lock();
    SysTime=Time_;
    UnLock();
    return ThreadMsg(MSG_UpdSysTime);
}
//---------------------------------------------------------------------------
bool MLockDsk::UpdateWorkTime(int Time_)
{
    Lock();
    WorkTime=Time_;
    UnLock();
    return ThreadMsg(MSG_UpdWorkTime);
}
//---------------------------------------------------------------------------
void MLockDsk::Hide()
{
   DWORD ExitCode;

    // Проверяем работает ли поток
    if ( hThread==NULL ) return;

    if ( ::GetExitCodeThread(hThread,&ExitCode)&&(ExitCode==STILL_ACTIVE) )
    {
        // Посылаем потоку сообщение и ждем завершения его работы
        ::PostThreadMessage(ThreadID,MSG_Exit,0,0);
        ::WaitForSingleObject(hThread,INFINITE);
    }
    // Удаляем объект потока
    ::CloseHandle(hThread);

    hThread=NULL;
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

