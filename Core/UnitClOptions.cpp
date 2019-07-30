//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitClOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void MClOptions::SetShellUser(const std::wstring &Name_)
{
	if ( Name_.length()>MAX_ClUNameLen )
	{
		throw std::out_of_range (
			"MClOptions::SetShellUser()\n"
			"Длина имени превышет MAX_ClUNameLen."
			);
	}

	ShellUser=Name_;
}

bool MClOptions::Copy(MClOptions *ClOptions_)
{
	ShellUser=ClOptions_->ShellUser;
    ToEndTime=ClOptions_->ToEndTime;
    MessageTime=ClOptions_->MessageTime;
    MsgEndTime=ClOptions_->MsgEndTime;
    RebootWait=ClOptions_->RebootWait;
    AutoLockTime=ClOptions_->AutoLockTime;
    Flags=ClOptions_->Flags;
    return true;
}

unsigned MClOptions::GetDataSize() const
{
    return
		sizeofLine(ShellUser)+
		sizeof(ToEndTime)+
        sizeof(MessageTime)+
        sizeof(MsgEndTime)+
        sizeof(RebootWait)+
        sizeof(AutoLockTime)+
        sizeof(Flags);
}

void *MClOptions::SetData(void *Data_) const
{
	Data_=MemSetLine(Data_,ShellUser);
	Data_=MemSet(Data_,ToEndTime);
	Data_=MemSet(Data_,MessageTime);
	Data_=MemSet(Data_,MsgEndTime);
	Data_=MemSet(Data_,RebootWait);
	Data_=MemSet(Data_,AutoLockTime);
	Data_=MemSet(Data_,Flags);
	return Data_;
}

const void *MClOptions::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGetLine(Data_,ShellUser,MAX_ClUNameLen,Limit_)) &&
		(Data_=MemGet(Data_,&ToEndTime,Limit_)) &&
		(Data_=MemGet(Data_,&MessageTime,Limit_)) &&
		(Data_=MemGet(Data_,&MsgEndTime,Limit_)) &&
		(Data_=MemGet(Data_,&RebootWait,Limit_)) &&
		(Data_=MemGet(Data_,&AutoLockTime,Limit_)) &&
		(Data_=MemGet(Data_,&Flags,Limit_))
		? Data_: nullptr;
}
//---------------------------------------------------------------------------
