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
#define MAX_SLFileSize  0x02000000  // ����������� �� ������ ������ - 32�����
#define MAX_SLRegSize   0x00000800  // ����������� �� ������ ������ ������� - 2�����
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
	typename list_type,			// ��� �������� ������-������
	typename base_type>			// ������� (������������) ��� ��� ���������
class MSLListItem:
	public MListItem <list_type, base_type>
{
//	friend MSLList <list_type, base_type>;

public:
	// ������� ����������� ������� ������ ������������ �� MSLListItem ������,
	// ���������� � �������������� �� �/�� ������.
	virtual std::size_t GetDataSize() const = 0;
	virtual void *SetData(void *Data_) const = 0;
	virtual const void *GetData(const void *Data_, const void *Limit_) = 0;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <
	typename list_type,			// ����������� ����� ������ (���)
	typename base_type>			// ������� ��� �������� ������
class MSLList:
	public MList <list_type, base_type>
{
public:
	// "�������" ���������� �� ������������� �������
	using MList <list_type, base_type> ::isTyped;
//	using MList <list_type, base_type> ::gFirst;
	using MList <list_type, base_type> ::Clear;
	using MList <list_type, base_type> ::Add;

public:      /// ??? ��������� MLog
	HKEY DefaultKey;			// HKCR/HKCU/HKLM/HKU
	std::wstring DefaultFile;	// ���� � ����� �� ����� / ������ �������
	std::wstring DefaultValue;	// ��� ��������� � �������
	std::uint32_t DefaultCode;	// �������� ���� ���������� ������
	mutable DWORD LastError;	// ��� ������ ��������� �������� � ������/��������

public:
	// ������� ����������� ������� ������ ������������ �� MSLList ������,
	// ���������� � �������������� �� �/�� ������.
	// ��������. MList �� ������������ ���������� ����������, � MSLList-��.
	virtual std::size_t GetDataSize() const { return 0; }
	virtual void *SetData(void *Data_) const { return Data_; }
	virtual const void *GetData(const void *Data_, const void *Limit_) { return Data_; }

public:
	// ������� ����������� ������� ������ ���� ��������� ������,
	// ���������� � �������������� �� �/�� ������.
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

	// ����� ������ ���������
	std::size_t Size=GetDataSize();
	if ( Size > std::numeric_limits<std::uint32_t>::max() )
	{
		throw std::out_of_range(
			"MSLList::GetAllDataSize()\n"
			"������ ��������� ��������� max(uint32_t).");
	}
	// �� �������� �������� ���� ��� ������� ������ ���������
	if ( Header_&&(Size!=0) ) DataSize+=sizeof(std::uint32_t)+Size;

	// ����� ������ ��������� ������ (��������� �������� � ������� �������� ������)
	bool typed=isTyped();
	for ( const auto &Item: *this )
	{
		std::size_t Size=Item.GetDataSize();
		if ( Size==0 ) continue;

		// ���� ������ ��������������, ��������� ������ ���� ����
		if ( typed ) Size+=sizeof(std::uint8_t);

		if ( Size > std::numeric_limits<std::uint32_t>::max() )
		{
			throw std::out_of_range(
				"MSLList::GetAllDataSize()\n"
				"������ �������� ��������� max(uint32_t).");
		}

		// �� �������� �������� ���� ��� ������� ������ ��������
		DataSize+=sizeof(std::uint32_t)+Size;
	}
	// ������ ����� ������
	DataSize+=sizeof(std::uint32_t);

	// ����� ������� (��� �������������), ��� ������ ������ ������������ � uint32_t
	if ( DataSize > std::numeric_limits<std::uint32_t>::max() )
	{
		throw std::out_of_range(
			"MSLList::GetAllDataSize()\n"
			"������ ������ ��������� max(uint32_t).");
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
		// ��������� ���� ��� ������� ������ ���������
		Data_+=sizeof(std::uint32_t);
		// ��������� ������ ��������� � ��������� ��� �������� ������
		NextData=static_cast<char*>(SetData(Data_));
		BlockSize=NextData-Data_;
		// ��������� ����� �� ���� ��������� ��������� (���� �� �������� ������)
		if ( BlockSize==0 ) Data_-=sizeof(std::uint32_t);
		else
		{
			// ��������� ������ ���������
			MemSet(
				Data_-sizeof(std::uint32_t),
				static_cast<std::uint32_t>(BlockSize));
			// ��������� �����
			Data_=NextData;
		}
	}

	for ( const auto &Item: *this )
	{
		// ��������� ���� ��� ������� ������ �����
		Data_+=sizeof(std::uint32_t);
		// ���� ������ ��������������, �� ��������� ���� ��� ���� ��������
		NextData=typed? Data_+sizeof(std::uint8_t): Data_;
		// ��������� ������ ����� � ��������� ��� �������� ������ (������� ���� ����)
		NextData=static_cast<char*>(Item.SetData(NextData));
		BlockSize=NextData-Data_;
		// ��������� ����� �� ���� ��������� ���� (���� �� �������� ������)
		if ( BlockSize!=0 )
		{
			// ��������� ������ ������ �����
			Data_=static_cast<char*>(MemSet(
				Data_-sizeof(std::uint32_t),
				static_cast<std::uint32_t>(BlockSize)));
			// ���� ������ ��������������, ��������� ��� ��������
			if ( typed ) MemSet(Data_,Item.gTypeID());
			// ��������� �����
			Data_=NextData;
		} else Data_-=sizeof(std::uint32_t);
	}
	// ������ ����� ������
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

	// ������� ������ ��� ������������
	Clear();
									/// ��������, ����� ��������� bad_alloc
	// ����� ������ ����� �� ��������� ������
	Data_=static_cast<const char*>(MemGet(Data_,&DataSize,Limit_));
	if ( Data_==nullptr ) goto error;
	// ��������� ����� ��� �������� � ������� ������ ����� �����
	Limit=Data_+DataSize;
	// �������� ����� �� ������������� ������� � �� ������������ ���������
	if ( (Limit>Limit_)||(Limit<Data_) ) goto error;
	// ����� ������
	{
		const char *result=static_cast<const char*>(GetData(Data_,Limit));
		// ���� ������ �� �������, ������ � ������ ��� ���������
		if ( Data_==result ) Data_-=sizeof(std::uint32_t);
		else if ( result!=Limit ) goto error;
		else Data_=result;
	}

	while(true)
	{
		// ����� ������ �����
		Data_=static_cast<const char*>(MemGet(Data_,&DataSize,Limit_));
		if ( Data_==nullptr ) goto error;
		// ���� ������ ����� ����� ���� (������ ����� ������),
		// �� ���������� ���������� ������
		if ( DataSize==0 ) break;
		// ��������� ����� ��� �������� � ������� ������ ����� �����
		Limit=Data_+DataSize;
		// �������� ����� �� ������������� ������� � �� ������������ ���������
		if ( (Limit>Limit_)||(Limit<Data_) ) goto error;
		// ���� ������ ��������������, ����� ��� ��������
		if ( typed )
		{
			Data_=static_cast<const char*>(MemGet(Data_, &TypeID, Limit_));
			if ( Data_==nullptr ) goto error;
		} else TypeID=0;
		// ��������� ����� ������� � ������
		try
		{
			base_type& Item=Add(TypeID);
			// ����� ������
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
	// ������� �������� ����������� ������
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
	// ���������� ������ ������ � ��������� �� ������������
	data_size=GetAllDataSize();         /// value limited to int32_t
	if ( data_size>MAX_SLFileSize )
    {
		throw std::runtime_error (      /// �������� �� return false ?
			"MSLList::SaveTo()\n"
			"������ ������ ��������� ����������� MAX_SLFileSize.");
    }
    // �������� ������ ��� ������.
    // bad_alloc �� �����, �.�. ������ ��� �� ������ ������
	all_data.resize(data_size);
	// ��������� ���� ������ � ������ � ������� �������� ������ ������
	if ( SetAllData(all_data.data()) != &all_data[all_data.size()] )
	{
		throw std::runtime_error (
			"MSLList::SaveTo()\n"
			"������ ������ MSLList::SetAllData() �� ������������� MSLList::GetAllDataSize().");
	}
	// ������� ������
	BasicEncode(all_data.data(),all_data.size(),Code_);

	// ������� ����
	if ( (file=::CreateFile(File_.c_str(),GENERIC_WRITE,0,nullptr,
        Always_? (Safe_?OPEN_ALWAYS:CREATE_ALWAYS) :CREATE_NEW,
        Safe_?FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH:FILE_ATTRIBUTE_NORMAL,
        nullptr))==INVALID_HANDLE_VALUE ) goto api_error;

    if ( Always_&&Safe_ )
    {
        // ������ ����� ������ �����
        if ( ::SetFilePointer(file,(LONG)data_size,nullptr,FILE_BEGIN)==0xFFFFFFFF ) goto api_error;
        if ( !SetEndOfFile(file) ) goto api_error;
        // ���������� ������� � ������
        if ( ::SetFilePointer(file,(LONG)0,nullptr,FILE_BEGIN)==0xFFFFFFFF ) goto api_error;
    }
    // ���������� ������
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
    // ��������� ����
	if ( (file=::CreateFile(File_.c_str(),GENERIC_READ|GENERIC_WRITE,0,nullptr,
		OPEN_EXISTING,
        Safe_?FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH:FILE_ATTRIBUTE_NORMAL,
        nullptr))==INVALID_HANDLE_VALUE ) goto api_error;
	// ���������� ������ ����� � ��������� �� ������������
    if ( (file_sizel=::GetFileSize(file,&file_sizeh))==0xFFFFFFFF ) goto api_error;
    if ( (file_sizeh!=0)||
         (file_sizel<sizeof(std::uint32_t))||    // ������ ������� ����� ������ ���� �� �����
         (file_sizel>=MAX_SLFileSize) ) goto error;
    // ���������� ������ ������ ��� ���������� � �����, ��� ������������
    // � �� �������� �� ����������� ������������� ���� (����� ����� �� ������� ���)
    data_size=GetAllDataSize(false);
    if ( data_size>MAX_SLFileSize )
    {
        throw std::runtime_error (      /// �������� �� return false ?
			"MSLList::AttachTo()\n"
			"������ ������ ��������� ����������� MAX_SLFileSize.");
    }
    if ( (data_size+file_sizel)>MAX_SLFileSize ) goto error;

    read_size=file_sizel;
    // ������������ ������� ������ ������� ��������
    // ����� ���������� (����������� ���������)
    if ( read_size>(sizeof(std::uint32_t)*2) ) read_size=sizeof(std::uint32_t)*2;
    // ���������� ����� ������ ������ ��� ������
    data_size+=read_size-sizeof(std::uint32_t);
    // �������� ������ ��� ����� ������ � ����� ������ �� �����
    try { all_data.resize(data_size); }
	catch ( std::bad_alloc &e )
    {
        // ��������� ���� � �������� ���������� ����
        ::CloseHandle(file);
        throw e;
    }

    // ������ ����� ������ ������ �� �����
    if ( (::SetFilePointer(file,-(LONG)read_size,nullptr,FILE_END)==0xFFFFFFFF)||
        (!::ReadFile(file,all_data.data(),read_size,&rw_size,nullptr))||
        (rw_size!=read_size) ) goto api_error;  /// ������ ��������� rw_size ?
    // �������������� �� � ��������� ������� ������� ����� ������ (0x00000000)
    BasicDecode(all_data.data(),read_size,Code_);
    if ( ((std::uint32_t*)&all_data[read_size])[-1]!=0 ) goto error;

    // ��������� ������ ��� ��������� � ������ � ������� �������� ������ ������
	if ( SetAllData(&all_data[read_size-sizeof(std::uint32_t)],false) !=
		&all_data[all_data.size()] )
	{
        throw std::runtime_error (
			"MSLList::AttachTo()\n"
			"������ ������ MSLList::SetAllData() �� ������������� MSLList::GetAllDataSize().");
	}

    // �������
    BasicEncode(all_data.data(),all_data.size(),Code_);

    if ( Safe_ )
    {
		// ����������� ����� � �����
        if ( (::SetFilePointer(file,(LONG)(data_size-read_size),nullptr,FILE_END)==0xFFFFFFFF)||
            (!SetEndOfFile(file)) ) goto api_error;
        // ��������� �� ����� ����� ���, ����� ������������ ����� ������ ������
        if ( ::SetFilePointer(file,-(LONG)data_size,nullptr,FILE_END)==0xFFFFFFFF ) goto api_error;
    } else
    {
        // ��������� �� ����� ����� ���, ����� ������������ ����� ������ ������
        if ( ::SetFilePointer(file,-(LONG)read_size,nullptr,FILE_END)==0xFFFFFFFF ) goto api_error;
    }
    // ��������� ������ � �����
    if ( (!::WriteFile(file,all_data.data(),data_size,&rw_size,nullptr))||
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
template <typename list_type, typename base_type>
bool MSLList<list_type,base_type>::LoadFrom(const std::wstring &File_, std::uint32_t Code_)
{
	HANDLE file=INVALID_HANDLE_VALUE;
	DWORD file_sizel, file_sizeh, rw_size;
	std::vector <char> all_data;

	LastError=0;
	// ��������� ����
	if ( (file=::CreateFile(File_.c_str(),GENERIC_READ,0,nullptr,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,nullptr))==INVALID_HANDLE_VALUE ) goto api_error;
	// ���������� ��� ������ � ��������� �� ������������
	if ( (file_sizel=::GetFileSize(file,&file_sizeh))==0xFFFFFFFF ) goto api_error;
	if ( (file_sizeh!=0)||
		 (file_sizel>MAX_SLFileSize) ) goto error;
	// �������� ������ ��� ������
	try { all_data.resize(file_sizel); }
	catch ( std::bad_alloc &e )
	{
		// ��������� ���� � �������� ���������� ����
		::CloseHandle(file);
		throw e;
	}
	// ��������� ������ �� �����
	if ( (!::ReadFile(file,all_data.data(),file_sizel,&rw_size,nullptr))||
		(rw_size!=file_sizel) ) goto api_error;
	// ��������� ���
	::CloseHandle(file); file=INVALID_HANDLE_VALUE;

	// ��������������
	BasicDecode(all_data.data(),file_sizel,Code_);
	// ������� � ������ (bad_alloc �� �����, �.�. ��� ���������)
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
	// ���������� ������ ������ � ��������� �� ������������
	size=GetAllDataSize();
	if ( size>MAX_SLRegSize )
	{
		throw std::runtime_error (      /// �������� �� return false ?
			"MSLList::StoreTo()\n"
			"������ ������ ��������� ����������� MAX_SLRegSize.");
	}
    // �������� ������ (bad_alloc �� �����, �.�. ��� ������ �� ������ ������)
    data.resize(size);
    // ��������� ���� ������ � ������ � ������� �������� ������ ������
	if ( SetAllData(data.data()) != &data[data.size()] )
	{
        throw std::runtime_error (
			"MSLList::StoreTo()\n"
			"������ ������ MSLList::SetAllData() �� ������������� MSLList::GetAllDataSize().");
    }
    // �������
	BasicEncode(data.data(),data.size(),Code_);

    // ������� ���� �������
	if ( ::RegCreateKeyEx(Key_,SubKey_.c_str(),0,nullptr,
		REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,nullptr,
		&key,nullptr)!=ERROR_SUCCESS ) goto api_error;
	// ��������� ��������
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
	// ��������� ���� �������
	if ( ::RegOpenKeyExW(Key_,SubKey_.c_str(),
		0,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS ) goto api_error;

	// ����� �������� ������ ��� ������ (��� ������� ����� ���������)
	try { data.resize(MAX_SLRegSize); }
	catch ( std::bad_alloc &e )
	{
		// ��������� ���� ������� � �������� ���������� ����
		::RegCloseKey(key); key=nullptr;
		throw e;
	}
	// ��������� ������� ���� ��-����� ������
	size=MAX_SLRegSize;
	if ( ::RegQueryValueEx(key,Value_.c_str(),
		nullptr,nullptr,(LPBYTE)data.data(),&size)!=ERROR_SUCCESS ) goto api_error;
	// ��������� ���� �������
	::RegCloseKey(key); key=nullptr;

	// �������������� ������
	BasicDecode(data.data(),size,Code_);
	// ������� ������ � ������ (bad_alloc �� �����, �.�. ��� ���������)
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
	// ���������� ������ ������ � ��������� �� ������������
	DWORD size=GetAllDataSize();
	if ( size>MAX_SLRegSize )
	{
		throw std::runtime_error (      /// �������� �� return false ?
			"MSLList::SaveAsReg()\n"
			"������ ������ ��������� ����������� MAX_SLRegSize.");
	}
	// �������� ������ (bad_alloc �� �����, �.�. ��� ������ �� ������ ������)
	std::vector <char> data;
	data.resize(size);
	// ��������� ���� ������ � ������ � ������� �������� ������ ������
	if ( SetAllData(data.data()) != (data.data()+data.size()) )
	{
		throw std::runtime_error (
			"MSLList::SaveAsReg()\n"
			"������ ������ MSLList::SetAllData() �� ������������� MSLList::GetAllDataSize().");
	}
	// �������
	BasicEncode(data.data(),data.size(),Code_);
	// ������������ � HEX
	std::vector <wchar_t> hex_data;
	hex_data.resize((data.size()*3+1)*sizeof(wchar_t));
	ByteToHEX(
		data.data(), data.size(),
		hex_data.data(), hex_data.size(),
		L',');

	// ��������� ��������� ����� reg-�����
	static const wchar_t hdr[]=L"REGEDIT4";
	static const wchar_t hk1[]=L"HKEY_LOCAL_MACHINE";
	static const wchar_t hk2[]=L"HKEY_CURRENT_USER";
	static const wchar_t hk3[]=L"HKEY_USERS";
	static const wchar_t hk4[]=L"HKEY_CLASSES_ROOT";
	const wchar_t *hk=nullptr;	if ( Key_==HKEY_LOCAL_MACHINE ) hk=hk1;
	else if ( Key_==HKEY_CURRENT_USER ) hk=hk2;	else if ( Key_==HKEY_USERS ) hk=hk3;	else if ( Key_==HKEY_CLASSES_ROOT ) hk=hk4;	else		throw std::runtime_error (			"MSLList::SaveAsReg()\n"
			"����� �� ������ ��� HKEY.");

	const std::wstring reg_str=
		L"\xFEFF"           			// UTF-16BE
		L"Windows Registry Editor Version 5.00\r\n"
		L"\r\n"
		L"["+std::wstring(hk)+L"\\"+SubKey_+L"]\r\n"+
		L"\""+Value_+L"\"=hex:"+hex_data.data()+L"\r\n";

	const wchar_t *reg_data=reg_str.c_str();    /// Windows only !!!
	const DWORD reg_size=reg_str.length()*sizeof(*reg_data);

	// ������� ���� � ���������� ������
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

