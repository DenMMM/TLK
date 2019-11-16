//---------------------------------------------------------------------------
#ifndef UnitSharedH
#define UnitSharedH
//---------------------------------------------------------------------------
#include <string>
//#include <winnt.h>
#include <winsock2.h>
//---------------------------------------------------------------------------
class MShared;
//---------------------------------------------------------------------------
#define SHRD_LockWait       500     // ����� �������� ������� � ����� ������ (����)
//---------------------------------------------------------------------------
// ��������� (��������)
#define mimNone             0       // ������ �������� � ����������
#define mimEndTime          1       // ����� �����������
#define mimLocked           2       // ��������� ������
#define mimPaused           3       // ��������� ������� ��������������� ��� ��������
#define mimFine             4       // �������� �����
#define mimTimePaused       5       // ����� ��������������
//---------------------------------------------------------------------------
class MShared
{
private:
    struct MSharedData
    {
        volatile __int64 SysTime;       // ��������� �����
        volatile unsigned SysTimeVer;
        volatile int CompNum;           // ����� ����������
        volatile unsigned CompNumVer;
        volatile int WorkTime;          // ���������� ����� ������
        volatile unsigned WorkTimeVer;
//        volatile int FineTime;          // ���������� ����� ������
//        volatile unsigned FineTimeVer;
        volatile bool WarnMsg;          // ���� - ���������� �������������� �� ��������� �������
        volatile unsigned WarnMsgVer;
        volatile int ImageMsg;          // ��������� ������ ����������
        volatile unsigned ImageMsgVer;
        volatile unsigned Games;        // ����������� ��� ������� ������ ��������
        volatile unsigned GamesVer;
        volatile bool ConfigMode;       // ���� - ����� ���������
        volatile unsigned ConfigModeVer;
        volatile bool Transp;           // ���� - ������������ TLK Shell
        volatile unsigned TranspVer;
    } *pData;                   // ��������� �� ������ � ����� ����� ������

    // ��������� ������� ������ shared-���� � ��������� ����
    void NextVer(volatile unsigned *Ver_)
    {
        if ( (++(*Ver_))==0 ) ++(*Ver_);
    }

    HANDLE hMap;                // Handle ������ ����� ������
    HANDLE hMtxMap;             // Handle �������� ��� ������������� ������� � ����
    HANDLE hMtxLive;            // Handle ��������-������� ������������� ������

#pragma pack(push, 1)
    // ������������ �������� ��������� ���������
    struct MInheritData
    {
        HANDLE hMap;
        HANDLE hMtxMap;
        HANDLE hMtxLive;
    };
#pragma pack(pop)
    
public:
    // ������� HEX-������ � ����������� ��� ��������� �������� (������)
	std::wstring InhToHEX() const;

    // ������� shared-������ (������)
    bool Create();
    // ������� shared-������ (�������� �������)
    bool Open(wchar_t *InhLine_);
    // ������� shared-������ � ���������� ��������� �������
    void Close();

    bool IsLive() const {
        return ::WaitForSingleObject(hMtxLive,0)!=WAIT_ABANDONED; }
    bool Lock() const {
        return ::WaitForSingleObject(hMtxMap,SHRD_LockWait)==WAIT_OBJECT_0; }
    bool UnLock() const {
        return ::ReleaseMutex(hMtxMap); }

    // ���������� shared-����� ����������� �������
    void UpdateSysTime(__int64 SysTime_);
    void UpdateCompNum(int Num_);
    void UpdateWorkTime(int Time_);
    void ShowWarnMsg(bool Show_);
    void ShowImageMessage(int Msg_);
    void ShowGames(unsigned Games_);
    void SetConfigMode(bool Mode_);
    void SetTransp(bool Transp_);

    // �������� ������ ���� � ��������� ��� �������� �������� ���������
	bool CheckSysTime(__int64 &SysTime_, unsigned &SysTimeVer_) const;
	bool CheckCompNum(int &Num_, unsigned &NumVer_) const;
	bool CheckWorkTime(int &Time_, unsigned &TimeVer_) const;
	bool CheckWarnMsg(bool &Msg_, unsigned &MsgVer_) const;
	bool CheckImageMessage(int &Msg_, unsigned &MsgVer_) const;
	bool CheckGames(unsigned &Games_, unsigned &GamesVer_) const;
	bool CheckConfigMode(bool &Mode_, unsigned &ModeVer_) const;
	bool CheckTransp(bool &Transp_, unsigned &TranspVer_) const;

    MShared()
    {
        pData=NULL;
        hMap=NULL;
        hMtxMap=NULL;
        hMtxLive=NULL;
    }
    ~MShared()
    {
        Close();
    }
};
//---------------------------------------------------------------------------
#endif
