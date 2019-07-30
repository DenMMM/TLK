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
#define mlrBegin        0x11    // ��� �����
#define mlrEnd          0x12    // ��� ������
#define mlrDataShState  0x13    // ������ ��������� ��������
#define mlrDataStates   0x14    // ������ ��������� �����������
#define mlrDataTariffs  0x15    // ������ �������
#define mlrDataFines    0x16    // ������ �������
#define mlrDataUsers    0x17    // ������ �������������

#define mlrStart        0x21    // ������ ���������� ������
#define mlrStop         0x22    // ��������� ���������� ������
#define mlrLogIn        0x23    // ������������ ����� ������
#define mlrLogOut       0x24    // ������������ �������� ������
#define mlrConfig       0x25    // ��������� �������/�������
#define mlrComputers    0x26    // ������� ������ �����������
#define mlrTariffs      0x27    // ������� ������ �������
#define mlrFines        0x28    // ������� ������ �������
#define mlrUsers        0x29    // ������� ������ �������������
#define mlrOptions      0x2A    // �������� ����� ���������

#define mlrRun          0x31    // ������ ����������
#define mlrFine         0x32    // ����������� ����������
#define mlrExchange     0x33    // ��������� � ������ ���������� �� ������
#define mlrLock         0x34    // ���������/�������� ����������
#define mlrPause        0x35    // ������������/������ �������
#define mlrOpen         0x36    // ��������/�������� ���������� ��� ������������
#define mlrPowerOn      0x37    // ��������� ������� ���������� (WOL)
#define mlrReboot       0x38    // ������������ ����������
#define mlrShutdown     0x39    // ���������� ������� ����������

//---------------------------------------------------------------------------
class MLogRecord:public MSLListItem {};
//---------------------------------------------------------------------------
class MLogRecords:public MSLList
{
private:
    MListItem *item_new(unsigned char TypeID_) const;
    void item_del(MListItem *Item_) const;

protected:
    bool Typed() const { return true; }

public:
    MLogRecords() {}
    ~MLogRecords() { Clear(); }
};
//---------------------------------------------------------------------------
class MLogRecordEvent:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // ����� �������

    void Copy(const MListItem *SrcItem_);

    MLogRecordEvent();
    ~MLogRecordEvent();
};
//---------------------------------------------------------------------------
class MLogRecordBegin:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrBegin; } };

class MLogRecordEnd:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrEnd; } };
//---------------------------------------------------------------------------
class MLogRecordStart:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrStart; } };

class MLogRecordStop:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrStop; } };
//---------------------------------------------------------------------------
class MLogRecordConfig:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // ����� �������
    bool Open;              // ��������� �������/�������

    unsigned char gTypeID() const { return mlrConfig; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordConfig();
    ~MLogRecordConfig();
};

class MLogRecordComputers:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrComputers; } };

class MLogRecordTariffs:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrTariffs; } };

class MLogRecordFines:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrFines; } };

class MLogRecordUsers:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrUsers; } };

class MLogRecordOptions:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrOptions; } };
//---------------------------------------------------------------------------
class MLogRecordLogIn:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // ����� �������
    unsigned User;          // ID-����� ������������

    unsigned char gTypeID() const { return mlrLogIn; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordLogIn();
    ~MLogRecordLogIn();
};

class MLogRecordLogOut:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrLogOut; } };
//---------------------------------------------------------------------------
class MLogRecordRun:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;                     // �����, ����� ���� ��������� �������
    char Number;                            // ����� ����������
    unsigned Tariff;                        // ID-����� ������
    __int64 StartTime;                      // �����, ������������ �������� ��������� ���������
    char Type;                              // =
    short BeginTime;                        // =    ���������� � ������
    short EndTime;                          // =
    short SizeTime;                         // =
    short WorkTime;                         // ������������ ����� ������
    double Cost;                            // ��������� ������ �� ���������� � ������� 'WorkTime'

    unsigned char gTypeID() const { return mlrRun; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordRun();
    ~MLogRecordRun();
};
//---------------------------------------------------------------------------
class MLogRecordFine:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;                 // �����, ����� ���� ��������� �������
    char Number;                        // ����� ����������
    unsigned Fine;                      // ID-����� ������
    short Time;                         // ����� ������

    unsigned char gTypeID() const { return mlrFine; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordFine();
    ~MLogRecordFine();
};
//---------------------------------------------------------------------------
class MLogRecordExchange:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;                 // �����, ����� ���� ��������� �������
    char From;                          // � ������ ���������� ���� ����� �����
    char To;                            // �� ����� ��������� ����� ���� ����������

    unsigned char gTypeID() const { return mlrExchange; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordExchange();
    ~MLogRecordExchange();
};
//---------------------------------------------------------------------------
class MLogRecordMode:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // �����, ����� ���� ��������� �������
    char Number;            // ����� ����������
    bool Apply;             // ����� ��� ����������/����

    void Copy(const MListItem *SrcItem_);

    MLogRecordMode();
    ~MLogRecordMode();
};

class MLogRecordLock:public MLogRecordMode
{ public: unsigned char gTypeID() const { return mlrLock; } };

class MLogRecordPause:public MLogRecordMode
{ public: unsigned char gTypeID() const { return mlrPause; } };

class MLogRecordOpen:public MLogRecordMode
{ public: unsigned char gTypeID() const { return mlrOpen; } };
//---------------------------------------------------------------------------
class MLogRecordCmd:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // �����, ����� ���� ��������� �������
    char Number;            // ����� ����������

    void Copy(const MListItem *SrcItem_);

    MLogRecordCmd();
    ~MLogRecordCmd();
};

class MLogRecordPowerOn:public MLogRecordCmd
{ public: unsigned char gTypeID() const { return mlrPowerOn; } };

class MLogRecordReboot:public MLogRecordCmd
{ public: unsigned char gTypeID() const { return mlrReboot; } };

class MLogRecordShutdown:public MLogRecordCmd
{ public: unsigned char gTypeID() const { return mlrShutdown; } };
//---------------------------------------------------------------------------
class MLogRecordDataShState:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // �����, ����� ���� ��������� �������
    unsigned State;         // ��������� ��������
    unsigned User;          // ������� ������������ (��� ����� �������)

    unsigned char gTypeID() const { return mlrDataShState; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordDataShState();
    ~MLogRecordDataShState();
};
//---------------------------------------------------------------------------
class MLogRecordDataStates:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;             // �����, ����� ���� ��������� �������
    Marray <MStateData> States;     // ������ ��������� �����������

    unsigned char gTypeID() const { return mlrDataStates; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordDataStates();
    ~MLogRecordDataStates();
};
//---------------------------------------------------------------------------
class MLogRecordDataTariffs:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;             // �����, ����� ���� ��������� �������
    Marray <MTariffData> Tariffs;   // ������ �������� �������

    unsigned char gTypeID() const { return mlrDataTariffs; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordDataTariffs();
    ~MLogRecordDataTariffs();
};
//---------------------------------------------------------------------------
class MLogRecordDataFines:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;                 // �����, ����� ���� ��������� �������
    Marray <MFineData> Fines;           // ������ �������� �������

    unsigned char gTypeID() const { return mlrDataFines; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordDataFines();
    ~MLogRecordDataFines();
};
//---------------------------------------------------------------------------
class MLogRecordDataUsers:public MLogRecord
{
private:
    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;         // �����, ����� ���� ��������� �������
    Marray <MUserData> Users;   // ������ ������ � �������������

    unsigned char gTypeID() const  { return mlrDataUsers; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordDataUsers();
    ~MLogRecordDataUsers();
};
//---------------------------------------------------------------------------
#endif




/*
    static MItemFunc item_func[2];

    template <class MItemType>
    static MListItem *item_new(unsigned Type_)
    {
        return (MListItem*)new MItemType;
    }

    template static MListItem *item_new<MLogRecordBegin>();
    template static MListItem *item_new<MLogRecordEnd>();
    template static MListItem *item_new<MLogRecordStart>();
    template static MListItem *item_new<MLogRecordWork>();
    template static MListItem *item_new<MLogRecordStop>();
    template static MListItem *item_new<MLogRecordConfig>();
    template static MListItem *item_new<MLogRecordComputers>();
    template static MListItem *item_new<MLogRecordTariffs>();
    template static MListItem *item_new<MLogRecordFines>();
    template static MListItem *item_new<MLogRecordUsers>();
    template static MListItem *item_new<MLogRecordOptions>();
    template static MListItem *item_new<MLogRecordLogIn>();
    template static MListItem *item_new<MLogRecordLogOut>();
    template static MListItem *item_new<MLogRecordRun>();
    template static MListItem *item_new<MLogRecordFine>();
    template static MListItem *item_new<MLogRecordExchange>();
    template static MListItem *item_new<MLogRecordLock>();
    template static MListItem *item_new<MLogRecordPause>();
    template static MListItem *item_new<MLogRecordOpen>();
    template static MListItem *item_new<MLogRecordWtLocker>();
    template static MListItem *item_new<MLogRecordPowerOn>();
    template static MListItem *item_new<MLogRecordReboot>();
    template static MListItem *item_new<MLogRecordShutdown>();
    template static MListItem *item_new<MLogRecordDataShellState>();
    template static MListItem *item_new<MLogRecordDataStates>();
    template static MListItem *item_new<MLogRecordDataTariffs>();
    template static MListItem *item_new<MLogRecordDataFines>();
    template static MListItem *item_new<MLogRecordDataUsers>();

    template void item_del(MListItem *Item_);

*/

