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
// Сообщения для потока
#define MSG_Exit            WM_USER+1   // Завершить работу
#define MSG_UnLock          WM_USER+2   // Вернуть прежний рабочий стол и завершить работу
#define MSG_UpdTransp       WM_USER+3   // Обновить режим прозрачности окна
#define MSG_UpdMsg          WM_USER+4   // Обновить картинку с сообщением
#define MSG_UpdCompNum      WM_USER+5   // Обновить номер компьютера
#define MSG_UpdSysTime      WM_USER+6   // Обновить системное время
#define MSG_UpdWorkTime     WM_USER+7   // Обновить время работы

#define TCOLOR_TRANSPARENT  RGB(0,0,0)          // Цвет - маркер прозрачности
#define TCOLOR_SHADOW       RGB(2,2,2)          // Цвет тени текста
#define TCOLOR_SYSTIME      RGB(2,2,2)          // Цвет основного текста
#define TCOLOR_COMPNUM      RGB(0,255,0)        // Цвет номера компьютера
#define TCOLOR_FINETIME     RGB(200,50,50)      // Цвет времени штрафа

#define IDC_MSGIMG          1       // Картинка с сообщением
#define IDC_MENUIMG         2       // Картинка-подложка меню
#define IDC_COMPNUM         3       // Текст - номер компьютера
#define IDC_COMPNUMSH       4       // Текст - тень номера компьютера
#define IDC_SYSTIMETXT      5       // Текст "Текущее время"
#define IDC_SYSTIME         6       // Текст - системное время
#define IDC_WORKTIMETXT     7       // Текст "Осталось"
#define IDC_WORKTIME        8       // Текст - оставшееся время работы
#define IDC_FINETIME        9       // Текст - время штрафа
#define IDC_FINETIMESH      10      // Текст - тень времени штрафа
//---------------------------------------------------------------------------
class MLockDsk
{
private:
	HANDLE hThread;             // Дескриптор потока
	DWORD ThreadID;             // ID потока
	DEVMODE dmPrev;             // Дескриптор прежних параметров экрана
	HDESK hPrevDsk;             // Дескриптор прежнего рабочего стола
	HDESK hMainDsk;             // Дескриптор рабочего стола блокировки
	UINT uTimer;

	bool Transp;                        // Флаг прозрачности окна
	__int64 SysTime;                    // Текущее системное время
	int CompNum;                        // Номер компьютера
	int WorkTime;                       // Оставшееся время работы
	std::wstring MsgFile;               // Путь к файлу с картинкой-сообщением
	mutable std::mutex mtxParam;		// Блокировка доступа к этим параметрам

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
		Hide(false);    // Завершим работу потока без разблокировки экрана
	}
};
//---------------------------------------------------------------------------
#endif
