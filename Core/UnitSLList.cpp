//---------------------------------------------------------------------------
#include <vector>
#include <stdio.h>
#include <stdexcept.h>
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
    bool typed=Typed();

    // ����� ������ ���������
    if ( Header_&&((Size=GetDataSize())!=0) ) DataSize+=sizeof(unsigned)+Size;
    // ����� ������ ��������� ������ (��������� �������� � ������� �������� ������)
    for ( MSLListItem *Item=(MSLListItem*)gFirst();
        Item; Item=(MSLListItem*)Item->gNext() )
    {
        Size=Item->GetDataSize(); if ( Size==0 ) continue;
        // �� �������� �������� ���� ��� ������� ������ ��������
        DataSize+=Size+sizeof(unsigned);
        // ���� ������ ��������������, ��������� ������ ���� ����
        if ( typed ) DataSize+=sizeof(unsigned char);
    }
    // ������ ����� ������
    DataSize+=sizeof(unsigned);
    return DataSize;
}
//---------------------------------------------------------------------------
char *MSLList::SetAllData(char *Data_, bool Header_) const
{
    const bool typed=Typed();
    unsigned BlockSize;
    char *NextData;

    if ( Header_ )
    {
        // ��������� ���� ��� ������� ������ ���������
        Data_+=sizeof(unsigned);
        // ��������� ������ ��������� � ��������� ��� �������� ������
        NextData=SetData(Data_);
        BlockSize=(unsigned)(NextData-Data_);
        // ��������� ����� �� ���� ��������� ��������� (���� �� �������� ������)
        if ( BlockSize==0 ) Data_-=sizeof(unsigned);
        else
        {
            // ��������� ������ ���������
            MemSet(Data_-sizeof(unsigned),BlockSize);
            // ��������� �����
            Data_=NextData;
        }
    }

    for ( MSLListItem *Item=(MSLListItem*)gFirst();
        Item; Item=(MSLListItem*)Item->gNext() )
    {
        // ��������� ���� ��� ������� ������ �����
        Data_+=sizeof(unsigned);
        // ���� ������ ��������������, �� ��������� ���� ��� ���� ��������
        NextData=typed? Data_+sizeof(unsigned char): Data_;
        // ��������� ������ ����� � ��������� ��� �������� ������ (������� ���� ����)
        NextData=Item->SetData(NextData);
        BlockSize=(unsigned)(NextData-Data_);
        // ��������� ����� �� ���� ��������� ���� (���� �� �������� ������)
        if ( BlockSize!=0 )
        {
            // ��������� ������ ������ �����
            Data_=MemSet(Data_-sizeof(unsigned),BlockSize);
            // ���� ������ ��������������, ��������� ��� ��������
            if ( typed ) MemSet(Data_,Item->gTypeID());
            // ��������� �����
            Data_=NextData;
        } else Data_-=sizeof(unsigned);
    }
    // ������ ����� ������
    Data_=MemSet(Data_,(unsigned)0);

    return Data_;
}
//---------------------------------------------------------------------------
const char *MSLList::GetAllData(const char *Data_, const char *Limit_)
{
    const bool typed=Typed();
    const char *Limit;
    unsigned DataSize;
    unsigned char TypeID;
    MSLListItem *Item;

    // ������� ������ ��� ������������
    Clear();
                                    /// ��������, ����� ��������� bad_alloc
    // ����� ������ ����� �� ��������� ������
    Data_=MemGet(Data_,&DataSize,Limit_);
    if ( Data_==NULL ) goto error;
    // ��������� ����� ��� �������� � ������� ������ ����� �����
    Limit=Data_+DataSize;
    // �������� ����� �� ������������� ������� � �� ������������ ���������
    if ( (Limit>Limit_)||(Limit<Data_) ) goto error;
    // ����� ������
    {
        const char *result=GetData(Data_,Limit);
        // ���� ������ �� �������, ������ � ������ ��� ���������
        if ( Data_==result ) Data_-=sizeof(unsigned);
        else if ( result!=Limit ) goto error;
        else Data_=result;
    }

    while(true)
    {
        // ����� ������ �����
        Data_=MemGet(Data_,&DataSize,Limit_);
        if ( Data_==NULL ) goto error;
        // ���� ������ ����� ����� ���� (������ ����� ������), �� ���������� ���������� ������
        if ( DataSize==0 ) break;
        // ��������� ����� ��� �������� � ������� ������ ����� �����
        Limit=Data_+DataSize;
        // �������� ����� �� ������������� ������� � �� ������������ ���������
        if ( (Limit>Limit_)||(Limit<Data_) ) goto error;
        // ���� ������ ��������������, ����� ��� ��������
        if ( typed )
        {
            Data_=MemGet(Data_,&TypeID,Limit_);
            if ( Data_==NULL ) goto error;
        } else TypeID=0;
        // ��������� ����� ������� � ������
        Item=(MSLListItem*)Add(TypeID);
        // ����� ������
        Data_=Item->GetData(Data_,Limit);
        if ( Data_!=Limit ) goto error;
    }

    return Data_;
error:
    // ������� �������� ����������� ������
    Clear();
    return NULL;
}
//---------------------------------------------------------------------------
bool MSLList::SaveTo(const std::string &File_, unsigned Code_, bool Always_, bool Safe_) const
{
    HANDLE file=INVALID_HANDLE_VALUE;
    DWORD data_size, rw_size;
    std::vector <char> all_data;

    LastError=0;
    // ���������� ������ ������ � ��������� �� ������������
    data_size=GetAllDataSize();
    if ( data_size>MAX_SLFileSize )
    {
        throw std::runtime_error (      /// �������� �� return false ?
            "MSLList::SaveTo()\n"
            "������ ������ ��������� ����������� MAX_SLFileSize."
            );
    }
    // �������� ������ ��� ������.
    // bad_alloc �� �����, �.�. ������ ��� �� ������ ������
    all_data.resize(data_size);
    // ��������� ����� ������ � ������ � ������� �������� ������ ������
    if ( SetAllData(all_data.begin())!=all_data.end() )
    {
        throw std::runtime_error (
            "MSLList::SaveTo()\n"
            "������ ������ MSLList::SetAllData() �� ������������� MSLList::GetAllDataSize()."
            );
    }
    // ������� ������
    BasicEncode(all_data.begin(),data_size,Code_);

    // ������� ����
    if ( (file=::CreateFile(File_.c_str(),GENERIC_WRITE,0,NULL,
        Always_? (Safe_?OPEN_ALWAYS:CREATE_ALWAYS) :CREATE_NEW,
        Safe_?FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH:FILE_ATTRIBUTE_NORMAL,
        NULL))==INVALID_HANDLE_VALUE ) goto api_error;

    if ( Always_&&Safe_ )
    {
        // ������ ����� ������ �����
        if ( ::SetFilePointer(file,(LONG)data_size,NULL,FILE_BEGIN)==0xFFFFFFFF ) goto api_error;
        if ( !SetEndOfFile(file) ) goto api_error;
        // ���������� ������� � ������
        if ( ::SetFilePointer(file,(LONG)0,NULL,FILE_BEGIN)==0xFFFFFFFF ) goto api_error;
    }
    // ���������� ������
    if ( (!::WriteFile(file,all_data.begin(),data_size,&rw_size,NULL))||
        (rw_size!=data_size) ) goto api_error;

    ::CloseHandle(file);
    return true;
api_error:
    LastError=::GetLastError();
    if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::AttachTo(const std::string &File_, unsigned Code_, bool Safe_) const
{
    HANDLE file=INVALID_HANDLE_VALUE;
    DWORD file_sizel, file_sizeh;
    DWORD read_size, data_size, rw_size;
    std::vector <char> all_data;

    LastError=0;
    // ��������� ����
    if ( (file=::CreateFile(File_.c_str(),GENERIC_READ|GENERIC_WRITE,0,NULL,
        OPEN_EXISTING,
        Safe_?FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH:FILE_ATTRIBUTE_NORMAL,
        NULL))==INVALID_HANDLE_VALUE ) goto api_error;
    // ���������� ������ ����� � ��������� �� ������������
    if ( (file_sizel=::GetFileSize(file,&file_sizeh))==0xFFFFFFFF ) goto api_error;
    if ( (file_sizeh!=0)||
         (file_sizel<sizeof(unsigned))||    // ������ ������� ����� ������ ���� �� �����
         (file_sizel>=MAX_SLFileSize) ) goto error;
    // ���������� ������ ������ ��� ���������� � �����, ��� ������������
    // � �� �������� �� ����������� ������������� ���� (����� ����� �� ������� ���)
    data_size=GetAllDataSize(false);
    if ( data_size>MAX_SLFileSize )
    {
        throw std::runtime_error (      /// �������� �� return false ?
            "MSLList::AttachTo()\n"
            "������ ������ ��������� ����������� MAX_SLFileSize."
            );
    }
    if ( (data_size+file_sizel)>MAX_SLFileSize ) goto error;

    read_size=file_sizel;
    // ������������ ������� ������ ������� ��������
    // ����� ���������� (����������� ���������)
    if ( read_size>(sizeof(unsigned)*2) ) read_size=sizeof(unsigned)*2;
    // ���������� ����� ������ ������ ��� ������
    data_size+=read_size-sizeof(unsigned);
    // �������� ������ ��� ����� ������ � ����� ������ �� �����
    try { all_data.resize(data_size); }
    catch(std::bad_alloc &e)
    {
        // ��������� ���� � �������� ���������� ����
        ::CloseHandle(file);
        throw e;
    }

    // ������ ����� ������ ������ �� �����
    if ( (::SetFilePointer(file,-(LONG)read_size,NULL,FILE_END)==0xFFFFFFFF)||
        (!::ReadFile(file,all_data.begin(),read_size,&rw_size,NULL))||
        (rw_size!=read_size) ) goto api_error;  /// ������ ��������� rw_size ?
    // �������������� �� � ��������� ������� ������� ����� ������ (0x00000000)
    BasicDecode(all_data.begin(),read_size,Code_);
    if ( ((unsigned*)&all_data[read_size])[-1]!=0 ) goto error;

    // ��������� ������ ��� ��������� � ������ � ������� �������� ������ ������
    if ( SetAllData(&all_data[read_size-sizeof(unsigned)],false)!=
        all_data.end() )
    {
        throw std::runtime_error (
            "MSLList::AttachTo()\n"
            "������ ������ MSLList::SetAllData() �� ������������� MSLList::GetAllDataSize()."
            );
    }

    // �������
    BasicEncode(all_data.begin(),data_size,Code_);

    if ( Safe_ )
    {
        // ����������� ����� � �����
        if ( (::SetFilePointer(file,(LONG)(data_size-read_size),NULL,FILE_END)==0xFFFFFFFF)||
            (!SetEndOfFile(file)) ) goto api_error;
        // ��������� �� ����� ����� ���, ����� ������������ ����� ������ ������
        if ( ::SetFilePointer(file,-(LONG)data_size,NULL,FILE_END)==0xFFFFFFFF ) goto api_error;
    } else
    {
        // ��������� �� ����� ����� ���, ����� ������������ ����� ������ ������
        if ( ::SetFilePointer(file,-(LONG)read_size,NULL,FILE_END)==0xFFFFFFFF ) goto api_error;
    }
    // ��������� ������ � �����
    if ( (!::WriteFile(file,all_data.begin(),data_size,&rw_size,NULL))||
        (rw_size!=data_size) ) goto api_error;    /// ������ ��������� rw_size ?

    ::CloseHandle(file);
    return true;
api_error:
    LastError=::GetLastError();
error:
    if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::LoadFrom(const std::string &File_, unsigned Code_)
{
    HANDLE file=INVALID_HANDLE_VALUE;
    DWORD file_sizel, file_sizeh, rw_size;
    std::vector <char> all_data;

    LastError=0;
    // ��������� ����
    if ( (file=::CreateFile(File_.c_str(),GENERIC_READ,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto api_error;
    // ���������� ��� ������ � ��������� �� ������������
    if ( (file_sizel=::GetFileSize(file,&file_sizeh))==0xFFFFFFFF ) goto api_error;
    if ( (file_sizeh!=0)||
         (file_sizel>MAX_SLFileSize) ) goto error;
    // �������� ������ ��� ������
    try { all_data.resize(file_sizel); }
    catch(std::bad_alloc &e)
    {
        // ��������� ���� � �������� ���������� ����
        ::CloseHandle(file);
        throw e;
    }
    // ��������� ������ �� �����
    if ( (!::ReadFile(file,all_data.begin(),file_sizel,&rw_size,NULL))||
        (rw_size!=file_sizel) ) goto api_error;
    // ��������� ���
    ::CloseHandle(file); file=INVALID_HANDLE_VALUE;

    // ��������������
    BasicDecode(all_data.begin(),file_sizel,Code_);
    // ������� � ������ (bad_alloc �� �����, �.�. ��� ���������)
    if ( GetAllData(all_data.begin(),all_data.end())!=all_data.end() ) goto error;

    return true;
api_error:
    LastError=::GetLastError();
error:
    if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::StoreTo(HKEY Key_, const std::string &SubKey_, const std::string &Value_, unsigned Code_) const
{
    HKEY key=NULL;
    std::vector <char> data;
    unsigned size;

    LastError=0;
    // ���������� ������ ������ � ��������� �� ������������
    size=GetAllDataSize();
    if ( size>MAX_SLRegSize )
    {
        throw std::runtime_error (      /// �������� �� return false ?
            "MSLList::StoreTo()\n"
            "������ ������ ��������� ����������� MAX_SLRegSize."
            );
    }
    // �������� ������ (bad_alloc �� �����, �.�. ��� ������ �� ������ ������)
    data.resize(size);
    // ��������� ���� ������ � ������ � ������� �������� ������ ������
    if ( SetAllData(data.begin())!=data.end() )
    {
        throw std::runtime_error (
            "MSLList::StoreTo()\n"
            "������ ������ MSLList::SetAllData() �� ������������� MSLList::GetAllDataSize()."
            );
    }
    // �������
    BasicEncode(data.begin(),size,Code_);

    // ������� ���� �������
    if ( ::RegCreateKeyEx(Key_,SubKey_.c_str(),NULL,NULL,REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,NULL,&key,NULL)!=ERROR_SUCCESS ) goto api_error;
    // ��������� ��������
    if ( ::RegSetValueEx(key,Value_.c_str(),NULL,
        REG_BINARY,data.begin(),size)!=ERROR_SUCCESS ) goto api_error;

    ::RegCloseKey(key);
    return true;
api_error:
    LastError=::GetLastError();
    if ( key!=NULL ) ::RegCloseKey(key);
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::QueryFrom(HKEY Key_, const std::string &SubKey_, const std::string &Value_, unsigned Code_)
{
    HKEY key=NULL;
    DWORD size;
    std::vector <char> data;

    LastError=0;
    // ��������� ���� �������
    if ( ::RegOpenKeyEx(Key_,SubKey_.c_str(),
        NULL,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS ) goto api_error;
    // ����� �������� ������ ��� ������ (��� ������� ����� ���������)
    try { data.resize(MAX_SLRegSize); }
    catch(std::bad_alloc &e)
    {
        // ��������� ���� ������� � �������� ���������� ����
        ::RegCloseKey(key); key=NULL;
        throw e;
    }
    // ��������� ������� ���� ��-����� ������
    size=MAX_SLRegSize;
    if ( ::RegQueryValueEx(key,Value_.c_str(),
        NULL,NULL,data.begin(),&size)!=ERROR_SUCCESS ) goto api_error;
    // ��������� ���� �������
    ::RegCloseKey(key); key=NULL;

    // �������������� ������
    BasicDecode(data.begin(),size,Code_);
    // ������� ������ � ������ (bad_alloc �� �����, �.�. ��� ���������)
    if ( GetAllData(&data[0],&data[size])==NULL ) goto error;

    return true;
api_error:
    LastError=::GetLastError();
error:
    if ( key!=NULL ) ::RegCloseKey(key);
    return false;
}
//---------------------------------------------------------------------------
bool MSLList::SaveAsReg(
    const char *File_,
    HKEY Key_,
    const char *SubKey_,
    const char *Value_,
    unsigned Code_) const
{
    static const char hdr[]="REGEDIT4";
    static const char hk1[]="HKEY_LOCAL_MACHINE";
    static const char hk2[]="HKEY_CURRENT_USER";
    static const char hk3[]="HKEY_USERS";
    static const char hk4[]="HKEY_CLASSES_ROOT";
    const char *hk;    std::vector <char> data;    std::vector <char> reg_data;    DWORD size, reg_size, rw_size;    HANDLE file;    LastError=0;    switch((int)Key_)    {        case HKEY_LOCAL_MACHINE: hk=hk1; break;        case HKEY_CURRENT_USER: hk=hk2; break;        case HKEY_USERS: hk=hk3; break;        case HKEY_CLASSES_ROOT: hk=hk4; break;        default:            throw std::runtime_error (                "MSLList::SaveAsReg()\n"
                "����� �� ������ ��� HKEY."
                );
    }

    // ���������� ������ ������ � ��������� �� ������������
    size=GetAllDataSize();
    if ( size>MAX_SLRegSize )
    {
        throw std::runtime_error (      /// �������� �� return false ?
            "MSLList::SaveAsReg()\n"
            "������ ������ ��������� ����������� MAX_SLRegSize."
            );
    }
    // �������� ������ (bad_alloc �� �����, �.�. ��� ������ �� ������ ������)
    data.resize(size);
    reg_size=
        sizeof(hdr)+
        sizeof(hk1)+
        +16+1+
        size*3+
        strlen(SubKey_)+
        strlen(Value_);
    reg_data.resize(reg_size);
    // ��������� ���� ������ � ������ � ������� �������� ������ ������
    if ( SetAllData(data.begin())!=data.end() )
    {
        throw std::runtime_error (
            "MSLList::SaveAsReg()\n"
            "������ ������ MSLList::SetAllData() �� ������������� MSLList::GetAllDataSize()."
            );
    }
    // �������
    BasicEncode(data.begin(),size,Code_);

    // ��������� ��������� ����� reg-�����
    reg_size=sprintf(reg_data.begin(),"%s\r\n\r\n[%s\\%s]\r\n\"%s\"=hex:",hdr,hk,SubKey_,Value_);
    // ��������� HEX-������
    reg_size+=ByteToHEX(data.begin(),size,&reg_data[reg_size],size*3,',');
    reg_size+=sprintf(&reg_data[reg_size],"\r\n");

    // ������� ����
    if ( (file=::CreateFile(File_,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto api_error;
    // ���������� ������
    if ( (!::WriteFile(file,reg_data.begin(),reg_size,&rw_size,NULL))||
        (rw_size!=reg_size) ) goto api_error;

    ::CloseHandle(file);
    return true;
api_error:
    LastError=::GetLastError();
    if ( file!=INVALID_HANDLE_VALUE ) ::CloseHandle(file);
    return false;
}
//---------------------------------------------------------------------------
void MSLList::SetDefaultFile(const std::string &File_, unsigned Code_)
{
    DefaultFile=File_;
    DefaultValue.clear();
    DefaultCode=Code_;
}
//---------------------------------------------------------------------------
void MSLList::SetDefaultKey(HKEY Key_, const std::string &SubKey_, const std::string &Value_, unsigned Code_)
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

