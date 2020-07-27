//---------------------------------------------------------------------------
#ifndef UnitComputersH
#define UnitComputersH
//---------------------------------------------------------------------------
#include <string>
#include <cstdint>

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
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::int8_t Number;					// Номер компьютера
	std::uint8_t Color;					// Цвет группы
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
	const_iterator Search(std::int8_t Number_) const;
	iterator Search(std::int8_t Number_)
	{
		const auto *const_this=this;
		return const_cast_iter(const_this->Search(Number_));
	}
};
//---------------------------------------------------------------------------
#endif

