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
    Active=true;
}

MUser::~MUser()
{
//
}

void MUser::Copy(const MListItem *SrcItem_)
{
    const MUser *usr=
        dynamic_cast<const MUser*>(SrcItem_);

    Login=usr->Login;
    Name=usr->Name;
    Active=usr->Active;
    Pass.Copy(&usr->Pass);
    MIDListItem::Copy(SrcItem_);
}

unsigned MUser::GetDataSize() const
{
    return
        Login.length()+sizeof('\0')+
        Name.length()+sizeof('\0')+
        sizeof(Active)+
        Pass.GetDataSize()+
        MIDListItem::GetDataSize();     // ID-����� ������������
}

char *MUser::SetData(char *Data_) const
{
    Data_=MIDListItem::SetData(Data_);
    Data_=MemSetCLine(Data_,Login.c_str());
    Data_=MemSetCLine(Data_,Name.c_str());
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

/*void MUser::SetLogin(char *Login_)
{
    if ( strlen(Login_)>MAX_UserLoginLen )
    {
        throw std::runtime_error (
            "MUser::SetLogin()\n"
            "����� ������ ��������� MAX_UserLoginLen."
            );

    }
    strcpy(Login,Login_);
}

void MUser::SetName(char *Name_)
{
    if ( strlen(Name_)>MAX_UserNameLen )
    {
        throw std::runtime_error (
            "MUser::SetName()\n"
            "����� ������ ��������� MAX_UserNameLen."
            );
    }
    strcpy(Name,Name_);
}*/


//---------------------------------------------------------------------------
unsigned MUsers::ActiveCount() const
{
    unsigned num=0;

    for ( MUser *User=(MUser*)gFirst(); User;
        User=(MUser*)User->gNext() ) if ( User->Active ) num++;

    return num;
}
//---------------------------------------------------------------------------

