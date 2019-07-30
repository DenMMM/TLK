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
#define mlrBegin        0x0101      // Лог начат
#define mlrEnd          0x0102      // Лог закрыт

#define mlrStart        0x0201      // Запуск админского модуля
#define mlrWork         0x0202      // Админский модуль работает
#define mlrStop         0x0203      // Остановка админского модуля

#define mlrConfig       0x0301      // Настройки открыты/закрыты
#define mlrComputers    0x0302      // Изменен список компьютеров
#define mlrTariffs      0x0303      // Изменен список тарифов
#define mlrFines        0x0304      // Изменен список штрафов
#define mlrUsers        0x0305      // Изменен список пользователей
#define mlrOptions      0x0306      // Изменены общие настройки

#define mlrLogIn        0x0401      // Пользователь начал работу
#define mlrLogOut       0x0402      // Пользователь закончил работу

#define mlrRun          0x0501      // Запуск компьютера
#define mlrFine         0x0502      // Штрафование компьютера
#define mlrExchange     0x0503      // Пересадка с одного компьютера на другой
#define mlrLock         0x0504      // Прикрытие/открытие компьютера
#define mlrPause        0x0505      // Приостановка/запуск времени
#define mlrOpen         0x0506      // Открытие/закрытие компьютера для обслуживания
#define mlrWtLocker     0x0507      // Отключение/включение TLK
#define mlrPowerOn      0x0508      // Включение питания компьютера
#define mlrReboot       0x0509      // Перезагрузка компьютера
#define mlrShutdown     0x050A      // Выключение питания компьютера

#define mlrDataShellState   0x0601      // Данные состояния оболочки
#define mlrDataStates       0x0602      // Данные состояний компьютеров
#define mlrDataTariffs      0x0603      // Данные тарифов
#define mlrDataFines        0x0604      // Данные штрафов
#define mlrDataUsers        0x0605      // Данные пользователей
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
    __int64 SystemTime;     // Время события

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;     // Время события
    bool Open;              // Настройки открыты/закрыты

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;     // Время события
    unsigned User;          // ID-номер пользователя

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;                     // Время, когда была применена команда
    int Number;                             // Номер компьютера
    unsigned Tariff;                        // ID-номер тарифа
    __int64 StartTime;                      // Время, относительно которого посчитана стоимость
    int Type;                               // =
    int BeginTime;                          // =    Информация о пакете
    int EndTime;                            // =
    int SizeTime;                           // =
    int WorkTime;                           // Поставленное время работы
    double Cost;                            // Стоимость работы на компьютере в течении 'WorkTime'

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;                 // Время, когда была применена команда
    int Number;                         // Номер компьютера
    unsigned Fine;                      // ID-номер штрафа
    int Time;                           // Время штрафа

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;                 // Время, когда была применена команда
    int From;                           // С какого компьютера было взято время
    int To;                             // На какой компьютер время было поставлено

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;     // Время, когда была применена команда
    int Number;             // Номер компьютера
    bool Apply;             // Режим был установлен/снят

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;     // Время, когда была применена команда
    int Number;             // Номер компьютера

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;     // Время, когда была применена команда
    unsigned State;         // Состояние оболочки
    unsigned User;          // Текущий пользователь (чья смена открыта)

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;     // Время, когда была применена команда
    unsigned NumStates;     // Количество записей в массиве
    MStateData *States;     // Массив состояний компьютеров

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;     // Время, когда была применена команда
    unsigned NumTariffs;    // Количество записей в массиве
    MTariffData *Tariffs;   // Массив описаний тарифов

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;     // Время, когда была применена команда
    unsigned NumFines;      // Количество записей в массиве
    MFineData *Fines;       // Массив описаний штрафов

    // Функции механизма сохранения/загрузки данных
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
    __int64 SystemTime;     // Время, когда была применена команда
    unsigned NumUsers;      // Количество записей в массиве
    MUserData *Users;       // Массив данных о пользователях

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MLogRecordDataUsers();
    ~MLogRecordDataUsers();
};
//---------------------------------------------------------------------------
#endif

