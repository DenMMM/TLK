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
    *Login=0;
    *Name=0;
    Active=true;
}

MUser::~MUser()
{
//
}

void MUser::Copy(const MListItem *SrcItem_)
{
    MUser *User_=(MUser*)SrcItem_;

    strcpy(Login,User_->Login);
    strcpy(Name,User_->Name);
    Active=User_->Active;
    Pass.Copy(&User_->Pass);
    MIDListItem::Copy(SrcItem_);
}

unsigned MUser::GetDataSize() const
{
    return
        strlen(Login)+1+
        strlen(Name)+1+
        sizeof(Active)+
        Pass.GetDataSize()+
        MIDListItem::GetDataSize();     // ID-номер пользователя
}

char *MUser::SetData(char *Data_) const
{
    Data_=MIDListItem::SetData(Data_);
    Data_=MemSetCLine(Data_,Login);
    Data_=MemSetCLine(Data_,Name);
    Data_=Pass.SetData(Data_);
    Data_=MemSet(Data_,Active);
    return Data_;
}

const char *MUser::GetData(const char *Data_, const char *Limit_)
{
    return
        (Data_=MIDListItem::GetData(Data_,Limit_))!=NULL &&
        (Data_=MemGetCLine(Data_,Login,MAX_UserLoginLen,Limit_))!=NULL &&
        (Data_=MemGetCLine(Data_,Name,MAX_UserNameLen,Limit_))!=NULL &&
        (Data_=Pass.GetData(Data_,Limit_))!=NULL &&
        (Data_=MemGet(Data_,&Active,Limit_))!=NULL
        ? Data_: NULL;
}

bool MUser::SetLogin(char *Login_)
{
    return strlen(Login_)>MAX_UserLoginLen?
        false: (bool)strcpy(Login,Login_);
}

bool MUser::SetName(char *Name_)
{
    return strlen(Name_)>MAX_UserNameLen?
        false: (bool)strcpy(Name,Name_);
}

void MUser::GetUserData(MUserData *Data_) const
{
    Data_->ID=ItemID;
    strcpy(Data_->Login,Login);
    strcpy(Data_->Name,Name);
}

void MUser::SetUserData(MUserData *Data_)
{
    ItemID=Data_->ID;
    strcpy(Login,Data_->Login);
    strcpy(Name,Data_->Name);
}
//---------------------------------------------------------------------------
unsigned MUsers::ActiveCount() const
{
    unsigned num=0;

    for ( MUser *User=(MUser*)gFirst(); User;
        User=(MUser*)User->gNext() ) if ( User->Active ) num++;

    return num;
}
//---------------------------------------------------------------------------

