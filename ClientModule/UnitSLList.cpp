//---------------------------------------------------------------------------
#include <windows.h>
#pragma hdrstop

#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void MSLList::constructor()
{
    MList::constructor();
    DefaultKey=NULL;
    DefaultFile=NULL;
    DefaultValue=NULL;
    DefaultCode=0;
}
//---------------------------------------------------------------------------
void MSLList::destructor()
{
    delete[] DefaultFile;
    delete[] DefaultValue;
    MList::destructor();
}
//---------------------------------------------------------------------------
unsigned MSLList::GetAllDataSize(bool Header_)
{
    unsigned DataSize=0, Size;
    // Берем размер заголовка
    if ( Header_ ) if ( (Size=GetDataSize())!=0 ) DataSize+=sizeof(unsigned)+Size;
    // Берем размер элементов списка (игнорируя элементы с нулевым размером данных)
    for ( MSLListItem *Item=(MSLListItem*)First;
        Item; Item=(MSLListItem*)Item->Next )
    {
        Size=Item->GetDataSize(); if ( Size==0 ) continue;
        DataSize+=sizeof(unsigned)+Size;
        // Если список типизированный, добавляем размер поля типа элемента
        if ( Typed() ) DataSize+=sizeof(unsigned);
    }
    // Маркер конца списка
    DataSize+=sizeof(unsigned);
    return DataSize;
}
//---------------------------------------------------------------------------
char *MSLList::SetAllData(char *Data_, bool Header_)
{
    char *NextData;
    unsigned BlockSize;

    if ( Header_ )
    {
        // Оставляем поле для размера данных заголовка
        Data_+=sizeof(unsigned);
        // Сохраняем данные заголовка и вычисляем его реальный размер
        NextData=SetData(Data_); BlockSize=(unsigned)(NextData-Data_);
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

    for ( MSLListItem *Item=(MSLListItem*)First;
        Item; Item=(MSLListItem*)Item->Next )
    {
        // Оставляем поле для размера данных блока
        Data_+=sizeof(unsigned);
        // Если список типизированный, то оставляем поле для типа элемента
        NextData=Typed()? Data_+sizeof(unsigned): Data_;
        // Сохраняем данные блока и вычисляем его реальный размер (включая поле типа)
        NextData=Item->SetData(NextData); BlockSize=(unsigned)(NextData-Data_);
        // Проверяем нужно ли было сохранять блок (были ли записаны данные)
        if ( BlockSize!=0 )
        {
            // Сохраняем размер данных блока
            Data_=MemSet(Data_-sizeof(unsigned),BlockSize);
            // Если список типизированный, сохраняем тип элемента
            if ( Typed() ) MemSet(Data_,Item->TypeID());
            // Переходим далее
            Data_=NextData;
        } else Data_-=sizeof(unsigned);
    }
    // Маркер конца списка
    Data_=MemSet(Data_,(unsigned)0);

    return Data_;
}
//---------------------------------------------------------------------------
char *MSLList::GetAllData(char *Data_, char *Limit_)
{
    char *Limit;
    unsigned DataSize, Type;
    MSLListItem *Item;

    // Берем размер заголовка
    Data_=MemGet(Data_,&DataSize,Limit_); if ( Data_==NULL ) return NULL;
    // Вычисляем лимит для операций с памятью внутри этого блока
    Limit=Data_+DataSize; if ( Limit>Limit_ ) return NULL;
    // Берем данные
    {
        char *result=GetData(Data_,Limit);
        if ( Data_==result ) Data_-=sizeof(unsigned);
        else if ( result!=Limit ) return NULL;
        else Data_=result;
    }

    while(true)
    {
        // Берем размер блока
        Data_=MemGet(Data_,&DataSize,Limit_); if ( Data_==NULL ) return NULL;
        // Если размер блока равен нулю (маркер конца списка), то прекращаем заполнение списка
        if ( DataSize==0 ) break;
        // Вычисляем лимит для операций с памятью внутри этого блока
        Limit=Data_+DataSize; if ( Limit>Limit_ ) return NULL;
        // Если список типизированный, берем тип элемента
        if ( Typed() )
            { Data_=MemGet(Data_,&Type,Limit_); if ( Data_==NULL ) return NULL; }
        else Type=0;
        // Добавляем новый элемент в список
        Item=(MSLListItem*)Add(Type); if ( Item==NULL ) return NULL;
        // Берем данные
        Data_=Item->GetData(Data_,Limit); if ( Data_!=Limit ) return NULL;
    }

    return Data_;
}
//---------------------------------------------------------------------------
bool MSLList::SaveTo(char *File_, unsigned Code_)
{
    HANDLE file=INVALID_HANDLE_VALUE;
    DWORD data_size, rw_size;
    char *all_data;

    // Определяем размер данных для сохранения
    data_size=GetAllDataSize(true);
    // Выделяем память под данные
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // Сохраняем данные в памяти
    if ( SetAllData(all_data,true)!=(all_data+data_size) ) goto error;
    // Шифруем данные
    BasicCode(all_data,data_size,Code_);

    // Создаем файл для сохранения
    if ( (file=::CreateFile(File_,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Сохраняем данные в файле
    if ( (!::WriteFile(file,all_data,data_size,&rw_size,NULL))||
        (rw_size!=data_size) ) goto error;

    ::CloseHandle(file);
    delete[] all_data;
    return true;
error:
    if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::AttachTo(char *File_, unsigned Code_)
{
    HANDLE file;
    DWORD old_size, data_size, rw_size;
    char *all_data=NULL;

    // Открываем файл
    if ( (file=::CreateFile(File_,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Опеределяем размер файла
    if ( (old_size=::SetFilePointer(file,0,NULL,FILE_END))==0xFFFFFFFF ) goto error;
    // Проверяем не короче ли файл, чем допустимо
    if ( old_size<sizeof(unsigned) ) goto error;
    //
    if ( old_size>(sizeof(unsigned)*2) ) old_size=sizeof(unsigned)*2;

    // Определяем общий размер данных для сохранения (без заголовка списка)
    data_size=GetAllDataSize(false)+old_size-sizeof(unsigned);
    // Выделяем память под новые данные и часть старых из файла
    if ( (all_data=new char[data_size])==NULL ) goto error;

    //
    if ( ::SetFilePointer(file,-(LONG)old_size,NULL,FILE_END)==0xFFFFFFFF ) goto error;
    // Читаем часть старых данных из файла
    if ( (!::ReadFile(file,all_data,old_size,&rw_size,NULL))||
        (rw_size!=old_size) ) goto error;
    // Расшифровываем их
    BasicEncode(all_data,old_size,Code_);
    // Проверяем есть ли в данных маркер конца списка (0x00000000)
    if ( ((unsigned*)(all_data+old_size))[-1]!=0 ) goto error;

    // Сохраняем новые данные в памяти (без заголовка списка)
    if ( SetAllData(all_data+old_size-sizeof(unsigned),false)!=
        (all_data+data_size) ) goto error;
    // Шифруем данные
    BasicCode(all_data,data_size,Code_);

    // Переводим указатель в конец файла
    if ( ::SetFilePointer(file,-(LONG)old_size,NULL,FILE_END)==0xFFFFFFFF ) goto error;
    // Сохраняем данные в файле
    if ( (!::WriteFile(file,all_data,data_size,&rw_size,NULL))||
        (rw_size!=data_size) ) goto error;

    ::CloseHandle(file);
    delete[] all_data;
    return true;
error:
    if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::LoadFrom(char *File_, unsigned Code_)
{
    HANDLE file;
    DWORD data_size, rw_size;
    char *all_data=NULL, *end_data;

    // Открываем файл
    if ( (file=::CreateFile(File_,GENERIC_READ,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Определяем его размер
    if ( (data_size=::GetFileSize(file,NULL))==0xFFFFFFFF ) goto error;
    // Выделяем память под данные
    if ( (all_data=new char[data_size])==NULL ) goto error;
    end_data=all_data+data_size;
    // Считываем данные из файла
    if ( (!::ReadFile(file,all_data,data_size,&rw_size,NULL))||
        (rw_size!=data_size) ) goto error;
    ::CloseHandle(file); file=NULL;

    // Расшифровываем данные
    BasicEncode(all_data,data_size,Code_);
    // Заносим данные в список
    if ( GetAllData(all_data,end_data)!=end_data ) goto error;

    delete[] all_data;
    return true;
error:
    if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::StoreTo(HKEY Key_, char *SubKey_, char *Value_, unsigned Code_)
{
    HKEY key=NULL;
    unsigned size;
    char *data;

    size=GetAllDataSize(true);
    if ( (data=new char[size])==NULL ) goto error;
    SetAllData(data,true);
    BasicCode(data,size,Code_);

    // Создаем ключ реестра
    if ( ::RegCreateKeyEx(Key_,SubKey_,NULL,NULL,REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,NULL,&key,NULL)!=ERROR_SUCCESS ) goto error;
    // Сохраняем параметр
    if ( ::RegSetValueEx(key,Value_,NULL,REG_BINARY,data,size)!=ERROR_SUCCESS )
        goto error;

    ::RegCloseKey(key);
    delete[] data;
    return true;
error:
    if ( key!=NULL ) ::RegCloseKey(key);
    delete[] data;
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::QueryFrom(HKEY Key_, char *SubKey_, char *Value_, unsigned Code_)
{
    HKEY key=NULL;
    unsigned long size;
    char *data=NULL;

    // Открываем раздел реестра
    if ( ::RegOpenKeyEx(Key_,SubKey_,NULL,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS ) goto error;
    // Определяем размер значения
    if ( ::RegQueryValueEx(key,Value_,NULL,NULL,NULL,&size)!=ERROR_SUCCESS ) goto error;
    // Выделяем память под данные
    if ( (data=new char[size])==NULL ) goto error;
    // Считываем данные
    if ( ::RegQueryValueEx(key,Value_,NULL,NULL,data,&size)!=ERROR_SUCCESS ) goto error;
    ::RegCloseKey(key); key=NULL;

    BasicEncode(data,size,Code_);
    if ( GetAllData(data,data+size)==NULL ) goto error;

    delete[] data;
    return true;
error:
    if ( key!=NULL ) ::RegCloseKey(key);
    delete[] data;
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::SetDefaultFile(char *File_, unsigned Code_)
{
    delete[] DefaultFile;
    DefaultFile=new char[strlen(File_)+1];
    if ( DefaultFile==NULL ) return false;
    strcpy(DefaultFile,File_);
    DefaultCode=Code_;
    return true;
}
//---------------------------------------------------------------------------
bool MSLList::SetDefaultKey(HKEY Key_, char *SubKey_, char *Value_, unsigned Code_)
{
    DefaultKey=Key_;
    delete[] DefaultValue;
    DefaultValue=new char[strlen(Value_)+1];
    if ( DefaultValue==NULL ) return false;
    strcpy(DefaultValue,Value_);
    return SetDefaultFile(SubKey_,Code_);
}
//---------------------------------------------------------------------------
bool MSLList::Save()
{
    return DefaultFile==NULL? false: SaveTo(DefaultFile,DefaultCode);
}
//---------------------------------------------------------------------------
bool MSLList::Attach()
{
    return DefaultFile==NULL? false: AttachTo(DefaultFile,DefaultCode);
}
//---------------------------------------------------------------------------
bool MSLList::Load()
{
    return DefaultFile==NULL? false: LoadFrom(DefaultFile,DefaultCode);
}
//---------------------------------------------------------------------------
bool MSLList::Store()
{
    return (DefaultFile==NULL)||(DefaultValue==NULL)? false:
        StoreTo(DefaultKey,DefaultFile,DefaultValue,DefaultCode);
}
//---------------------------------------------------------------------------
bool MSLList::Query()
{
    return (DefaultFile==NULL)||(DefaultValue==NULL)? false:
        QueryFrom(DefaultKey,DefaultFile,DefaultValue,DefaultCode);
}
//---------------------------------------------------------------------------

