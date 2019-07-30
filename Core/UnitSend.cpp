//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitSend.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MSend::MSend()
{
    Thread=NULL;
    ThreadID=0;
    Socket=INVALID_SOCKET;
    Computers=NULL;
    Games=NULL;
    Options=NULL;
    Data=NULL;
    Break=false;
}
//---------------------------------------------------------------------------
MSend::~MSend()
{
}
//---------------------------------------------------------------------------
void MSend::Event(MComputer *Computer_, int Event_)
{
    if ( (Window!=NULL)&&(!Break) )
        ::SendMessage(Window,MinMsg+Event_,(WPARAM)Computer_,NULL);
}
//---------------------------------------------------------------------------
bool MSend::Create()
{
    BOOL a;
    unsigned long b;

    // Создаем сокет
    if ( (Socket=::socket(PF_INET,SOCK_STREAM,IPPROTO_IP))==INVALID_SOCKET ) goto error;
    if ( ::setsockopt(Socket,SOL_SOCKET,SO_REUSEADDR,(char*)&a,sizeof(a)) ) goto error;
    b=TRUE; if ( ::ioctlsocket(Socket,FIONBIO,&b) ) goto error;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Connect(char *IP_, unsigned Time_)
{
    MSG Msg;
    sockaddr_in Address;
    fd_set w_fds, e_fds;
    timeval timeout;
    int result;
    DWORD stime;

    // Задаем адрес компьютера
    memset(&Address,0,sizeof(Address));
    Address.sin_family=AF_INET;
    Address.sin_port=::htons(SEND_Port);
    Address.sin_addr.s_addr=::inet_addr(IP_);
    if ( Address.sin_addr.s_addr==INADDR_NONE ) goto exit;
    // Запускаем установление соединения
//    if ( ::connect(Socket,(sockaddr*)&Address,sizeof(sockaddr_in))==SOCKET_ERROR ) goto exit;
    ::connect(Socket,(sockaddr*)&Address,sizeof(sockaddr_in));
    // Ожидаем установления соединения
    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HANDLE)-1,0,0,PM_REMOVE) )
            { Break=true; goto exit; }
        // Ожидаем возможности передать еще данные
        FD_ZERO(&w_fds); FD_SET(Socket,&w_fds);
        FD_ZERO(&e_fds); FD_SET(Socket,&e_fds);
        if ( (result=::select(0,NULL,&w_fds,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(Socket,&e_fds) ) goto exit;
        if ( FD_ISSET(Socket,&w_fds) ) return true;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Send(char *Data_, unsigned Size_, unsigned Time_)
{
    MSG Msg;
    fd_set w_fds, e_fds;
    timeval timeout;
    int result;
    DWORD stime;

    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HANDLE)-1,0,0,PM_REMOVE) )
            { Break=true; goto exit; }
        // Ожидаем возможности передать еще данные
        FD_ZERO(&w_fds); FD_SET(Socket,&w_fds);
        FD_ZERO(&e_fds); FD_SET(Socket,&e_fds);
        if ( (result=::select(0,NULL,&w_fds,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(Socket,&e_fds) ) goto exit;
        // Предаем очередную порцию данных
        if ( (result=::send(Socket,Data_,Size_,0))==SOCKET_ERROR ) goto exit;
        if ( (Size_-=result)==0 ) return true;
        Data_+=result;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Recv(char *Data_, unsigned Size_, unsigned Time_)
{
    MSG Msg;
    fd_set r_fds, e_fds;
    timeval timeout;
    int result;
    DWORD stime;

    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HANDLE)-1,0,0,PM_REMOVE) )
            { Break=true; goto exit; }
        // Ожидаем возможности принять еще данные
        FD_ZERO(&r_fds); FD_SET(Socket,&r_fds);
        FD_ZERO(&e_fds); FD_SET(Socket,&e_fds);
        if ( (result=::select(0,&r_fds,NULL,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(Socket,&e_fds) ) goto exit;
        // Принимаем очередную порцию данных
        result=::recv(Socket,Data_,Size_,0);
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
    int result;
    DWORD stime;
    char buffer[1];

    // Оповещаем партнера, что отправки данных больше не будет
    if ( ::shutdown(Socket,SD_SEND)==SOCKET_ERROR ) goto exit;
    //
    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HANDLE)-1,0,0,PM_REMOVE) )
            { Break=true; goto exit; }
        // Ожидаем возможности принять еще данные
        FD_ZERO(&r_fds); FD_SET(Socket,&r_fds);
        FD_ZERO(&e_fds); FD_SET(Socket,&e_fds);
        if ( (result=::select(0,&r_fds,NULL,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(Socket,&e_fds) ) goto exit;
        // Проверяем пришел ли маркер конца файла
        result=::recv(Socket,buffer,sizeof(buffer),0);
        if ( result==SOCKET_ERROR ) goto exit;
        if ( result==0 ) return true;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
void MSend::Close()
{
    // Останавливаем прием/отправку данных
    ::shutdown(Socket,SD_BOTH);
    // Закрываем сокет
    ::closesocket(Socket);
    Socket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
DWORD WINAPI MSend::ThreadFunc(LPVOID Data)
{
    ((MSend*)Data)->Break=false;
    // Создаем очередь сообщений для потока
    ::PeekMessage(NULL,(HANDLE)-1,0,0,PM_NOREMOVE);
    // Обработка тела потока
    switch(((MSend*)Data)->Mode)
    {
        case mssSend: ((MSend*)Data)->ThreadSend(); break;
        case mssGetGames:
        case mssGetConfig: ((MSend*)Data)->ThreadGet(); break;
        default: break;
    }
    //
    delete[] ((MSend*)Data)->Data; ((MSend*)Data)->Data=NULL;
    ((MSend*)Data)->Event(NULL,mseFreeParam);
    // Завершаем работу потока
    ::ExitThread(0);
    return 0;
}
//---------------------------------------------------------------------------
void MSend::ThreadSend()
{
    unsigned Buffer;
    MSG Message;

    for ( MComputer **pComputer=Computers;
        *pComputer!=NULL; pComputer++ )
    {
        if ( !Create() ) goto next;
        // Устанавливаем соединение
        Event(*pComputer,mseConnecting);
        if ( !Connect((*pComputer)->Address,2) )
            { Event(*pComputer,mseNotConnect); goto next; }
        Event(*pComputer,mseSending);
        // Отправляем номер версии интерфейса и принимаем подтверждение
        Buffer=SEND_Version;
        if ( (!Send((char*)&Buffer,sizeof(unsigned),3))||
            (!Recv((char*)&Buffer,sizeof(unsigned),3))||
            (Buffer!=mstAccept) ) { Event(*pComputer,msePrclError); goto next; }
        // Отправляем данные (вместе с типом и размером) и принимаем подтверждение
        if ( (!Send(Data,DataSize,10))||
            (!Recv((char*)&Buffer,sizeof(unsigned),10))||
            (Buffer!=mstAccept) ) { Event(*pComputer,msePrclError); goto next; }
        // Завершаем соединение
        Event(*pComputer,mseDisconnecting);
        if ( !Disconnect(10) ) { Event(*pComputer,msePrclError); goto next; }
        Event(*pComputer,mseExecute);
next:
        Close();
        if ( Break ) break;
    }
}
//---------------------------------------------------------------------------
void MSend::ThreadGet()
{
    MSG Message;
    unsigned Buffer;
    MSLList *DataObject;
    MComputer *Computer;

    Computer=(MComputer*)Computers;
    if ( !Create() ) goto error;
    // Устанавливаем соединение
    Event(Computer,mseConnecting);
    if ( !Connect((Computer)->Address,2) )
        { Event(Computer,mseNotConnect); goto error; }
    Event(Computer,mseReceiving);
    // Отправляем номер версии интерфейса и принимаем подтверждение
    Buffer=SEND_Version;
    if ( (!Send((char*)&Buffer,sizeof(unsigned),2))||
        (!Recv((char*)&Buffer,sizeof(unsigned),2))||
        (Buffer!=mstAccept) ) { Event(Computer,msePrclError); goto error; }
    // Отправляем запрос на данные и принимаем их размер
    switch(Mode)
    {
        case mssGetGames: Buffer=mstGetGames; DataObject=Games; break;
        case mssGetConfig: Buffer=mstGetConfig; DataObject=Options; break;
        default: goto error;
    }
    if ( (!Send((char*)&Buffer,sizeof(unsigned),3))||
        (!Recv((char*)&DataSize,sizeof(unsigned),10))||
        (DataSize<SEND_MinData)||
        (DataSize>SEND_MaxData) ) { Event(Computer,msePrclError); goto error; }
    // Выделяем память под данные
    if ( (Data=new char[DataSize])==NULL ) goto error;
    // Принимаем их и повторный размер, сверяем его с первым
    if ( (!Recv(Data,DataSize,10))||
        (!Recv((char*)&Buffer,sizeof(unsigned),10))||
        (Buffer!=DataSize) ) { Event(Computer,msePrclError); goto error; }
    //
    if ( !DataObject->GetAllData(Data,Data+DataSize) ) goto error;
    // Отправляем подтверждение
    Buffer=mstAccept;
    if ( !Send((char*)&Buffer,sizeof(unsigned),10) )
        { Event(Computer,msePrclError); goto error; }
    // Завершаем соединение
    Event(Computer,mseDisconnecting);
    if ( !Disconnect(10) ) { Event(Computer,msePrclError); goto error; }
    Event(Computer,mseExecute);
error:
    Close();
    delete[] Data; Data=NULL;
}
//---------------------------------------------------------------------------
void MSend::SetShell(HWND Window_, UINT MinMsg_)
{
    Window=Window_;
    MinMsg=MinMsg_;
}
//---------------------------------------------------------------------------
bool MSend::Send(MComputer **Computers_, MGames *Games_, MClOptions *Options_)
{
    char *pt;

    Stop();

    Computers=Computers_;
    // Оределяем размер данных
    if ( Games_!=NULL ) DataSize=Games_->GetAllDataSize(true);
    else if ( Options_!=NULL ) DataSize=Options_->GetAllDataSize(true);
    else DataSize=0;
    // Выделяем память под буффер
    Data=new char[DataSize+sizeof(unsigned)*3];
    if ( Data==NULL ) goto error;
    // Заполняем его
    if ( Games_!=NULL )
    {
        pt=MemSet(Data,(unsigned)mstGames);
        pt=MemSet(pt,(unsigned)DataSize);
        pt=Games_->SetAllData(pt,true);
    } else if ( Options_!=NULL )
    {
        pt=MemSet(Data,(unsigned)mstConfig);
        pt=MemSet(pt,(unsigned)DataSize);
        pt=Options_->SetAllData(pt,true);
    }
    // Завершающий блок
    pt=MemSet(pt,(unsigned)DataSize);
    DataSize=pt-Data;

    //
    Mode=mssSend;
    // Создаем поток для выполнения отправки данных
    if ( (Thread=::CreateThread(NULL,0,&ThreadFunc,
        (LPVOID)this,0,&ThreadID))==NULL ) goto error;

    return true;
error:
    delete[] Data; Data=NULL;
    return false;
}
//---------------------------------------------------------------------------
bool MSend::Get(MComputer *Computer_, MGames *Games_, MClOptions *Options_)
{
    Stop();

    if ( Games_!=NULL ) Mode=mssGetGames;
    else if ( Options_!=NULL ) Mode=mssGetConfig;
    else goto error;

    Computers=(MComputer**)Computer_;
    Games=Games_; Options=Options_;

    // Создаем поток для выполнения приема данных
    if ( (Thread=::CreateThread(NULL,0,&ThreadFunc,
        (LPVOID)this,0,&ThreadID))==NULL ) goto error;

    return true;
error:
    return false;
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
    ::CloseHandle(Thread); Thread=NULL; ThreadID=0;
}
//---------------------------------------------------------------------------
MSendCl::MSendCl()
{
    Thread=NULL;
    ThreadID=0;
    Socket=INVALID_SOCKET;
    RemoteSocket=INVALID_SOCKET;
    Data=NULL;
    State=NULL;
}
//---------------------------------------------------------------------------
MSendCl::~MSendCl()
{
}
//---------------------------------------------------------------------------
bool MSendCl::Create()
{
    BOOL a;
    unsigned long b;
    sockaddr_in Address;

    // Создаем сокет
    if ( (Socket=::socket(PF_INET,SOCK_STREAM,IPPROTO_IP))==INVALID_SOCKET ) goto error;
    //
    b=TRUE;
    if ( ::setsockopt(Socket,SOL_SOCKET,SO_REUSEADDR,(char*)&a,sizeof(a))||
        ::ioctlsocket(Socket,FIONBIO,&b) ) goto error;
    // Задаем адрес для ожидания соединений
    memset(&Address,0,sizeof(Address));
    Address.sin_family=AF_INET;
    Address.sin_port=::htons(SEND_Port);
    Address.sin_addr.s_addr=INADDR_ANY;
    // Присоединяем сокет
    if ( ::bind (Socket,(sockaddr*)&Address,sizeof(Address))==SOCKET_ERROR ) goto error;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool MSendCl::Listen()
{
    MSG Msg;
    fd_set fds;
    timeval timeout;
    int result;

    // Переводим сокет в режим приема соединений
    if ( ::listen(Socket,1) ) goto exit;
    // Ожидаем соединения
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HANDLE)-1,0,0,PM_REMOVE) ) goto exit;
        FD_ZERO(&fds); FD_SET(Socket,&fds);
        if ( (result=::select(0,&fds,NULL,NULL,&timeout))==SOCKET_ERROR ) goto exit;
    } while((result!=1)||(!FD_ISSET(Socket,&fds)));

    return true;
exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSendCl::Accept()
{
    // Принимаем соединение
    RemoteSocket=::accept(Socket,NULL,NULL);
    return RemoteSocket!=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
bool MSendCl::Send(char *Data_, unsigned Size_, unsigned Time_)
{
    MSG Msg;
    fd_set w_fds, e_fds;
    timeval timeout;
    int result;
    DWORD stime;

    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HANDLE)-1,0,0,PM_NOREMOVE) ) goto exit;
        // Ожидаем возможности передать еще данные
        FD_ZERO(&w_fds); FD_SET(RemoteSocket,&w_fds);
        FD_ZERO(&e_fds); FD_SET(RemoteSocket,&e_fds);
        if ( (result=::select(0,NULL,&w_fds,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(RemoteSocket,&e_fds) ) goto exit;
        // Предаем очередную порцию данных
        if ( (result=::send(RemoteSocket,Data_,Size_,0))==SOCKET_ERROR ) goto exit;
        if ( (Size_-=result)==0 ) return true;
        Data_+=result;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSendCl::Recv(char *Data_, unsigned Size_, unsigned Time_)
{
    MSG Msg;
    fd_set r_fds, e_fds;
    timeval timeout;
    int result;
    DWORD stime;

    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HANDLE)-1,0,0,PM_NOREMOVE) ) goto exit;
        // Ожидаем возможности передать еще данные
        FD_ZERO(&r_fds); FD_SET(RemoteSocket,&r_fds);
        FD_ZERO(&e_fds); FD_SET(RemoteSocket,&e_fds);
        if ( (result=::select(0,&r_fds,NULL,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(RemoteSocket,&e_fds) ) goto exit;
        // Принимаем очередную порцию данных
        result=::recv(RemoteSocket,Data_,Size_,0);
        if ( (result==SOCKET_ERROR)||(result==0) ) goto exit;
        if ( (Size_-=result)==0 ) return true;
        Data_+=result;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
bool MSendCl::Disconnect(unsigned Time_)
{
    MSG Msg;
    fd_set r_fds, e_fds;
    timeval timeout;
    int result;
    DWORD stime;
    char buffer[1];

    // Оповещаем партнера, что отправки данных больше не будет
    if ( ::shutdown(RemoteSocket,SD_SEND)==SOCKET_ERROR ) goto exit;
    //
    Time_*=1000; stime=::GetTickCount();
    timeout.tv_sec=0; timeout.tv_usec=100*1000;
    do
    {
        if ( ::PeekMessage(&Msg,(HANDLE)-1,0,0,PM_NOREMOVE) ) goto exit;
        // Ожидаем возможности принять еще данные
        FD_ZERO(&r_fds); FD_SET(RemoteSocket,&r_fds);
        FD_ZERO(&e_fds); FD_SET(RemoteSocket,&e_fds);
        if ( (result=::select(0,&r_fds,NULL,&e_fds,&timeout))==0 ) continue;
        if ( (result==SOCKET_ERROR)||FD_ISSET(Socket,&e_fds) ) goto exit;
        // Проверяем пришел ли маркер конца файла
        result=::recv(Socket,buffer,sizeof(buffer),0);
        if ( result==SOCKET_ERROR ) goto exit;
        if ( result==0 ) return true;
    } while((::GetTickCount()-stime)<Time_);

exit:
    return false;
}
//---------------------------------------------------------------------------
void MSendCl::CloseRemote()
{
    // Останавливаем прием/отправку данных
    ::shutdown(RemoteSocket,SD_BOTH);
    // Закрываем сокет
    ::closesocket(RemoteSocket);
    RemoteSocket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
void MSendCl::Close()
{
    // Останавливаем прием/отправку данных
    ::shutdown(Socket,SD_BOTH);
    // Закрываем сокет
    ::closesocket(Socket);
    Socket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
DWORD WINAPI MSendCl::ThreadFunc(LPVOID Data)
{
    // Создаем очередь сообщений для потока
    ::PeekMessage(NULL,(HANDLE)-1,0,0,PM_NOREMOVE);
    // Обработка тела потока
    ((MSendCl*)Data)->ThreadProc();
    // Завершаем работу потока
    ::ExitThread(0);
    return 0;
}
//---------------------------------------------------------------------------
void MSendCl::ThreadProc()
{
    unsigned Buffer, Type;

    // Создаем сокет
    if ( !Create() ) goto error;
    // Обрабатываем поступающие соединения
    while(Listen())
    {
        // Принимаем соединение
        if ( !Accept() ) continue;
        // Принимаем номер версии интерфейса
        Buffer=0;
        if ( (!Recv((char*)&Buffer,sizeof(unsigned),3))||
            (Buffer!=SEND_Version) ) goto next;
        // Отправляем подтверждение и принимаем тип данных
        Buffer=mstAccept;
        if ( (!Send((char*)&Buffer,sizeof(unsigned),3))||
             (!Recv((char*)&Type,sizeof(unsigned),3))||
             ((Type!=mstConfig)&&(Type!=mstGames)&&
             (Type!=mstGetConfig)&&(Type!=mstGetGames)) ) goto next;
        // ОБрабатываем запрос
        if ( (Type==mstConfig)||(Type==mstGames) ) ProcessRecv(Type);
        else ProcessSend(Type);
        // Завершаем соединение
        Disconnect(10);
next:
        // Закрываем сокет
        CloseRemote();
    }
    // Закрываем принимающий сокет
    Close();

error:
    return;
}
//---------------------------------------------------------------------------
bool MSendCl::ProcessRecv(unsigned Type_)
{
    unsigned Buffer, Size;
    MClOptions Options;
    MGames Games;

    // Принимаем размер данных
    if ( (!Recv((char*)&Size,sizeof(unsigned),3))||
         (Size<SEND_MinData)||
         (Size>SEND_MaxData) ) goto error;
    // Выделяем память под данные
    if ( (Data=new char[Size])==NULL ) goto error;
    // Принимаем данные
    if ( !Recv(Data,Size,10) ) goto error;
    // Принимаем размер еще раз и сверяем с первым
    if ( (!Recv((char*)&Buffer,sizeof(unsigned),10))||
         (Buffer!=Size) ) goto error;
    // Передаем новые данные управляющему компоненту
    switch(Type_)
    {
        case mstConfig:
             if ( !(Options.GetAllData(Data,Data+Size)&&
                State->NewOptions(&Options)) ) goto error;
            break;
        case mstGames:
            if ( !(Games.GetAllData(Data,Data+Size)&&
                State->NewGames(&Games)) ) goto error;
            Games.Clear();
            break;
        default: goto error;
    }
    // Отправляем подтверждение
    Buffer=mstAccept;
    if ( !Send((char*)&Buffer,sizeof(unsigned),10) ) goto error;

    delete[] Data; Data=NULL;
    return  true;
error:
    delete[] Data; Data=NULL;
    return false;
}
//---------------------------------------------------------------------------
bool MSendCl::ProcessSend(unsigned Type_)
{
    MSLList *DataObject;
    unsigned Buffer, Size;
    char *pt;

    // Запрашиваем данные у управляющего компонента
    switch(Type_)
    {
        case mstGetConfig:
            if ( ((DataObject=new MClOptions)==NULL)||
                (!State->GetOptions((MClOptions*)DataObject)) ) goto error;
            break;
        case mstGetGames:
            if ( ((DataObject=new MGames)==NULL)||
                (!State->GetGames((MGames*)DataObject)) ) goto error;
            break;
        default: DataObject=NULL; goto error;
    }
    // Определяем размер данных и выделяем память под буффер
    Size=DataObject->GetAllDataSize(true);
    if ( (Data=new char[Size+sizeof(unsigned)*2])==NULL ) goto error;
    // Заполняем буффер
    pt=MemSet(Data,(unsigned)Size);
    pt=DataObject->SetAllData(pt,true);
    pt=MemSet(pt,(unsigned)Size);
    Size=pt-Data;
    // Отправляем данные и принимаем подтверждение
    if ( !(Send(Data,Size,10)&&
        Recv((char*)&Buffer,sizeof(unsigned),10)&&
        (Buffer==mstAccept)) ) goto error;

    delete DataObject;
    delete[] Data; Data=NULL;
    return true;
error:
    delete DataObject;
    delete[] Data; Data=NULL;
    return false;
}
//---------------------------------------------------------------------------
void MSendCl::Associate(MStateCl *State_)
{
    State=State_;
}
//---------------------------------------------------------------------------
bool MSendCl::Start()
{
    // Создаем поток для выполнения отправки и приема данных
    return (Thread=::CreateThread(NULL,0,&ThreadFunc,
        (LPVOID)this,0,&ThreadID))!=NULL;
}
//---------------------------------------------------------------------------
void MSendCl::Stop()
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
    ::CloseHandle(Thread); Thread=NULL; ThreadID=0;
}
//---------------------------------------------------------------------------

