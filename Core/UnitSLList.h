//---------------------------------------------------------------------------
#ifndef UnitSLListH
#define UnitSLListH
//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <winsock2.h>
//#include <windows.h>

#include "UnitList.h"
//---------------------------------------------------------------------------
class MSLListItem;
class MSLList;
//---------------------------------------------------------------------------
#define MAX_SLFileSize  0x02000000  // Ограничение на размер файлов - 32Мбайт
#define MAX_SLRegSize   0x00000800  // Ограничение на размер ключей реестра - 2Кбайт
//---------------------------------------------------------------------------
//class MSLListItem: public MListItem
class MSLListItem:
	public MListItem::Proxy <MListItem, MSLListItem>
{
public:
	// Функции определения размера данных производного от MSLListItem класса,
	// сохранения и восстановления их в/из памяти.
	virtual unsigned GetDataSize() const = 0;
	virtual void *SetData(void *Data_) const = 0;
	virtual const void *GetData(const void *Data_, const void *Limit_) = 0;

	MSLListItem() = default;
	virtual ~MSLListItem() override = default;
};
//---------------------------------------------------------------------------
class MSLList:
	public MList::Typed <MList, MSLList, MSLListItem>
{
public:      /// ??? проверить MLog
	HKEY DefaultKey;			// HKCR/HKCU/HKLM/HKU
	std::wstring DefaultFile;	// путь к файлу на диске / раздел реестра
	std::wstring DefaultValue;	// имя параметра в реестре
	unsigned DefaultCode;		// двоичный ключ шифрования данных
	mutable DWORD LastError;	// код ошибки последней операции с файлом/реестром

public:
	// Функции определения размера данных производного от MSLList класса,
	// сохранения и восстановления их в/из памяти.
	// Заглушки. MList не предполагает дополнения атрибутами, а MSLList-да.
	virtual unsigned GetDataSize() const { return 0; }
	virtual void *SetData(void *Data_) const { return Data_; }
	virtual const void *GetData(const void *Data_, const void *Limit_) { return Data_; }

public:
	// Функции определения размера данных всех элементов списка,
	// сохранения и восстановления их в/из памяти.
	unsigned GetAllDataSize(bool Header_=true) const;
	void *SetAllData(void *Data_, bool Header_=true) const;
	const void *GetAllData(const void *Data_, const void *Limit_);

	bool SaveTo(const std::wstring &File_, unsigned Code_, bool Always_=true, bool Safe_=false) const;
	bool AttachTo(const std::wstring &File_, unsigned Code_, bool Safe_=false) const;
	bool LoadFrom(const std::wstring &File_, unsigned Code_);
	bool StoreTo(HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, unsigned Code_) const;
	bool QueryFrom(HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, unsigned Code_);
	bool SaveAsReg(const wchar_t *File_, HKEY Key_, const wchar_t *SubKey_, const wchar_t *Value_, unsigned Code_) const;    /// std::string
	void SetDefaultFile(const std::wstring &File_, unsigned Code_);
	void SetDefaultKey(HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, unsigned Code_);
	bool Save(bool Always_=true, bool Safe_=false) const;
	bool Attach(bool Safe_=false) const;
	bool Load();
	DWORD gLastErr() const { return LastError; }

	MSLList():
		DefaultCode(0),
		LastError(0)
	{
	}

	~MSLList() = default;
};
//---------------------------------------------------------------------------
#endif

