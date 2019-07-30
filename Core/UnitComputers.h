//---------------------------------------------------------------------------
#ifndef UnitComputersH
#define UnitComputersH
//---------------------------------------------------------------------------
class MComputer;
class MComputers;
//---------------------------------------------------------------------------
#include "UnitSLList.h"
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
class MComputer:public MSLListItem
{
private:
    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

public:
    char Number;                                // Номер компьютера
    char Color;                                 // Цвет группы
    char Address[MAX_CompAddrLen+1];            // IP-адрес компьютера
    bool NotUsed;                               // Игнорировать компьютер

    char *SetAddress(char *Address_);
    void Copy(const MListItem *SrcItem_);

    MComputer();
    ~MComputer();
};
//---------------------------------------------------------------------------
class MComputers:public MSLList
{
private:
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MComputer; }
    void item_del(MListItem *Item_) const { delete (MComputer*)Item_; }

public:
    MComputer *Search(char Number_) const;
    void Sort();

    MComputers() {}
    ~MComputers() { Clear(); }
};
//---------------------------------------------------------------------------
#endif

