//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitMessage.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
DWORD WINAPI MMessage::ThreadFunc(LPVOID Data)
{
    // ������� ������� ��������� ��� ������
	::PeekMessage(nullptr, (HWND)-1, 0, 0, PM_NOREMOVE);
    // ��������� ���� ������
    reinterpret_cast<MMessage*>(Data)->ThreadProc();
    // ��������� ������ ������
    ::ExitThread(0);
    return 0;
}
//---------------------------------------------------------------------------
void MMessage::ThreadProc()
{
    MSG Msg;
    HWND dsk_wnd;
    HDC dsk_dc, comp_dc;
    HANDLE old_bitmap;

    do
    {
        if ( ((dsk_wnd=::GetDesktopWindow())==nullptr)||
            ((dsk_dc=::GetWindowDC(dsk_wnd))==nullptr) ) goto next;
        if ( (comp_dc=::CreateCompatibleDC(dsk_dc))==nullptr )
            { ::ReleaseDC(dsk_wnd,dsk_dc); goto next; }
        old_bitmap=::SelectObject(comp_dc,hBitmap);
        ::BitBlt(dsk_dc,0,0,Bitmap.bmWidth,Bitmap.bmHeight,
            comp_dc,0,0,SRCINVERT);
//        old_bitmap=::SelectObject(comp_dc,old_bitmap);
        ::SelectObject(comp_dc,old_bitmap);
        ::DeleteDC(comp_dc);
        ::ReleaseDC(dsk_wnd,dsk_dc);
next:
        ::Sleep(20);
    } while(!::PeekMessage(&Msg,nullptr,0,0,PM_REMOVE));

    // ������� �� ������ ����������� ����� ��������
    ::DeleteObject(hBitmap); hBitmap=nullptr;
}
//---------------------------------------------------------------------------
bool MMessage::Show()
{
    DWORD ExitCode;

    if ( Thread )
    {
        // ��������� �� �������� �� ��� ��� �����
        if ( (!::GetExitCodeThread(Thread,&ExitCode))||
            (ExitCode==STILL_ACTIVE) ) return false;
        // ������������ ���������� ������� �����
        ::CloseHandle(Thread); Thread=nullptr; ThreadID=0;
    }

    // ��������� �������� � ����������
    if ( (hBitmap=::LoadImage(nullptr, File.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE))==nullptr ) goto error;
	if ( ::GetObject(hBitmap, sizeof(BITMAP), &Bitmap)==0 ) goto error;
    // ������� ����� ��� ������ ���������
    return (Thread=::CreateThread(nullptr,0,&ThreadFunc,
        (LPVOID)this,0,&ThreadID))!=nullptr;
error:
    ::DeleteObject(hBitmap); hBitmap=nullptr;
    return false;
}
//---------------------------------------------------------------------------
void MMessage::Stop()
{
//    DWORD ExitCode;
//    // ��������� �������� �� �����
//    if ( (Thread==nullptr)||
//        (!::GetExitCodeThread(Thread,&ExitCode))||
//        (ExitCode!=STILL_ACTIVE) ) return false;
    if ( Thread==nullptr ) return;
    // �������� ������ ��������� � ���� ���������� ��� ������
    ::PostThreadMessage(ThreadID,WM_USER,0,0);
    ::WaitForSingleObject(Thread,INFINITE);
    // ��������� ��������� ������
    ::CloseHandle(Thread); Thread=nullptr; ThreadID=0;
}
//---------------------------------------------------------------------------

