//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <string.h>
#include "UnitOptionsLoadSave.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
int GetStateDataSize(MState *State_)
{
    int Size=
        sizeof(int)+
        sizeof(unsigned int)+
        sizeof(double)+
        sizeof(int)+
        sizeof(double)+
        sizeof(int)+
        sizeof(double)+
        sizeof(unsigned int)+
        sizeof(unsigned int);
    return Size;
}
//---------------------------------------------------------------------------
char *SetStateData(char *Data_, MState *State_)
{
    *((int*)Data_)=State_->Number; Data_+=sizeof(int);
    *((unsigned int*)Data_)=State_->State; Data_+=sizeof(unsigned int);
    *((double*)Data_)=State_->StartWorkTime; Data_+=sizeof(double);
    *((int*)Data_)=State_->SizeWorkTime; Data_+=sizeof(int);
    *((double*)Data_)=State_->StartFineTime; Data_+=sizeof(double);
    *((int*)Data_)=State_->SizeFineTime; Data_+=sizeof(int);
    *((double*)Data_)=State_->StopTimerTime; Data_+=sizeof(double);
    *((unsigned int*)Data_)=State_->GamesPages; Data_+=sizeof(unsigned int);
    *((unsigned int*)Data_)=State_->StateVer; Data_+=sizeof(unsigned int);
    return Data_;
}
//---------------------------------------------------------------------------
char *GetStateData(char *Data_, char *LimitData_, MState *State_)
{
    // Номер компьютера
    if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
    State_->Number=*((int*)Data_); Data_+=sizeof(int);
    // Состояние компьютера
    if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
    State_->State=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
    // Время запуска компьютера в работу (абсолютное время)
    if ( (Data_+sizeof(double))>LimitData_ ) return NULL;
    State_->StartWorkTime=*((double*)Data_); Data_+=sizeof(double);
    // На какое время запущен компьютер (в минутах)
    if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
    State_->SizeWorkTime=*((int*)Data_); Data_+=sizeof(int);
    // Время, когда был применен штраф (абсолютное время)
    if ( (Data_+sizeof(double))>LimitData_ ) return NULL;
    State_->StartFineTime=*((double*)Data_); Data_+=sizeof(double);
    // Время ожидания в режиме штрафа (в минутах)
    if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
    State_->SizeFineTime=*((int*)Data_); Data_+=sizeof(int);
    // Время остановки отсчета времени (абсолютное время)
    if ( (Data_+sizeof(double))>LimitData_ ) return NULL;
    State_->StopTimerTime=*((double*)Data_); Data_+=sizeof(double);
    // Доступные для запуска группы программ
    if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
    State_->GamesPages=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
    // Версия состояния
    if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
    State_->StateVer=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);

    return Data_;
}
//---------------------------------------------------------------------------
bool StateSave(MState *State_)
{
    HKEY key=NULL;
    int data_size;
    char *all_data=NULL;

    State_->Lock();
    data_size=GetStateDataSize(State_);
    if ( (all_data=new char[data_size])==NULL ) goto error;
    SetStateData(all_data,State_);
    BasicCode(all_data,data_size,0xD7C1A2D3);

    // Создаем ключ реестра
    if ( ::RegCreateKeyEx(HKEY_LOCAL_MACHINE,"Software\\MMM Groups\\Time Locker\\3.0\\Client",
        NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&key,NULL)!=ERROR_SUCCESS ) goto error;
    // Сохраняем параметр
    if ( ::RegSetValueEx(key,"State",NULL,REG_BINARY,all_data,data_size)!=ERROR_SUCCESS )
        goto error;

    ::RegCloseKey(key);
    State_->UnLock();
    delete[] all_data;
    return true;
error:
    if ( key!=NULL ) ::RegCloseKey(key);
    State_->UnLock();
    delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
bool StateLoad(MState *State_)
{
    HKEY key=NULL;
    unsigned long data_size;
    char *all_data=NULL;

    State_->Lock();
    // Открываем
    if ( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\MMM Groups\\Time Locker\\3.0\\Client",
        NULL,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS ) goto error;
    // Определяем размер
    if ( ::RegQueryValueEx(key,"State",NULL,NULL,NULL,&data_size)!=ERROR_SUCCESS ) goto error;
    // Выделяем память под данные
    if ( (all_data=new char[data_size+1])==NULL ) goto error;
    // Считываем данные
    if ( ::RegQueryValueEx(key,"State",NULL,NULL,all_data,&data_size)!=ERROR_SUCCESS ) goto error;
    ::RegCloseKey(key); key=NULL;

    BasicEncode(all_data,data_size,0xD7C1A2D3);
    all_data[data_size]=0;
    if ( GetStateData(all_data,all_data+data_size,State_)==NULL ) goto error;

    State_->UnLock();
    delete[] all_data;
    return true;
error:
    if ( key!=NULL ) ::RegCloseKey(key);
    State_->UnLock();
    delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
char *GetGamesData(char *Data_, char *LimitData_, MGames *Games_)
{
    static int str_length;
    static MGame *NewGame;

    while(Data_<LimitData_)
    {
        NewGame=(MGame*)Games_->Add();
        // Название программы
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=64) ) return NULL;
        NewGame->Name=Data_; Data_+=str_length+1;
        // Командная строка для запуска
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=MAX_PATH) ) return NULL;
        NewGame->CommandLine=Data_; Data_+=str_length+1;
        // Файл с иконкой
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=MAX_PATH) ) return NULL;
        NewGame->IconFile=Data_; Data_+=str_length+1;
        // Наличие потомков у разобранного узла
        if ( (Data_+sizeof(char))>LimitData_ ) return NULL;
        if ( *Data_==1 )
        {
            Data_++;
            NewGame->SubGames=new MGames;
            if ( (Data_=GetGamesData(Data_,LimitData_,NewGame->SubGames))==NULL ) return NULL;
        }
        // Окончание одного уровня дерева
        if ( (Data_+sizeof(char))>LimitData_ ) return NULL;
        if ( *Data_==2 ) { Data_++; break; }
    }

    return Data_;
}
//---------------------------------------------------------------------------
bool GamesLoad(MGames *Games_)
{
    char file_name[MAX_PATH];
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    strcpy(file_name,win_dir);
    strcat(file_name,"TLK.GMS");
    // Открываем файл со списком игр
    if ( (file=::CreateFile(file_name,GENERIC_READ,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Определяем его размер
    if ( (data_size=::GetFileSize(file,NULL))==0xFFFFFFFF ) goto error;
    // Выделяем память под данные
    if ( (all_data=new char[data_size+1])==NULL ) goto error;
    // Считываем данные из файла
    if ( ::ReadFile(file,all_data,data_size,&rw_size,NULL)==NULL ) goto error;
    ::CloseHandle(file); file=NULL;

    BasicEncode(all_data,data_size,0xEAF39ED4);
    all_data[data_size]=0; Games_->Clear();
    if ( GetGamesData(all_data,all_data+data_size,Games_)==NULL ) goto error;

    delete[] all_data;
    return true;
error:
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------

