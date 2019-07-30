//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
AnsiString PrgDir;
//---------------------------------------------------------------------------
void BasicCode(char *Data_, int DataSize_, unsigned int Code_)
{
    for ( int i=0, j=DataSize_-sizeof(unsigned int); i<=j; i++ ) *((unsigned int*)(Data_+i))+=Code_;
}
//---------------------------------------------------------------------------
void BasicEncode(char *Data_, int DataSize_, unsigned int Code_)
{
    for ( int i=DataSize_-sizeof(unsigned int); i>=0; i-- ) *((unsigned int*)(Data_+i))-=Code_;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CorrectComputersData(MComputers *Computers_,MTariffs *Tariffs_)
{
    bool changed=false;
    for ( MComputer *Computer_=(MComputer*)Computers_->FirstItem; Computer_;
        Computer_=(MComputer*)Computer_->NextItem )
    {
        if ( Tariffs_->SearchID(Computer_->TariffID) ) continue;
        Computer_->TariffID=0;
        changed=true;
    }
    return changed;
}
//---------------------------------------------------------------------------
bool CorrectTariffsData(MTariffs *Tariffs_, MComputers *Computers_)
{
    bool changed=false;
    for ( MTariff *Tariff_=(MTariff*)Tariffs_->FirstItem; Tariff_;
        Tariff_=(MTariff*)Tariff_->NextItem )
    {
        for ( MListOfIDsID *ListOfIDsID_=(MListOfIDsID*)Tariff_->Computers.FirstItem,
            *LOfIDsID_; ListOfIDsID_; ListOfIDsID_=LOfIDsID_ )
        {
            if ( ListOfIDsID_ ) LOfIDsID_=(MListOfIDsID*)ListOfIDsID_->NextItem;
            if ( Computers_->SearchID(ListOfIDsID_->ID) ) continue;
            Tariff_->Computers.Delete(ListOfIDsID_);
            changed=true;
        }
    }
    return changed;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MComputerSL::FromComputer(MComputer *Computer_)
{
    ID=Computer_->ID;
    //
    strcpy(Address,Computer_->Address.c_str());
    Number=Computer_->Number;
    GroupColor=Computer_->GroupColor;
    //
    State=Computer_->State;
    StartWorkTime=Computer_->StartWorkTime;
    SizeWorkTime=Computer_->SizeWorkTime;
    StartFineTime=Computer_->StartFineTime;
    SizeFineTime=Computer_->SizeFineTime;
    StopTimerTime=Computer_->StopTimerTime;
    FaceType=Computer_->FaceType;
    GamesPages=Computer_->GamesPages;
    TariffID=Computer_->TariffID;
    StateVer=Computer_->StateVer;
    NetSync=Computer_->NetSync|Computer_->NotSync;
}
//---------------------------------------------------------------------------
void MComputerSL::ToComputer(MComputer *Computer_)
{
    //
    Computer_->ID=ID;
    Computer_->Address=Address;
    Computer_->Number=Number;
    Computer_->GroupColor=GroupColor;
    //
    Computer_->State=State;
    Computer_->StartWorkTime=StartWorkTime;
    Computer_->SizeWorkTime=SizeWorkTime;
    Computer_->StartFineTime=StartFineTime;
    Computer_->SizeFineTime=SizeFineTime;
    Computer_->StopTimerTime=StopTimerTime;
    Computer_->FaceType=FaceType;
    Computer_->GamesPages=GamesPages;
    Computer_->TariffID=TariffID;
    Computer_->StateVer=StateVer;
    Computer_->NetSync=NetSync;
    Computer_->NotSync=0;
}
//---------------------------------------------------------------------------
bool ComputerSave(MComputer *Computer_, char *FileName_)
{
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size, computers_num;
    MComputerSL *all_data=NULL, *current_data;

    // Открываем файл со списком компьютеров
    if ( (file=::CreateFile(FileName_,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Определяем его размер
    if ( (data_size=::GetFileSize(file,NULL))==0xFFFFFFFF ) goto error;
    // Вычисляем количество полных записей в файле
    computers_num=data_size/sizeof(MComputerSL);
    // Выделяем память под данные
    if ( (all_data=new MComputerSL[computers_num+1])==NULL ) goto error;
    // Считываем данные из файла
    if ( ::ReadFile(file,all_data,computers_num*sizeof(MComputerSL),&rw_size,NULL)==NULL ) goto error;
    //
    BasicEncode((char*)all_data,data_size,0x5DCF39EA);
    // Переводим текущую позицию в файле к его началу
    if ( ::SetFilePointer(file,0,NULL,FILE_BEGIN)==0xFFFFFFFF ) goto error;

    // Ищем по ID-номеру описатель компьютера
    current_data=all_data;
    while(computers_num)
    {
        if ( current_data->ID==Computer_->ID ) break;
        computers_num--; current_data++;
    }
    if ( computers_num==0 ) goto error;
    // Копируем данные в структуру для сохранения
    current_data->FromComputer(Computer_);

    BasicCode((char*)all_data,data_size,0x5DCF39EA);
    // Сохраняем данные в файле
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
bool ComputersSave(MComputers *Computers_, char *FileName_)
{
    HANDLE file=NULL;
    DWORD rw_size;
    MComputerSL *all_data=NULL, *current_data;

    // Выделяем память под данные
    if ( (all_data=new MComputerSL[Computers_->CountItems])==NULL ) goto error;
    // Записываем туда параметры компьютеров
    current_data=all_data;
    for ( MComputer *Computer=(MComputer*)Computers_->FirstItem; Computer;
        Computer=(MComputer*)Computer->NextItem )
    {
        current_data->FromComputer(Computer);
        current_data++;
    }
    //
    BasicCode((char*)all_data,Computers_->CountItems*sizeof(MComputerSL),0x5DCF39EA);
    // Создаем файл для сохранения списка компьютеров
    if ( (file=::CreateFile(FileName_,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Сохраняем данные в файле
    if ( ::WriteFile(file,all_data,Computers_->CountItems*sizeof(MComputerSL),
        &rw_size,NULL)==NULL ) goto error;

    ::CloseHandle(file);
    delete[] all_data;
    return true;
error:
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
bool ComputersLoad(MComputers *Computers_, char *FileName_)
{
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size, computers_num;
    MComputerSL *all_data=NULL, *current_data;

    // Открываем файл со списком компьютеров
    if ( (file=::CreateFile(FileName_,GENERIC_READ,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Определяем его размер
    if ( (data_size=::GetFileSize(file,NULL))==0xFFFFFFFF ) goto error;
    // Вычисляем количество полных записей в файле
    computers_num=data_size/sizeof(MComputerSL);
    // Выделяем память под данные
    if ( (all_data=new MComputerSL[computers_num])==NULL ) goto error;
    // Считываем данные из файла
    if ( ::ReadFile(file,all_data,computers_num*sizeof(MComputerSL),&rw_size,NULL)==NULL ) goto error;
    ::CloseHandle(file); file=NULL;

    //
    BasicEncode((char*)all_data,computers_num*sizeof(MComputerSL),0x5DCF39EA);
    //
    current_data=all_data;
    Computers_->Clear();
    while(computers_num)
    {
        MComputer *NewComputer;
        // Добавляем компьютер в список
        NewComputer=(MComputer*)Computers_->Add();
        current_data->ToComputer(NewComputer);
        computers_num--; current_data++;
    }

    delete[] all_data;
    return true;
error:
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int GetGamesDataSize(MGames *Games_)
{
    int Size=1;
    for ( MGame *Game=(MGame*)Games_->FirstItem; Game; Game=(MGame*)Game->NextItem )
    {
        Size+=Game->Name.Length()+1;
        Size+=Game->CommandLine.Length()+1;
        Size+=Game->IconFile.Length()+1;
        if ( Game->SubGames ) Size+=GetGamesDataSize(Game->SubGames)+1;
    }
    return Size;
}
//---------------------------------------------------------------------------
char *SetGamesData(char *Data_, MGames *Games_)
{
    for ( MGame *Game=(MGame*)Games_->FirstItem; Game; Game=(MGame*)Game->NextItem )
    {
        Data_=strcpy(Data_,Game->Name.c_str())+Game->Name.Length()+1;
        Data_=strcpy(Data_,Game->CommandLine.c_str())+Game->CommandLine.Length()+1;
        Data_=strcpy(Data_,Game->IconFile.c_str())+Game->IconFile.Length()+1;
        if ( Game->SubGames==NULL ) continue;
        *Data_=1; Data_++;
        Data_=SetGamesData(Data_,Game->SubGames);
    }
    *Data_=2; Data_++;
    return Data_;
}
//---------------------------------------------------------------------------
char *GetGamesData(char *Data_, char *LimitData_, MGames *Games_)
{
    static int str_length;
    static MGame *NewGame;

    while(Data_<LimitData_)
    {
        NewGame=(MGame*)Games_->Add();
        // Название программы
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=64) ) return NULL;
        NewGame->Name=Data_; Data_+=str_length+1;
        // Командная строка для запуска
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=MAX_PATH) ) return NULL;
        NewGame->CommandLine=Data_; Data_+=str_length+1;
        // Файл с иконкой
        str_length=strlen(Data_);
        if ( ((Data_+str_length+1)>LimitData_)||(str_length>=MAX_PATH) ) return NULL;
        NewGame->IconFile=Data_; Data_+=str_length+1;
        // Наличие потомков у разобранного узла
        if ( (Data_+sizeof(char))>LimitData_ ) return NULL;
        if ( *Data_==1 )
        {
            Data_++;
            NewGame->SubGames=new MGames;
            if ( (Data_=GetGamesData(Data_,LimitData_,NewGame->SubGames))==NULL ) return NULL;
        }
        // Окончание одного уровня дерева
        if ( (Data_+sizeof(char))>LimitData_ ) return NULL;
        if ( *Data_==2 ) { Data_++; break; }
    }

    return Data_;
}
//---------------------------------------------------------------------------
bool GamesSave(MGames *Games_, char *FileName_)
{
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    data_size=GetGamesDataSize(Games_);
    if ( (all_data=new char[data_size])==NULL ) goto error;
    SetGamesData(all_data,Games_);
    //
    BasicCode(all_data,data_size,0x5DCF39EA);

    // Создаем файл для сохранения списка игр
    if ( (file=::CreateFile(FileName_,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Сохраняем данные в файле
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
bool GamesLoad(MGames *Games_, char *FileName_)
{
    HANDLE file=NULL;
    DWORD rw_size;
    int data_size;
    char *all_data=NULL;

    // Открываем файл со списком игр
    if ( (file=::CreateFile(FileName_,GENERIC_READ,0,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE ) goto error;
    // Определяем его размер
    if ( (data_size=::GetFileSize(file,NULL))==0xFFFFFFFF ) goto error;
    // Выделяем память под данные
    if ( (all_data=new char[data_size+1])==NULL ) goto error;
    // Считываем данные из файла
    if ( ::ReadFile(file,all_data,data_size,&rw_size,NULL)==NULL ) goto error;
    ::CloseHandle(file); file=NULL;

    //
    BasicEncode(all_data,data_size,0x5DCF39EA);
    //
    all_data[data_size]=0; Games_->Clear();
    if ( GetGamesData(all_data,all_data+data_size,Games_)==NULL ) goto error;

    delete[] all_data;
    return true;
error:
    if ( (file!=NULL)&&(file!=INVALID_HANDLE_VALUE) ) ::CloseHandle(file);
    if ( all_data!=NULL ) delete[] all_data;
    return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

