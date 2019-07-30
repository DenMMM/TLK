//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitEvents.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MEvent::MEvent()
{
    Time=0.;
    ComputerNumber=0;
    TimeSize=0;
    Cost=0.;
}
//---------------------------------------------------------------------------
MEvent::~MEvent()
{
//
}
//---------------------------------------------------------------------------
char *MEvents::GetData_(char *Data_, char *LimitData_, bool InfoOnly_)
{
    MEvent *NewEvent;
    int str_length;

    //
    if ( (Data_+sizeof(double))>LimitData_ ) return NULL;
    OpenTime=*((double*)Data_); Data_+=sizeof(double);
    //
    str_length=strlen(Data_);
    if ( ((Data_+str_length+1)>LimitData_)||(str_length>=255) ) return NULL;
    Name=Data_; Data_+=str_length+1;
    //
    str_length=strlen(Data_);
    if ( ((Data_+str_length+1)>LimitData_)||(str_length>=255) ) return NULL;
    Job=Data_; Data_+=str_length+1;
    //
    if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
    Rights=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);

    while(true)
    {
        // Окончание списка
        if ( (Data_+sizeof(double))>LimitData_ ) { CloseTime=OpenTime; return Data_; }
        if ( (*((double*)Data_))<0. ) { Data_+=sizeof(double); break; }
        //
        if ( !InfoOnly_ ) NewEvent=(MEvent*)Add();
        // Системное время
        if ( (Data_+sizeof(double))>LimitData_ ) return NULL;
        if ( !InfoOnly_ ) NewEvent->Time=*((double*)Data_); Data_+=sizeof(double);
        // Номер компьютера
        if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
        if ( !InfoOnly_ ) NewEvent->ComputerNumber=*((int*)Data_); Data_+=sizeof(int);
        // Название тарифа
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=255) ) return NULL;
        if ( !InfoOnly_ ) NewEvent->TariffName=Data_; Data_+=str_length+1;
        // Номер компьютера
        if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
        if ( !InfoOnly_ ) NewEvent->TimeSize=*((int*)Data_); Data_+=sizeof(int);
        // На какую сумму
        if ( (Data_+sizeof(double))>LimitData_ ) return NULL;
        if ( !InfoOnly_ ) NewEvent->Cost=*((double*)Data_); Data_+=sizeof(double);
    }

    //
    if ( (Data_+sizeof(double))>LimitData_ ) CloseTime=OpenTime;
    else CloseTime=*((double*)Data_);
    Data_+=sizeof(double);

    return Data_;
}
//---------------------------------------------------------------------------
bool MEvents::Load_(AnsiString FileName_, bool InfoOnly_)
{
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    // Открываем файл со списком штрафов
    if ( (file=::CreateFile(FileName_.c_str(),GENERIC_READ,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Определяем его размер
    if ( (data_size=::GetFileSize(file,NULL))==0xFFFFFFFF ) goto error;
    // Выделяем память под данные
    if ( (all_data=new char[data_size+1])==NULL ) goto error;
    // Считываем данные из файла
    if ( ::ReadFile(file,all_data,data_size,&rw_size,NULL)==NULL ) goto error;
    ::CloseHandle(file); file=NULL;

    BasicEncode(all_data,data_size,0x7F29C13E);
    all_data[data_size]=0; Clear();
    //
    if ( GetData_(all_data,all_data+data_size,InfoOnly_)==NULL ) goto error;

    delete[] all_data;
    return true;
error:
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
bool MEvents::Load(AnsiString FileName_)
{
    return Load_(FileName_,false);
}
//---------------------------------------------------------------------------
bool MEvents::LoadInfo(AnsiString FileName_)
{
    return Load_(FileName_,true);
}
//---------------------------------------------------------------------------

