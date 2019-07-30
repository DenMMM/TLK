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
    unsigned ShowTime;

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    void ThreadProc();
public:
    void SetFile(char *File_);
    bool Show(unsigned ShowTime_);
    void Stop();

    MMessage();
    ~MMessage();
};

//---------------------------------------------------------------------------
#endif
