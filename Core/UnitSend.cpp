//---------------------------------------------------------------------------
#include <winsock2.h>
#pragma hdrstop

#include "UnitSend.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MSend::MSend()
{
    lSocket=INVALID_SOCKET;
    rSocket=INVALID_SOCKET;
    Thread=NULL;
    ThreadID=0;
    Break=false;
    NetCode=0;
    NetMAC=NULL;
    Init=false;
}
//---------------------------------------------------------------------------
MSend::~MSend()
{
}
//---------------------------------------------------------------------------
bool MSend::NetInit(unsigned Code_, MAuth *MAC_)
{
    WSADATA WSAData;
    NetCode=Code_;
    NetMAC=MAC_;
    Init=!::WSAStartup(MAKEWORD(1,1),&WSAData);
    return Init;
}
//---------------------------------------------------------------------------
bool MSend::NetFree()
{
    Init=::WSACleanup();
    return !Init;
}
//---------------------------------------------------------------------------
bool MSend::Create(bool Srv_)
{
    BOOL a;
    unsigned long b;
    sockaddr_in Address;

    // Создаем сокет
    if ( Srv_ )
    {
        if ( (rSocket=::socket(PF_INET,SOCK_STREAM,IPPROTO_IP))==INVALID_SOCKET ) goto error;
        if ( ::setsockopt(rSocket,SOL_SOCKET,SO_REUSEADDR,(char*)&a,sizeof(a)) ) goto error;
        b=TRUE; if ( ::ioctlsocket(rSocket,FIONBIO,&b) ) goto error;
    } else
    {
        if ( (lSocket=::socket(PF_INET,SOCK_STREAM,IPPROTO_IP))==INVALID_SOCKET ) goto error;
        if ( ::setsockopt(lSocket,SOL_SOCKET,SO_REUSEADDR,(char*)&a,sizeof(a)) ) goto error;
        b=TRUE; if ( ::ioctlsocket(lSocket,FIONBIO,&b) ) goto error;
        // Задаем адрес для ожидания соединений
        memset(&Address,0,sizeof(Address));
        Address.sin_family=AF_INET;
        Address.sin_port=::htons(SEND_Port);
        Address.sin_addr.s_addr=INADDR_ANY;
        // Присоединяем сокет
        if ( ::bind (lSocket,(sockaddr*)&Address,sizeof(Address))==SOCKET_ERROR ) goto error;
    }

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Listen()
{
    MSG Msg;
    fd_set fds;
    timeval timeout;
    int result;

    // Переводим сокет в режим приема соединений
    if ( ::listen(lSocket,1) ) goto exit;
    // Ожидаем соединения
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HWND)-1,0,0,PM_REMOVE) )
        {
            Break=true;
            goto exit;
        }
        FD_ZERO(&fds); FD_SET(lSocket,&fds);
        if ( (result=::select(0,&fds,NULL,NULL,&timeout))==SOCKET_ERROR ) goto exit;
    } while((result!=1)||(!FD_ISSET(lSocket,&fds)));

    return true;
exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Accept()
{
    // Принимаем соединение
    rSocket=::accept(lSocket,NULL,NULL);
    return rSocket!=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
bool MSend::Connect(char *IP_, unsigned Time_)
{
    MSG Msg;
    sockaddr_in Address;
    fd_set w_fds, e_fds;
    timeval timeout;
    DWORD stime;
    int result;

    // Задаем адрес компьютера
    memset(&Address,0,sizeof(Address));
    Address.sin_family=AF_INET;
    Address.sin_port=::htons(SEND_Port);
    Address.sin_addr.s_addr=::inet_addr(IP_);
    if ( Address.sin_addr.s_addr==INADDR_NONE ) goto exit;
    // Запускаем установление соединения
//    if ( ::connect(rSocket,(sockaddr*)&Address,sizeof(sockaddr_in))==SOCKET_ERROR ) goto exit;
    ::connect(rSocket,(sockaddr*)&Address,sizeof(sockaddr_in));
    // Ожидаем установления соединения
    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HWND)-1,0,0,PM_REMOVE) )
        {
            Break=true;
            goto exit;
        }
        // Ожидаем возможности передать еще данные
        FD_ZERO(&w_fds); FD_SET(rSocket,&w_fds);
        FD_ZERO(&e_fds); FD_SET(rSocket,&e_fds);
        if ( (result=::select(0,NULL,&w_fds,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(rSocket,&e_fds) ) goto exit;
        if ( FD_ISSET(rSocket,&w_fds) ) return true;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Snd(char *Data_, unsigned Size_, unsigned Time_)
{
    MSG Msg;
    fd_set w_fds, e_fds;
    timeval timeout;
    DWORD stime;
    int result;

    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HWND)-1,0,0,PM_REMOVE) )
        {
            Break=true;
            goto exit;
        }
        // Ожидаем возможности передать еще данные
        FD_ZERO(&w_fds); FD_SET(rSocket,&w_fds);
        FD_ZERO(&e_fds); FD_SET(rSocket,&e_fds);
        if ( (result=::select(0,NULL,&w_fds,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(rSocket,&e_fds) ) goto exit;
        // Предаем очередную порцию данных
        if ( (result=::send(rSocket,Data_,Size_,0))==SOCKET_ERROR ) goto exit;
        if ( (Size_-=result)==0 ) return true;
        Data_+=result;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Rcv(char *Data_, unsigned Size_, unsigned Time_)
{
    MSG Msg;
    fd_set r_fds, e_fds;
    timeval timeout;
    DWORD stime;
    int result;

    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HWND)-1,0,0,PM_REMOVE) )
        {
            Break=true;
            goto exit;
        }
        // Ожидаем возможности принять еще данные
        FD_ZERO(&r_fds); FD_SET(rSocket,&r_fds);
        FD_ZERO(&e_fds); FD_SET(rSocket,&e_fds);
        if ( (result=::select(0,&r_fds,NULL,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(rSocket,&e_fds) ) goto exit;
        // Принимаем очередную порцию данных
        result=::recv(rSocket,Data_,Size_,0);
        if ( (result==SOCKET_ERROR)||(result==0) ) goto exit;
        if ( (Size_-=result)==0 ) return true;
        Data_+=result;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Disconnect(unsigned Time_)
{
    MSG Msg;
    fd_set r_fds, e_fds;
    timeval timeout;
    DWORD stime;
    int result;
    char buffer[1];

    // Оповещаем партнера, что отправки данных больше не будет
    if ( ::shutdown(rSocket,SD_SEND)==SOCKET_ERROR ) goto exit;
    //
    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HWND)-1,0,0,PM_REMOVE) )
        {
            Break=true;
            goto exit;
        }
        // Ожидаем возможности принять еще данные
        FD_ZERO(&r_fds); FD_SET(rSocket,&r_fds);
        FD_ZERO(&e_fds); FD_SET(rSocket,&e_fds);
        if ( (result=::select(0,&r_fds,NULL,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(rSocket,&e_fds) ) goto exit;
        // Проверяем пришел ли маркер конца файла
        result=::recv(rSocket,buffer,sizeof(buffer),0);
///        result=::recv(lSocket,buffer,sizeof(buffer),0);  // ошибка ?
        if ( result==SOCKET_ERROR ) goto exit;
        if ( result==0 ) return true;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
void MSend::lClose()
{
    // Останавливаем прием/отправку данных
    ::shutdown(lSocket,SD_BOTH);
    // Закрываем сокет
    ::closesocket(lSocket);
    lSocket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
void MSend::rClose()
{
    // Останавливаем прием/отправку данных
    ::shutdown(rSocket,SD_BOTH);
    // Закрываем сокет
    ::closesocket(rSocket);
    rSocket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
bool MSend::SndHello(unsigned Seed_)
{
    // Заполняем hello-блок со своим random
    Packet.Hello.Version=SEND_Version;
    Packet.Hello.Seed=Seed_;
    // Добавляем MAC и шифруем
    NetMAC->Calc((char*)&Packet.Hello,
        sizeof(Packet.Hello)-sizeof(Packet.Hello.MAC),
        Packet.Hello.MAC,sizeof(Packet.Hello.MAC));
    BasicEncode((char*)&Packet.Hello,sizeof(Packet.Hello),NetCode);

    // Отправляем клиенту hello
    return Snd((char*)&Packet.Hello,sizeof(Packet.Hello),3);
}
//---------------------------------------------------------------------------
bool MSend::RcvHello(unsigned *Seed_)
{
    if ( !Rcv((char*)&Packet.Hello,sizeof(Packet.Hello),3) ) goto error;
    BasicDecode((char*)&Packet.Hello,sizeof(Packet.Hello),NetCode);
    // Сверяем версию интерфейса и MAC
    if ( (Packet.Hello.Version!=SEND_Version)||
        (!NetMAC->Check((char*)&Packet.Hello,
        sizeof(Packet.Hello)-sizeof(Packet.Hello.MAC),
        Packet.Hello.MAC,sizeof(Packet.Hello.MAC))) ) goto error;
    // Возвращаем результат
    *Seed_=Packet.Hello.Seed;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::SndRequest(unsigned char Type_, unsigned Seed_, unsigned Size_)
{
    // Заполняем блок-информер
    Packet.Request.Type=Type_;
    Packet.Request.Seed=Seed_;
    Packet.Request.Size=Size_;
    // Добавляем к блоку MAC и шифруем его
    NetMAC->Calc((char*)&Packet.Request,
        sizeof(Packet.Request)-sizeof(Packet.Request.MAC),
        Packet.Request.MAC,sizeof(Packet.Request.MAC));
    BasicEncode((char*)&Packet.Request,sizeof(Packet.Request),NetCode);

    // Отправляем клиенту
    return Snd((char*)&Packet.Request,sizeof(Packet.Request),3);
}
//---------------------------------------------------------------------------
bool MSend::RcvRequest(unsigned char *Type_, unsigned Seed_, unsigned *Size_)
{
    if ( !Rcv((char*)&Packet.Request,sizeof(Packet.Request),3) ) goto error;
    BasicDecode((char*)&Packet.Request,sizeof(Packet.Request),NetCode);
    // Сверяем seed и MAC
    if ( (Packet.Request.Seed!=Seed_)||
        (!NetMAC->Check((char*)&Packet.Request,
        sizeof(Packet.Request)-sizeof(Packet.Request.MAC),
        Packet.Request.MAC,sizeof(Packet.Request.MAC))) ) goto error;
    // Возвращаем результат
    *Type_=Packet.Request.Type;
    *Size_=Packet.Request.Size;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::SndObject(MSLList *Obj_, unsigned Type_, unsigned Seed_)
{
    Marray <char> Data;
    char *pt;
    unsigned Size;

    try
    {
        // Определяем размер данных и выделяем память под буффер (+ seed,MAC)
        Size=Obj_->GetAllDataSize()+sizeof(unsigned)+MAC_Size;
        Data.Alloc(Size);
        // Заполняем его
        pt=MemSet(&Data[0],(unsigned)Seed_);
        pt=Obj_->SetAllData(pt);
        // Добавляем MAC и шифруем
        NetMAC->Calc(&Data[0],pt-&Data[0],pt,&Data[0]+Size-pt);
        BasicEncode(&Data[0],Size,NetCode);

        // Отправляем клиенту информер и данные
        if ( (!SndRequest(Type_,Seed_,Size))||
            (!Snd(&Data[0],Size,10)) ) goto error;
    }
    catch (std::bad_alloc &e) { goto error; }

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::RcvObject(MSLList *Obj_, unsigned Size_, unsigned Seed_)
{
    Marray <char> Data;

    try
    {
        // Проверяем допустимость размера данных и выделяем память
        if ( (Size_<SEND_MinData)||
            (Size_>SEND_MaxData) ) goto error;
        Data.Alloc(Size_);

        // Принимаем их и расшифровываем
        if ( !Rcv(&Data[0],Size_,10) ) goto error;
        BasicDecode(&Data[0],Size_,NetCode);
        // Сверяем seed и MAC
        if ( (*((unsigned*)&Data[0])!=Seed_)||
            (!NetMAC->Check(&Data[0],Size_-MAC_Size,
            &Data[0]+(Size_-MAC_Size),MAC_Size)) ) goto error;

        // Восстанавливаем объект из принятых данных (за вычетом seed и MAC)
        if ( !Obj_->GetAllData(&Data[0]+sizeof(unsigned),
            &Data[0]+(Size_-MAC_Size)) ) goto error;
    }
    catch (std::bad_alloc &e) { goto error; }

    return true;
error:
    // Очистим возможно не полностью загруженный объект
    Obj_->Clear();
    return false;
}
//---------------------------------------------------------------------------
DWORD WINAPI MSend::ThreadF(LPVOID Data)
{
    MSG msg;
    
    // Создаем очередь сообщений для потока
///    ::PeekMessage(NULL,(HWND)-1,0,0,PM_NOREMOVE);
    ::PeekMessage(&msg,NULL,WM_USER,WM_USER,PM_NOREMOVE);
    ((MSend*)Data)->Break=false;
    // Обработка тела потока
    ((MSend*)Data)->ThreadP();
    // Завершаем работу потока
    ::ExitThread(0);
    return 0;
}
//---------------------------------------------------------------------------
bool MSend::Start()
{
    if ( !Init ) return false;
    // Создаем поток для выполнения отправки и приема данных
    return (Thread=::CreateThread(NULL,0,&ThreadF,
        (LPVOID)this,0,&ThreadID))!=NULL;
}
//---------------------------------------------------------------------------
void MSend::Stop()
{
    DWORD ExitCode;
    // Проверяем работает ли поток
    if ( Thread==NULL ) return;
    if ( ::GetExitCodeThread(Thread,&ExitCode)&&(ExitCode==STILL_ACTIVE) )
    {
        // Посылаем потоку сообщение и ждем завершения его работы
        ::PostThreadMessage(ThreadID,WM_USER,0,0);
        ::WaitForSingleObject(Thread,INFINITE);
    }
    // Закрываем описатель потока
    ::CloseHandle(Thread);
    Thread=NULL;
    ThreadID=0;
}
//---------------------------------------------------------------------------
MSendSrv::MSendSrv()
{
    Window=NULL;
    Mode=mssNone;
    Comps=NULL;
    Comp=NULL;
    DataObject=NULL;
}
//---------------------------------------------------------------------------
MSendSrv::~MSendSrv()
{
}
//---------------------------------------------------------------------------
void MSendSrv::Event(MComputer *Computer_, int Event_)
{
//    ::Sleep(1000);  /// для визуальной отладки событий
    if ( Window==NULL ) return;
    ::PostMessage(Window,MinMsg+Event_,(WPARAM)Computer_,NULL);     // асинхронно
//    ::SendMessage(Window,MinMsg+Event_,(WPARAM)Computer_,NULL);     // возможна блокировка потоков
}
//---------------------------------------------------------------------------
void MSendSrv::ThreadP()
{
    // Обработка тела потока
    switch(Mode)
    {
        case mssSendGames:
        case mssSendConfig:
            ThreadSend();
            break;
        case mssGetGames:
        case mssGetConfig:
            ThreadGet();
            break;
        default:
            break;
    }
    //
    Mode=mssNone;
    Event(NULL,mseFreeParam);
}
//---------------------------------------------------------------------------
void MSendSrv::ThreadSend()
{
    unsigned Seed, RmtSeed, Size;
    unsigned char Type;
    MComputer *pComp;

    for ( unsigned i=0; i<Comps->Count(); i++ )
    {
        // Создаем сокет для исходящего соединения
        if ( !Create(true) ) goto next;
        pComp=(*Comps)[i];
        // Устанавливаем соединение
        Event(pComp,mseConnecting);
        if ( !Connect(pComp->Address,2) ) { Event(pComp,mseNotConnect); goto next; }
        Event(pComp,mseSending);

        // Выполнем обмен hello и генерируем ID сеанса
        Seed=BasicRand();
        if ( (!SndHello(Seed))||
            (!RcvHello(&RmtSeed)) ) { Event(pComp,mseProtError); goto next; }
        Seed=BasicMix(Seed,RmtSeed);

        // Отправляем блок-информер и данные
        switch(Mode)
        {
            case mssSendGames:  Type=mstSendGames; break;
            case mssSendConfig: Type=mstSendConfig; break;
            default: goto next;
        }
        if ( !SndObject(DataObject,Type,Seed) ) { Event(pComp,mseProtError); goto next; }

        // Принимаем подтверждение
        if ( !RcvRequest(&Type,Seed,&Size) ) { Event(pComp,mseProtError); goto next; }
        if ( (Type!=mstAccept)||(Size!=0) ) { Event(pComp,mseProtError); goto next; }

        // Завершаем соединение
        Event(pComp,mseDisconnecting);
        if ( !Disconnect(10) ) { Event(pComp,mseProtError); goto next; }

        Event(pComp,mseExecute);
next:
        rClose();
        if ( Break ) break;
    }
}
//---------------------------------------------------------------------------
void MSendSrv::ThreadGet()
{
    unsigned Seed, RmtSeed, Size;
    unsigned char Type;

    if ( !Create(true) ) goto error;
    // Устанавливаем соединение
    Event(Comp,mseConnecting);
    if ( !Connect((Comp)->Address,2) ) { Event(Comp,mseNotConnect); goto error; }
    Event(Comp,mseReceiving);

    // Выполнем обмен hello и генерируем ID сеанса
    Seed=BasicRand();
    if ( (!SndHello(Seed))||
        (!RcvHello(&RmtSeed)) ) { Event(Comp,mseProtError); goto error; }
    Seed=BasicMix(Seed,RmtSeed);

    // Отправляем запрос на данные
    switch(Mode)
    {
        case mssGetGames:   Type=mstGetGames; break;
        case mssGetConfig:  Type=mstGetConfig; break;
        default: goto error;
    }
    if ( !SndRequest(Type,Seed,0) ) { Event(Comp,mseProtError); goto error; }

    // Принимаем ответ и данные
    if ( (!RcvRequest(&Type,Seed,&Size))||
        (Type!=mstAccept)||
        (!RcvObject(DataObject,Size,Seed)) ) { Event(Comp,mseProtError); goto error; }

    // Завершаем соединение
    Event(Comp,mseDisconnecting);
    if ( !Disconnect(10) ) { Event(Comp,mseProtError); goto error; }

    Event(Comp,mseExecute);
error:
    rClose();
}
//---------------------------------------------------------------------------
bool MSendSrv::NetInit(HWND Window_, UINT MinMsg_, unsigned Code_, MAuth *MAC_)
{
    Window=Window_;
    MinMsg=MinMsg_;
    return MSend::NetInit(Code_,MAC_);
}
//---------------------------------------------------------------------------
bool MSendSrv::NetFree()
{
    return MSend::NetFree();
}
//---------------------------------------------------------------------------
bool MSendSrv::Send(Marray <MComputer*> *Computers_, MGames *Games_, MClOptions *Options_)
{
    if ( Mode!=mssNone ) goto error;
//    // Прерываем текущие сетевые операции
//    Stop();

    if ( Computers_==NULL ) goto error; /// throw ?
    Comps=Computers_;
    if ( Games_!=NULL )
    {
        DataObject=Games_;
        Mode=mssSendGames;
    } else if ( Options_!=NULL )
    {
        DataObject=Options_;
        Mode=mssSendConfig;
    } else goto error;                  /// throw ?

    // Создаем поток, который разошлет данные компьютерам
    return Start();
error:
    return false;
}
//---------------------------------------------------------------------------
bool MSendSrv::Get(MComputer *Computer_, MGames *Games_, MClOptions *Options_)
{
    if ( Mode!=mssNone ) goto error;
//    // Прерываем текущие сетевые операции
//    Stop();

    if ( Computer_==NULL ) goto error;  /// throw ?
    Comp=Computer_;
    if ( Games_!=NULL )
    {
        DataObject=Games_;
        Mode=mssGetGames;
    } else if ( Options_!=NULL )
    {
        DataObject=Options_;
        Mode=mssGetConfig;
    } else goto error;                  /// throw ?

    // Создаем поток, который займется приемом данных
    return Start();
error:
    return false;
}
//---------------------------------------------------------------------------
void MSendSrv::Stop()
{
    MSend::Stop();
}
//---------------------------------------------------------------------------
MSendCl::MSendCl()
{
    State=NULL;
}
//---------------------------------------------------------------------------
MSendCl::~MSendCl()
{
}
//---------------------------------------------------------------------------
void MSendCl::ThreadP()
{
    unsigned Seed, RmtSeed, Size;
    unsigned char Type;

    // Создаем принимающий (слушающий) сокет
    if ( !Create(false) ) return;
    // Обрабатываем поступающие соединения
    while(Listen())
    {
        // Принимаем соединение
        if ( !Accept() ) continue;

        // Принимаем hello и random сервера
        if ( !RcvHello(&RmtSeed) ) goto next;
        // Отправляем свой
        Seed=BasicRand();
        if ( !SndHello(Seed) ) goto next;
        // Формируем сеансовый ID и принимаем запрос
        Seed=BasicMix(Seed,RmtSeed);
        if ( !RcvRequest(&Type,Seed,&Size) ) goto next;

        // Обрабатываем запрос
        switch(Type)
        {
            case mstSendGames:
                if ( (!RcvObject(&Games,Size,Seed))||
                    (!State->NewGames(&Games))||
                    (!SndRequest(mstAccept,Seed,0)) ) goto next;
                break;
            case mstSendConfig:
                if ( (!RcvObject(&Options,Size,Seed))||
                    (!State->NewOptions(&Options))||
                    (!SndRequest(mstAccept,Seed,0)) ) goto next;
                break;
            case mstGetGames:
                if ( (!State->GetGames(&Games))||
                    (!SndObject(&Games,mstAccept,Seed)) ) goto next;
                break;
            case mstGetConfig:
                if ( (!State->GetOptions(&Options))||
                    (!SndObject(&Options,mstAccept,Seed)) ) goto next;
                break;
            default: goto next;
        }

        // Завершаем соединение
        Disconnect(10);
next:
        // Закрываем сокет
        rClose();
        // Подчищаем память до следующего соединения
        Games.Clear();
    }
    // Закрываем принимающий сокет
    lClose();
}
//---------------------------------------------------------------------------
bool MSendCl::NetInit(MStateCl *State_, unsigned Code_, MAuth *MAC_)
{
    State=State_;
    return MSend::NetInit(Code_,MAC_);
}
//---------------------------------------------------------------------------
bool MSendCl::NetFree()
{
    return MSend::NetFree();
}
//---------------------------------------------------------------------------
bool MSendCl::Start()
{
    return MSend::Start();
}
//---------------------------------------------------------------------------
void MSendCl::Stop()
{
    MSend::Stop();
}
//---------------------------------------------------------------------------

