//---------------------------------------------------------------------------
#ifndef UnitFinesH
#define UnitFinesH
//---------------------------------------------------------------------------
#include <string>
#include <cstdint>

#include "UnitIDList.h"
//---------------------------------------------------------------------------
class MFinesItem;
class MFines;
//---------------------------------------------------------------------------
#define MAX_Fines           20      // Предел количества штрафов
#define MAX_FineDescrLen    50      // Допустимая длина описания штрафа
//---------------------------------------------------------------------------
class MFinesItem:
	public MIDListItemSimple <
		MIDListItem <MFines, MFinesItem>,
		MFinesItem>
{
public:
	// Функции механизма сохранения/загрузки данных
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::int16_t Time;				// Время штрафа
	std::wstring Descr;             // Описание штрафа

	// Поддержка логов
	struct LogData
	{
		std::uint32_t UUID;			// ID-номер штрафа
		std::wstring Descr;         // Описание штрафа
	};

	LogData gLogData() const
	{
		LogData ld;
		ld.UUID=gUUID();
		ld.Descr=Descr;

		return ld;
	}

	void sFromLog(const LogData &Data_)
	{
		UUID=Data_.UUID;
		Descr=Data_.Descr;
	}

	MFinesItem():
		Time(0)
	{
	}
};
//---------------------------------------------------------------------------
class MFines:
	public MIDListSimple <
		MIDList <MFines, MFinesItem>,
		MFinesItem>
{
};
//---------------------------------------------------------------------------
#endif

