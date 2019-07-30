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
#define mlrBegin        0x11    // Лог начат
#define mlrEnd          0x12    // Лог закрыт
#define mlrDataShState  0x13    // Данные состояния оболочки
#define mlrDataStates   0x14    // Данные состояний компьютеров
#define mlrDataTariffs  0x15    // Данные тарифов
#define mlrDataFines    0x16    // Данные штрафов
#define mlrDataUsers    0x17    // Данные пользователей

#define mlrStart        0x21    // Запуск админского модуля
#define mlrStop         0x22    // Остановка админского модуля
#define mlrLogIn        0x23    // Пользователь начал работу
#define mlrLogOut       0x24    // Пользователь закончил работу
#define mlrConfig       0x25    // Настройки открыты/закрыты
#define mlrComputers    0x26    // Изменен список компьютеров
#define mlrTariffs      0x27    // Изменен список тарифов
#define mlrFines        0x28    // Изменен список штрафов
#define mlrUsers        0x29    // Изменен список пользователей
#define mlrOptions      0x2A    // Изменены общие настройки

#define mlrRun          0x31    // Запуск компьютера
#define mlrFine         0x32    // Штрафование компьютера
#define mlrExchange     0x33    // Пересадка с одного компьютера на другой
#define mlrLock         0x34    // Прикрытие/открытие компьютера
#define mlrPause        0x35    // Приостановка/запуск времени
#define mlrOpen         0x36    // Открытие/закрытие компьютера для обслуживания
#define mlrPowerOn      0x37    // Включение питания компьютера (WOL)
#define mlrReboot       0x38    // Перезагрузка компьютера
#define mlrShutdown     0x39    // Выключение питания компьютера

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
    ~MLogRecords() { Clear(); }
};
//---------------------------------------------------------------------------
class MLogRecordEvent:public MLogRecord
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // Время события

    void Copy(const MListItem *SrcItem_);

    MLogRecordEvent()
    {
        SystemTime=0;
    }
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
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // Время события
    bool Open;              // Настройки открыты/закрыты

    unsigned char gTypeID() const { return mlrConfig; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordConfig();
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
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // Время события
    unsigned User;          // ID-номер пользователя

    unsigned char gTypeID() const { return mlrLogIn; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordLogIn();
};

class MLogRecordLogOut:public MLogRecordEvent
{ public: unsigned char gTypeID() const { return mlrLogOut; } };
//---------------------------------------------------------------------------
class MLogRecordRun:public MLogRecord
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;                     // Время, когда была применена команда
    char Number;                            // Номер компьютера
    unsigned Tariff;                        // ID-номер тарифа
    __int64 StartTime;                      // Время, относительно которого посчитана стоимость
    char Type;                              // =
    short BeginTime;                        // =    Информация о пакете
    short EndTime;                          // =
    short SizeTime;                         // =
    short WorkTime;                         // Поставленное время работы
    double Cost;                            // Стоимость работы на компьютере в течении 'WorkTime'

    unsigned char gTypeID() const { return mlrRun; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordRun();
};
//---------------------------------------------------------------------------
class MLogRecordFine:public MLogRecord
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;                 // Время, когда была применена команда
    char Number;                        // Номер компьютера
    unsigned Fine;                      // ID-номер штрафа
    short Time;                         // Время штрафа

    unsigned char gTypeID() const { return mlrFine; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordFine();
};
//---------------------------------------------------------------------------
class MLogRecordExchange:public MLogRecord
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;                 // Время, когда была применена команда
    char From;                          // С какого компьютера было взято время
    char To;                            // На какой компьютер время было поставлено

    unsigned char gTypeID() const { return mlrExchange; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordExchange();
};
//---------------------------------------------------------------------------
class MLogRecordMode:public MLogRecord
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // Время, когда была применена команда
    char Number;            // Номер компьютера
    bool Apply;             // Режим был установлен/снят

    void Copy(const MListItem *SrcItem_);

    MLogRecordMode();
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
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // Время, когда была применена команда
    char Number;            // Номер компьютера

    void Copy(const MListItem *SrcItem_);

    MLogRecordCmd();
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
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;     // Время, когда была применена команда
    unsigned State;         // Состояние оболочки
    unsigned User;          // Текущий пользователь (чья смена открыта)

    unsigned char gTypeID() const { return mlrDataShState; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordDataShState();
};
//---------------------------------------------------------------------------
class MLogRecordDataStates:public MLogRecord
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;                 // Время, когда была применена команда
    Marray <MState::LogData> States;    // Массив состояний компьютеров

    unsigned char gTypeID() const { return mlrDataStates; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordDataStates();
};
//---------------------------------------------------------------------------
class MLogRecordDataTariffs:public MLogRecord
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;                 // Время, когда была применена команда
    Marray <MTariff::LogData> Tariffs;  // Массив описаний тарифов

    unsigned char gTypeID() const { return mlrDataTariffs; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordDataTariffs();
};
//---------------------------------------------------------------------------
class MLogRecordDataFines:public MLogRecord
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;                 // Время, когда была применена команда
    Marray <MFine::LogData> Fines;      // Массив описаний штрафов

    unsigned char gTypeID() const { return mlrDataFines; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordDataFines();
};
//---------------------------------------------------------------------------
class MLogRecordDataUsers:public MLogRecord
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    __int64 SystemTime;             // Время, когда была применена команда
    Marray <MUser::LogData> Users;  // Массив данных о пользователях

    unsigned char gTypeID() const  { return mlrDataUsers; }
    void Copy(const MListItem *SrcItem_);

    MLogRecordDataUsers();
};
//---------------------------------------------------------------------------
#endif

