//---------------------------------------------------------------------------
#ifndef UnitCommonH
#define UnitCommonH
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
bool SystemTimeToInt64(SYSTEMTIME *lpSystemTime, __int64 *lpInt64);
bool Int64ToSystemTime(__int64 *lpInt64, SYSTEMTIME *lpSystemTime);
int ExtractHoursMin(__int64 Int64);
bool GetLocalTimeInt64(__int64 *lpInt64);
bool SetLocalTimeInt64(__int64 *lpInt64);
//---------------------------------------------------------------------------
void BasicCode(char *Data_, unsigned DataSize_, unsigned Code_);
void BasicEncode(char *Data_, unsigned DataSize_, unsigned Code_);
//---------------------------------------------------------------------------
bool GeneratePassword(char *Line_, unsigned Length_,
    bool Capital_, bool Small_, bool Numbers_);
//---------------------------------------------------------------------------
template <class settype>
char *MemSet(char *Mem_, settype Data_)
{
    *((settype*)Mem_)=Data_;
    return Mem_+sizeof(settype);
}

char *MemSetCLine(char *Mem_, char *Line_);
char *MemGetCLine(char *Mem_, char *Line_, unsigned MaxLength_, char *Limit_);

template <class gettype>
char *MemGet(char *Mem_, gettype *Data_, char *Limit_)
{
    if ( (Mem_+sizeof(gettype))>Limit_ ) return NULL;
    *Data_=*((gettype*)Mem_);
    return Mem_+sizeof(gettype);
}

template <class type>
type *MemSrch(type *Mem_, type *Limit_, type Data_)
{
    do { if ( Mem_>=Limit_ ) return NULL; }
    while( *(Mem_++)!=Data_ );
    return Mem_;
}
//---------------------------------------------------------------------------
template <class type>
int DComp(type &D1_, type &D2_)
{
    return D1_==D2_? 0: (D1_<D2_? -1: 1);
}
//---------------------------------------------------------------------------
#endif

