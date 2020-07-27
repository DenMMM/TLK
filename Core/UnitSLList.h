//---------------------------------------------------------------------------
#ifndef UnitSLListH
#define UnitSLListH
//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <stdexcept>
#include <cwchar>
#include <cstdint>
#include <stdio.h>
#include <winsock2.h>
//#include <windows.h>

#include "UnitList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#define MAX_SLFileSize  0x02000000  // Ограничение на размер файлов - 32Мбайт
#define MAX_SLRegSize   0x00000800  // Ограничение на размер ключей реестра - 2Кбайт
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
class MSLListItem;

template <
	typename parent_item, typename base_type,
	typename item_type, std::uint8_t type_id>
using MSLListItemTyped = MListItemTyped <parent_item, base_type, item_type, type_id>;

template <typename parent_item, typename item_type>
using MSLListItemSimple = MListItemSimple <parent_item, item_type>;

template <typename list_type, typename base_type>
class MSLList;

template <typename parent_list, typename item_type>
using MSLListSimple = MListSimple <parent_list, item_type>;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <
	typename list_type,			// Имя будущего класса-списка
	typename base_type>			// Базовый (родительский) тип его элементов
class MSLListItem:
	public MListItem <list_type, base_type>
{
//	friend MSLList <list_type, base_type>;

public:
	// Функции определения размера данных производного от MSLListItem класса,
	// сохранения и восстановления их в/из памяти.
	virtual std::size_t GetDataSize() const = 0;
	virtual void *SetData(void *Data_) const = 0;
	virtual const void *GetData(const void *Data_, const void *Limit_) = 0;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <
	typename list_type,			// Создаваемый класс списка (имя)
	typename base_type>			// Базовый тип элемента списка
class MSLList:
	public MList <list_type, base_type>
{
public:
	// "Вытащим" объявления из родительского шаблона
	using MList <list_type, base_type> ::isTyped;
//	using MList <list_type, base_type> ::gFirst;
	using MList <list_type, base_type> ::Clear;
	using MList <list_type, base_type> ::Add;

public:      /// ??? проверить MLog
	HKEY DefaultKey;			// HKCR/HKCU/HKLM/HKU
	std::wstring DefaultFile;	// путь к файлу на диске / раздел реестра
	std::wstring DefaultValue;	// имя параметра в реестре
	std::uint32_t DefaultCode;	// двоичный ключ шифрования данных
	mutable DWORD LastError;	// код ошибки последней операции с файлом/реестром

public:
	// Функции определения размера данных производного от MSLList класса,
	// сохранения и восстановления их в/из памяти.
	// Заглушки. MList не предполагает дополнения атрибутами, а MSLList-да.
	virtual std::size_t GetDataSize() const { return 0; }
	virtual void *SetData(void *Data_) const { return Data_; }
	virtual const void *GetData(const void *Data_, const void *Limit_) { return Data_; }

public:
	// Функции определения размера данных всех элементов списка,
	// сохранения и восстановления их в/из памяти.
	std::size_t GetAllDataSize(bool Header_=true) const;
	void *SetAllData(void *Data_, bool Header_=true) const;
	const void *GetAllData(const void *Data_, const void *Limit_);

	bool SaveTo(const std::wstring &File_, std::uint32_t Code_, bool Always_=true, bool Safe_=false) const;
	bool AttachTo(const std::wstring &File_, std::uint32_t Code_, bool Safe_=false) const;
	bool LoadFrom(const std::wstring &File_, std::uint32_t Code_);
	bool StoreTo(HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, std::uint32_t Code_) const;
	bool QueryFrom(HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, std::uint32_t Code_);
	bool SaveAsReg(const std::wstring &File_, HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, std::uint32_t Code_) const;
	void SetDefaultFile(const std::wstring &File_, std::uint32_t Code_);
	void SetDefaultKey(HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, std::uint32_t Code_);
	bool Save(bool Always_=true, bool Safe_=false) const;
	bool Attach(bool Safe_=false) const;
	bool Load();
	DWORD gLastErr() const noexcept { return LastError; }

	MSLList():
		DefaultCode(0),
		LastError(0)
	{
	}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
std::size_t MSLList<list_type,base_type>::GetAllDataSize(bool Header_) const
{
	std::size_t DataSize=0;

	// Берем размер заголовка
	std::size_t Size=GetDataSize();
	if ( Size > std::numeric_limits<std::uint32_t>::max() )
	{
		throw std::out_of_range(
			"MSLList::GetAllDataSize()\n"
			"Размер заголовка превышает max(uint32_t).");
	}
	// Не забываем добавить поле для размера данных заголовка
	if ( Header_&&(Size!=0) ) DataSize+=sizeof(std::uint32_t)+Size;

	// Берем размер элементов списка (игнорируя элементы с нулевым размером данных)
	bool typed=isTyped();
	for ( const auto &Item: *this )
	{
		std::size_t Size=Item.GetDataSize();
		if ( Size==0 ) continue;

		// Если список типизированный, добавляем размер поля типа
		if ( typed ) Size+=sizeof(std::uint8_t);

		if ( Size > std::numeric_limits<std::uint32_t>::max() )
		{
			throw std::out_of_range(
				"MSLList::GetAllDataSize()\n"
				"Размер элемента превышает max(uint32_t).");
		}

		// Не забываем добавить поле для размера данных элемента
		DataSize+=sizeof(std::uint32_t)+Size;
	}
	// Маркер конца списка
	DataSize+=sizeof(std::uint32_t);

	// Будем считать (для совместимости), что размер данных укладывается в uint32_t
	if ( DataSize > std::numeric_limits<std::uint32_t>::max() )
	{
		throw std::out_of_range(
			"MSLList::GetAllDataSize()\n"
			"Размер списка превышает max(uint32_t).");
	}

	return DataSize;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
void *MSLList<list_type,base_type>::SetAllData(void *Data__, bool Header_) const
{
	const bool typed=isTyped();
	std::size_t BlockSize;
	char *Data_=static_cast<char*>(Data__);
	char *NextData;

	if ( Header_ )
	{
		// Оставляем поле для размера данных заголовка
		Data_+=sizeof(std::uint32_t);
		// Сохраняем данные заголовка и вычисляем его реальный размер
		NextData=static_cast<char*>(SetData(Data_));
		BlockSize=NextData-Data_;
		// Проверяем нужно ли было сохранять заголовок (были ли записаны данные)
		if ( BlockSize==0 ) Data_-=sizeof(std::uint32_t);
		else
		{
			// Сохраняем размер заголовка
			MemSet(
				Data_-sizeof(std::uint32_t),
				static_cast<std::uint32_t>(BlockSize));
			// Переходим далее
			Data_=NextData;
		}
	}

	for ( const auto &Item: *this )
	{
		// Оставляем поле для размера данных блока
		Data_+=sizeof(std::uint32_t);
		// Если список типизированный, то оставляем поле для типа элемента
		NextData=typed? Data_+sizeof(std::uint8_t): Data_;
		// Сохраняем данные блока и вычисляем его реальный размер (включая поле типа)
		NextData=static_cast<char*>(Item.SetData(NextData));
		BlockSize=NextData-Data_;
		// Проверяем нужно ли было сохранять блок (были ли записаны данные)
		if ( BlockSize!=0 )
		{
			// Сохраняем размер данных блока
			Data_=static_cast<char*>(MemSet(
				Data_-sizeof(std::uint32_t),
				static_cast<std::uint32_t>(BlockSize)));
			// Если список типизированный, сохраняем тип элемента
			if ( typed ) MemSet(Data_,Item.gTypeID());
			// Переходим далее
			Data_=NextData;
		} else Data_-=sizeof(std::uint32_t);
	}
	// Маркер конца списка
	Data_=static_cast<char*>(MemSet(Data_,(std::uint32_t)0));

	return Data_;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
const void *MSLList<list_type,base_type>::GetAllData(const void *Data__, const void *Limit_)
{
	const bool typed=isTyped();
	const char *Data_=static_cast<const char*>(Data__);
	const char *Limit;
	std::uint32_t DataSize;
	std::uint8_t TypeID;

	// Очищаем список для профилактики
	Clear();
									/// возможно, стоит проверять bad_alloc
	// Берем размер вроде бы заголовка списка
	Data_=static_cast<const char*>(MemGet(Data_,&DataSize,Limit_));
	if ( Data_==nullptr ) goto error;
	// Вычисляем лимит для операций с памятью внутри этого блока
	Limit=Data_+DataSize;
	// Проверим выход на установленную границу и на переполнение указателя
	if ( (Limit>Limit_)||(Limit<Data_) ) goto error;
	// Берем данные
	{
		const char *result=static_cast<const char*>(GetData(Data_,Limit));
		// Если ничего не считали, значит у списка нет заголовка
		if ( Data_==result ) Data_-=sizeof(std::uint32_t);
		else if ( result!=Limit ) goto error;
		else Data_=result;
	}

	while(true)
	{
		// Берем размер блока
		Data_=static_cast<const char*>(MemGet(Data_,&DataSize,Limit_));
		if ( Data_==nullptr ) goto error;
		// Если размер блока равен нулю (маркер конца списка),
		// то прекращаем заполнение списка
		if ( DataSize==0 ) break;
		// Вычисляем лимит для операций с памятью внутри этого блока
		Limit=Data_+DataSize;
		// Проверим выход на установленную границу и на переполнение указателя
		if ( (Limit>Limit_)||(Limit<Data_) ) goto error;
		// Если список типизированный, берем тип элемента
		if ( typed )
		{
			Data_=static_cast<const char*>(MemGet(Data_, &TypeID, Limit_));
			if ( Data_==nullptr ) goto error;
		} else TypeID=0;
		// Добавляем новый элемент в список
		try
		{
			base_type& Item=Add(TypeID);
			// Берем данные
			Data_=static_cast<const char*>(Item.GetData(Data_,Limit));
			if ( Data_!=Limit ) goto error;
		}
		catch (std::invalid_argument &e)
		{
			goto error;
		}
	}

	return Data_;
error:
	// Очищаем частично заполненный список
	Clear();
    return nullptr;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
bool MSLList<list_type,base_type>::SaveTo(const std::wstring &File_, std::uint32_t Code_, bool Always_, bool Safe_) const
{
	HANDLE file=INVALID_HANDLE_VALUE;
	DWORD data_size, rw_size;
	std::vector <char> all_data;

	LastError=0;
	// Определяем размер данных и проверяем на допустимость
	data_size=GetAllDataSize();         /// value limited to int32_t
	if ( data_size>MAX_SLFileSize )
    {
		throw std::runtime_error (      /// заменить на return false ?
			"MSLList::SaveTo()\n"
			"Размер данных превышает ограничение MAX_SLFileSize.");
    }
    // Выделяем память под данные.
    // bad_alloc не ловим, т.к. ничего еще не начали делать
	all_data.resize(data_size);
	// Сохраняем весь список в памяти и сверяем реальный размер данных
	if ( SetAllData(all_data.data()) != &all_data[all_data.size()] )
	{
		throw std::runtime_error (
			"MSLList::SaveTo()\n"
			"Размер данных MSLList::SetAllData() не соответствует MSLList::GetAllDataSize().");
	}
	// Шифруем данные
	BasicEncode(all_data.data(),all_data.size(),Code_);

	// Создаем файл
	if ( (file=::CreateFile(File_.c_str(),GENERIC_WRITE,0,nullptr,
        Always_? (Safe_?OPEN_ALWAYS:CREATE_ALWAYS) :CREATE_NEW,
        Safe_?FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH:FILE_ATTRIBUTE_NORMAL,
        nullptr))==INVALID_HANDLE_VALUE ) goto api_error;

    if ( Always_&&Safe_ )
    {
        // Задаем новый размер файла
        if ( ::SetFilePointer(file,(LONG)data_size,nullptr,FILE_BEGIN)==0xFFFFFFFF ) goto api_error;
        if ( !SetEndOfFile(file) ) goto api_error;
        // Возвращаем позицию к началу
        if ( ::SetFilePointer(file,(LONG)0,nullptr,FILE_BEGIN)==0xFFFFFFFF ) goto api_error;
    }
    // Записываем данные
	if ( (!::WriteFile(file,all_data.data(),data_size,&rw_size,nullptr))||
        (rw_size!=data_size) ) goto api_error;

    ::CloseHandle(file);
    return true;
api_error:
    LastError=::GetLastError();
    if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
    return false;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
bool MSLList<list_type,base_type>::AttachTo(const std::wstring &File_, std::uint32_t Code_, bool Safe_) const
{
	HANDLE file=INVALID_HANDLE_VALUE;
	DWORD file_sizel, file_sizeh;
	DWORD read_size, data_size, rw_size;
	std::vector <char> all_data;

	LastError=0;
    // Открываем файл
	if ( (file=::CreateFile(File_.c_str(),GENERIC_READ|GENERIC_WRITE,0,nullptr,
		OPEN_EXISTING,
        Safe_?FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH:FILE_ATTRIBUTE_NORMAL,
        nullptr))==INVALID_HANDLE_VALUE ) goto api_error;
	// Определяем размер файла и проверяем на допустимость
    if ( (file_sizel=::GetFileSize(file,&file_sizeh))==0xFFFFFFFF ) goto api_error;
    if ( (file_sizeh!=0)||
         (file_sizel<sizeof(std::uint32_t))||    // короче маркера конца списка быть не может
         (file_sizel>=MAX_SLFileSize) ) goto error;
    // Определяем размер данных для добавления к файлу, его допустимость
    // и не превысит ли ограничение увеличившийся файл (иначе потом не откроем его)
    data_size=GetAllDataSize(false);
    if ( data_size>MAX_SLFileSize )
    {
        throw std::runtime_error (      /// заменить на return false ?
			"MSLList::AttachTo()\n"
			"Размер данных превышает ограничение MAX_SLFileSize.");
    }
    if ( (data_size+file_sizel)>MAX_SLFileSize ) goto error;

    read_size=file_sizel;
    // Ограничиваем область чтения двойным размером
    // блока шифрования (особенность алгоритма)
    if ( read_size>(sizeof(std::uint32_t)*2) ) read_size=sizeof(std::uint32_t)*2;
    // Определяем общий размер данных для записи
    data_size+=read_size-sizeof(std::uint32_t);
    // Выделяем память под новые данные и часть старых из файла
    try { all_data.resize(data_size); }
	catch ( std::bad_alloc &e )
    {
        // Закрываем файл и передаем исключение выше
        ::CloseHandle(file);
        throw e;
    }

    // Читаем часть старых данных из файла
    if ( (::SetFilePointer(file,-(LONG)read_size,nullptr,FILE_END)==0xFFFFFFFF)||
        (!::ReadFile(file,all_data.data(),read_size,&rw_size,nullptr))||
        (rw_size!=read_size) ) goto api_error;  /// ошибка обработки rw_size ?
    // Расшифровываем их и проверяем наличие маркера конца списка (0x00000000)
    BasicDecode(all_data.data(),read_size,Code_);
    if ( ((std::uint32_t*)&all_data[read_size])[-1]!=0 ) goto error;

    // Сохраняем список без заголовка в памяти и сверяем реальный размер данных
	if ( SetAllData(&all_data[read_size-sizeof(std::uint32_t)],false) !=
		&all_data[all_data.size()] )
	{
        throw std::runtime_error (
			"MSLList::AttachTo()\n"
			"Размер данных MSLList::SetAllData() не соответствует MSLList::GetAllDataSize().");
	}

    // Шифруем
    BasicEncode(all_data.data(),all_data.size(),Code_);

    if ( Safe_ )
    {
		// Резервируем место в файле
        if ( (::SetFilePointer(file,(LONG)(data_size-read_size),nullptr,FILE_END)==0xFFFFFFFF)||
            (!SetEndOfFile(file)) ) goto api_error;
        // Отступаем от конца файла так, чтобы перезаписать часть старых данных
        if ( ::SetFilePointer(file,-(LONG)data_size,nullptr,FILE_END)==0xFFFFFFFF ) goto api_error;
    } else
    {
        // Отступаем от конца файла так, чтобы перезаписать часть старых данных
        if ( ::SetFilePointer(file,-(LONG)read_size,nullptr,FILE_END)==0xFFFFFFFF ) goto api_error;
    }
    // Сохраняем данные в файле
    if ( (!::WriteFile(file,all_data.data(),data_size,&rw_size,nullptr))||
        (rw_size!=data_size) ) goto api_error;    /// ошибка обработки rw_size ?

    ::CloseHandle(file);
    return true;
api_error:
    LastError=::GetLastError();
error:
    if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
    return false;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
bool MSLList<list_type,base_type>::LoadFrom(const std::wstring &File_, std::uint32_t Code_)
{
	HANDLE file=INVALID_HANDLE_VALUE;
	DWORD file_sizel, file_sizeh, rw_size;
	std::vector <char> all_data;

	LastError=0;
	// Открываем файл
	if ( (file=::CreateFile(File_.c_str(),GENERIC_READ,0,nullptr,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,nullptr))==INVALID_HANDLE_VALUE ) goto api_error;
	// Определяем его размер и проверяем на допустимость
	if ( (file_sizel=::GetFileSize(file,&file_sizeh))==0xFFFFFFFF ) goto api_error;
	if ( (file_sizeh!=0)||
		 (file_sizel>MAX_SLFileSize) ) goto error;
	// Выделяем память под данные
	try { all_data.resize(file_sizel); }
	catch ( std::bad_alloc &e )
	{
		// Закрываем файл и передаем исключение выше
		::CloseHandle(file);
		throw e;
	}
	// Считываем данные из файла
	if ( (!::ReadFile(file,all_data.data(),file_sizel,&rw_size,nullptr))||
		(rw_size!=file_sizel) ) goto api_error;
	// Закрываем его
	::CloseHandle(file); file=INVALID_HANDLE_VALUE;

	// Расшифровываем
	BasicDecode(all_data.data(),file_sizel,Code_);
	// Заносим в список (bad_alloc не ловим, т.к. уже безопасно)
	if ( GetAllData(all_data.data(),&all_data[file_sizel]) !=
		&all_data[file_sizel] ) goto error;

	return true;
api_error:
	LastError=::GetLastError();
error:
	if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
	return false;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
bool MSLList<list_type,base_type>::StoreTo(HKEY Key_, const std::wstring &SubKey_,
	const std::wstring &Value_, std::uint32_t Code_) const
{
	HKEY key=nullptr;
	std::vector <char> data;
	std::size_t size;

	LastError=0;
	// Определяем размер данных и проверяем на допустимость
	size=GetAllDataSize();
	if ( size>MAX_SLRegSize )
	{
		throw std::runtime_error (      /// заменить на return false ?
			"MSLList::StoreTo()\n"
			"Размер данных превышает ограничение MAX_SLRegSize.");
	}
    // Выделяем память (bad_alloc не ловим, т.к. еще ничего не начали делать)
    data.resize(size);
    // Сохраняем весь список в памяти и сверяем реальный размер данных
	if ( SetAllData(data.data()) != &data[data.size()] )
	{
        throw std::runtime_error (
			"MSLList::StoreTo()\n"
			"Размер данных MSLList::SetAllData() не соответствует MSLList::GetAllDataSize().");
    }
    // Шифруем
	BasicEncode(data.data(),data.size(),Code_);

    // Создаем ключ реестра
	if ( ::RegCreateKeyEx(Key_,SubKey_.c_str(),0,nullptr,
		REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,nullptr,
		&key,nullptr)!=ERROR_SUCCESS ) goto api_error;
	// Сохраняем параметр
	if ( ::RegSetValueEx(key,Value_.c_str(),0,REG_BINARY,
		(LPBYTE)data.data(),size)!=ERROR_SUCCESS ) goto api_error;

    ::RegCloseKey(key);
    return true;
api_error:
	LastError=::GetLastError();
	if ( key!=nullptr ) ::RegCloseKey(key);
	return false;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
bool MSLList<list_type,base_type>::QueryFrom(HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, std::uint32_t Code_)
{
	HKEY key=nullptr;
	DWORD size;
	std::vector <char> data;

	LastError=0;
	// Открываем ключ реестра
	if ( ::RegOpenKeyExW(Key_,SubKey_.c_str(),
		0,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS ) goto api_error;

	// Сразу выделяем память под данные (для реестра буфер маленький)
	try { data.resize(MAX_SLRegSize); }
	catch ( std::bad_alloc &e )
	{
		// Закрываем ключ реестра и передаем исключение выше
		::RegCloseKey(key); key=nullptr;
		throw e;
	}
	// Считываем сколько есть по-факту данных
	size=MAX_SLRegSize;
	if ( ::RegQueryValueEx(key,Value_.c_str(),
		nullptr,nullptr,(LPBYTE)data.data(),&size)!=ERROR_SUCCESS ) goto api_error;
	// Закрываем ключ реестра
	::RegCloseKey(key); key=nullptr;

	// Расшифровываем данные
	BasicDecode(data.data(),size,Code_);
	// Заносим данные в список (bad_alloc не ловим, т.к. уже безопасно)
	if ( GetAllData(&data[0], &data[size])==nullptr ) goto error;

	return true;
api_error:
	LastError=::GetLastError();
error:
	if ( key!=nullptr ) ::RegCloseKey(key);
	return false;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
bool MSLList<list_type,base_type>::SaveAsReg(
	const std::wstring &File_,
	HKEY Key_,
	const std::wstring &SubKey_,
	const std::wstring &Value_,
	std::uint32_t Code_) const
{
	LastError=0;
	// Определяем размер данных и проверяем на допустимость
	DWORD size=GetAllDataSize();
	if ( size>MAX_SLRegSize )
	{
		throw std::runtime_error (      /// заменить на return false ?
			"MSLList::SaveAsReg()\n"
			"Размер данных превышает ограничение MAX_SLRegSize.");
	}
	// Выделяем память (bad_alloc не ловим, т.к. еще ничего не начали делать)
	std::vector <char> data;
	data.resize(size);
	// Сохраняем весь список в памяти и сверяем реальный размер данных
	if ( SetAllData(data.data()) != (data.data()+data.size()) )
	{
		throw std::runtime_error (
			"MSLList::SaveAsReg()\n"
			"Размер данных MSLList::SetAllData() не соответствует MSLList::GetAllDataSize().");
	}
	// Шифруем
	BasicEncode(data.data(),data.size(),Code_);
	// Конвертируем в HEX
	std::vector <wchar_t> hex_data;
	hex_data.resize((data.size()*3+1)*sizeof(wchar_t));
	ByteToHEX(
		data.data(), data.size(),
		hex_data.data(), hex_data.size(),
		L',');

	// Формируем текстовую часть reg-файла
	static const wchar_t hdr[]=L"REGEDIT4";
	static const wchar_t hk1[]=L"HKEY_LOCAL_MACHINE";
	static const wchar_t hk2[]=L"HKEY_CURRENT_USER";
	static const wchar_t hk3[]=L"HKEY_USERS";
	static const wchar_t hk4[]=L"HKEY_CLASSES_ROOT";
	const wchar_t *hk=nullptr;	if ( Key_==HKEY_LOCAL_MACHINE ) hk=hk1;
	else if ( Key_==HKEY_CURRENT_USER ) hk=hk2;	else if ( Key_==HKEY_USERS ) hk=hk3;	else if ( Key_==HKEY_CLASSES_ROOT ) hk=hk4;	else		throw std::runtime_error (			"MSLList::SaveAsReg()\n"
			"Задан не верный тип HKEY.");

	const std::wstring reg_str=
		L"\xFEFF"           			// UTF-16BE
		L"Windows Registry Editor Version 5.00\r\n"
		L"\r\n"
		L"["+std::wstring(hk)+L"\\"+SubKey_+L"]\r\n"+
		L"\""+Value_+L"\"=hex:"+hex_data.data()+L"\r\n";

	const wchar_t *reg_data=reg_str.c_str();    /// Windows only !!!
	const DWORD reg_size=reg_str.length()*sizeof(*reg_data);

	// Создаем файл и записываем данные
	HANDLE file;
	DWORD rw_size;
	if ( (file=::CreateFile(File_.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,nullptr)) == INVALID_HANDLE_VALUE ) goto api_error;
	if ( (!::WriteFile(file,reg_data,reg_size,&rw_size,nullptr))||
		(rw_size!=reg_size) ) goto api_error;

	::CloseHandle(file);
	return true;
api_error:
	LastError=::GetLastError();
	if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
	return false;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
void MSLList<list_type,base_type>::SetDefaultFile(
	const std::wstring &File_, std::uint32_t Code_)
{
	DefaultFile=File_;
	DefaultValue.clear();
	DefaultCode=Code_;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
void MSLList<list_type,base_type>::SetDefaultKey(
	HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, std::uint32_t Code_)
{
	DefaultFile=SubKey_;
	DefaultValue=Value_;
	DefaultKey=Key_;
	DefaultCode=Code_;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
bool MSLList<list_type,base_type>::Save(bool Always_, bool Safe_) const
{
	if ( DefaultValue.empty() )
		return DefaultFile.empty()? false:
		SaveTo(DefaultFile,DefaultCode,Always_,Safe_);
	else
		return DefaultFile.empty()? false:
		StoreTo(DefaultKey,DefaultFile,DefaultValue,DefaultCode);
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
bool MSLList<list_type,base_type>::Attach(bool Safe_) const
{
	return (DefaultFile.empty())||(!DefaultValue.empty())?
		false: AttachTo(DefaultFile,DefaultCode,Safe_);
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
bool MSLList<list_type,base_type>::Load()
{
    if ( DefaultValue.empty() )
        return DefaultFile.empty()?
            false: LoadFrom(DefaultFile,DefaultCode);
    else
        return DefaultFile.empty()?
            false: QueryFrom(DefaultKey,DefaultFile,DefaultValue,DefaultCode);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif

