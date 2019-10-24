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
	public MSLListItemSimple <
		MSLListItem <MComputers, MComputersItem>,
		MComputersItem>
{
public:
	// Функции механизма сохранения/загрузки данных
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	char Number;						// Номер компьютера
	char Color;							// Цвет группы
	std::wstring Address;				// IP-адрес компьютера
	bool NotUsed;						// Игнорировать компьютер

	MComputersItem():
		Number(0),
		Color(mgcNone),
		NotUsed(false)
	{
	}
};
//---------------------------------------------------------------------------
class MComputers:
	public MSLListSimple <
		MSLList <MComputers, MComputersItem>,
		MComputersItem>
{
public:
	iterator Search(char Number_);
};
//---------------------------------------------------------------------------
#endif

