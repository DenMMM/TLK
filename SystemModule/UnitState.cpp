//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitState.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MState::MState()
{
    CurrentUser=NULL;
    CurrentAdmin=NULL;
    LogFileNumber=0;
//    LogFileNumber=ComputersSecCode=UsersSecCode=0;
    memset(LogLastBytes,0,3);
}
//---------------------------------------------------------------------------
MState::~MState()
{
//
}
//---------------------------------------------------------------------------
int MState::GetStateDataSize()
{
    return
        sizeof(unsigned int)+  //
        AdminPassword.Length()+1+  // Пароль сисадмина
        sizeof(unsigned int)+  // ID-номер текущего пользователя
        sizeof(unsigned int)+  // ID-номер текущего админа (чья смена)
        sizeof(unsigned int)+  // Текущий номер файла лога
//        sizeof(unsigned int)+  // Код шифровки файла с данными о компьютерах
//        sizeof(unsigned int);  // Код шифровки файла с данными о пользователях
        sizeof(char)*3;  // Последние байты файла лога
}
//---------------------------------------------------------------------------
char *MState::SetStateData(char *Data_)
{
    *((unsigned int*)Data_)=0x55555555; Data_+=sizeof(unsigned int);
    Data_=strcpy(Data_,AdminPassword.c_str())+AdminPassword.Length()+1;
    if ( CurrentUser ) *((unsigned int*)Data_)=CurrentUser->ID;
    else *((unsigned int*)Data_)=-1;
    Data_+=sizeof(unsigned int);
    if ( CurrentAdmin ) *((unsigned int*)Data_)=CurrentAdmin->ID;
    else *((unsigned int*)Data_)=-1;
    Data_+=sizeof(unsigned int);
    *((unsigned int*)Data_)=LogFileNumber; Data_+=sizeof(unsigned int);
//    *((unsigned int*)Data_)=ComputersSecCode; Data_+=sizeof(unsigned int);
//    *((unsigned int*)Data_)=UsersSecCode; Data_+=sizeof(unsigned int);
    Data_=(char*)memcpy(Data_,LogLastBytes,3)+3;

    return Data_;
}
//---------------------------------------------------------------------------
char *MState::GetStateData(char *Data_, char *LimitData_)
{
    int str_length;

    //
    if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
    if ( *((unsigned int*)Data_)!=0x55555555 ) return NULL;
    else Data_+=sizeof(unsigned int);
    // Пароль сисадмина
    str_length=strlen(Data_);
    if ( ((Data_+str_length+1)>LimitData_)||(str_length>16) ) return NULL;
    AdminPassword=Data_; Data_+=str_length+1;
    // ID-номер текущего пользователя
    if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
    CurrentUser=(MUser*)(*((unsigned int*)Data_)); Data_+=sizeof(unsigned int);
    // ID-номер текущего админа (чья смена)
    if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
    CurrentAdmin=(MUser*)(*((unsigned int*)Data_)); Data_+=sizeof(unsigned int);
    // Текущий номер файла лога
    if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
    LogFileNumber=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
    // Код шифровки файла с данными о компьютерах
//    if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
//    ComputersSecCode=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
    // Код шифровки файла с данными о пользователях
//    if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
//    UsersSecCode=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
    //
    if ( (Data_+sizeof(char)*3)>LimitData_ ) return NULL;
    Data_=(char*)memcpy(LogLastBytes,Data_,3)+3;

    return Data_;
}
//---------------------------------------------------------------------------
bool MState::Save()
{
    HKEY key=NULL;
    int data_size;
    char *all_data=NULL;

    data_size=GetStateDataSize();
    if ( (all_data=new char[data_size])==NULL ) goto error;
    SetStateData(all_data);
    BasicCode(all_data,data_size,0xD7C1A2D3);

    // Создаем ключ реестра
    if ( ::RegCreateKeyEx(HKEY_LOCAL_MACHINE,"Software\\MMM Groups\\Time Locker\\3.0\\Admin",
        NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,NULL,&key,NULL)!=ERROR_SUCCESS ) goto error;
    // Сохраняем параметр
    if ( ::RegSetValueEx(key,"State",NULL,REG_BINARY,all_data,data_size)!=ERROR_SUCCESS )
        goto error;

    ::RegCloseKey(key);
    delete[] all_data;
    return true;
error:
    if ( key!=NULL ) ::RegCloseKey(key);
    delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
bool MState::Load()
{
    HKEY key=NULL;
    unsigned long data_size;
    char *all_data=NULL;

    // Открываем
    if ( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\MMM Groups\\Time Locker\\3.0\\Admin",
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
    if ( GetStateData(all_data,all_data+data_size)==NULL ) goto error;

    delete[] all_data;
    return true;
error:
    if ( key!=NULL ) ::RegCloseKey(key);
    delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

