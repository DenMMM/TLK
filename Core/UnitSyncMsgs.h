//---------------------------------------------------------------------------
#ifndef UnitSyncMsgsH
#define UnitSyncMsgsH
//---------------------------------------------------------------------------
#include "UnitAuth.h"
//---------------------------------------------------------------------------
struct MSyncData;
struct MSyncPHeader;
struct MSyncPHello;
struct MSyncPData;
struct MSyncPConf;
struct MSyncPWOL;
//---------------------------------------------------------------------------
#define MAC_AddrLength      6
//---------------------------------------------------------------------------
#pragma pack(push, 1)

struct MSyncData
{
    __int64 SystemTime;         // ��������� �����, ������������ ��� ���� ��������
    char Number;                // ����� ����������, � ������� ������������� ���������
    unsigned State;             // ��������� ���������� (������ ������)
    __int64 StartWorkTime;      // ����� ������� � ������ (���������� �����)
    int SizeWorkTime;           // �� ����� ����� ������� (� �������)
    __int64 StartFineTime;      // �����, ����� ��� �������� ����� (���������� �����)
    int SizeFineTime;           // ����� �������� � ������ ������ (� �������)
    __int64 StopTimerTime;      // ����� ��������� ������� ������� (���������� �����)
    unsigned Programs;          // ����� ��������, ����������� ��� �������
    unsigned Commands;          // �������������� �������
};

struct MSyncPHeader
{
    unsigned char Version;      // ������ �������� ����������
    unsigned char Type;         // ��� ������
    unsigned int Seed;          // ��������� ID (������� �� random ������� � �������)
};

struct MSyncPHello
{
    MSyncPHeader Header;        // ���������
    unsigned char MAC[MAC_Size];// MAC ������
};

struct MSyncPData
{
    MSyncPHeader Header;        // ���������
    MSyncData Data;             // ������ ��� ������������� � ��������
    unsigned char MAC[MAC_Size];// MAC ������
};

struct MSyncPConf
{
    MSyncPHeader Header;        // ���������
    unsigned char MAC[MAC_Size];// MAC ������
};

struct MSyncPWOL
{
    unsigned int ZeroFirst;                     // ���� � ������ ������
    unsigned char Sync[MAC_AddrLength];         // ������������������ ��� �������������
    unsigned char Magic[16][MAC_AddrLength];    // 16 ��� MAC-�����
};

#pragma pack(pop)
//---------------------------------------------------------------------------
#endif
