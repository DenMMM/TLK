//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitShared.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
std::string MShared::InhToHEX() const
{
    MInheritData Data;
    char hex[sizeof(Data)*2+2]; /// "+2" - баг в ByteToHEX()

    // Заполним поля
    Data.hMap=hMap;
    Data.hMtxMap=hMtxMap;
    Data.hMtxLive=hMtxLive;
    // Зашифруем
    BasicEncode((char*)&Data,sizeof(Data),ENC_Code);    /// смысл есть ?
    // Сконвертируем в HEX-строку
    ByteToHEX((char*)&Data,sizeof(Data),hex,sizeof(hex)-1,'\0');

    return std::string(hex);
}
//---------------------------------------------------------------------------
bool MShared::Create()
{
    SECURITY_ATTRIBUTES sa;

    sa.nLength=sizeof(sa);
    sa.lpSecurityDescriptor=NULL;
    sa.bInheritHandle=TRUE;

    // Создадим область общей памяти
    hMap=::CreateFileMapping(INVALID_HANDLE_VALUE,&sa,
        PAGE_READWRITE,0,sizeof(MSharedData),NULL);
    if ( hMap==NULL ) goto error;
    pData=(MSharedData*)::MapViewOfFile(hMap,FILE_MAP_WRITE,0,0,0);
    if ( pData==NULL ) goto error;
    // Создадим мьютекс для синхронизации доступа к ней
    hMtxMap=::CreateMutex(&sa,FALSE,NULL);
    if ( hMtxMap==NULL ) goto error;
    // Создадим мьютекс-маркер состояния процесса службы
    hMtxLive=::CreateMutex(&sa,TRUE,NULL);
    if ( hMtxLive==NULL ) goto error;

    return true;
error:
#ifdef _DEBUG
{
    char msg[256+1];
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,
        ::GetLastError(),MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
        (LPTSTR)msg,256,NULL);
    ::MessageBox(NULL,msg,"MShared::Create()",MB_SERVICE_NOTIFICATION);
}
#endif
    return false;
}
//---------------------------------------------------------------------------
bool MShared::Open(char *InhLine_)
{
    MInheritData Data;

    // Восстановим из HEX-строки наследованные хэндлы
    if ( HEXToByte(InhLine_,(char*)&Data,sizeof(Data))!=sizeof(Data) ) return false;
    // Расшифруем
    BasicDecode((char*)&Data,sizeof(Data),ENC_Code);
    //
    hMap=Data.hMap;
    hMtxMap=Data.hMtxMap;
    hMtxLive=Data.hMtxLive;
    // Отобразим общий блок памяти в пространство процесса
    pData=(MSharedData*)::MapViewOfFile(hMap,FILE_MAP_READ,0,0,sizeof(MSharedData));
    return pData!=NULL;
}
//---------------------------------------------------------------------------
void MShared::Close()
{
    ::UnmapViewOfFile(pData); pData=NULL;
    ::CloseHandle(hMtxLive); hMtxLive=NULL;
    ::CloseHandle(hMtxMap); hMtxMap=NULL;
    ::CloseHandle(hMap); hMap=NULL;
}
//---------------------------------------------------------------------------
void MShared::UpdateSysTime(__int64 SysTime_)
{
    pData->SysTime=SysTime_;
    NextVer(&pData->SysTimeVer);
}
//---------------------------------------------------------------------------
void MShared::UpdateCompNum(int Num_)
{
    pData->CompNum=Num_;
    NextVer(&pData->CompNumVer);
}
//---------------------------------------------------------------------------
void MShared::UpdateWorkTime(int Time_)
{
    pData->WorkTime=Time_;
    NextVer(&pData->WorkTimeVer);
}
//---------------------------------------------------------------------------
void MShared::ShowWarnMsg(bool Show_)
{
    pData->WarnMsg=Show_;
    NextVer(&pData->WarnMsgVer);
}
//---------------------------------------------------------------------------
void MShared::ShowImageMessage(int Msg_)
{
    pData->ImageMsg=Msg_;
    NextVer(&pData->ImageMsgVer);
}
//---------------------------------------------------------------------------
void MShared::ShowGames(unsigned Games_)
{
    pData->Games=Games_;
    NextVer(&pData->GamesVer);
}
//---------------------------------------------------------------------------
void MShared::SetConfigMode(bool Mode_)
{
    pData->ConfigMode=Mode_;
    NextVer(&pData->ConfigModeVer);
}
//---------------------------------------------------------------------------
void MShared::SetTransp(bool Transp_)
{
    pData->Transp=Transp_;
    NextVer(&pData->TranspVer);
}
//---------------------------------------------------------------------------
bool MShared::CheckSysTime(__int64 *SysTime_, unsigned *SysTimeVer_) const
{
    if ( *SysTimeVer_==pData->SysTimeVer ) return false;
    *SysTime_=pData->SysTime;
    *SysTimeVer_=pData->SysTimeVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckCompNum(int *Num_, unsigned *NumVer_) const
{
    if ( *NumVer_==pData->CompNumVer ) return false;
    *Num_=pData->CompNum;
    *NumVer_=pData->CompNumVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckWorkTime(int *Time_, unsigned *TimeVer_) const
{
    if ( *TimeVer_==pData->WorkTimeVer ) return false;
    *Time_=pData->WorkTime;
    *TimeVer_=pData->WorkTimeVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckWarnMsg(bool *Msg_, unsigned *MsgVer_) const
{
    if ( *MsgVer_==pData->WarnMsgVer ) return false;
    *Msg_=pData->WarnMsg;
    *MsgVer_=pData->WarnMsgVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckImageMessage(int *Msg_, unsigned *MsgVer_) const
{
    if ( *MsgVer_==pData->ImageMsgVer ) return false;
    *Msg_=pData->ImageMsg;
    *MsgVer_=pData->ImageMsgVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckGames(unsigned *Games_, unsigned *GamesVer_) const
{
    if ( *GamesVer_==pData->GamesVer ) return false;
    *Games_=pData->Games;
    *GamesVer_=pData->GamesVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckConfigMode(bool *Mode_, unsigned *ModeVer_) const
{
    if ( *ModeVer_==pData->ConfigModeVer ) return false;
    *Mode_=pData->ConfigMode;
    *ModeVer_=pData->ConfigModeVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckTransp(bool *Transp_, unsigned *TranspVer_) const
{
    if ( *TranspVer_==pData->TranspVer ) return false;
    *Transp_=pData->Transp;
    *TranspVer_=pData->TranspVer;
    return true;
}
//---------------------------------------------------------------------------

