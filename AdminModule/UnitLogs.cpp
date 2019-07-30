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
        sizeof(double)+  // ��������� �����
        User_->FullName.Length()+1+  // ��� ���������
        User_->JobTitle.Length()+1+  // ���������
        sizeof(unsigned int);  // �����
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

    // ���������� ������ ��������� ����
    data_size=GetLogHeaderDataSize(User_);
    // �������� ������ ��� ������
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // ������� ������ ���������� ��� ����������
    SetLogHeaderData(all_data,User_);
    //
    BasicCode(all_data,data_size,0x7F29C13E);
    //
    memcpy(State->LogLastBytes,all_data+data_size-3,3);
    if ( !State->Save() ) goto error;

    strcpy(file_name,PrgDir.c_str());
    sprintf(file_name+strlen(file_name),"%s%08X%s","LOGS\\",State->LogFileNumber,".TLG");
    // ������� ����
    if ( (file=::CreateFile(file_name,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // ��������� ������ � �����
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
    // �������� ������ ��� ������
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // ������� ������ ���������� ��� ����������
    memcpy(all_data+3,Data_,DataSize_);

    strcpy(file_name,PrgDir.c_str());
    sprintf(file_name+strlen(file_name),"%s%08X%s","LOGS\\",State->LogFileNumber,".TLG");
    // ��������� ����
    if ( (file=::CreateFile(file_name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // ��������� ������� ������� � ����� � ��� ����� (����� ��� ����� ��� ���������� ��������)
    if ( ::SetFilePointer(file,-3,NULL,FILE_END)==0xFFFFFFFF ) goto error;
    // ��������� ������ �� �����
    if ( (::ReadFile(file,all_data,3,&rw_size,NULL)==NULL)||(rw_size!=3) ) goto error;
    // ��������� ���������� ����������� ����� ����� (��������� ��� �����)
    if ( memcmp(State->LogLastBytes,all_data,3) ) goto error;
    //
    BasicCode(all_data,data_size,0x7F29C13E);
    // ��������� ������� ������� � ����� � ��� ����� (����� ��� �����)
    if ( ::SetFilePointer(file,-3,NULL,FILE_END)==0xFFFFFFFF ) goto error;
    // ��������� ������ � �����
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
            sizeof(double)+  // ��������� �����
            sizeof(int)+  // ����� ����������
            2+1+  // �������� ������
            sizeof(int)+  // �� ����� �����
            sizeof(double);  // �� ����� �����
    } else
    {
        return
            sizeof(double)+  // ��������� �����
            sizeof(int)+  // ����� ����������
            Tariff_->Name.Length()+1+  // �������� ������
            sizeof(int)+  // �� ����� �����
            sizeof(double);  // �� ����� �����
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
        case 0: Data_=strcpy(Data_,"!*")+2+1; break;  // ������ ���������� ��� ����������
        case 1: Data_=strcpy(Data_,"!a")+2+1; break;  // ������� � ���������� �����
        case 2: Data_=strcpy(Data_,"!n")+2+1; break;  // ������� � ���������� ����� ������
        case 3: Data_=strcpy(Data_,"!w")+2+1; break;  // ���������� ���������
        case 4: Data_=strcpy(Data_,"!l")+2+1; break;  // ��������� ���������
        case 5: Data_=strcpy(Data_,"!s")+2+1; break;  // � ��������� ����� ��������
        case 6: Data_=strcpy(Data_,"!u")+2+1; break;  // �������� ����� ��� ����� ������ ������������
        case 7: Data_=strcpy(Data_,"!g")+2+1; break;  // ��������� ������ ���
        case 8: Data_=strcpy(Data_,"!f")+2+1; break;  // �����
        case 9: Data_=strcpy(Data_,"!r")+2+1; break;  // ��������� ��������
        case 10: Data_=strcpy(Data_,"!c")+2+1; break;  // ��������� �������
        case 11: Data_=strcpy(Data_,"!t")+2+1; break;  // ��������� � �������
        case 12: Data_=strcpy(Data_,"!o")+2+1; break;  // ��������� � ... ����������
        case 13: Data_=strcpy(Data_,"!i")+2+1; break;  // ��������� �� ... ���������
        case 14: Data_=strcpy(Data_,"!L")+2+1; break;  // ��������� �������
        case 15: Data_=strcpy(Data_,"!U")+2+1; break;  // ��������� ������
        case 16: Data_=strcpy(Data_,"!P")+2+1; break;  // ��������� �������������
        case 17: Data_=strcpy(Data_,"!R")+2+1; break;  // ��������� �������
        case 18: Data_=strcpy(Data_,"!B")+2+1; break;  // ��������� ������������
        case 19: Data_=strcpy(Data_,"!S")+2+1; break;  // ��������� ��������
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

    // ���������� ������ ������ ��� ����������
    data_size=GetLogRunDataSize(Tariff_);
    // �������� ������ ��� ������
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // ������� ������ ���������� ��� ����������
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

    // ���������� ������ ������ ��� ����������
    data_size=GetLogCloseDataSize();
    // �������� ������ ��� ������
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // ������� ������ ���������� ��� ����������
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

