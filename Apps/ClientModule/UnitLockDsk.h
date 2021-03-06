//---------------------------------------------------------------------------
#ifndef UnitLockDskH
#define UnitLockDskH
//---------------------------------------------------------------------------
#include <string>
#include <mutex>
#include <winsock2.h>
//#include <windows.h>
#include "UnitStates.h"
//---------------------------------------------------------------------------
#define LOCKDSK_Name    L"TLK_Lock"
//---------------------------------------------------------------------------
// ��������� ��� ������
#define MSG_Exit            WM_USER+1   // ��������� ������
#define MSG_UnLock          WM_USER+2   // ������� ������� ������� ���� � ��������� ������
#define MSG_UpdTransp       WM_USER+3   // �������� ����� ������������ ����
#define MSG_UpdMsg          WM_USER+4   // �������� �������� � ����������
#define MSG_UpdCompNum      WM_USER+5   // �������� ����� ����������
#define MSG_UpdSysTime      WM_USER+6   // �������� ��������� �����
#define MSG_UpdWorkTime     WM_USER+7   // �������� ����� ������

#define TCOLOR_TRANSPARENT  RGB(0,0,0)          // ���� - ������ ������������
#define TCOLOR_SHADOW       RGB(2,2,2)          // ���� ���� ������
#define TCOLOR_SYSTIME      RGB(2,2,2)          // ���� ��������� ������
#define TCOLOR_COMPNUM      RGB(0,255,0)        // ���� ������ ����������
#define TCOLOR_FINETIME     RGB(200,50,50)      // ���� ������� ������

#define IDC_MSGIMG          1       // �������� � ����������
#define IDC_MENUIMG         2       // ��������-�������� ����
#define IDC_COMPNUM         3       // ����� - ����� ����������
#define IDC_COMPNUMSH       4       // ����� - ���� ������ ����������
#define IDC_SYSTIMETXT      5       // ����� "������� �����"
#define IDC_SYSTIME         6       // ����� - ��������� �����
#define IDC_WORKTIMETXT     7       // ����� "��������"
#define IDC_WORKTIME        8       // ����� - ���������� ����� ������
#define IDC_FINETIME        9       // ����� - ����� ������
#define IDC_FINETIMESH      10      // ����� - ���� ������� ������
//---------------------------------------------------------------------------
class MLockDsk
{
private:
	HANDLE hThread;             // ���������� ������
	DWORD ThreadID;             // ID ������
	DEVMODE dmPrev;             // ���������� ������� ���������� ������
	HDESK hPrevDsk;             // ���������� �������� �������� �����
	HDESK hMainDsk;             // ���������� �������� ����� ����������
	UINT uTimer;

	bool Transp;                        // ���� ������������ ����
	__int64 SysTime;                    // ������� ��������� �����
	int CompNum;                        // ����� ����������
	int WorkTime;                       // ���������� ����� ������
	std::wstring MsgFile;               // ���� � ����� � ���������-����������
	mutable std::mutex mtxParam;		// ���������� ������� � ���� ����������

	static DWORD WINAPI ThreadF(LPVOID Data_);
	DWORD ThreadP();

    HMODULE hInst;
    HBRUSH hbTransp;
    HFONT hfCompNum;
    HFONT hfSysTime;
    HFONT hfFineTime;

    HWND hwMain;
    HWND hwMenuImg;
    HWND hwCompNum;
    HWND hwCompNumSh;
    HWND hwSysTimeTxt;
    HWND hwSysTime;
    HHOOK hhSysTime;
    HWND hwWorkTimeTxt;
    HWND hwWorkTime;
//    HWND hwFineTime;
//    HWND hwFineTimeSh;
    HWND hwMsgImg;

//    HBITMAP hMsgImg;
	HANDLE hMsgImg;

    static LRESULT CALLBACK WndProc(HWND hWnd_,
        UINT Msg_, WPARAM wParam_, LPARAM lParam_);

    HWND CreateText(
        const wchar_t *Text_,
        DWORD Left_, DWORD Top_,
        DWORD Width_, DWORD Height_,
        HWND Parent_, DWORD Id_) const
    {
		return ::CreateWindowExW(
			0, L"STATIC", Text_,
			SS_SIMPLE|WS_CHILD|WS_VISIBLE,
			Left_, Top_, Width_, Height_,
			Parent_, (HMENU)Id_, hInst, NULL);
    }

    void UpdTransp() const;
    bool LoadImg();
    void UpdCompNum() const;
    void UpdSysTime() const;
    void UpdWorkTime() const;

    bool ThreadMsg(UINT Msg_) const;
    static void UpdBkg(HWND hBkg_, HWND hText_);

public:
    bool SetTransp(bool Transp_);
    bool Show(const wchar_t *File_);
    bool UpdateCompNum(int Num_);
    bool UpdateSysTime(__int64 Time_);
    bool UpdateWorkTime(int Time_);
    void Hide(bool UnLock_=true);

	MLockDsk():
		hThread(nullptr),
		ThreadID(0),
		hPrevDsk(nullptr),
		hMainDsk(nullptr),
		Transp(false),
		SysTime(0),
		CompNum(0),
		WorkTime(0)
	{
	}

	~MLockDsk()
	{
		Hide(false);    // �������� ������ ������ ��� ������������� ������
	}
};
//---------------------------------------------------------------------------
#endif
