//---------------------------------------------------------------------------
#ifndef UnitFinesH
#define UnitFinesH
//---------------------------------------------------------------------------
#include <string>

#include "UnitIDList.h"
//---------------------------------------------------------------------------
class MFinesItem;
class MFines;
//---------------------------------------------------------------------------
#define MAX_Fines           20      // Предел количества штрафов
#define MAX_FineDescrLen    50      // Допустимая длина описания штрафа
//---------------------------------------------------------------------------
class MFinesItem:
	public MIDListItem::Simple <
		MIDListItem::Proxy <MIDListItem, MFinesItem>,
		MFinesItem>
{
public:
	// Функции механизма сохранения/загрузки данных
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	short Time;                   	// Время штрафа
	std::wstring Descr;             // Описание штрафа

	// Поддержка логов
	struct LogData
	{
		unsigned UUID;				// ID-номер штрафа
		std::wstring Descr;         // Описание штрафа

		LogData &operator=(const MFinesItem &Fine_)
		{
			UUID=Fine_.gUUID();
			Descr=Fine_.Descr;
			return *this;
		}
	};

	MFinesItem &operator=(const LogData &Data_)
	{
		UUID=Data_.UUID;
		Descr=Data_.Descr;
		return *this;
	}

	MFinesItem():
		Time(0)
	{
	}
};
//---------------------------------------------------------------------------
class MFines:
	public MIDList::Simple <MIDList, MFines, MFinesItem>
{
};
//---------------------------------------------------------------------------
#endif

