//---------------------------------------------------------------------------
#ifndef UnitMessageH
#define UnitMessageH
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
class MMessage
{
private:
    HANDLE Thread;              // ���������� ������
    DWORD ThreadID;             // ������������� ������
    HANDLE hBitmap;
    BITMAP Bitmap;
    char *File;
    unsigned ShowTime;

    static DWORD WINAPI ThreadFunc(LPVOID Data);
    void ThreadProc();
public:
    bool SetFile(char *File_);
    bool Show(unsigned ShowTime_);
    void Stop();

    MMessage();
    ~MMessage();
};

//---------------------------------------------------------------------------
#endif
