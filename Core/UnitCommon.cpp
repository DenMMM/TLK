//---------------------------------------------------------------------------
//#include <stddef.h>
#include <stdio.h>
#pragma hdrstop

#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
char *MemSetCLine(char *Mem_, const char *Line_)
{
    while ( (*(Mem_++)=*(Line_++))!=0 );
    return Mem_;
}

const char *MemGetCLine(const char *Mem_, char *Line_,
    unsigned MaxLength_, const char *Limit_)
{
    if ( (unsigned)(Limit_-Mem_)>(++MaxLength_) ) Limit_=Mem_+MaxLength_;

    do { if ( Mem_>=Limit_ ) return NULL; }
    while( (*(Line_++)=*(Mem_++))!=0 );

    return Mem_;
}
//---------------------------------------------------------------------------
void BasicEncode(unsigned char *Data_, unsigned DataSize_, unsigned Code_, int Round_)
{
    unsigned char *limit;
    unsigned blk;

    if ( DataSize_<sizeof(blk) ) return;

    limit=Data_+DataSize_-sizeof(blk);
    // Считаем начальный блок целиком
    blk=*((unsigned*)Data_);
    goto begin;

    do
    {
        // Сохраним в выход младший байт блока
        Data_[0]=blk;
        // Сдвинем блок вправо и загрузим старший байт из входа
        blk>>=8;
        blk|=( (unsigned)Data_[sizeof(blk)] )<<(sizeof(blk)*8-8);
        ++Data_;
begin:
        // Перемешаем биты блока
        for ( int i=Round_; i; i-- )
            blk=BasicEncodeRound(blk,Code_);
    } while(Data_!=limit);

    // Сохраним в выход последний блок целиком
    *((unsigned*)Data_)=blk;
}

void BasicDecode(unsigned char *Data_, unsigned DataSize_, unsigned Code_, int Round_)
{
    unsigned char *limit;
    unsigned blk;

    if ( DataSize_<sizeof(blk) ) return;

    limit=Data_;
    Data_+=DataSize_-sizeof(blk);

    // Считаем начальный блок целиком
    blk=*((unsigned*)Data_);
    goto begin;

    do
    {
        --Data_;
        // Сохраним в выход старший байт блока
        Data_[sizeof(blk)]=blk>>(sizeof(blk)*8-8);
        // Сдвинем блок влево и загрузим младший байт из входа
        blk<<=8;
        blk|=Data_[0];
begin:
        // Перемешаем биты блока
        for ( int i=Round_; i; i-- )
            blk=BasicDecodeRound(blk,Code_);
    } while(Data_!=limit);

    // Сохраним в выход последний блок целиком
    *((unsigned*)Data_)=blk;
}

unsigned BasicMix(unsigned Key1_, unsigned Key2_)
{
    unsigned k1=Key1_;
    unsigned k2=Key2_;

    // Шифруем каждый ключ друг другом
    BasicEncode((char*)&Key2_,sizeof(Key2_),k1,sizeof(Key2_)*8);
    BasicEncode((char*)&Key1_,sizeof(Key1_),k2,sizeof(Key1_)*8);
    // XOR результатов с проверкой на равенство
    return (Key1_^=Key2_)? Key1_: Key2_;
}

unsigned BasicRand()
{
    unsigned rnd=0;
    for ( int i=sizeof(rnd); i; i--)
        rnd=(rnd<<8)|random(256);
    return rnd;
}

bool TimeRand(char *Buff_, unsigned Size_)
{
    LARGE_INTEGER ent1;
    DWORD ent2;

    // Считываем счетчик тактов процессора
    if ( !::QueryPerformanceCounter(&ent1) ) return false;
    // Считываем таймер активности ОС
    ent2=::GetTickCount();

    // Заполним выходной буфер псевдослучайными значениями
    for ( unsigned i=0; i<Size_; i++ ) Buff_[i]=random(256);
    // Добавим энтропии
    BasicEncode(Buff_,Size_,ent1.LowPart,32);
    BasicEncode(Buff_,Size_,ent1.HighPart,32);
    BasicEncode(Buff_,Size_,ent2,32);

    return true;
}
//---------------------------------------------------------------------------
int ByteToHEX(const unsigned char *Bytes_, int BytesCount_,
    char *Line_, int LineSize_, char Delim_)
{
    static const char sym[]={
        '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

    int pin=0, pout=0;
    while((pin<BytesCount_)&&(pout<(LineSize_-2)))
    {
        Line_[pout+0]=sym[Bytes_[pin]>>4];
        Line_[pout+1]=sym[Bytes_[pin]&0x0F];
        Line_[pout+2]=Delim_;
        pin++;
        pout+=3;
    }
    // Буфер для строки закончился раньше, чем считали все байты
    if ( pin!=BytesCount_ )
    {
        throw std::runtime_error (
        TEXT("ByteToHEX\n")
        TEXT("Не достаточный размер буфера для HEX-строки.")
        );
    }
    // Последний разделитель заменим концом строки
    Line_[--pout]=0;

    return pout;
}

int HEXToByte(const char *Line_, unsigned char *Buff_, int BuffSize_)
{
    static const char sym[]={
        '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',
        '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

    int pin=0, pout=0;
    const char *pos;
    unsigned char byte;

    while(pout<BuffSize_)
    {
        // Ищем символы старшей половинки байта
        do
        {
            if ( Line_[pin]==0 ) return pout;
            pos=(const char*)memchr(sym,Line_[pin++],sizeof(sym));
        } while(pos==NULL);
        byte=((pos-sym)%16)<<4;

        // Ищем символы младшей половинки байта
        do
        {
            if ( Line_[pin]==0 ) return pout;
            pos=(const char*)memchr(sym,Line_[pin++],sizeof(sym));
        } while(pos==NULL);
        byte+=(pos-sym)%16;

        // Сохраняем сформированный байт в выходном буфере
        Buff_[pout]=byte;
        pout++;
    }

    return pout;
}
//---------------------------------------------------------------------------
bool GeneratePassword(char *Line_, unsigned Len_,
    bool Cap_, bool Low_, bool Num_)
{
    static const char cap[]={'A','B','C','D','E','F','G','H','I','J','K','L','M',
                             'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    static const char low[]={'a','b','c','d','e','f','g','h','i','j','k','l','m',
                             'n','o','p','q','r','s','t','u','v','w','x','y','z'};
    static const char dig[]={'0','1','2','3','4','5','6','7','8','9'};
    char buff[sizeof(cap)+
             sizeof(low)+
             sizeof(dig)];
    unsigned use;

    // Считаем сколько символов в буфере будет использовано и попутно заполняем его
    use=0;
    if ( Cap_ ) { memcpy(buff+use,cap,sizeof(cap)); use+=sizeof(cap); }
    if ( Low_ ) { memcpy(buff+use,low,sizeof(low)); use+=sizeof(low); }
    if ( Num_ ) { memcpy(buff+use,dig,sizeof(dig)); use+=sizeof(dig); }
    if ( use==0 ) goto error;

    // Заполняем строку с паролем случайными символами из буфера
    for (unsigned i=0; i<Len_; i++)
        Line_[i]=buff[random(use)];
    *(Line_+Len_)=0;

    return true;
error:
    *Line_=0;
    return false;
}
//---------------------------------------------------------------------------
int ResMessageBox(HWND Owner_, UINT uCaption_, UINT uMessage_, UINT Type_, DWORD LastErr_)
{
    char Caption[128], Message[128+2+128];
    HINSTANCE HInstance;
    DWORD Error;

    HInstance=::GetModuleHandle(NULL);
    // Загружаем заголовок сообщения
    if ( ::LoadString(HInstance,uCaption_,Caption,128)==0 )
        sprintf(Caption,"<< Текст заголовка не найден ! >>");
    // Загружаем сообщение
    if ( ::LoadString(HInstance,uMessage_,Message,128)==0 )
        sprintf(Message,"<< Текст сообщения не найден ! >>");

    if ( LastErr_!=0 )
    {
        // Добавляем пару пустых строк
        strcat(Message,"\n\n");
        // Добавляем к строке сообщение об ошибке Windows
        ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,
            LastErr_,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
            (LPTSTR)(Message+strlen(Message)),128,NULL);
    }
    
    // Выводим сообщение/диалог
    return ::MessageBox(Owner_,Message,Caption,Type_);
}
//---------------------------------------------------------------------------
bool WinExit(UINT uFlags)
{
    OSVERSIONINFO vi;
    HANDLE Process, Token=NULL;
    LUID Luid;
    TOKEN_PRIVILEGES Privileges;

    // Запрашиваем информацию о версии ОС
    vi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
    if ( !GetVersionEx(&vi) ) goto error;
    // Проверяем не WinNT-система ли и требуются ли дополнительные права на выполнение операции
    if ( (vi.dwPlatformId==VER_PLATFORM_WIN32_NT)&&
        (uFlags&(EWX_POWEROFF|EWX_REBOOT|EWX_SHUTDOWN)) )
    {
        // Берем описатель процесса
        Process=::GetCurrentProcess();
        // Берем токен процесса
        if ( !::OpenProcessToken(Process,TOKEN_ADJUST_PRIVILEGES,&Token) ) goto error;
        // Запрашиваем LUID для SE_SHUTDOWN_NAME
        if ( !::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&Luid) ) goto error;
        // Устанавливаем привелегию для выполнения выхода из системы
        Privileges.PrivilegeCount=1;
        Privileges.Privileges[0].Luid=Luid;
        Privileges.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
        if ( !::AdjustTokenPrivileges(Token,FALSE,&Privileges,0,NULL,NULL) ) goto error;
        //
        ::CloseHandle(Token); Token=NULL;
    }

    if ( !::ExitWindowsEx(uFlags,0) ) goto error;

    return true;
error:
    if ( Token ) ::CloseHandle(Token);
    return false;
}
//---------------------------------------------------------------------------
void RegExecList(HKEY hkey, LPCTSTR subkey)
{
    HKEY key;
    DWORD index, type, name_size, data_size;
    char name[256], value[256];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    if ( ::RegOpenKeyEx(hkey,subkey,0,
        KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS ) return;

    index=0;
    while(true)
    {
        name_size=data_size=256;
        if ( ::RegEnumValue(key,index,name,&name_size,NULL,
            &type,value,&data_size)!=ERROR_SUCCESS ) break;
        if ( (data_size>1)&&(type==REG_SZ) )
        {
            memset(&si,0,sizeof(STARTUPINFO));
            si.cb=sizeof(STARTUPINFO);
            ::CreateProcess(NULL,value,NULL,NULL,FALSE,
                CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
                NULL,NULL,&si,&pi);
        }
        if ( (++index)>=100 ) break;
    }

    ::RegCloseKey(key);
}
//---------------------------------------------------------------------------

