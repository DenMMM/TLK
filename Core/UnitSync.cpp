//---------------------------------------------------------------------------
#include <cstdlib>
#include <stdexcept>
#include <winsock2.h>
#include <ip2string.h>
#include <ntstatus.h>
#include <iphlpapi.h>
#pragma hdrstop

#include "UnitSync.h"
#include "UnitAuth.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
unsigned MSyncStatesItem::GetDataSize() const
{
    return KnownMAC?
        sizeof(IP)+
        sizeof(MAC): 0;
}
//---------------------------------------------------------------------------
void *MSyncStatesItem::SetData(void *Data_) const
{
    if ( !KnownMAC ) return Data_;
    Data_=MemSet(Data_,IP);
    Data_=MemSetBin(Data_,MAC,sizeof(MAC));
    return Data_;
}
//---------------------------------------------------------------------------
const void *MSyncStatesItem::GetData(const void *Data_, const void *Limit_)
{
	if ( !(
		(Data_=MemGet(Data_,&IP,Limit_)) &&
		(Data_=MemGetBin(Data_,MAC,sizeof(MAC),Limit_))
		) ) return nullptr;

	KnownMAC=true;

	return Data_;
}
//---------------------------------------------------------------------------
void MSyncStatesItem::Associate(MStatesItem *State_, u_long IP_)
{
    State=State_;
    KnownMAC=(IP==IP_);
    IP=IP_;
    Process=mspNone;
}
//---------------------------------------------------------------------------
bool MSyncStatesItem::UpdateMAC(unsigned char *MAC_)
{
    bool NeedSave;
    // Проверяем нужно ли обновить MAC-адрес
    NeedSave=(!KnownMAC)||(memcmp(MAC,MAC_,MAC_AddrLength)!=0);
    // При необходимости обновляем MAC-адрес
    if ( NeedSave ) { memcpy(MAC,MAC_,MAC_AddrLength); KnownMAC=true; }
    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncStatesItem::Start()
{
    if ( (State==nullptr)||(IP==INADDR_NONE) ) return false;
    Process=mspWait;
    LastSendTime=::GetTickCount()-SYNC_WaitInterval;
    return true;
}
//---------------------------------------------------------------------------
bool MSyncStatesItem::Stop()
{
    bool NeedSave=false;
    switch(Process)
    {
        case mspMagic:
        case mspHello:
        case mspSyncData:
            NeedSave=State->NetEnd();
            break;
        default:
            break;
    }
    Process=mspNone;
    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncStatesItem::Send(SOCKET Socket_, SOCKET SocketBC_, unsigned Code_, MAuth *MAC_)
{
    bool NeedSave=false;

    switch(Process)
    {
        case mspWait:
            if ( !State->NetBegin() ) break;
            if ( KnownMAC&&State->NetPwrOnNeed() )
            {
                // Задаем широковещательный адрес
                memset(&Address,0,sizeof(Address));
                Address.sin_family=AF_INET;
                Address.sin_port=::htons(0);
                Address.sin_addr.s_addr=INADDR_BROADCAST;
                // Заполняем magic-пакет
                Packet.WOL.ZeroFirst=0;
                memset(Packet.WOL.Sync,0xFF,MAC_AddrLength);
                for ( int i=0; i<16; i++ )
                    memcpy(Packet.WOL.Magic[i],MAC,MAC_AddrLength);
                // Задаем размер пакета и включаем его отправку
                PacketSize=sizeof(Packet.WOL);
                LastSendTime=::GetTickCount()-SYNC_SendInterval;
                SendCount=0; Process=mspMagic;
            } else if ( State->NetSyncNewData()||
                ((::GetTickCount()-LastSendTime)>=SYNC_WaitInterval) )
            {
                // Заполняем адрес получателя для всех пакетов в сеансе
                memset(&Address,0,sizeof(Address));
                Address.sin_family=AF_INET;
                Address.sin_port=::htons(SYNC_Port);
                Address.sin_addr.s_addr=IP;
                // Запрашиваем новые данные для синхронизации после обмена hello
                State->NetSyncData(&SyncData);
                //
                Seed=BasicRand();
                // Заполняем заголовок hello-пакета со своим random
                Packet.Hello.Header.Version=SYNC_Version;
                Packet.Hello.Header.Type=mptHello;
                Packet.Hello.Header.Seed=Seed;
                // Добавляем MAC
				MAC_->Calc(&Packet.Hello,
					sizeof(Packet.Hello)-sizeof(Packet.Hello.MAC),
					Packet.Hello.MAC,sizeof(Packet.Hello.MAC));
				// Задаем размер пакета для отправки и шифруем его
				PacketSize=sizeof(Packet.Hello);
                BasicEncode(&Packet,PacketSize,Code_);
                // Включаем отправку, отложив ее на случайное время
                LastSendTime=::GetTickCount()-Seed%SYNC_SendInterval;
                SendCount=0; Process=mspHello;
            } else NeedSave=State->NetEnd();
            break;
        case mspMagic:
        case mspHello:
        case mspSyncData:
            // Проверяем не пора ли отправить следующий пакет
            if ( (::GetTickCount()-LastSendTime)<SYNC_SendInterval ) break;
            // Проверяем не отправлено ли заданное количество пакетов
            if ( SendCount>=SYNC_SendRetryes )
            {
                // Завершаем сетевые операции с компьютером (WOL-успешно, SYNC-нет)
                if ( Process==mspMagic ) State->NetPwrOnExecuted();
                else State->NetSyncExecuted(false);
                NeedSave=State->NetEnd();
                // Переходим в режим ожидания новых данных
                Process=mspWait;
            } else
            {
                // Отправляем пакет
                ::sendto(Process==mspMagic?SocketBC_:Socket_,
                    (char*)&Packet,PacketSize,0,
                    (sockaddr*)&Address,sizeof(Address));
                // Сохраняем время отправки пакета и увеличиваем счетчик
                LastSendTime=::GetTickCount(); SendCount++;
            }
            break;
        default: break;
    }

    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncStatesItem::Recv(char *Packet_, int PacketSize_, unsigned Code_, MAuth *MAC_)
{
    MSyncPHeader *RecvHeader=(MSyncPHeader*)Packet_;
    bool NeedSave=false;

    if ( Process==mspNone ) goto error;
    // Проверяем допустимые размеры пакета
    if ( (PacketSize_<SYNC_MinSize)||
        (PacketSize_>SYNC_MaxSize) ) goto error;
    // Расшифровываем его
    BasicDecode(Packet_,PacketSize_,Code_);
    // Сверяем версию и MAC
    if ( (RecvHeader->Version!=SYNC_Version)||
        (!MAC_->Check(Packet_,PacketSize_-MAC_Size,
        Packet_+PacketSize_-MAC_Size,MAC_Size)) ) goto error;

    // Обрабатываем пакет
    switch((int)RecvHeader->Type)
    {
        case mptHello:
            // Ждем ли мы этот пакет и верный ли у него размер ?
            if ( (Process!=mspHello)||
                (PacketSize_!=sizeof(MSyncPHello)) ) break;
            // Адрес получателя не трогаем, он заполнен для mspHello
//            memset(&Address,0,sizeof(Address));
//            Address.sin_family=AF_INET;
//            Address.sin_port=::htons(SYNC_Port);
//            Address.sin_addr.s_addr=::inet_addr(IP);
            // Можем сгенерировать сеансовый ID из своего random и клиента
            Seed=BasicMix(Seed,RecvHeader->Seed);
            // Заполняем заголовок пакета
            Packet.Data.Header.Version=SYNC_Version;
            Packet.Data.Header.Type=mptSyncData;
            Packet.Data.Header.Seed=Seed;
            // Копируем в пакет запрошенные еще на этапе hello данные для синхронизации
            Packet.Data.Data=SyncData;
            // Добавляем MAC
            MAC_->Calc(&Packet.Data,
                sizeof(Packet.Data)-sizeof(Packet.Data.MAC),
                Packet.Data.MAC,sizeof(Packet.Data.MAC));
            // Задаем размер пакета для отправки и шифруем его
            PacketSize=sizeof(Packet.Data);
            BasicEncode(&Packet,PacketSize,Code_);
            //  Включаем отправку
            LastSendTime=::GetTickCount()-SYNC_SendInterval;
            SendCount=0; Process=mspSyncData;
            break;
        case mptSyncConf:
            // Ждем ли мы этот пакет, верный ли у него размер и seed ?
            if ( (Process!=mspSyncData)||
                (PacketSize_!=sizeof(MSyncPConf))||
                (RecvHeader->Seed!=Seed) ) break;
            // Завершаем сетевые операции с компьютером успешно
            State->NetSyncExecuted(true);
            NeedSave=State->NetEnd();
            // Переходим в режим ожидания новых данных
            Process=mspWait;
            break;
        default: break;
    }

error:
    return NeedSave;
}
//---------------------------------------------------------------------------
void MSyncStates::Associate(MStates *States_, MComputers *Computers_)
{
	in_addr IP_inaddr;
	u_long IP;

	auto iLastSyncState=begin();
	auto iEnd=end();

	for ( auto &State: *States_ )
	{
		// Ищем компьютер, с которым ассоциировано состояние
		auto iComputer=Computers_->Search(State.Associated());
		if ( iComputer==Computers_->end() ) continue;
		// Конвертируем IP-адрес из текста в u_long
//		IP=::inet_addr(Computer->Address.c_str());
//		if ( IP==INADDR_NONE ) continue;
		const wchar_t *term;
		if ( ::RtlIpv4StringToAddressW(
			iComputer->Address.c_str(),
			TRUE,
			&term,               				/// nullptr allowed ?
			&IP_inaddr
			) != STATUS_SUCCESS ) continue;
		IP=IP_inaddr.S_un.S_addr;

		// Ищем по этому адресу состояние синхронизации
		// (чтобы сохранить возможно известный MAC-адрес)
		// или добавляем новое
		auto iSyncState=Search(IP);
		if ( iSyncState==iEnd ) iSyncState=iterator(&Add(),this);   /// подумать над 'this'
		// Ассоциируем состояние синхронизации с состоянием компьютера и его адресом
		iSyncState->Associate(&State,IP);
		// Если состояние не последнее в списке, переносим его к началу
		if ( iLastSyncState!=iEnd ) Swap(iLastSyncState,iSyncState);
		iLastSyncState=++iSyncState;
	}
	// Удаляем оставшиеся не нужными объекты
	while( iLastSyncState!=iEnd ) iLastSyncState=Del(iLastSyncState);
}
//---------------------------------------------------------------------------
bool MSyncStates::Start() const
{
	for ( auto &SyncState: *this )
	{
		if ( !SyncState.Start() ) return false;
	}

	return true;
}
//---------------------------------------------------------------------------
bool MSyncStates::Stop() const
{
	bool NeedSave=false;

	for ( auto &SyncState: *this )
	{
		NeedSave|=SyncState.Stop();
	}

	return NeedSave;
}
//---------------------------------------------------------------------------
MSyncStates::iterator MSyncStates::Search(u_long IP_)
{
	auto iSyncState=begin();
	auto iEnd=end();

	while ( iSyncState!=iEnd )
	{
		if ( iSyncState->gIP()==IP_ ) break;
		++iSyncState;
	}

	return iSyncState;
}
//---------------------------------------------------------------------------
void MSync::SetARPFile(wchar_t *File_, unsigned Code_, bool AutoSave_)
{
    AutoSaveARP=AutoSave_;
    SyncStates.SetDefaultFile(File_,Code_);
}
//---------------------------------------------------------------------------
bool MSync::NetInit(unsigned Code_, MAuth *MAC_)
{
    WSADATA WSAData;
    NetCode=Code_;
    NetMAC=MAC_;
    Init=!::WSAStartup(MAKEWORD(1,1),&WSAData);
    return Init;
}
//---------------------------------------------------------------------------
bool MSync::NetFree()
{
    Init=::WSACleanup();
    return !Init;
}
//---------------------------------------------------------------------------
bool MSync::Associate(MStates *States_, MComputers *Computers_)
{
    States=nullptr;
    SyncStates.Associate(States_,Computers_);
    States=States_;
    // Сохраним кэш MAC-адресов
    return AutoSaveARP? SyncStates.Save(): true;
}
//---------------------------------------------------------------------------
bool MSync::Start()
{
    sockaddr_in Address;
    BOOL SetValue;

    if ( (!Init)||(States==nullptr)||(Thread!=nullptr) ) return false;

    Socket=SocketBC=INVALID_SOCKET;
    // Создаем сокет для отправки/приема датаграмм
    if ( (Socket=::socket(AF_INET,SOCK_DGRAM,
        IPPROTO_IP))==INVALID_SOCKET ) goto error;
    // Задаем параметры адреса сокета
    memset(&Address,0,sizeof(Address));
    Address.sin_family=AF_INET;
    Address.sin_port=::htons(0);            // порт выбирает ОС !
    Address.sin_addr.s_addr=INADDR_ANY;
    // Присоединяем сокет
    if ( ::bind(Socket,(sockaddr*)&Address,
        sizeof(Address))==SOCKET_ERROR ) goto error;

    // Создаем сокет для отправки широковещательных пакетов
    if ( (SocketBC=::socket(AF_INET,SOCK_DGRAM,
        IPPROTO_IP))==INVALID_SOCKET ) goto error;
    // Устанавливаем для него опцию broadcast
    SetValue=TRUE;
    if ( ::setsockopt(SocketBC,SOL_SOCKET,SO_BROADCAST,
        (char*)&SetValue,sizeof(SetValue))==SOCKET_ERROR ) goto error;
    // Задаем параметры адреса сокета
    memset(&Address,0,sizeof(Address));
    Address.sin_family=AF_INET;
    Address.sin_port=::htons(0);
    Address.sin_addr.s_addr=INADDR_ANY;
    // Присоединяем сокет
    if ( ::bind(SocketBC,(sockaddr*)&Address,
        sizeof(Address))==SOCKET_ERROR ) goto error;

	//
    if ( !SyncStates.Start() ) goto error;

    // Создаем поток для выполнения отправки и приема данных
    Thread=::CreateThread(nullptr,0,&ThreadFunc,(LPVOID)this,0,&ThreadID);
    if ( Thread==nullptr ) goto error;

    return true;
error:
    // Закрываем сокеты
    if ( Socket!=INVALID_SOCKET ) { ::closesocket(Socket); Socket=INVALID_SOCKET; }
    if ( SocketBC!=INVALID_SOCKET ) { ::closesocket(SocketBC); SocketBC=INVALID_SOCKET; }
    return false;
}
//---------------------------------------------------------------------------
void MSync::Stop()
{
    if ( Thread==nullptr ) return;
    // Посылаем потоку сообщение
    ::PostThreadMessage(ThreadID,WM_USER,0,0);
    // Ждем завершения потока
    ::WaitForSingleObject(Thread,INFINITE);
    // Закрываем описатель потока
    ::CloseHandle(Thread);
    Thread=nullptr; ThreadID=0;
    // Закрываем сокеты
    ::closesocket(Socket); Socket=INVALID_SOCKET;
	::closesocket(SocketBC); SocketBC=INVALID_SOCKET;
	// При необходимости сохраняем состояния компьютеров
	if ( SyncStates.Stop() ) States->Save();
}
//---------------------------------------------------------------------------
DWORD WINAPI MSync::ThreadFunc(LPVOID Data)
{
    // Создаем очередь сообщений для потока
    ::PeekMessageW(nullptr,(HWND)-1,0,0,PM_NOREMOVE);
    // Запускаем обработчик
    ((MSync*)Data)->ThreadExecute();
    // Сбрасываем индикатор - счетчик пакетов
    ((MSync*)Data)->sPCount(0);
    // Завершаем работу потока
    ::ExitThread(0);
    return 0;
}
//---------------------------------------------------------------------------
void MSync::ThreadExecute()
{
    MSG Message;
    sockaddr_in FromAddr;
    int RecvSize, AddrSize;
    bool NeedSave;
    DWORD StartCycleTime;
    unsigned Process;

    while(true)
    {
        StartCycleTime=::GetTickCount();

        // Выполняем цикл отправки/приема данных
        NeedSave=false; Process=0;
		for ( auto &SyncState: SyncStates )
		{
			// Опустошаем очередь принятых пакетов
			while(PollData(Socket))
            {
                // Читаем данные из сокета
                memset(&FromAddr,0,sizeof(FromAddr)); AddrSize=sizeof(FromAddr);
                RecvSize=::recvfrom(Socket,(char*)&Packet,sizeof(Packet),
                    0,(sockaddr*)&FromAddr,&AddrSize);
                if ( (RecvSize==0)||(RecvSize==SOCKET_ERROR) ) continue;
				// Ищем состояние синхронизации по IP-адресу, с которого пришел пакет
				auto iRecvState=SyncStates.Search(FromAddr.sin_addr.s_addr);
				// Передаем его для обработки и отправки ответа сразу же
				if ( iRecvState!=SyncStates.end() )
                {
                    NeedSave|=iRecvState->Recv((char*)&Packet,RecvSize,NetCode,NetMAC);
					NeedSave|=SyncState.Send(Socket,SocketBC,NetCode,NetMAC);
				}
            }
            // Вяло отправляем данные "по-расписанию"
			NeedSave|=SyncState.Send(Socket,SocketBC,NetCode,NetMAC);
            Process+=SyncState.gPCount();
        }
        // Проверяем нужно ли сохранить обновленные состояния компьютеров
        if ( NeedSave )
        {
            // Скроем возможные ошибки выделения памяти
            try { States->Save(); }
            catch (std::bad_alloc &e) {}
        }
        // Обновляем MAC-адреса и при необходимости сохраняем на диск
        if ( UpdateMAC(&SyncStates)&&AutoSaveARP )
        {
            // Скроем возможные ошибки выделения памяти
            try { SyncStates.Save(); }
            catch (std::bad_alloc &e) {}
        }
        // Обновляем индикатор процесса синхронизации
        sPCount(Process);

        // Проверяем не пора ли завершить работу
        if ( ::PeekMessage(&Message,(HWND)-1,0,0,PM_REMOVE) ) return;

        // Засыпаем, если пакеты быстро разослали
        if ( (::GetTickCount()-StartCycleTime)<(SYNC_SendInterval/4) )
            ::Sleep(SYNC_SendInterval/4);
    }
}
//---------------------------------------------------------------------------
bool MSync::PollData(SOCKET Socket_)
{
    fd_set WaitSockets;
    timeval WaitTimer;

    // Заносим сокет в таблицу
    FD_ZERO(&WaitSockets);
    FD_SET(Socket_,&WaitSockets);
    // Устанавливаем таймер ожидания данных в ноль
    WaitTimer.tv_sec=0;
    WaitTimer.tv_usec=0;

    return ::select(0,&WaitSockets,nullptr,nullptr,&WaitTimer)==1;
}
//---------------------------------------------------------------------------
bool MSync::UpdateMAC(MSyncStates *States_)
{
    PMIB_IPNETTABLE Table=nullptr;
    ULONG TableSize;
	PMIB_IPNETROW Record;
	bool NeedSave=false;

	// Берем размер таблицы
	TableSize=0;
	if ( ::GetIpNetTable(nullptr,&TableSize,TRUE)!=
		ERROR_INSUFFICIENT_BUFFER ) goto error;
	// Выделяем память под таблицу
	try { Table=(PMIB_IPNETTABLE)new char[TableSize]; }
	catch (std::bad_alloc &e) { goto error; }
	memset(Table,0,TableSize);
	// Берем таблицу
	if ( ::GetIpNetTable(Table,&TableSize,TRUE)!=0 ) goto error;

	// Обновляем MAC-адреса в списке состояний
	for ( DWORD i=0; i<Table->dwNumEntries; i++ )
	{
		Record=&Table->table[i];
		// Проверяем тип записи (нужны только динамические и статические)
		if ( (Record->dwType!=MIB_IPNET_TYPE_DYNAMIC)&&
			(Record->dwType!=MIB_IPNET_TYPE_STATIC) ) continue;
		// Проверяем совпадает ли длина MAC-адреса с ожидаемой
		if ( Record->dwPhysAddrLen!=MAC_AddrLength ) continue;
		// Ищем состояние, ассоциированное с тем же адресом, что взят из таблицы
		auto iState=States_->Search(Record->dwAddr);
		if ( iState==States_->end() ) continue;
		// Обновляем MAC-адрес
		NeedSave|=iState->UpdateMAC(Record->bPhysAddr);
    }

    delete[] Table;
    return NeedSave;
error:
    delete[] Table;
    return false;
}
//---------------------------------------------------------------------------
bool MSyncCl::PollData(SOCKET Socket_)
{
    fd_set WaitSockets;
    timeval WaitTimer;

    // Заносим сокет в таблицу
    FD_ZERO(&WaitSockets);
    FD_SET(Socket_,&WaitSockets);
    // Устанавливаем таймер ожидания данных в ноль
    WaitTimer.tv_sec=0;
    WaitTimer.tv_usec=0;

    return ::select(0,&WaitSockets,nullptr,nullptr,&WaitTimer)==1;
}
//---------------------------------------------------------------------------
bool MSyncCl::NetInit(unsigned Code_, MAuth *MAC_)
{
    WSADATA WSAData;
    NetCode=Code_;
    NetMAC=MAC_;
    Init=!::WSAStartup(MAKEWORD(1,1),&WSAData);
    return Init;
}
//---------------------------------------------------------------------------
bool MSyncCl::NetFree()
{
    Init=::WSACleanup();
    return !Init;
}
//---------------------------------------------------------------------------
void MSyncCl::Associate(MStateCl *State_)
{
    State=State_;
}
//---------------------------------------------------------------------------
bool MSyncCl::Start()
{
    sockaddr_in Address;
    BOOL SetValue;

    if ( (!Init)||(Thread!=nullptr) ) return false;

    Socket=INVALID_SOCKET;
    // Создаем сокет для приема/отправки датаграмм
    if ( (Socket=::socket(AF_INET,SOCK_DGRAM,
        IPPROTO_IP))==INVALID_SOCKET ) goto error;
    // Задаем параметры адреса сокета
    memset(&Address,0,sizeof(Address));
    Address.sin_family=AF_INET;
    Address.sin_port=::htons(SYNC_Port);
    Address.sin_addr.s_addr=INADDR_ANY;
    // Присоединяем сокет
    if ( ::bind(Socket,(sockaddr*)&Address,
        sizeof(Address))==SOCKET_ERROR ) goto error;

    //
    LastSendTime=::GetTickCount();
    Process=mspWait;

    // Создаем поток для выполнения отправки и приема данных
    Thread=::CreateThread(nullptr,0,&ThreadFunc,(LPVOID)this,0,&ThreadID);
    if ( Thread==nullptr ) goto error;

    return true;
error:
    // Закрываем сокет
    if ( Socket!=INVALID_SOCKET ) { ::closesocket(Socket); Socket=INVALID_SOCKET; }
    return false;
}
//---------------------------------------------------------------------------
void MSyncCl::Stop()
{
    if ( Thread==nullptr ) return;
    // Посылаем потоку сообщение
    ::PostThreadMessage(ThreadID,WM_USER,0,0);
    // Ждем завершения потока
    ::WaitForSingleObject(Thread,INFINITE);
    // Закрываем описатель потока
    ::CloseHandle(Thread); Thread=nullptr; ThreadID=0;
    // Закрываем сокет
    ::closesocket(Socket); Socket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
DWORD WINAPI MSyncCl::ThreadFunc(LPVOID Data)
{
    // Создаем очередь сообщений для потока
    ::PeekMessage(nullptr,(HWND)-1,0,0,PM_NOREMOVE);

    ((MSyncCl*)Data)->ThreadExecute();

    // Завершаем работу потока
    ::ExitThread(0);
    return 0;
}
//---------------------------------------------------------------------------
void MSyncCl::ThreadExecute()
{
    MSG Message;
    sockaddr_in FromAddr;
    int RecvSize, AddrSize;
    bool NeedSave;
    DWORD StartCycleTime;

    while(true)
    {
        StartCycleTime=::GetTickCount();

        NeedSave=false;
        // Опустошаем буфер пакетов
        while(PollData(Socket))
        {
            // Читаем данные из сокета
            memset(&FromAddr,0,sizeof(FromAddr)); AddrSize=sizeof(FromAddr);
            RecvSize=::recvfrom(Socket,(char*)&RcvPacket,sizeof(RcvPacket),
                0,(sockaddr*)&FromAddr,&AddrSize);
            if ( (RecvSize==0)||(RecvSize==SOCKET_ERROR) ) continue;
            // Передаем для обработки и отправки ответа
            NeedSave|=Recv(&FromAddr,(char*)&RcvPacket,RecvSize);
        }
        NeedSave|=Send(Socket);
        // Сохраняем обновленное состояние, если нужно
        if ( NeedSave )
        {
            // Скроем возможные ошибки выделения памяти
            try { State->Save(); }
            catch (std::bad_alloc &e) {}
        }

        // Проверяем очередь сообщений
        if ( ::PeekMessage(&Message,(HWND)-1,0,0,PM_REMOVE) ) break;

        // Засыпаем, если быстро управились
        if ( (::GetTickCount()-StartCycleTime)<(SYNC_SendInterval/4) )
            ::Sleep(SYNC_SendInterval/4);
    }
}
//---------------------------------------------------------------------------
bool MSyncCl::Recv(sockaddr_in *From_, char *Packet_, int PacketSize_)
{
    MSyncPHeader *RecvHeader=(MSyncPHeader*)Packet_;
    bool NeedSave=false;

    if ( Process==mspNone ) goto error;
    // Если сеанс начат, игнорируем пакеты с других IP
    if ( (Process!=mspWait)&&
        (SndAddr.sin_addr.s_addr!=From_->sin_addr.s_addr) ) goto error;
    // Проверяем допустимые размеры пакета
    if ( (PacketSize_<SYNC_MinSize)||
        (PacketSize_>SYNC_MaxSize) ) goto error;
    // Расшифровываем его
    BasicDecode(Packet_,PacketSize_,NetCode);
    // Сверяем версию и MAC
    if ( (RecvHeader->Version!=SYNC_Version)||
        (!NetMAC->Check(Packet_,PacketSize_-MAC_Size,
        Packet_+PacketSize_-MAC_Size,MAC_Size)) ) goto error;

    // Обрабатываем пакет
    switch((int)RecvHeader->Type)
    {
        case mptHello:
            // Ждем ли мы этот пакет и верный ли у него размер ?
            if ( (Process!=mspWait)||
                (PacketSize_!=sizeof(MSyncPHello)) ) break;
            // Запоминаем адрес сервера, с которым начали обмен
            SndAddr=*From_;
//            memset(&SndAddr,0,sizeof(SndAddr));
//            SndAddr.sin_family=AF_INET;
//            SndAddr.sin_port=From_->sin_port;
//            SndAddr.sin_addr.s_addr=From_->sin_addr.s_addr;
            // Заполняем заголовок hello-пакета со своим random
            SndPacket.Hello.Header.Version=SYNC_Version;
            SndPacket.Hello.Header.Type=mptHello;
            SndPacket.Hello.Header.Seed=BasicRand();
            // И сразу генерируем сеансовый ID, т.к. random сервера уже приняли
            Seed=BasicMix(SndPacket.Hello.Header.Seed,RecvHeader->Seed);
            // Добавляем к пакету MAC
            NetMAC->Calc((char*)&SndPacket.Hello,
                sizeof(SndPacket.Hello)-sizeof(SndPacket.Hello.MAC),
                SndPacket.Hello.MAC,sizeof(SndPacket.Hello.MAC));
            // Задаем размер пакета для отправки и шифруем его
            SndPacketSize=sizeof(SndPacket.Hello);
            BasicEncode((char*)&SndPacket,SndPacketSize,NetCode);
            // Включаем отправку
            LastSendTime=::GetTickCount()-SYNC_SendInterval;
            SendCount=0; Process=mspHello;
            break;
        case mptSyncData:
            // Ждем ли мы этот пакет и верный ли у него размер ?
            if ( (Process!=mspHello)||
                (SendCount==0)||            // защита от replay brute-force 
                (PacketSize_!=sizeof(MSyncPData))||
                (RecvHeader->Seed!=Seed) ) break;
            // Извлекаем новый режим работы клиента, но применим его
            // только после отправки подтверждения серверу
            SyncData=((MSyncPData*)Packet_)->Data;
            // Заполняем пакет c ответом серверу
            SndPacket.Conf.Header.Version=SYNC_Version;
            SndPacket.Conf.Header.Type=mptSyncConf;
            SndPacket.Conf.Header.Seed=Seed;
            // Добавляем к пакету MAC
            NetMAC->Calc((char*)&SndPacket.Conf,
                sizeof(SndPacket.Conf)-sizeof(SndPacket.Conf.MAC),
                SndPacket.Conf.MAC,sizeof(SndPacket.Conf.MAC));
            // Задаем размер пакета для отправки и шифруем его
            SndPacketSize=sizeof(SndPacket.Conf);
            BasicEncode((char*)&SndPacket,SndPacketSize,NetCode);
            // Включаем отправку
            LastSendTime=::GetTickCount()-SYNC_SendInterval;
            SendCount=0; Process=mspSyncConf;
            break;
        default: break;
    }

error:
    return NeedSave;
}
//---------------------------------------------------------------------------
bool MSyncCl::Send(SOCKET Socket_)
{
    bool NeedSave=false;

    switch(Process)
    {
        case mspHello:
        case mspSyncConf:
            // Проверяем не пора ли отправить следующий пакет
            if ( (::GetTickCount()-LastSendTime)<SYNC_SendInterval ) break;
            // Проверяем не отправлено ли заданное количество пакетов
            if ( SendCount>=SYNC_SendRetryes )
            {
                // Применяем новое состояние/выполняем команду (reboot, shutdown),
                // если данные были получены и клиент отправлял подтверждение серверу
                if ( Process==mspSyncConf )
                    NeedSave=State->NewSyncData(&SyncData);
                // Переходим в режим ожидания новых данных
                Process=mspWait;
            } else
            {
                // Отправляем пакет
                ::sendto(Socket_,(char*)&SndPacket,SndPacketSize,
                    0,(sockaddr*)&SndAddr,sizeof(SndAddr));
                // Сохраняем время отправки пакета и увеличиваем счетчик
                LastSendTime=::GetTickCount(); SendCount++;
            }
            break;
        default: break;
    }

    return NeedSave;
}
//---------------------------------------------------------------------------

