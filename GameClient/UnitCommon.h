//---------------------------------------------------------------------------
#ifndef UnitCommonH
#define UnitCommonH
//---------------------------------------------------------------------------
class MOptions
{
public:
    AnsiString ServerAddress; // Имя или IP-адрес сервера
    bool ServerAuto; // Автонастройка адреса сервера
    AnsiString PasswordToConnect; // Пароль для проверки подлинности сервера
    int NumberComputer; // Номер компьютера
    AnsiString OptionsPassword; // Пароль на изменение настроек

    int __fastcall Load(); // Загрузка общих настроек
    int __fastcall Save(); // Сохранение общих настроек

    int BeginTime; // Время начала работы компьютера (его разблокирования)
    int SizeTime; // Время, отведенное для работы на компьютере
    int Mode; // Режим работы программы

    int BossLogTimer;

    int __fastcall LoadMode(); // Загрузка параметров состояния программы
    int __fastcall SaveMode(); // Сохранение параметров состояния программы

    __fastcall MOptions();
};

extern MOptions Options;
//---------------------------------------------------------------------------
#endif

