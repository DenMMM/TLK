//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitShared.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
std::wstring MShared::InhToHEX() const
{
    MInheritData Data;
    wchar_t hex[sizeof(Data)*2+1];

    // Заполним поля
    Data.hMap=hMap;
    Data.hMtxMap=hMtxMap;
    Data.hMtxLive=hMtxLive;
    // Зашифруем
	BasicEncode(&Data,sizeof(Data),ENC_Code);	/// смысл есть ?
	// Сконвертируем в HEX-строку
	ByteToHEX(
		&Data, sizeof(Data),
		hex, sizeof(hex)/sizeof(hex[0]),
		L'\0');

	return std::wstring(hex);
}
//---------------------------------------------------------------------------
bool MShared::Create()
{
    SECURITY_ATTRIBUTES sa;

    sa.nLength=sizeof(sa);
    sa.lpSecurityDescriptor=nullptr;
    sa.bInheritHandle=TRUE;

    // Создадим область общей памяти
    hMap=::CreateFileMapping(INVALID_HANDLE_VALUE,&sa,
        PAGE_READWRITE,0,sizeof(MSharedData),nullptr);
    if ( hMap==nullptr ) goto error;
	pData=reinterpret_cast<MSharedData*>(
		::MapViewOfFile(hMap,FILE_MAP_WRITE,0,0,0));
    if ( pData==nullptr ) goto error;
    // Создадим мьютекс для синхронизации доступа к ней
    hMtxMap=::CreateMutex(&sa,FALSE,nullptr);
    if ( hMtxMap==nullptr ) goto error;
    // Создадим мьютекс-маркер состояния процесса службы
    hMtxLive=::CreateMutex(&sa,TRUE,nullptr);
    if ( hMtxLive==nullptr ) goto error;

    return true;
error:
#ifdef _DEBUG
{
    wchar_t msg[256+1];
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,nullptr,
		::GetLastError(),MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		(LPTSTR)msg,256,nullptr);
	::MessageBox(nullptr, msg, L"MShared::Create()", MB_SERVICE_NOTIFICATION);
}
#endif
    return false;
}
//---------------------------------------------------------------------------
bool MShared::Open(wchar_t *InhLine_)
{
	MInheritData Data;

	// Восстановим из HEX-строки наследованные хэндлы
	if ( HEXToByte(InhLine_, &Data, sizeof(Data))!=sizeof(Data) ) return false;
	// Расшифруем
	BasicDecode(&Data, sizeof(Data), ENC_Code);
	//
    hMap=Data.hMap;
    hMtxMap=Data.hMtxMap;
    hMtxLive=Data.hMtxLive;
    // Отобразим общий блок памяти в пространство процесса
	pData=reinterpret_cast<MSharedData*>(
		::MapViewOfFile(hMap,FILE_MAP_READ,0,0,sizeof(MSharedData)));
    return pData!=nullptr;
}
//---------------------------------------------------------------------------
void MShared::Close()
{
    ::UnmapViewOfFile(pData); pData=nullptr;
    ::CloseHandle(hMtxLive); hMtxLive=nullptr;
    ::CloseHandle(hMtxMap); hMtxMap=nullptr;
	::CloseHandle(hMap); hMap=nullptr;
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
bool MShared::CheckSysTime(__int64 &SysTime_, unsigned &SysTimeVer_) const
{
	if ( SysTimeVer_==pData->SysTimeVer ) return false;
	SysTime_=pData->SysTime;
	SysTimeVer_=pData->SysTimeVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckCompNum(int &Num_, unsigned &NumVer_) const
{
	if ( NumVer_==pData->CompNumVer ) return false;
	Num_=pData->CompNum;
    NumVer_=pData->CompNumVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckWorkTime(int &Time_, unsigned &TimeVer_) const
{
	if ( TimeVer_==pData->WorkTimeVer ) return false;
	Time_=pData->WorkTime;
    TimeVer_=pData->WorkTimeVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckWarnMsg(bool &Msg_, unsigned &MsgVer_) const
{
	if ( MsgVer_==pData->WarnMsgVer ) return false;
	Msg_=pData->WarnMsg;
    MsgVer_=pData->WarnMsgVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckImageMessage(int &Msg_, unsigned &MsgVer_) const
{
	if ( MsgVer_==pData->ImageMsgVer ) return false;
	Msg_=pData->ImageMsg;
    MsgVer_=pData->ImageMsgVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckGames(unsigned &Games_, unsigned &GamesVer_) const
{
	if ( GamesVer_==pData->GamesVer ) return false;
	Games_=pData->Games;
    GamesVer_=pData->GamesVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckConfigMode(bool &Mode_, unsigned &ModeVer_) const
{
	if ( ModeVer_==pData->ConfigModeVer ) return false;
	Mode_=pData->ConfigMode;
    ModeVer_=pData->ConfigModeVer;
    return true;
}
//---------------------------------------------------------------------------
bool MShared::CheckTransp(bool &Transp_, unsigned &TranspVer_) const
{
	if ( TranspVer_==pData->TranspVer ) return false;
	Transp_=pData->Transp;
    TranspVer_=pData->TranspVer;
    return true;
}
//---------------------------------------------------------------------------

