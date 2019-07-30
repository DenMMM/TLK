//---------------------------------------------------------------------------
#include <vector>
#include <stdio.h>
#include <cwchar>
#include <stdexcept>
#include <winsock2.h>
//#include <windows.h>
#pragma hdrstop

#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
unsigned MSLList::GetAllDataSize(bool Header_) const
{
    unsigned DataSize=0, Size;
	bool typed=isTyped();

    // Берем размер заголовка
    if ( Header_&&((Size=GetDataSize())!=0) ) DataSize+=sizeof(unsigned)+Size;
    // Берем размер элементов списка (игнорируя элементы с нулевым размером данных)
	for ( MSLListItem *Item=gFirst(); Item; Item=Item->gNext() )
    {
        Size=Item->GetDataSize(); if ( Size==0 ) continue;
        // Не забываем добавить поле для размера данных элемента
        DataSize+=Size+sizeof(unsigned);
        // Если список типизированный, добавляем размер поля типа
        if ( typed ) DataSize+=sizeof(unsigned char);
    }
    // Маркер конца списка
    DataSize+=sizeof(unsigned);
    return DataSize;
}
//---------------------------------------------------------------------------
void *MSLList::SetAllData(void *Data__, bool Header_) const
{
	const bool typed=isTyped();
	unsigned BlockSize;
    char *Data_=static_cast<char*>(Data__);
	char *NextData;

	if ( Header_ )
	{
		// Оставляем поле для размера данных заголовка
		Data_+=sizeof(unsigned);
        // Сохраняем данные заголовка и вычисляем его реальный размер
		NextData=static_cast<char*>(SetData(Data_));
		BlockSize=static_cast<unsigned>(NextData-Data_);
        // Проверяем нужно ли было сохранять заголовок (были ли записаны данные)
        if ( BlockSize==0 ) Data_-=sizeof(unsigned);
        else
        {
            // Сохраняем размер заголовка
            MemSet(Data_-sizeof(unsigned),BlockSize);
            // Переходим далее
            Data_=NextData;
        }
    }

	for ( MSLListItem *Item=gFirst();
		Item; Item=Item->gNext() )
	{
        // Оставляем поле для размера данных блока
        Data_+=sizeof(unsigned);
        // Если список типизированный, то оставляем поле для типа элемента
        NextData=typed? Data_+sizeof(unsigned char): Data_;
        // Сохраняем данные блока и вычисляем его реальный размер (включая поле типа)
		NextData=static_cast<char*>(Item->SetData(NextData));
        BlockSize=static_cast<unsigned>(NextData-Data_);
        // Проверяем нужно ли было сохранять блок (были ли записаны данные)
        if ( BlockSize!=0 )
        {
            // Сохраняем размер данных блока
			Data_=static_cast<char*>(MemSet(Data_-sizeof(unsigned),BlockSize));
			// Если список типизированный, сохраняем тип элемента
			if ( typed ) MemSet(Data_,Item->gTypeID());
			// Переходим далее
			Data_=NextData;
		} else Data_-=sizeof(unsigned);
	}
	// Маркер конца списка
	Data_=static_cast<char*>(MemSet(Data_,(unsigned)0));

    return Data_;
}
//---------------------------------------------------------------------------
const void *MSLList::GetAllData(const void *Data__, const void *Limit_)
{
	const bool typed=isTyped();
	const char *Data_=static_cast<const char*>(Data__);
	const char *Limit;
    unsigned DataSize;
    unsigned char TypeID;
	MSLListItem *Item;

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
        if ( Data_==result ) Data_-=sizeof(unsigned);
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
		Item=Add(TypeID); if ( Item==nullptr ) goto error;
		// Убедимся, что созданный объект имеет правильную базу
//		Item=&dynamic_cast<MSLListItem&>(*Item);    /// пока _DEBUG ?
		// Берем данные
        Data_=static_cast<const char*>(Item->GetData(Data_,Limit));
        if ( Data_!=Limit ) goto error;
    }

    return Data_;
error:
    // Очищаем частично заполненный список
    Clear();
    return nullptr;
}
//---------------------------------------------------------------------------
bool MSLList::SaveTo(const std::wstring &File_, unsigned Code_, bool Always_, bool Safe_) const
{
    HANDLE file=INVALID_HANDLE_VALUE;
    DWORD data_size, rw_size;
    std::vector <char> all_data;

    LastError=0;
    // Определяем размер данных и проверяем на допустимость
    data_size=GetAllDataSize();
    if ( data_size>MAX_SLFileSize )
    {
		throw std::runtime_error (      /// заменить на return false ?
            "MSLList::SaveTo()\n"
            "Размер данных превышает ограничение MAX_SLFileSize."
            );
    }
    // Выделяем память под данные.
    // bad_alloc не ловим, т.к. ничего еще не начали делать
	all_data.resize(data_size);
	// Сохраняем весть список в памяти и сверяем реальный размер данных
	if ( SetAllData(all_data.data()) != &all_data[all_data.size()] )
	{
		throw std::runtime_error (
			"MSLList::SaveTo()\n"
			"Размер данных MSLList::SetAllData() не соответствует MSLList::GetAllDataSize()."
			);
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
bool MSLList::AttachTo(const std::wstring &File_, unsigned Code_, bool Safe_) const
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
         (file_sizel<sizeof(unsigned))||    // короче маркера конца списка быть не может
         (file_sizel>=MAX_SLFileSize) ) goto error;
    // Определяем размер данных для добавления к файлу, его допустимость
    // и не превысит ли ограничение увеличившийся файл (иначе потом не откроем его)
    data_size=GetAllDataSize(false);
    if ( data_size>MAX_SLFileSize )
    {
        throw std::runtime_error (      /// заменить на return false ?
            "MSLList::AttachTo()\n"
            "Размер данных превышает ограничение MAX_SLFileSize."
            );
    }
    if ( (data_size+file_sizel)>MAX_SLFileSize ) goto error;

    read_size=file_sizel;
    // Ограничиваем область чтения двойным размером
    // блока шифрования (особенность алгоритма)
    if ( read_size>(sizeof(unsigned)*2) ) read_size=sizeof(unsigned)*2;
    // Определяем общий размер данных для записи
    data_size+=read_size-sizeof(unsigned);
    // Выделяем память под новые данные и часть старых из файла
    try { all_data.resize(data_size); }
    catch(std::bad_alloc &e)
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
    if ( ((unsigned*)&all_data[read_size])[-1]!=0 ) goto error;

    // Сохраняем список без заголовка в памяти и сверяем реальный размер данных
	if ( SetAllData(&all_data[read_size-sizeof(unsigned)],false) !=
		&all_data[all_data.size()] )
	{
        throw std::runtime_error (
            "MSLList::AttachTo()\n"
            "Размер данных MSLList::SetAllData() не соответствует MSLList::GetAllDataSize()."
            );
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
bool MSLList::LoadFrom(const std::wstring &File_, unsigned Code_)
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
    catch(std::bad_alloc &e)
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
bool MSLList::StoreTo(HKEY Key_, const std::wstring &SubKey_,
	const std::wstring &Value_, unsigned Code_) const
{
    HKEY key=nullptr;
    std::vector <BYTE> data;
    unsigned size;

    LastError=0;
    // Определяем размер данных и проверяем на допустимость
    size=GetAllDataSize();
    if ( size>MAX_SLRegSize )
    {
        throw std::runtime_error (      /// заменить на return false ?
            "MSLList::StoreTo()\n"
            "Размер данных превышает ограничение MAX_SLRegSize."
            );
    }
    // Выделяем память (bad_alloc не ловим, т.к. еще ничего не начали делать)
    data.resize(size);
    // Сохраняем весь список в памяти и сверяем реальный размер данных
	if ( SetAllData(data.data()) != &data[data.size()] )
	{
        throw std::runtime_error (
            "MSLList::StoreTo()\n"
            "Размер данных MSLList::SetAllData() не соответствует MSLList::GetAllDataSize()."
            );
    }
    // Шифруем
	BasicEncode(data.data(),data.size(),Code_);

    // Создаем ключ реестра
	if ( ::RegCreateKeyEx(Key_,SubKey_.c_str(),0,nullptr,
		REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,nullptr,
		&key,nullptr)!=ERROR_SUCCESS ) goto api_error;
	// Сохраняем параметр
	if ( ::RegSetValueEx(key,Value_.c_str(),0,REG_BINARY,
		data.data(),size)!=ERROR_SUCCESS ) goto api_error;

    ::RegCloseKey(key);
    return true;
api_error:
    LastError=::GetLastError();
    if ( key!=nullptr ) ::RegCloseKey(key);
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::QueryFrom(HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, unsigned Code_)
{
    HKEY key=nullptr;
    DWORD size;
	std::vector <BYTE> data;

	LastError=0;
	// Открываем ключ реестра
	if ( ::RegOpenKeyEx(Key_,SubKey_.c_str(),
		0,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS ) goto api_error;
	// Сразу выделяем память под данные (для реестра буфер маленький)
	try { data.resize(MAX_SLRegSize); }
	catch(std::bad_alloc &e)
	{
		// Закрываем ключ реестра и передаем исключение выше
		::RegCloseKey(key); key=nullptr;
		throw e;
	}
	// Считываем сколько есть по-факту данных
	size=MAX_SLRegSize;
	if ( ::RegQueryValueEx(key,Value_.c_str(),
		nullptr,nullptr,data.data(),&size)!=ERROR_SUCCESS ) goto api_error;
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
bool MSLList::SaveAsReg(
	const wchar_t *File_,
	HKEY Key_,
	const wchar_t *SubKey_,
	const wchar_t *Value_,
    unsigned Code_) const
{
    static const char hdr[]="REGEDIT4";
    static const char hk1[]="HKEY_LOCAL_MACHINE";
    static const char hk2[]="HKEY_CURRENT_USER";
    static const char hk3[]="HKEY_USERS";
    static const char hk4[]="HKEY_CLASSES_ROOT";
    const char *hk;    std::vector <char> data;    std::vector <wchar_t> reg_data;    DWORD size, reg_size, rw_size;    HANDLE file;    LastError=0;	if ( Key_==HKEY_LOCAL_MACHINE ) hk=hk1;	else if ( Key_==HKEY_CURRENT_USER ) hk=hk2;	else if ( Key_==HKEY_USERS ) hk=hk3;	else if ( Key_==HKEY_CLASSES_ROOT ) hk=hk4;	else		throw std::runtime_error (			"MSLList::SaveAsReg()\n"
			"Задан не верный тип HKEY."
			);

    // Определяем размер данных и проверяем на допустимость
    size=GetAllDataSize();
    if ( size>MAX_SLRegSize )
    {
        throw std::runtime_error (      /// заменить на return false ?
            "MSLList::SaveAsReg()\n"
            "Размер данных превышает ограничение MAX_SLRegSize."
            );
    }
    // Выделяем память (bad_alloc не ловим, т.к. еще ничего не начали делать)
    data.resize(size);
    reg_size=
        sizeof(hdr)+
        sizeof(hk1)+
        +16+1+
        size*3+
        wcslen(SubKey_)+
        wcslen(Value_);
    reg_data.resize(reg_size);
    // Сохраняем весь список в памяти и сверяем реальный размер данных
	if ( SetAllData(data.data()) != (data.data()+data.size()) )
    {
        throw std::runtime_error (
            "MSLList::SaveAsReg()\n"
            "Размер данных MSLList::SetAllData() не соответствует MSLList::GetAllDataSize()."
            );
    }
    // Шифруем
	BasicEncode(data.data(),data.size(),Code_);

    // Формируем текстовую часть reg-файла
	reg_size=swprintf(
		reg_data.data(), reg_data.size(),
		L"%s\r\n\r\n[%s\\%s]\r\n\"%s\"=hex:",
		hdr, hk, SubKey_, Value_);
    // Добавляем HEX-строку
    reg_size+=ByteToHEX(data.data(),size,&reg_data[reg_size],size*3,L',');
	reg_size+=swprintf(
		&reg_data[reg_size],
		reg_data.size()-reg_size,
		L"\r\n");

    // Создаем файл
	if ( (file=::CreateFile(File_,GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,nullptr)) == INVALID_HANDLE_VALUE ) goto api_error;
	// Записываем данные
	if ( (!::WriteFile(file,reg_data.data(),reg_size,&rw_size,nullptr))||
		(rw_size!=reg_size) ) goto api_error;

	::CloseHandle(file);
	return true;
api_error:
	LastError=::GetLastError();
	if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
	return false;
}
//---------------------------------------------------------------------------
void MSLList::SetDefaultFile(const std::wstring &File_, unsigned Code_)
{
    DefaultFile=File_;
    DefaultValue.clear();
    DefaultCode=Code_;
}
//---------------------------------------------------------------------------
void MSLList::SetDefaultKey(HKEY Key_, const std::wstring &SubKey_, const std::wstring &Value_, unsigned Code_)
{
    DefaultFile=SubKey_;
    DefaultValue=Value_;
    DefaultKey=Key_;
    DefaultCode=Code_;
}
//---------------------------------------------------------------------------
bool MSLList::Save(bool Always_, bool Safe_) const
{
    if ( DefaultValue.empty() )
        return DefaultFile.empty()? false:
        SaveTo(DefaultFile,DefaultCode,Always_,Safe_);
    else
        return DefaultFile.empty()? false:
        StoreTo(DefaultKey,DefaultFile,DefaultValue,DefaultCode);
}
//---------------------------------------------------------------------------
bool MSLList::Attach(bool Safe_) const
{
    return (DefaultFile.empty())||(!DefaultValue.empty())?
        false: AttachTo(DefaultFile,DefaultCode,Safe_);
}
//---------------------------------------------------------------------------
bool MSLList::Load()
{
    if ( DefaultValue.empty() )
        return DefaultFile.empty()?
            false: LoadFrom(DefaultFile,DefaultCode);
    else
        return DefaultFile.empty()?
            false: QueryFrom(DefaultKey,DefaultFile,DefaultValue,DefaultCode);
}
//---------------------------------------------------------------------------

