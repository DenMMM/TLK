//---------------------------------------------------------------------------
#ifndef UnitComputersH
#define UnitComputersH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
//---------------------------------------------------------------------------
#define MAX_Computers               100     //
#define MAX_ComputerAddressLength   15      //
//---------------------------------------------------------------------------
#define mgcNone ((int)-1)   //
#define mgcAqua ((int)0)    //
#define mgcLime ((int)1)    //
#define mgcRed ((int)2)     //
#define mgcYellow ((int)3)  //
//---------------------------------------------------------------------------
class MComputer;
class MComputers;
//---------------------------------------------------------------------------
class MComputer:public MSLListItem
{
public:
    bool Copy(MListItem *SrcItem_);
public:
    int Number;                                 // Номер компьютера
    char Address[MAX_ComputerAddressLength+1];  // IP-адрес компьютера
    int GroupColor;                             // Цвет группы
    bool NotUsed;                               // Маркер не использования

    char *SetAddress(char *Address_);

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    MComputer();
    ~MComputer();
};
//---------------------------------------------------------------------------
class MComputers:public MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MComputer; }
    void operator_delete(MListItem *DelItem_) { delete (MComputer*)DelItem_; }
public:
    MComputer *Search(int Number_);
    void Sort();

    MComputers() { constructor(); }
    ~MComputers() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

