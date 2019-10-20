//---------------------------------------------------------------------------
#ifndef UnitMessageH
#define UnitMessageH
//---------------------------------------------------------------------------
#include <string>
#include <winsock2.h>
//#include <windows.h>
//---------------------------------------------------------------------------
class MMessage
{
private:
    HANDLE Thread;              // Дескриптор потока
    DWORD ThreadID;             // Идентификатор потока
    HANDLE hBitmap;
    BITMAP Bitmap;

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    void ThreadProc();
public:
    std::wstring File;

    bool Show();
    void Stop();

    MMessage();
    ~MMessage();
};

//---------------------------------------------------------------------------
#endif
