//---------------------------------------------------------------------------
#ifndef UnitLogRecordsH
#define UnitLogRecordsH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitStates.h"
#include "UnitTariffs.h"
#include "UnitFines.h"
#include "UnitUsers.h"
//---------------------------------------------------------------------------
// Main Log Record
#define mlrBegin        0x0101      // ��� �����
#define mlrEnd          0x0102      // ��� ������

#define mlrStart        0x0201      // ������ ���������� ������
#define mlrWork         0x0202      // ��������� ������ ��������
#define mlrStop         0x0203      // ��������� ���������� ������

#define mlrConfig       0x0301      // ��������� �������/�������
#define mlrComputers    0x0302      // ������� ������ �����������
#define mlrTariffs      0x0303      // ������� ������ �������
#define mlrFines        0x0304      // ������� ������ �������
#define mlrUsers        0x0305      // ������� ������ �������������
#define mlrOptions      0x0306      // �������� ����� ���������

#define mlrLogIn        0x0401      // ������������ ����� ������
#define mlrLogOut       0x0402      // ������������ �������� ������

#define mlrRun          0x0501      // ������ ����������
#define mlrFine         0x0502      // ����������� ����������
#define mlrExchange     0x0503      // ��������� � ������ ���������� �� ������
#define mlrLock         0x0504      // ���������/�������� ����������
#define mlrPause        0x0505      // ������������/������ �������
#define mlrOpen         0x0506      // ��������/�������� ���������� ��� ������������
#define mlrWtLocker     0x0507      // ����������/��������� TLK
#define mlrPowerOn      0x0508      // ��������� ������� ����������
#define mlrReboot       0x0509      // ������������ ����������
#define mlrShutdown     0x050A      // ���������� ������� ����������

#define mlrDataShellState   0x0601      // ������ ��������� ��������
#define mlrDataStates       0x0602      // ������ ��������� �����������
#define mlrDataTariffs      0x0603      // ������ �������
#define mlrDataFines        0x0604      // ������ �������
#define mlrDataUsers        0x0605      // ������ �������������
//---------------------------------------------------------------------------
class MLogRecord:public MSLListItem {};
//---------------------------------------------------------------------------
class MLogRecords:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_);
    void operator_delete(MListItem *DelItem_);
protected:
    bool Typed() { return true; }
public:
    MLogRecords() { constructor(); }
    ~MLogRecords() { destructor(); }
};
//---------------------------------------------------------------------------
class MLogRecordEvent:public MLogRecord
{
public:
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;     // ����� �������

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordEvent();
    ~MLogRecordEvent();
};
//---------------------------------------------------------------------------
class MLogRecordBegin:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrBegin; } };

class MLogRecordEnd:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrEnd; } };
//---------------------------------------------------------------------------
class MLogRecordStart:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrStart; } };

class MLogRecordWork:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrWork; } };

class MLogRecordStop:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrStop; } };
//---------------------------------------------------------------------------
class MLogRecordConfig:public MLogRecord
{
public:
    unsigned TypeID() { return mlrConfig; }
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;     // ����� �������
    bool Open;              // ��������� �������/�������

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordConfig();
    ~MLogRecordConfig();
};

class MLogRecordComputers:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrComputers; } };

class MLogRecordTariffs:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrTariffs; } };

class MLogRecordFines:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrFines; } };

class MLogRecordUsers:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrUsers; } };

class MLogRecordOptions:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrOptions; } };
//---------------------------------------------------------------------------
class MLogRecordLogIn:public MLogRecord
{
public:
    unsigned TypeID() { return mlrLogIn; }
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;     // ����� �������
    unsigned User;          // ID-����� ������������

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordLogIn();
    ~MLogRecordLogIn();
};

class MLogRecordLogOut:public MLogRecordEvent
{ public: unsigned TypeID() { return mlrLogOut; } };
//---------------------------------------------------------------------------
class MLogRecordRun:public MLogRecord
{
public:
    unsigned TypeID() { return mlrRun; }
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;                     // �����, ����� ���� ��������� �������
    int Number;                             // ����� ����������
    unsigned Tariff;                        // ID-����� ������
    __int64 StartTime;                      // �����, ������������ �������� ��������� ���������
    int Type;                               // =
    int BeginTime;                          // =    ���������� � ������
    int EndTime;                            // =
    int SizeTime;                           // =
    int WorkTime;                           // ������������ ����� ������
    double Cost;                            // ��������� ������ �� ���������� � ������� 'WorkTime'

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordRun();
    ~MLogRecordRun();
};
//---------------------------------------------------------------------------
class MLogRecordFine:public MLogRecord
{
public:
    unsigned TypeID() { return mlrFine; }
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;                 // �����, ����� ���� ��������� �������
    int Number;                         // ����� ����������
    unsigned Fine;                      // ID-����� ������
    int Time;                           // ����� ������

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordFine();
    ~MLogRecordFine();
};
//---------------------------------------------------------------------------
class MLogRecordExchange:public MLogRecord
{
public:
    unsigned TypeID() { return mlrExchange; }
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;                 // �����, ����� ���� ��������� �������
    int From;                           // � ������ ���������� ���� ����� �����
    int To;                             // �� ����� ��������� ����� ���� ����������

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordExchange();
    ~MLogRecordExchange();
};
//---------------------------------------------------------------------------
class MLogRecordMode:public MLogRecord
{
public:
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;     // �����, ����� ���� ��������� �������
    int Number;             // ����� ����������
    bool Apply;             // ����� ��� ����������/����

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordMode();
    ~MLogRecordMode();
};

class MLogRecordLock:public MLogRecordMode
{ public: unsigned TypeID() { return mlrLock; } };

class MLogRecordPause:public MLogRecordMode
{ public: unsigned TypeID() { return mlrPause; } };

class MLogRecordOpen:public MLogRecordMode
{ public: unsigned TypeID() { return mlrOpen; } };

class MLogRecordWtLocker:public MLogRecordMode
{ public: unsigned TypeID() { return mlrWtLocker; } };
//---------------------------------------------------------------------------
class MLogRecordCmd:public MLogRecord
{
public:
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;     // �����, ����� ���� ��������� �������
    int Number;             // ����� ����������

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordCmd();
    ~MLogRecordCmd();
};

class MLogRecordPowerOn:public MLogRecordCmd
{ public: unsigned TypeID() { return mlrPowerOn; } };

class MLogRecordReboot:public MLogRecordCmd
{ public: unsigned TypeID() { return mlrReboot; } };

class MLogRecordShutdown:public MLogRecordCmd
{ public: unsigned TypeID() { return mlrShutdown; } };
//---------------------------------------------------------------------------
class MLogRecordDataShellState:public MLogRecord
{
public:
    unsigned TypeID() { return mlrDataShellState; }
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;     // �����, ����� ���� ��������� �������
    unsigned State;         // ��������� ��������
    unsigned User;          // ������� ������������ (��� ����� �������)

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordDataShellState();
    ~MLogRecordDataShellState();
};
//---------------------------------------------------------------------------
class MLogRecordDataStates:public MLogRecord
{
public:
    unsigned TypeID() { return mlrDataStates; }
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;     // �����, ����� ���� ��������� �������
    unsigned NumStates;     // ���������� ������� � �������
    MStateData *States;     // ������ ��������� �����������

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordDataStates();
    ~MLogRecordDataStates();
};
//---------------------------------------------------------------------------
class MLogRecordDataTariffs:public MLogRecord
{
public:
    unsigned TypeID() { return mlrDataTariffs; }
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;     // �����, ����� ���� ��������� �������
    unsigned NumTariffs;    // ���������� ������� � �������
    MTariffData *Tariffs;   // ������ �������� �������

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordDataTariffs();
    ~MLogRecordDataTariffs();
};
//---------------------------------------------------------------------------
class MLogRecordDataFines:public MLogRecord
{
public:
    unsigned TypeID() { return mlrDataFines; }
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;     // �����, ����� ���� ��������� �������
    unsigned NumFines;      // ���������� ������� � �������
    MFineData *Fines;       // ������ �������� �������

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordDataFines();
    ~MLogRecordDataFines();
};
//---------------------------------------------------------------------------
class MLogRecordDataUsers:public MLogRecord
{
public:
    unsigned TypeID() { return mlrDataUsers; }
    bool Copy(MListItem *SrcItem_);
public:
    __int64 SystemTime;     // �����, ����� ���� ��������� �������
    unsigned NumUsers;      // ���������� ������� � �������
    MUserData *Users;       // ������ ������ � �������������

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordDataUsers();
    ~MLogRecordDataUsers();
};
//---------------------------------------------------------------------------
#endif

