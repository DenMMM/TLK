//---------------------------------------------------------------------------
#ifndef UnitCommonH
#define UnitCommonH
//---------------------------------------------------------------------------
class MOptions
{
public:
    AnsiString ServerAddress; // ��� ��� IP-����� �������
    bool ServerAuto; // ������������� ������ �������
    AnsiString PasswordToConnect; // ������ ��� �������� ����������� �������
    int NumberComputer; // ����� ����������
    AnsiString OptionsPassword; // ������ �� ��������� ��������

    int __fastcall Load(); // �������� ����� ��������
    int __fastcall Save(); // ���������� ����� ��������

    int BeginTime; // ����� ������ ������ ���������� (��� ���������������)
    int SizeTime; // �����, ���������� ��� ������ �� ����������
    int Mode; // ����� ������ ���������

    int BossLogTimer;

    int __fastcall LoadMode(); // �������� ���������� ��������� ���������
    int __fastcall SaveMode(); // ���������� ���������� ��������� ���������

    __fastcall MOptions();
};

extern MOptions Options;
//---------------------------------------------------------------------------
#endif

