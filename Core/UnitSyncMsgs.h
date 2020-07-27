//---------------------------------------------------------------------------
#ifndef UnitSyncMsgsH
#define UnitSyncMsgsH
//---------------------------------------------------------------------------
#include <cstdint>

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
	std::int64_t SystemTime;	// ��������� �����, ������������ ��� ���� ��������
	std::uint8_t Number;		// ����� ����������, � ������� ������������� ���������
	std::uint32_t State;		// ��������� ���������� (������ ������)
	std::int64_t StartWorkTime;	// ����� ������� � ������ (���������� �����)
	std::int32_t SizeWorkTime;	// �� ����� ����� ������� (� �������)       	/// int16_t ?
	std::int64_t StartFineTime;	// �����, ����� ��� �������� ����� (���������� �����)
	std::int32_t SizeFineTime;	// ����� �������� � ������ ������ (� �������)   /// int16_t ?
	std::int64_t StopTimerTime;	// ����� ��������� ������� ������� (���������� �����)
	std::uint32_t Programs;		// ����� ��������, ����������� ��� �������
	std::uint32_t Commands;		// �������������� �������
};

struct MSyncPHeader
{
	std::uint8_t Version;		// ������ �������� ����������
	std::uint8_t Type;			// ��� ������
	std::uint32_t Seed;			// ��������� ID (������� �� random ������� � �������)
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
	std::uint32_t ZeroFirst;					// ���� � ������ ������
	unsigned char Sync[MAC_AddrLength];			// ������������������ ��� �������������
	unsigned char Magic[16][MAC_AddrLength];	// 16 ��� MAC-�����
};

#pragma pack(pop)
//---------------------------------------------------------------------------
#endif
