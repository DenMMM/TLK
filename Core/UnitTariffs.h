//---------------------------------------------------------------------------
#ifndef UnitTariffsH
#define UnitTariffsH
//---------------------------------------------------------------------------
#include <string>
#include <cstdint>

#include "UnitComputers.h"
#include "UnitIDList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
class MTariffRunTimesItem;
class MTariffRunTimes;
class MTariffTimesItem;
class MTariffTimes;
class MTariffsInfoItem;
class MTariffsInfo;
class MTariffsItem;
class MTariffs;
//---------------------------------------------------------------------------
#define MAX_Tariffs             30
#define MAX_TariffTimes         20
#define MAX_TariffNameLen       50
#define MAX_TariffTimeCost      1000000.
//---------------------------------------------------------------------------
#define mgpNone     0
#define mgp1        1
#define mgp2        2
#define mgp3        4
#define mgp4        8
#define mgp5        16
#define mgp6        32
#define mgp7        64
#define mgp8        128
#define mgpRoute    256
#define mgpDesktop  512
#define mgpAll (mgp1|mgp2|mgp3|mgp4|mgp5|mgp6|mgp7|mgp8)
//---------------------------------------------------------------------------
#define mttUndefined    0
#define mttHours        1       // ���������
#define mttFlyPacket    2       // ����� N �����
#define mttPacket       3       // ����� � A �� B ����� (����, ��������)
//---------------------------------------------------------------------------
//#define Cost_Precision  0.50
//---------------------------------------------------------------------------
class MTariffRunTimesItem:
	public MListItemSimple <
		MListItem <MTariffRunTimes, MTariffRunTimesItem>,
		MTariffRunTimesItem>
{
public:
	std::uint32_t TariffID;	// ID-����� ������
	std::int8_t Number;	// ����� ����������
	std::int64_t StartTime;	// ����� ������ ������
	std::uint8_t Type;	// =
	std::int16_t BeginTime;	// =    ���������� � ������
	std::int16_t EndTime;	// =    � ����� ��������� ������
	std::int16_t SizeTime;	// =
	std::int16_t WorkTime;	// ������� ������� ������� �������� �������� �� ������ � ��� ������
	std::int16_t MaxTime;	// ����������� �� ������������ ����� ������ (�������� ����� ��������)
	double Cost;		// ��������� ������ �� ���������� � ������� 'WorkTime'

	MTariffRunTimesItem():
		TariffID(0),
		Number(0),
		StartTime(0),
		Type(mttUndefined),
		BeginTime(0),
		EndTime(0),
		SizeTime(0),
		WorkTime(0),
		MaxTime(0),
		Cost(0.)
	{
	}
};

class MTariffRunTimes:
	public MListSimple <
		MList <MTariffRunTimes, MTariffRunTimesItem>,
		MTariffRunTimesItem>
{
};
//---------------------------------------------------------------------------
class MTariffTimesItem:
	public MSLListItemSimple <
		MSLListItem <MTariffTimes, MTariffTimesItem>,
		MTariffTimesItem>
{
public:
	// ������� ��������� ����������/�������� ������
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::uint8_t Type;	// ��� ������: ���������, �����, "���������" �����
	std::int16_t BeginTime;	// ����� ������ �������� ������ � ������� � ������ �����
	std::int16_t EndTime;	// ����� ��������� �������� ������ � ������� � ������ �����
	std::int16_t SizeTime;	// ������������ �� ������� ������ � �������
	double Cost;		// ���������

	std::int16_t MaxWorkTime(std::int16_t Time_) const;

	MTariffTimesItem():
		Type(mttHours),
		BeginTime(0),
		EndTime(24*60),
		SizeTime(0),
		Cost(0.)
	{
	}
};

class MTariffTimes:
	public MSLListSimple <
		MSLList <MTariffTimes, MTariffTimesItem>,
		MTariffTimesItem>
{
};
//---------------------------------------------------------------------------
class MTariffsInfoItem:
	public MListItemSimple <
		MListItem <MTariffsInfo, MTariffsInfoItem>,
		MTariffsInfoItem>
{
public:
	std::uint32_t ID;
	std::wstring Name;

	MTariffsInfoItem():
		ID(0)
	{
	}
};

class MTariffsInfo:
	public MListSimple <
		MList <MTariffsInfo, MTariffsInfoItem>,
		MTariffsInfoItem>
{
public:
	const_iterator Search(std::uint32_t ID_) const;
	iterator Search(std::uint32_t ID_)
	{
		const auto *const_this=this;
		return const_cast_iter(const_this->Search(ID_));
	}
};
//---------------------------------------------------------------------------
class MTariffsItem:
	public MIDListItemSimple <
		MIDListItem <MTariffs, MTariffsItem>,
		MTariffsItem>
{
public:
	// ������� ��������� ����������/�������� ������
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

private:
	void CostPacket(MTariffRunTimesItem &RunTime_) const;
	void CostFlyPacket(MTariffRunTimesItem &RunTime_) const;
	void CostHours(MTariffRunTimesItem &RunTime_) const;

public:
	std::wstring Name;			// �������� ������
	std::uint32_t Programs;			// ������ �������� ��� �������
	bool Reboot;				// �������������� ��������� ����� �������
	std::vector <std::int8_t> Comps;	// ������ �����������, � ������� �������� �����
	MTariffTimes Times;			// ���� ������ �� �������

	// ��������� ���� �� ��� ��������� ������� ������ �� ������
	bool CheckForTime(std::int64_t Time_) const;
	// ��������� �������� �� ����� � ����������
	bool CheckForComp(std::int8_t Num_) const;
	//
	void Cost(MTariffRunTimesItem &RunTime_, double Prec_) const;
	MTariffsInfoItem GetInfo() const;
	MTariffRunTimes GetRunTimes(std::int64_t Time_) const;

	// ��������� �����
	struct LogData
	{
		std::uint32_t UUID;				// ID-����� ������
		std::wstring Name;       		// �������� ������
	};

	LogData gLogData() const
	{
		LogData ld;
		ld.UUID=UUID;
		ld.Name=Name;

		return ld;
	}

	void sFromLog(const LogData &Data_)
	{
		UUID=Data_.UUID;
		Name=Data_.Name;
	}

	MTariffsItem():
		Programs(0),
		Reboot(false)
	{
        Comps.reserve(MAX_Comps);       // �����������
	}
};

class MTariffs:
	public MIDListSimple <
		MIDList <MTariffs, MTariffsItem>,
		MTariffsItem>
{
public:
	MTariffsInfo GetForTime(std::int64_t Time_) const;
};
//---------------------------------------------------------------------------
#endif

