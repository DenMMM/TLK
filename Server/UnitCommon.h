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
    AnsiString PasswordToConnect; // ������ ��� �������� ����������� �������
    int ClientNumber; // ���������� ��������
    MClient Clients[MaxClients]; // ������ ���������� ��������
    int TariffNumber; // ���������� �������
    MTariff Tariffs[MaxTariffs]; // ������ ���������� �������
    int Mode; // ����� ������ �������
    AnsiString AdminPassword; // ������ ������������ ��������������

    int __fastcall Load(); // �������� �����
    int __fastcall Save(); // ���������� �����

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
    AnsiString ServerAddress; // ��� ��� IP-����� �������
    bool ServerAuto; // ������������� ������ �������
    AnsiString PasswordToConnect; // ������ ��� �������� ����������� �������
    int NumberComputer; // ����� ����������
    AnsiString OptionsPassword; // ������ �� ��������� ��������

    int __fastcall Load(AnsiString FileName); // �������� ����� ��������
    int __fastcall Save(AnsiString FileName); // ���������� ����� ��������
};

extern MOptions Options;

//---------------------------------------------------------------------------
#endif
