//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include "UnitOptionsLoadSave.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
int GetTariffsDataSize(MTariffs *Tariffs_)
{
    int Size=0;
    for ( MTariff *Tariff=(MTariff*)Tariffs_->FirstItem; Tariff;
        Tariff=(MTariff*)Tariff->NextItem )
    {
        Size+=
            sizeof(unsigned int)+
            Tariff->Name.Length()+1+
            Tariff->IconFileName.Length()+1+
            sizeof(int)+
            sizeof(unsigned int)+
            sizeof(bool)+
            Tariff->Times.CountItems*(sizeof(int)+sizeof(int)+sizeof(int)+
            sizeof(unsigned int)+sizeof(double)+sizeof(bool))+sizeof(int)+
            Tariff->Computers.CountItems*sizeof(unsigned int)+sizeof(unsigned int);
    }
    return Size;
}
//---------------------------------------------------------------------------
char *SetTariffsData(char *Data_, MTariffs *Tariffs_)
{
    for ( MTariff *Tariff=(MTariff*)Tariffs_->FirstItem; Tariff;
        Tariff=(MTariff*)Tariff->NextItem )
    {
        // ����� ����������
        *((unsigned int*)Data_)=Tariff->ID; Data_+=sizeof(unsigned int);
        Data_=strcpy(Data_,Tariff->Name.c_str())+Tariff->Name.Length()+1;
        Data_=strcpy(Data_,Tariff->IconFileName.c_str())+Tariff->IconFileName.Length()+1;
        *((int*)Data_)=Tariff->FaceType; Data_+=sizeof(int);
        *((unsigned int*)Data_)=Tariff->GamesPages; Data_+=sizeof(unsigned int);
        *((bool*)Data_)=Tariff->Reboot; Data_+=sizeof(bool);
        // ��������� �� �������
        for ( MTariffTime *TariffTime=(MTariffTime*)Tariff->Times.FirstItem; TariffTime;
            TariffTime=(MTariffTime*)TariffTime->NextItem )
        {
            *((int*)Data_)=TariffTime->BeginTime; Data_+=sizeof(int);
            *((int*)Data_)=TariffTime->SizeTime; Data_+=sizeof(int);
            *((int*)Data_)=TariffTime->EndTime; Data_+=sizeof(int);
            *((unsigned int*)Data_)=TariffTime->WeekDays; Data_+=sizeof(unsigned int);
            *((double*)Data_)=TariffTime->Cost; Data_+=sizeof(double);
            *((bool*)Data_)=TariffTime->EnableDesc; Data_+=sizeof(bool);
        }
        // ������ ����� ������ (��� �������� �� ������������ ��� BeginTime)
        *((int*)Data_)=-1; Data_+=sizeof(int);
        // ������ �����������
        for ( MListOfIDsID *ListOfIDsID=(MListOfIDsID*)Tariff->Computers.FirstItem;
            ListOfIDsID; ListOfIDsID=(MListOfIDsID*)ListOfIDsID->NextItem )
        {
            *((unsigned int*)Data_)=ListOfIDsID->ID; Data_+=sizeof(unsigned int);
        }
        // ������ ����� ������ (��� �������� �� ������������ ��� ID)
        *((unsigned int*)Data_)=-1; Data_+=sizeof(unsigned int);
    }
    return Data_;
}
//---------------------------------------------------------------------------
char *GetTariffsData(char *Data_, char *LimitData_, MTariffs *Tariffs_)
{
    MTariff *NewTariff;
    MTariffTime *NewTariffTime;
    MListOfIDsID *NewListOfIDsID;
    int str_length;

    while(Data_<LimitData_)
    {
        // ��������� ������
        if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
        if ( *((int*)Data_)==(-1) ) { Data_+=sizeof(unsigned int); break; }
        //
        NewTariff=(MTariff*)Tariffs_->Add();
        // ID-����� ������
        if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
        NewTariff->ID=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
        // �������� ������
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=64) ) return NULL;
        NewTariff->Name=Data_; Data_+=str_length+1;
        // ��� ����� � �������
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=MAX_PATH) ) return NULL;
        NewTariff->IconFileName=Data_; Data_+=str_length+1;
        // ��� ���������� ���������� ����� ���������
        if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
        NewTariff->FaceType=*((int*)Data_); Data_+=sizeof(int);
        // ������� � ����������� ��� �������
        if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
        NewTariff->GamesPages=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
        //
        if ( (Data_+sizeof(bool))>LimitData_ ) return NULL;
        NewTariff->Reboot=*((bool*)Data_); Data_+=sizeof(bool);
        // ��������� �� �������
        while(Data_<LimitData_)
        {
            // ��������� ������
            if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
            if ( *((int*)Data_)==(-1) ) { Data_+=sizeof(int); break; }
            //
            NewTariffTime=(MTariffTime*)NewTariff->Times.Add();
            // ��������� ����� ��������
            if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
            NewTariffTime->BeginTime=*((int*)Data_); Data_+=sizeof(int);
            // ����� ��������
            if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
            NewTariffTime->SizeTime=*((int*)Data_); Data_+=sizeof(int);
            // �������� ����� ��������
            if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
            NewTariffTime->EndTime=*((int*)Data_); Data_+=sizeof(int);
            // ��� ������
            if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
            NewTariffTime->WeekDays=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
            // ���������
            if ( (Data_+sizeof(double))>LimitData_ ) return NULL;
            NewTariffTime->Cost=*((double*)Data_); Data_+=sizeof(double);
            // ������� ����������� ������ �� �����
            if ( (Data_+sizeof(bool))>LimitData_ ) return NULL;
            NewTariffTime->EnableDesc=*((bool*)Data_); Data_+=sizeof(bool);
        }
        // ������ �����������
        while(true)
        {
            // ��������� ������
            if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
            if ( *((int*)Data_)==(-1) ) { Data_+=sizeof(unsigned int); break; }
            //
            NewListOfIDsID=(MListOfIDsID*)NewTariff->Computers.Add();
            // ID-����� ����������
            if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
            NewListOfIDsID->ID=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
        }
    }

    return Data_;
}
//---------------------------------------------------------------------------
bool TariffsSave(MTariffs *Tariffs_)
{
    char file_name[MAX_PATH];
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    // ���������� ������ ����������� ������
    data_size=GetTariffsDataSize(Tariffs_);
    // �������� ������ ��� ������
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // ������� ������ ���������� ��� ����������
    SetTariffsData(all_data,Tariffs_);
    //
    BasicCode(all_data,data_size,0x5DCF39EA);

    strcpy(file_name,PrgDir.c_str());
    strcat(file_name,"TARIFFS.DAT");
    // ������� ���� ��� ���������� ������ �������
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
bool TariffsLoad(MTariffs *Tariffs_, TImageList *ImageList_)
{
    char file_name[MAX_PATH];
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    strcpy(file_name,PrgDir.c_str());
    strcat(file_name,"TARIFFS.DAT");
    // ��������� ���� �� ������� �������
    if ( (file=::CreateFile(file_name,GENERIC_READ,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // ���������� ��� ������
    if ( (data_size=::GetFileSize(file,NULL))==0xFFFFFFFF ) goto error;
    // �������� ������ ��� ������
    if ( (all_data=new char[data_size+1])==NULL ) goto error;
    // ��������� ������ �� �����
    if ( ::ReadFile(file,all_data,data_size,&rw_size,NULL)==NULL ) goto error;
    ::CloseHandle(file); file=NULL;

    //
    BasicEncode(all_data,data_size,0x5DCF39EA);
    //
    all_data[data_size]=0; Tariffs_->Clear();
    //
    if ( GetTariffsData(all_data,all_data+data_size,Tariffs_)==NULL ) goto error;
    delete[] all_data;

    TIcon *Icon;
    Icon=new Graphics::TIcon;
    //
    for ( MTariff *Tariff=(MTariff*)Tariffs_->FirstItem; Tariff;
        Tariff=(MTariff*)Tariff->NextItem )
    {
        try { Icon->LoadFromFile(Tariff->IconFileName); }
        catch(EFOpenError *error) { continue; }
        Tariff->IconNumber=ImageList_->AddIcon(Icon);
    }
    delete Icon;

    return true;
error:
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
int GetFinesDataSize(MFines *Fines_)
{
    int Size=0;
    for ( MFine *Fine=(MFine*)Fines_->FirstItem; Fine; Fine=(MFine*)Fine->NextItem )
        Size+=sizeof(int)+Fine->Comment.Length()+1;
    return Size;
}
//---------------------------------------------------------------------------
char *SetFinesData(char *Data_, MFines *Fines_)
{
    for ( MFine *Fine=(MFine*)Fines_->FirstItem; Fine; Fine=(MFine*)Fine->NextItem )
    {
        *((int*)Data_)=Fine->Time; Data_+=sizeof(int);
        Data_=strcpy(Data_,Fine->Comment.c_str())+Fine->Comment.Length()+1;
    }
    return Data_;
}
//---------------------------------------------------------------------------
char *GetFinesData(char *Data_, char *LimitData_, MFines *Fines_)
{
    MFine *NewFine;
    int str_length;

    while(Data_<LimitData_)
    {
        NewFine=(MFine*)Fines_->Add();
        // ����� ������
        if ( (Data_+sizeof(int))>LimitData_ ) return NULL;
        NewFine->Time=*((int*)Data_); Data_+=sizeof(int);
        // ����������� � ������
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=64) ) return NULL;
        NewFine->Comment=Data_; Data_+=str_length+1;
    }

    return Data_;
}
//---------------------------------------------------------------------------
bool FinesSave(MFines *Fines_)
{
    char file_name[MAX_PATH];
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    // ���������� ������ ����������� ������
    data_size=GetFinesDataSize(Fines_);
    // �������� ������ ��� ������
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // ������� ������ ���������� ��� ����������
    SetFinesData(all_data,Fines_);
    //
    BasicCode(all_data,data_size,0x5DCF39EA);

    strcpy(file_name,PrgDir.c_str());
    strcat(file_name,"FINES.DAT");
    // ������� ���� ��� ���������� ������ �������
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
bool FinesLoad(MFines *Fines_)
{
    char file_name[MAX_PATH];
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    strcpy(file_name,PrgDir.c_str());
    strcat(file_name,"FINES.DAT");
    // ��������� ���� �� ������� �������
    if ( (file=::CreateFile(file_name,GENERIC_READ,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // ���������� ��� ������
    if ( (data_size=::GetFileSize(file,NULL))==0xFFFFFFFF ) goto error;
    // �������� ������ ��� ������
    if ( (all_data=new char[data_size+1])==NULL ) goto error;
    // ��������� ������ �� �����
    if ( ::ReadFile(file,all_data,data_size,&rw_size,NULL)==NULL ) goto error;
    ::CloseHandle(file); file=NULL;

    //
    BasicEncode(all_data,data_size,0x5DCF39EA);
    //
    all_data[data_size]=0; Fines_->Clear();
    //
    if ( GetFinesData(all_data,all_data+data_size,Fines_)==NULL ) goto error;

    delete[] all_data;
    return true;
error:
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
int GetUsersDataSize(MUsers *Users_)
{
    int Size=0;
    for ( MUser *User=(MUser*)Users_->FirstItem; User; User=(MUser*)User->NextItem )
    {
        Size+=sizeof(unsigned int);
        Size+=User->Login.Length()+1;
        Size+=User->Password.Length()+1;
        Size+=User->JobTitle.Length()+1;
        Size+=User->FullName.Length()+1;
        Size+=sizeof(unsigned int);
        Size+=sizeof(bool);
    }
    return Size;
}
//---------------------------------------------------------------------------
char *SetUsersData(char *Data_, MUsers *Users_)
{
    for ( MUser *User=(MUser*)Users_->FirstItem; User; User=(MUser*)User->NextItem )
    {
        *((unsigned int*)Data_)=User->ID; Data_+=sizeof(unsigned int);
        Data_=strcpy(Data_,User->Login.c_str())+User->Login.Length()+1;
        Data_=strcpy(Data_,User->Password.c_str())+User->Password.Length()+1;
        Data_=strcpy(Data_,User->JobTitle.c_str())+User->JobTitle.Length()+1;
        Data_=strcpy(Data_,User->FullName.c_str())+User->FullName.Length()+1;
        *((unsigned int*)Data_)=User->Rights; Data_+=sizeof(unsigned int);
        *((bool*)Data_)=User->Active; Data_+=sizeof(bool);
    }
    return Data_;
}
//---------------------------------------------------------------------------
char *GetUsersData(char *Data_, char *LimitData_, MUsers *Users_)
{
    MUser *NewUser;
    int str_length;

    while(Data_<LimitData_)
    {
        NewUser=(MUser*)Users_->Add();
        // ID-����� ������������
        if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
        NewUser->ID=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
        // ����� ������������
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=16) ) return NULL;
        NewUser->Login=Data_; Data_+=str_length+1;
        // ������ ������������ ��� ����� � ���������
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=16) ) return NULL;
        NewUser->Password=Data_; Data_+=str_length+1;
        // ��������� ������������
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=64) ) return NULL;
        NewUser->JobTitle=Data_; Data_+=str_length+1;
        // ������ ��� ������������
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=64) ) return NULL;
        NewUser->FullName=Data_; Data_+=str_length+1;
        // ����� ������� ������������
        if ( (Data_+sizeof(unsigned int))>LimitData_ ) return NULL;
        NewUser->Rights=*((unsigned int*)Data_); Data_+=sizeof(unsigned int);
        // ������� ���������� ������
        if ( (Data_+sizeof(bool))>LimitData_ ) return NULL;
        NewUser->Active=*((bool*)Data_); Data_+=sizeof(bool);
    }

    return Data_;
}
//---------------------------------------------------------------------------
bool UsersSave(MUsers *Users_)
{
    char file_name[MAX_PATH];
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    // ���������� ������ ����������� ������
    data_size=GetUsersDataSize(Users_);
    // �������� ������ ��� ������
    if ( (all_data=new char[data_size])==NULL ) goto error;
    // ������� ������ ���������� ��� ����������
    SetUsersData(all_data,Users_);
    //
    BasicCode(all_data,data_size,0x5DCF39EA);

    strcpy(file_name,PrgDir.c_str());
    strcat(file_name,"USERS.DAT");
    // ������� ���� ��� ���������� ������ �������������
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
bool UsersLoad(MUsers *Users_)
{
    char file_name[MAX_PATH];
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    strcpy(file_name,PrgDir.c_str());
    strcat(file_name,"USERS.DAT");
    // ��������� ���� �� ������� �������������
    if ( (file=::CreateFile(file_name,GENERIC_READ,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // ���������� ��� ������
    if ( (data_size=::GetFileSize(file,NULL))==0xFFFFFFFF ) goto error;
    // �������� ������ ��� ������
    if ( (all_data=new char[data_size+1])==NULL ) goto error;
    // ��������� ������ �� �����
    if ( ::ReadFile(file,all_data,data_size,&rw_size,NULL)==NULL ) goto error;
    ::CloseHandle(file); file=NULL;

    //
    BasicEncode(all_data,data_size,0x5DCF39EA);
    //
    all_data[data_size]=0; Users_->Clear();
    //
    if ( GetUsersData(all_data,all_data+data_size,Users_)==NULL ) goto error;

    delete[] all_data;
    return true;
error:
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------

