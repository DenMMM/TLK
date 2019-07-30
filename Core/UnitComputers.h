//---------------------------------------------------------------------------
#ifndef UnitComputersH
#define UnitComputersH
//---------------------------------------------------------------------------
#include <string>

#include "UnitSLList.h"
//---------------------------------------------------------------------------
class MComputersItem;
class MComputers;
//---------------------------------------------------------------------------
#define MAX_Comps                   50      // Сколько компьютеров разрешено обслуживать TLK
#define MAX_CompAddrLen             15      // Длина IP-адреса
//---------------------------------------------------------------------------
#define mgcNone                     0       // Без цветовой метки
#define mgcAqua                     1       // Голубой
#define mgcGreen                    2       // Зеленый
#define mgcRed                      3       // Красный
#define mgcYellow                   4       // Желтый
//---------------------------------------------------------------------------
class MComputersItem:
	public MSLListItem::Simple <
		MSLListItem::Proxy <MSLListItem, MComputersItem>,
		MComputersItem>
{
public:
	// Функции механизма сохранения/загрузки данных
	unsigned GetDataSize() const override;
	void *SetData(void *Data_) const override;
	const void *GetData(const void *Data_, const void *Limit_) override;

public:
	char Number;						// Номер компьютера
	char Color;							// Цвет группы
	std::wstring Address;				// IP-адрес компьютера
	bool NotUsed;						// Игнорировать компьютер

	void Copy(const MListItem *SrcItem_) override;

	MComputersItem():
		Number(0),
		Color(mgcNone),
		NotUsed(false)
	{
	}

	virtual ~MComputersItem()
	{
	}
};
//---------------------------------------------------------------------------
class MComputers:
	public MSLList::Simple <MSLList, MComputers, MComputersItem>
{
public:
	MComputersItem *Search(char Number_) const;
	void Sort();

	MComputers() {}
	~MComputers() {}
};
//---------------------------------------------------------------------------
#endif

