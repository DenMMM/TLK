//---------------------------------------------------------------------------
#ifndef UnitSLListH
#define UnitSLListH
//---------------------------------------------------------------------------
#include <winsock2.h>
//#include <windows.h>
//---------------------------------------------------------------------------
#include "UnitList.h"
//---------------------------------------------------------------------------
class MSLListItem;
class MSLList;
//---------------------------------------------------------------------------
class MSLListItem:public MListItem
{
public:
    virtual unsigned GetDataSize()=0;
    virtual char *SetData(char *Data_)=0;
    virtual char *GetData(char *Data_, char *Limit_)=0;
};
//---------------------------------------------------------------------------
class MSLList:public MList
{
protected:
    // Перегружаем методы базового класса MList
    void constructor();
    void destructor();

public:
    HKEY DefaultKey;            // HKCR/HKCU/HKLM/HKU
    char *DefaultFile;          // путь к файлу на диске / раздел реестра
    char *DefaultValue;         // имя параметра в реестре
    unsigned DefaultCode;       // двоичный ключ шифрования данных

    virtual unsigned GetDataSize() { return 0; }
    virtual char *SetData(char *Data_) { return Data_; }
    virtual char *GetData(char *Data_, char *Limit_) { return Data_; }

    unsigned GetAllDataSize(bool Header_);
    char *SetAllData(char *Data_, bool Header_);
    char *GetAllData(char *Data_, char *Limit_);

    bool SaveTo(char *File_, unsigned Code_);
    bool AttachTo(char *File_, unsigned Code_);
    bool LoadFrom(char *File_, unsigned Code_);
    bool StoreTo(HKEY Key_, char *SubKey_, char *Value_, unsigned Code_);
    bool QueryFrom(HKEY Key_, char *SubKey_, char *Value_, unsigned Code_);
    bool SetDefaultFile(char *File_, unsigned Code_);
    bool SetDefaultKey(HKEY Key_, char *SubKey_, char *Value_, unsigned Code_);
    bool Save();                // сохранить в файл/реестр
    bool Attach();              // добавить к файлу
    bool Load();                // считать из файла/реестра
};
//---------------------------------------------------------------------------
#endif

