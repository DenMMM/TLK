//---------------------------------------------------------------------------
#ifndef UnitCommonH
#define UnitCommonH
//---------------------------------------------------------------------------
#include <winsock2.h>
//#include <windows.h>
#include <userenv.h>
#include <mem.h>
#include <stdexcept.h>
//---------------------------------------------------------------------------
#define ENC_Code        0x5BC935CF  // Код шифрования файлов
#define ENC_Net         0x9ABD5BAE  // Код шифрования сетевых данных
//---------------------------------------------------------------------------
template <typename type>
class Mptr
{
private:
    type *ptr;

public:
    void operator()(type *ptr_)
    {
        if ( ptr!=NULL )
        {
            // Удалим объект, чтобы не потерялся
            delete ptr_;
            // Кинем исплючение
            throw std::runtime_error (
                "Munique_ptr::()\n"
                "Попытка присвоения не пустому указателю."
                );
        }
        ptr=ptr_;
    }
    type* operator->() { return ptr; }
    operator type*() { return ptr; }

    Mptr() { ptr=NULL; }
    ~Mptr() { delete ptr; }
};
//---------------------------------------------------------------------------
template <typename titem>
class Marray
{
private:
    unsigned ICount;
    titem *Items;

public:
    void Alloc(unsigned Count_=0)
    {
        if ( Count_==0 || Count_!=ICount  )
        {
            delete[] Items;
            Items=NULL;
        }
        if ( Count_!=0 )
        {
            Items=new titem[Count_];
            ICount=Count_;
        }
    }

    unsigned Count() const
    {
        return ICount;
    }

    titem &operator[](unsigned Num_) const
    {
        if ( Num_>=ICount )
        {
            throw std::out_of_range (
                "MArray::[]\n"
                "Попытка выйти за пределы массива."
                );
        }
        return Items[Num_];
    }

    Marray &operator=(const Marray &Src_)
    {
        // Убедимся, что не сами себя копируем
        if ( &Src_==this ) return *this;
        // Пересоздадим массив
        Alloc(Src_.ICount);
        // Скопируем все элементы
        for (unsigned i=0; i<ICount; i++)
            Items[i]=Src_.Items[i];         /// возможно, стоит обработать bad_alloc

        return *this;
    }

    Marray()
    {
        ICount=0;
        Items=NULL;
    }

    ~Marray()
    {
        delete[] Items;
    }
};
//---------------------------------------------------------------------------
// Сравнение двух чисел: "0" - равны, "-1" - первое меньше второго, "+1" - наоборот
template <typename type>
inline int DComp(type &D1_, type &D2_)
{
    return D1_==D2_? 0: (D1_<D2_? -1: 1);
}

// Побитовый циклический сдвиг влево
template <typename type>
inline type BitsLeft(type Bits_)
{
    return (Bits_<<1)|(Bits_>>(sizeof(type)*8-1));
}

// Побитовый циклический сдвиг вправо
template <typename type>
inline type BitsRight(type Bits_)
{
    return (Bits_>>1)|(Bits_<<(sizeof(type)*8-1));
}
//---------------------------------------------------------------------------
// Сохранение переменной в области памяти со сдвигом указателя
template <typename type>
inline char *MemSet(char *Mem_, const type Data_)
{
    *((type*)Mem_)=Data_;
    return Mem_+sizeof(type);
}

// Чтение переменной из области памяти со сдвигом указателя
// и контролем выхода за границу области
template <typename type>
inline const char *MemGet(const char *Mem_, type *Data_, const char *Limit_)
{
    if ( (Mem_+sizeof(type))>Limit_ ) return NULL;
    *Data_=*((type*)Mem_);
    return Mem_+sizeof(type);
}

// Поиск значения в области памяти с контролем выхода за границу
template <typename type>
type *MemSrch(type *Mem_, const type *Limit_, type Data_)
{
    do { if ( Mem_>=Limit_ ) return NULL; }
    while( *(Mem_++)!=Data_ );
    return Mem_;
}

// Копирование ANSI-строк со сдвигом указателя, контролем длины и выхода за границу
char *MemSetCLine(char *Mem_, const char *Line_);
const char *MemGetCLine(const char *Mem_, char *Line_,
    unsigned MaxLength_, const char *Limit_);

inline char *MemSetBLine(char *Mem_, const char *Line_, unsigned Size_)
{
    memcpy(Mem_,Line_,Size_);
    return Mem_+Size_;
}

inline const char *MemGetBLine(const char *Mem_, char *Line_, unsigned Size_, const char *Limit_)
{
    if ( (Mem_+Size_)>Limit_) return NULL;
    memcpy(Line_,Mem_,Size_);
    return Mem_+Size_;
}

inline bool MemSlowCmp(const char *Mem1_, const char *Mem2_, unsigned Size_)
{
    unsigned diff_cnt=0;

    while(Size_)
    {
        diff_cnt+=*(Mem1_++)!=*(Mem2_++);
        --Size_;
    }

    return diff_cnt==0;
}
//---------------------------------------------------------------------------
inline bool SystemTimeToInt64(const SYSTEMTIME *lpSystemTime, __int64 *lpInt64)
{
    FILETIME FileTime;
    LARGE_INTEGER LargeInteger;

    if ( !::SystemTimeToFileTime(lpSystemTime,&FileTime) ) return false;
    memcpy(&LargeInteger,&FileTime,sizeof(LargeInteger));
    *lpInt64=LargeInteger.QuadPart;

    return true;
}

inline bool Int64ToSystemTime(const __int64 *lpInt64, SYSTEMTIME *lpSystemTime)
{
    LARGE_INTEGER LargeInteger;
    FILETIME FileTime;

    LargeInteger.QuadPart=*lpInt64;
    memcpy(&FileTime,&LargeInteger,sizeof(FileTime));

    return ::FileTimeToSystemTime(&FileTime,lpSystemTime);
}

inline int ExtractHoursMin(const __int64 Int64)
{
    SYSTEMTIME ssTime;
    if ( !Int64ToSystemTime(&Int64,&ssTime) ) return -1;
    return ssTime.wHour*60+ssTime.wMinute;
}

inline bool GetLocalTimeInt64(__int64 *lpInt64)
{
    SYSTEMTIME ssTime;
    ::GetLocalTime(&ssTime);
    return SystemTimeToInt64(&ssTime,lpInt64);
}

inline bool SetLocalTimeInt64(const __int64 *lpInt64)
{
    SYSTEMTIME ssTime;
    return Int64ToSystemTime(lpInt64,&ssTime)&&
        ::SetLocalTime(&ssTime);
}
//---------------------------------------------------------------------------
// Основной раунд шифрования
template <typename type>
inline type BasicEncodeRound(type Blk_, type Code_)
{
    Blk_+=Code_;                             // складываем с циклическим переносом
    Blk_=BitsRight(Blk_);                    // побитовый циклический сдвиг вправо
    Blk_^=Code_;                             // побитовое исключающее "или"
    return Blk_;
}

// Основной раунд дешифрования
template <typename type>
inline type BasicDecodeRound(type Blk_, type Code_)
{
    Blk_^=Code_;                             // побитовое исключающее "или"
    Blk_=BitsLeft(Blk_);                     // побитовый циклический сдвиг вправо
    Blk_-=Code_;                             // складываем с циклическим переносом
    return Blk_;
}

// Простейшее блочно-потоковое шифрование/дешифрование
void BasicEncode(unsigned char *Data_, unsigned DataSize_, unsigned Code_, int Round_=8);
void BasicDecode(unsigned char *Data_, unsigned DataSize_, unsigned Code_, int Round_=8);
unsigned BasicMix(unsigned Key1_, unsigned Key2_);
unsigned BasicRand();
bool TimeRand(char *Buff_, unsigned Size_);
//---------------------------------------------------------------------------
int ByteToHEX(const unsigned char *Bytes_, int BytesCount_,
    char *Line_, int LineSize_, char Delim_);
int HEXToByte(const char *Line_, unsigned char *Buff_, int BuffSize_);
//---------------------------------------------------------------------------
bool GeneratePassword(char *Line_, unsigned Len_,
    bool Cap_, bool Low_, bool Num_);
//---------------------------------------------------------------------------
int ResMessageBox(HWND Owner_, UINT uCaption, UINT uMessage, UINT Type_, DWORD LastErr_=0);
//---------------------------------------------------------------------------
// Выход из системы
bool WinExit(UINT uFlags);
//---------------------------------------------------------------------------
// Запуск программ по списку из реестра
void RegExecList(HKEY hKey_, LPCTSTR SubKey_, HANDLE hToken_=INVALID_HANDLE_VALUE);
//---------------------------------------------------------------------------
#endif

