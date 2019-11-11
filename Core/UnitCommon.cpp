//---------------------------------------------------------------------------
#include <stdio.h>
//#include <windows.h>
#include <winsock2.h>
#include <ntstatus.h>
#include <netioapi.h>
#pragma hdrstop

#include "UnitCommon.h"
#include "UnitEncode.h"
#include "fasthash.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MTimeRand BasicRand([]() -> std::seed_seq& {
	static std::seed_seq seq(
		std::initializer_list<uint_fast32_t>({
			::GetCurrentProcessId(),
			CalcHwMacHash()
		}));
	return seq;
}());
//---------------------------------------------------------------------------
/// шаблон тут просится
void *MemSetLine(void *Mem__, const std::string &Line__)
{
	char *Mem_=static_cast<char*>(Mem__);
	const char *Line_=Line__.c_str();           /// лучше использовать UTF8

	while ( (*(Mem_++)=*(Line_++))!='\0' );
	return Mem_;
}

void *MemSetLine(void *Mem__, const std::wstring &Line__)
{
	wchar_t *Mem_=static_cast<wchar_t*>(Mem__);
	const wchar_t *Line_=Line__.c_str();        /// лучше использовать UTF8

	while ( (*(Mem_++)=*(Line_++))!=L'\0' );
	return Mem_;
}

const void *MemGetLine(const void *Mem__, std::string &Line_,
	size_t MaxLength_, const void *Limit__)
{
	const char *Mem_=static_cast<const char*>(Mem__);
	const char *Limit_=static_cast<const char*>(Limit__);

	if ( (size_t)(Limit_-Mem_)>(++MaxLength_) ) Limit_=Mem_+MaxLength_;
	Line_.clear();
	Line_.reserve(MaxLength_);      			// оптимизация

	do
	{
		if ( Mem_>=Limit_ ) return nullptr;     /// throw ?
		if ( *Mem_=='\0' ) break;
		Line_.push_back(*Mem_);
		Mem_++;
	} while(true);
	Mem_++;

	return Mem_;
}

const void *MemGetLine(const void *Mem__, std::wstring &Line_,
	size_t MaxLength_, const void *Limit__)
{
	const wchar_t *Mem_=static_cast<const wchar_t*>(Mem__);
	const wchar_t *Limit_=static_cast<const wchar_t*>(Limit__);

	if ( (size_t)(Limit_-Mem_)>(++MaxLength_) ) Limit_=Mem_+MaxLength_;
	Line_.clear();
	Line_.reserve(MaxLength_);      			// оптимизация

	do
	{
		if ( Mem_>=Limit_ ) return nullptr;     /// throw ?
		if ( *Mem_==L'\0' ) break;
		Line_.push_back(*Mem_);
		Mem_++;
	} while(true);
	Mem_++;

	return Mem_;
}
//---------------------------------------------------------------------------
void TimeRand(void *Buff__, size_t Size_)
{
	std::uniform_int_distribution <unsigned> dist(
		std::numeric_limits<unsigned char>::min(),
		std::numeric_limits<unsigned char>::max());

	unsigned char *Buff_=static_cast<unsigned char*>(Buff__);
	for ( size_t i=0; i<Size_; i++ ) Buff_[i]=dist(BasicRand);
}

bool CngRand(void *Buff_, size_t Size_)
{
/*
	BCRYPT_ALG_HANDLE hAlg;

	if ( ::BCryptOpenAlgorithmProvider(
		&hAlg, BCRYPT_RNG_ALGORITHM,
		nullptr, 0) != STATUS_SUCCESS ) return false;
*/
	if ( !BCRYPT_SUCCESS(::BCryptGenRandom(
		nullptr, //hAlg,
		(PUCHAR)Buff_, Size_,
		BCRYPT_USE_SYSTEM_PREFERRED_RNG)) ) return false;

	return true;
}
//---------------------------------------------------------------------------
size_t ByteToHEX(const void *Bytes__, size_t BytesCount_,
	wchar_t *Line_, size_t LineSize_, wchar_t Delim_)
{
	static const wchar_t sym[]={
		L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
		L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F'};
	const unsigned char *Bytes_=static_cast<const unsigned char*>(Bytes__);

	size_t pin=0, pout=0;
	if ( Delim_==L'\0' )
	{
		while((pin<BytesCount_)&&(pout<(LineSize_-2)))
		{
			Line_[pout+0]=sym[Bytes_[pin]>>4];
			Line_[pout+1]=sym[Bytes_[pin]&0x0F];
			pout+=2;
			pin++;
		}
	} else
	{
		while((pin<BytesCount_)&&(pout<(LineSize_-3)))
		{
			Line_[pout+0]=sym[Bytes_[pin]>>4];
			Line_[pout+1]=sym[Bytes_[pin]&0x0F];
			Line_[pout+2]=Delim_;
			pout+=3;
			pin++;
		}
	}

	// Буфер для строки закончился раньше, чем считали все байты
	if ( pin!=BytesCount_ )
	{
		throw std::runtime_error (
			"ByteToHEX()\n"
			"Не достаточный размер буфера для HEX-строки.");
	}

	// Последний разделитель заменим концом строки
	Line_[Delim_==L'\0'? pout: --pout]=0;

	return pout;
}

size_t HEXToByte(const wchar_t *Line_, void *Buff__, size_t BuffSize_)
{
	static const wchar_t sym[]={
		L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
		L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F',
		L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
		L'8', L'9', L'a', L'b', L'c', L'd', L'e', L'f'};
	unsigned char *Buff_=static_cast<unsigned char*>(Buff__);

    size_t pin=0, pout=0;
    const wchar_t *pos;
    unsigned char byte;

    while(pout<BuffSize_)
    {
        // Ищем символы старшей половинки байта
        do
        {
            if ( Line_[pin]==0 ) return pout;
			pos=static_cast<const wchar_t*>(
				memchr(sym, Line_[pin++], sizeof(sym)));
		} while(pos==nullptr);
		byte=((pos-sym)%16)<<4;

		// Ищем символы младшей половинки байта
		do
		{
			if ( Line_[pin]==0 ) return pout;
			pos=static_cast<const wchar_t*>(
				memchr(sym, Line_[pin++], sizeof(sym)));
        } while(pos==nullptr);
        byte+=(pos-sym)%16;

        // Сохраняем сформированный байт в выходном буфере
        Buff_[pout]=byte;
        pout++;
    }

    return pout;
}
//---------------------------------------------------------------------------
int ResMessageBox(HWND Owner_, UINT uCaption_, UINT uMessage_, UINT Type_, DWORD LastErr_)
{
    wchar_t Caption[128], Message[128+2+128];
    HINSTANCE HInstance;
    DWORD Error;

    HInstance=::GetModuleHandle(nullptr);
    // Загружаем заголовок сообщения
	if ( ::LoadString(HInstance, uCaption_, Caption, 128)==0 )
		swprintf(Caption, sizeof(Caption), L"<< Текст заголовка не найден ! >>");
	// Загружаем сообщение
	if ( ::LoadString(HInstance,uMessage_,Message,128)==0 )
		swprintf(Message, sizeof(Caption), L"<< Текст сообщения не найден ! >>");

    if ( LastErr_!=0 )
    {
        // Добавляем пару пустых строк
		wcscat(Message, L"\n\n");
        // Добавляем к строке сообщение об ошибке Windows
        ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,nullptr,
            LastErr_,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
            (LPTSTR)(Message+wcslen(Message)),128,nullptr);
    }
    
    // Выводим сообщение/диалог
    return ::MessageBox(Owner_,Message,Caption,Type_);
}
//---------------------------------------------------------------------------
bool WinExit(UINT uFlags)
{
	OSVERSIONINFOW vi;
	HANDLE Process, Token=nullptr;
    LUID Luid;
    TOKEN_PRIVILEGES Privileges;

/*
	// Запрашиваем информацию о версии ОС
	vi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	if ( !GetVersionExW(&vi) ) goto error;
	// Проверяем не WinNT-система ли и требуются ли дополнительные права на выполнение операции
	if ( (vi.dwPlatformId==VER_PLATFORM_WIN32_NT)&&
		(uFlags&(EWX_POWEROFF|EWX_REBOOT|EWX_SHUTDOWN)) )
*/
    {
        // Берем описатель процесса
        Process=::GetCurrentProcess();
        // Берем токен процесса
        if ( !::OpenProcessToken(Process,TOKEN_ADJUST_PRIVILEGES,&Token) ) goto error;
        // Запрашиваем LUID для SE_SHUTDOWN_NAME
        if ( !::LookupPrivilegeValue(nullptr,SE_SHUTDOWN_NAME,&Luid) ) goto error;
        // Устанавливаем привелегию для выполнения выхода из системы
        Privileges.PrivilegeCount=1;
        Privileges.Privileges[0].Luid=Luid;
        Privileges.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
        if ( !::AdjustTokenPrivileges(Token,FALSE,&Privileges,0,nullptr,nullptr) ) goto error;
        //
        ::CloseHandle(Token); Token=nullptr;
    }

    if ( !::ExitWindowsEx(uFlags,0) ) goto error;

	return true;
error:
	if ( Token ) ::CloseHandle(Token);
	return false;
}
//---------------------------------------------------------------------------
void RegExecList(HKEY hKey_, LPCWSTR SubKey_, HANDLE hToken_)
{
	HKEY key;
	DWORD index, type, name_size, data_size;
	wchar_t name[256+1], value[MAX_PATH+1];
	LPVOID lpEnv=nullptr;

	// Запросим параметры среды пользователя
	if ( (hToken_!=INVALID_HANDLE_VALUE)&&
		(!::CreateEnvironmentBlock(&lpEnv,hToken_,FALSE)) ) return;

	// Откроем раздел реестра
	if ( ::RegOpenKeyExW(
			hKey_, SubKey_, 0,
			KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE,
			&key)!=ERROR_SUCCESS )
	{
		if ( lpEnv!=nullptr ) ::DestroyEnvironmentBlock(lpEnv);
		return;
	}

	index=0;
	while(true)
	{
		name_size=256;
		data_size=MAX_PATH;

		// Получим значение очередного параметра реестра
		if ( ::RegEnumValueW(key, index, name, &name_size, nullptr,
			&type, (BYTE*)value, &data_size)!=ERROR_SUCCESS ) break;

		if ( (data_size>1)&&(type==REG_SZ) )
		{
			bool result=false;
			STARTUPINFOW si;
			PROCESS_INFORMATION pi;

			memset(&si,0,sizeof(STARTUPINFO));
			si.cb=sizeof(STARTUPINFO);

			// Запустим процесс
			if ( hToken_==INVALID_HANDLE_VALUE )
			{
				result=::CreateProcessW(
					nullptr, value,
					nullptr, nullptr, FALSE,
					CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
					nullptr, nullptr, &si, &pi);
			} else
			{
				result=::CreateProcessAsUserW(
					hToken_,
					nullptr, value,
					nullptr, nullptr, FALSE,
					CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS|CREATE_UNICODE_ENVIRONMENT,
					lpEnv, nullptr, &si, &pi);
			}
			if (result)
			{
				::CloseHandle(pi.hThread);
				::CloseHandle(pi.hProcess);
			}
		}
		if ( (++index)>=100 ) break;
	}

	::RegCloseKey(key);
	if ( lpEnv!=nullptr ) ::DestroyEnvironmentBlock(lpEnv);
}
//---------------------------------------------------------------------------
uint32_t CalcHwMacHash()
{
	// Запросим таблицу сетевых интерфейсов
	PMIB_IF_TABLE2 pTable=nullptr;
	if ( ::GetIfTable2(&pTable)!=NO_ERROR ) return 0;       /// throw ?

	// Посчитаем хэш MAC-адресов
	uint32_t uHash=0;
	for ( ULONG i=0, j=pTable->NumEntries; i<j; ++i )
	{
		PMIB_IF_ROW2 pRow=&pTable->Table[i];

		// Учтем только физические интерфейсы
		if ( !pRow->InterfaceAndOperStatusFlags.HardwareInterface ) continue;

		uHash=fasthash32(
			pRow->PhysicalAddress,
			pRow->PhysicalAddressLength,
			uHash);
	}

	// Освободим память
	::FreeMibTable(pTable);

	return uHash;
}
//---------------------------------------------------------------------------

