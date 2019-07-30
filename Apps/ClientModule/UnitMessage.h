//---------------------------------------------------------------------------
#ifndef UnitMessageH
#define UnitMessageH
//---------------------------------------------------------------------------
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
    char *File;

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    void ThreadProc();
public:
    void SetFile(char *File_);
    bool Show();
    void Stop();

    MMessage();
    ~MMessage();
};

//---------------------------------------------------------------------------
#endif
