//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include "UnitLogs.h"
#include "UnitCommon.h"
#include "UnitFormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
unsigned int LogFileNumber=0;
//---------------------------------------------------------------------------
int GetLogHeaderDataSize(MUser *User_)
{
    return
        sizeof(double)+  // Системное время
        User_->FullName.Length()+1+  // Имя работника
        User_->JobTitle.Length()+1+  // Должность
        sizeof(unsigned int);  // Права
}
//---------------------------------------------------------------------------
char *SetLogHeaderData(char *Data_, MUser *User_)
{
    *((double*)Data_)=(double)(Date()+Time()); Data_+=sizeof(double);
    Data_=strcpy(Data_,User_->FullName.c_str())+User_->FullName.Length()+1;
    Data_=strcpy(Data_,User_->JobTitle.c_str())+User_->JobTitle.Length()+1;
    *((unsigned int*)Data_)=User_->Rights; Data_+=sizeof(unsigned int);

    return Data_;
}
//---------------------------------------------------------------------------
bool LogNewFile(MUser *User_)
{
    char file_name[MAX_PATH];
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    // Определяем размер заголовка лога
    data_size=GetLogHeaderDataSize(User_);
    // Выделяем памать под данные
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // Заносим нужную информацию для сохранения
    SetLogHeaderData(all_data,User_);
    //
    BasicCode(all_data,data_size,0x7F29C13E);
    //
    memcpy(State->LogLastBytes,all_data+data_size-3,3);
    if ( !State->Save() ) goto error;

    strcpy(file_name,PrgDir.c_str());
    sprintf(file_name+strlen(file_name),"%s%08X%s","LOGS\\",State->LogFileNumber,".TLG");
    // Создаем файл
    if ( (file=::CreateFile(file_name,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Сохраняем данные в файле
    if ( ::WriteFile(file,all_data,data_size,&rw_size,NULL)==NULL ) goto error;

    ::CloseHandle(file);
    delete[] all_data;
    return true;
error:
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
bool LogAddRecord(char *Data_, int DataSize_)
{
    char file_name[MAX_PATH];
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    data_size=DataSize_+3;
    // Выделяем памать под данные
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // Заносим нужную информацию для сохранения
    memcpy(all_data+3,Data_,DataSize_);

    strcpy(file_name,PrgDir.c_str());
    sprintf(file_name+strlen(file_name),"%s%08X%s","LOGS\\",State->LogFileNumber,".TLG");
    // Открываем файл
    if ( (file=::CreateFile(file_name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Переводим текущую позицию в файле к его концу (минус три байта для корректной шифровки)
    if ( ::SetFilePointer(file,-3,NULL,FILE_END)==0xFFFFFFFF ) goto error;
    // Считываем данные из файла
    if ( (::ReadFile(file,all_data,3,&rw_size,NULL)==NULL)||(rw_size!=3) ) goto error;
    // Проверяем совпадение контрольных кодов файла (последние три байта)
    if ( memcmp(State->LogLastBytes,all_data,3) ) goto error;
    //
    BasicCode(all_data,data_size,0x7F29C13E);
    // Переводим текущую позицию в файле к его концу (минус три байта)
    if ( ::SetFilePointer(file,-3,NULL,FILE_END)==0xFFFFFFFF ) goto error;
    // Сохраняем данные в файле
    if ( ::WriteFile(file,all_data,data_size,&rw_size,NULL)==NULL ) goto error;
    //
    memcpy(State->LogLastBytes,all_data+data_size-3,3);
    if ( !State->Save() ) goto error;

    ::CloseHandle(file);
    delete[] all_data;
    return true;
error:
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
int GetLogRunDataSize(MTariff *Tariff_)
{
    if ( ((unsigned int)Tariff_)<20 )
    {
        return
            sizeof(double)+  // Системное время
            sizeof(int)+  // Номер компьютера
            2+1+  // Название тарифа
            sizeof(int)+  // На какое время
            sizeof(double);  // На какую сумму
    } else
    {
        return
            sizeof(double)+  // Системное время
            sizeof(int)+  // Номер компьютера
            Tariff_->Name.Length()+1+  // Название тарифа
            sizeof(int)+  // На какое время
            sizeof(double);  // На какую сумму
    }
}
//---------------------------------------------------------------------------
char *SetLogRunData(char *Data_, MComputer *Computer_, MTariff *Tariff_, int Time_, double Cost_)
{
    *((double*)Data_)=(double)(Date()+Time()); Data_+=sizeof(double);
    if ( Computer_==NULL ) *((int*)Data_)=0;
    else *((int*)Data_)=Computer_->Number;
    Data_+=sizeof(int);
    switch((int)Tariff_)
    {
        case 0: Data_=strcpy(Data_,"!*")+2+1; break;  // Запуск компьютера под сисадмином
        case 1: Data_=strcpy(Data_,"!a")+2+1; break;  // Перевод в автономный режим
        case 2: Data_=strcpy(Data_,"!n")+2+1; break;  // Перевод в нормальный режим работы
        case 3: Data_=strcpy(Data_,"!w")+2+1; break;  // Отключение программы
        case 4: Data_=strcpy(Data_,"!l")+2+1; break;  // Включение программы
        case 5: Data_=strcpy(Data_,"!s")+2+1; break;  // В программу вошел сисадмин
        case 6: Data_=strcpy(Data_,"!u")+2+1; break;  // Сисадмин вышел или вошел другой пользователь
        case 7: Data_=strcpy(Data_,"!g")+2+1; break;  // Изменение списка игр
        case 8: Data_=strcpy(Data_,"!f")+2+1; break;  // Штраф
        case 9: Data_=strcpy(Data_,"!r")+2+1; break;  // Программа запущена
        case 10: Data_=strcpy(Data_,"!c")+2+1; break;  // Программа закрыта
        case 11: Data_=strcpy(Data_,"!t")+2+1; break;  // Изменения в тарифах
        case 12: Data_=strcpy(Data_,"!o")+2+1; break;  // Пересадка с ... компьютера
        case 13: Data_=strcpy(Data_,"!i")+2+1; break;  // Пересадка на ... компьютер
        case 14: Data_=strcpy(Data_,"!L")+2+1; break;  // Компьютер прикрыт
        case 15: Data_=strcpy(Data_,"!U")+2+1; break;  // Компьютер открыт
        case 16: Data_=strcpy(Data_,"!P")+2+1; break;  // Компьютер приостановлен
        case 17: Data_=strcpy(Data_,"!R")+2+1; break;  // Компьютер запущен
        case 18: Data_=strcpy(Data_,"!B")+2+1; break;  // Компьютер перезагружен
        case 19: Data_=strcpy(Data_,"!S")+2+1; break;  // Компьютер выключен
        default: Data_=strcpy(Data_,Tariff_->Name.c_str())+Tariff_->Name.Length()+1; break;
    }
    *((int*)Data_)=Time_; Data_+=sizeof(int);
    *((double*)Data_)=Cost_; Data_+=sizeof(double);

    return Data_;
}
//---------------------------------------------------------------------------
bool LogCompRun(MComputer *Computer_, MTariff *Tariff_, int Time_, double Cost_)
{
    int data_size;
    char *all_data=NULL;

    // Определяем размер данных для добавления
    data_size=GetLogRunDataSize(Tariff_);
    // Выделяем памать под данные
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // Заносим нужную информацию для сохранения
    SetLogRunData(all_data,Computer_,Tariff_,Time_,Cost_);
    //
    if ( !LogAddRecord(all_data,data_size) ) goto error;

    delete[] all_data;
    return true;
error:
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
int GetLogCloseDataSize()
{
    return
        sizeof(double)+
        sizeof(double);
}
//---------------------------------------------------------------------------
char *SetLogCloseData(char *Data_)
{
    *((double*)Data_)=-1.; Data_+=sizeof(double);
    *((double*)Data_)=(double)(Date()+Time()); Data_+=sizeof(double);

    return Data_;
}
//---------------------------------------------------------------------------
bool LogClose()
{
    int data_size;
    char *all_data=NULL;

    // Определяем размер данных для добавления
    data_size=GetLogCloseDataSize();
    // Выделяем памать под данные
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // Заносим нужную информацию для сохранения
    SetLogCloseData(all_data);
    //
    if ( !LogAddRecord(all_data,data_size) ) goto error;

    delete[] all_data;
    return true;
error:
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------

