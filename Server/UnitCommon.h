//---------------------------------------------------------------------------
#ifndef UnitCommonH
#define UnitCommonH
//---------------------------------------------------------------------------
#define MaxClients 50
#define MaxTariffs 30
//---------------------------------------------------------------------------

class MClient
{
public:
    AnsiString Address;
    int Number;
    int Mode;
    int TimeSize;
    int TimeToEnd;
    bool NotInUse, StatUsed;

    MClient()
    {
        Number=0;
        Mode=-1;
        TimeSize=0;
        TimeToEnd=24*60;
        NotInUse=false;
        StatUsed=false;
    }
};

class MTariff
{
public:
    int Begin, Size;

    MTariff()
    {
        Begin=-1;
        Size=0;
    }
};

class MOptions
{
public:
    AnsiString PasswordToConnect; // Пароль для проверки подлинности сервера
    int ClientNumber; // Количество клиентов
    MClient Clients[MaxClients]; // Массив описателей клиентов
    int TariffNumber; // Количество тарифов
    MTariff Tariffs[MaxTariffs]; // Массив описателей тарифов
    int Mode; // Режим работы сервера
    AnsiString AdminPassword; // Пароль технического администратора

    int __fastcall Load(); // Загрузка опций
    int __fastcall Save(); // Сохранение опций

    MOptions()
    {
        ClientNumber=0;
        TariffNumber=0;
        Mode=0;
        AdminPassword="admin";
    }
};

class MClientOptions
{
public:
    AnsiString ServerAddress; // Имя или IP-адрес сервера
    bool ServerAuto; // Автонастройка адреса сервера
    AnsiString PasswordToConnect; // Пароль для проверки подлинности сервера
    int NumberComputer; // Номер компьютера
    AnsiString OptionsPassword; // Пароль на изменение настроек

    int __fastcall Load(AnsiString FileName); // Загрузка общих настроек
    int __fastcall Save(AnsiString FileName); // Сохранение общих настроек
};

extern MOptions Options;

//---------------------------------------------------------------------------
#endif
