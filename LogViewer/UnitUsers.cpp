//---------------------------------------------------------------------------
#include <string.h>
#pragma hdrstop

#include "UnitUsers.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

MUser::MUser()
{
    ID=0;
    *Login=0;
    *Password=0;
    *Name=0;
    Active=true;
}

MUser::~MUser()
{
//
}

bool MUser::Copy(MListItem *SrcItem_)
{
    MUser *User_=(MUser*)SrcItem_;
    ID=User_->ID;
    strcpy(Login,User_->Login);
    strcpy(Password,User_->Password);
    strcpy(Name,User_->Name);
    Active=User_->Active;
    return true;
}

unsigned MUser::GetDataSize()
{
    return
        sizeof(ID)+
        strlen(Login)+1+
        strlen(Password)+1+
        strlen(Name)+1+
        sizeof(Active);
}

char *MUser::SetData(char *Data_)
{
    Data_=MemSet(Data_,ID);
    Data_=MemSetCLine(Data_,Login);
    Data_=MemSetCLine(Data_,Password);
    Data_=MemSetCLine(Data_,Name);
    Data_=MemSet(Data_,Active);
    return Data_;
}

char *MUser::GetData(char *Data_, char *Limit_)
{
    if ( (Data_=MemGet(Data_,&ID,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetCLine(Data_,Login,MAX_UserLoginLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetCLine(Data_,Password,MAX_UserPasswordLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGetCLine(Data_,Name,MAX_UserNameLength,Limit_))==NULL ) goto error;
    if ( (Data_=MemGet(Data_,&Active,Limit_))==NULL ) goto error;
    return Data_;
error:
    return NULL;
}

bool MUser::SetLogin(char *Login_)
{
    return strlen(Login_)>MAX_UserLoginLength?
        false: (bool)strcpy(Login,Login_);
}

bool MUser::SetPassword(char *Password_)
{
    return strlen(Password_)>MAX_UserPasswordLength?
        false: (bool)strcpy(Password,Password_);
}

bool MUser::SetName(char *Name_)
{
    return strlen(Name_)>MAX_UserNameLength?
        false: (bool)strcpy(Name,Name_);
}

bool MUser::CheckPassword(char *Password_)
{
    return strcmp(Password,Password_)==0;
}

void MUser::GetUserData(MUserData *Data_)
{
    Data_->ID=ID;
    strcpy(Data_->Login,Login);
    strcpy(Data_->Name,Name);
}

void MUser::SetUserData(MUserData *Data_)
{
    ID=Data_->ID;
    strcpy(Login,Data_->Login);
    strcpy(Name,Data_->Name);
}

//---------------------------------------------------------------------------

unsigned MUsers::NextID()
{
    do { LastID+=random(10)+1; } while(!LastID);
    return LastID;
}

void MUsers::SetIDs()
{
    for ( MUser *User=(MUser*)First; User;
        User=(MUser*)User->Next ) if ( User->ID==0 ) User->ID=NextID();
}

MUser *MUsers::Search(unsigned ID_)
{
    MUser *User=(MUser*)First;
    for ( ; User; User=(MUser*)User->Next ) if ( User->ID==ID_ ) break;
    return User;
}

//---------------------------------------------------------------------------

