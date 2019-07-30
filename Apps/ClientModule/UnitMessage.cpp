//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitMessage.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MMessage::MMessage()
{
    Thread=NULL;
    ThreadID=0;
    hBitmap=NULL;
    BITMAP Bitmap;
    File=NULL;
}
//---------------------------------------------------------------------------
MMessage::~MMessage()
{
    delete[] File;
}
//---------------------------------------------------------------------------
DWORD WINAPI MMessage::ThreadFunc(LPVOID Data)
{
    // ������� ������� ��������� ��� ������
    ::PeekMessage(NULL,(HANDLE)-1,0,0,PM_NOREMOVE);
    // ��������� ���� ������
    ((MMessage*)Data)->ThreadProc();
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
        if ( ((dsk_wnd=::GetDesktopWindow())==NULL)||
            ((dsk_dc=::GetWindowDC(dsk_wnd))==NULL) ) goto next;
        if ( (comp_dc=::CreateCompatibleDC(dsk_dc))==NULL )
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
    } while(!::PeekMessage(&Msg,NULL,0,0,PM_REMOVE));

    // ������� �� ������ ����������� ����� ��������
    ::DeleteObject(hBitmap); hBitmap=NULL;
}
//---------------------------------------------------------------------------
void MMessage::SetFile(char *File_)
{
    delete[] File;
    File=new char[strlen(File_)+1];
    strcpy(File,File_);
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
        ::CloseHandle(Thread); Thread=NULL; ThreadID=0;
    }

    // ��������� �������� � ����������
    if ( (hBitmap=::LoadImage(NULL,File,IMAGE_BITMAP,0,0,LR_LOADFROMFILE))==NULL ) goto error;
    if ( ::GetObject(hBitmap,sizeof(BITMAP),&Bitmap)==NULL ) goto error;
    // ������� ����� ��� ������ ���������
    return (Thread=::CreateThread(NULL,0,&ThreadFunc,
        (LPVOID)this,0,&ThreadID))!=NULL;
error:
    ::DeleteObject(hBitmap); hBitmap=NULL;
    return false;
}
//---------------------------------------------------------------------------
void MMessage::Stop()
{
//    DWORD ExitCode;
//    // ��������� �������� �� �����
//    if ( (Thread==NULL)||
//        (!::GetExitCodeThread(Thread,&ExitCode))||
//        (ExitCode!=STILL_ACTIVE) ) return false;
    if ( Thread==NULL ) return;
    // �������� ������ ��������� � ���� ���������� ��� ������
    ::PostThreadMessage(ThreadID,WM_USER,0,0);
    ::WaitForSingleObject(Thread,INFINITE);
    // ��������� ��������� ������
    ::CloseHandle(Thread); Thread=NULL; ThreadID=0;
}
//---------------------------------------------------------------------------

