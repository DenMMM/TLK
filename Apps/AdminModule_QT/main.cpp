//---------------------------------------------------------------------------
#include <QApplication>
#include <QDir>

#include "unitGlobal.h"
#include "mainWindow.h"
//---------------------------------------------------------------------------
int on_app_start();
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const int error = on_app_start();

    if (error !=0)
        return error;

    mainwindow w;
    w.SetShell();   ///
    w.show();

    return a.exec();
}
//---------------------------------------------------------------------------
int on_app_start()
{
    try
    {
        // Создадим объекты глобального пользования
        ShellState.reset(new MShellState);
        Options.reset(new MOptions);
        Computers.reset(new MComputers);
        Tariffs.reset(new MTariffs);
        Fines.reset(new MFines);
        Users.reset(new MUsers);
        States.reset(new MStates);
        Sync.reset(new MSync);
        Auth.reset(new MAuth);
        Log.reset(new MLog);

        // Настроим пути хранения файлов и коды шифрования
        Options->SetDefaultKey(
            HKEY_LOCAL_MACHINE,
            L"Software\\MMM Groups\\TLK\\3.0\\Admin",
            L"Options",
            ENC_Code);

        Auth->SetDefaultKey(
            HKEY_LOCAL_MACHINE,
            L"Software\\MMM Groups\\TLK\\3.0\\Admin",
            L"Auth",
            ENC_Code);

        const auto app_path = QDir(QApplication::applicationDirPath());
        const QString path(app_path.toNativeSeparators(app_path.currentPath() + app_path.separator()));
        QString file;

        file = path + "COMPUTERS.DAT";
        Computers->SetDefaultFile(file.toStdWString(), ENC_Code);
        file = path + "TARIFFS.DAT";
        Tariffs->SetDefaultFile(file.toStdWString(), ENC_Code);
        file = path + "FINES.DAT";
        Fines->SetDefaultFile(file.toStdWString(), ENC_Code);
        file = path + "USERS.DAT";
        Users->SetDefaultFile(file.toStdWString(), ENC_Code);
        file = path + "STATES.DAT";
        States->SetDefaultFile(file.toStdWString(),ENC_Code);
        file = path + "ARP.DAT";
        Sync->SetARPFile(file.toStdWString(), ENC_Code, true);
        file = path + "LOGS";
        Log->SetDefault(file.toStdWString(), ENC_Code);

#ifdef _DEBUG
        if ( !Options->Load() )         /// сброс пароля
        {
            Options->Pass.Set(L"");
            Options->Save();
        }
#else
        // Загрузим основные настройки
        if ( !Options->Load() )
        {
            ResMessageBox(HWND(NULL), 1, 30, MB_APPLMODAL|MB_OK|MB_ICONERROR);
            // Без пароля на настройки запускаться не будем
            return -1;
        }
#endif

        // Загрузим остальные настройки, возможно не все
        bool all_load=true;
        all_load=Auth->Load()&&all_load;
        all_load=Computers->Load()&&all_load;
        all_load=Tariffs->Load()&&all_load;
        all_load=Fines->Load()&&all_load;
        all_load=Users->Load()&&all_load;

        if ( !all_load )
        {
            ShellState->State |= mssErrorConfig;
            ResMessageBox(HWND(NULL), 1, 2, MB_APPLMODAL|MB_OK|MB_ICONERROR);
        }

        // Загрузим состояния компьютеров
        if ( !States->Load() )
        {
            States->Clear();
            ShellState->State |= mssErrorState;
            ResMessageBox(HWND(NULL), 1, 7, MB_APPLMODAL|MB_OK|MB_ICONERROR, States->gLastErr());
        } else
            // Хоть состояния и загрузились, сверим их с актуальным списком компьютеров
            States->Update(*Computers);			/// опасное действие ?

        // Загрузим ARP-кэш
        if ( !Sync->LoadARP() )
            ResMessageBox(HWND(NULL), 0, 29, MB_APPLMODAL|MB_OK|MB_ICONWARNING, Sync->gLastErr());

        // Подготавливаем синхронизацию по сети с клиентами
        Sync->NetInit(ENC_Net, Auth.get());
        Sync->Associate(States.get(), Computers.get());

        // Таймер нужен до логов, т.к. им нужно системное время
//    Timer->Interval=1000;
//    Timer->Enabled=true;
//    TimerTimer(nullptr);

        // Открываем лог
        if ( Log->Open() )
        {
            // Определяем пользователя, открывшего смену
            ShellState->User=Log->LastUser();
            // Проверяем есть ли он еще в списке пользователей и активен ли его логин
            auto iUser=Users->SrchUUID(ShellState->User);
            if ( (iUser==Users->end()) || (!iUser->Active) ) ShellState->User=0;
            // Сохраним в логе стартовые параметры, чтобы потом четко разобрать все режимы
            if ( !Log->AddStart(
                *ShellState,
                *States,
                *Tariffs,
                *Fines,
                *Users) )
            {
                ShellState->State |= mssErrorLog;
                ResMessageBox(HWND(NULL), 1, 4, MB_APPLMODAL|MB_OK|MB_ICONERROR, Log->gLastErr());
            }
        } else
        {
            // Если ошибка лога связана с нарушением его формата,
            // попытаемся начать новый файл
            if ( (Log->gLastErr() != 0)||
                (!Log->Begin(
                    *ShellState,
                    *States,
                    *Tariffs,
                    *Fines,
                    *Users)) )
            {
                ShellState->State |= mssErrorLog;
                ResMessageBox(HWND(NULL), 1, 4, MB_APPLMODAL|MB_OK|MB_ICONERROR, Log->gLastErr());
            } else
                ResMessageBox(HWND(NULL), 0, 32, MB_APPLMODAL|MB_OK|MB_ICONWARNING);
        }

//    NFilterAllClick(NFilterAll);
//    StatusBar->SimplePanel=true;
//        mainWindow_.SetShell();

//    TimerNet->Interval=250;
//    TimerNet->Enabled=true;
    // Запускаем синхронизацию
    Sync->Start();

    // Включим буферизацию отрисовки, чтобы не мерцало
//    ListViewComputers->DoubleBuffered=true;
//    ProgressBarNetProcess->DoubleBuffered=true;
    }
    catch (std::bad_alloc &e)
    {
        ResMessageBox(HWND(NULL), 1, 13, MB_APPLMODAL|MB_OK|MB_ICONERROR);
        return -1;
    }
    catch (std::exception &e)
    {
        ::MessageBoxA(HWND(NULL), e.what(),
            "TLK - ошибка", MB_APPLMODAL|MB_OK|MB_ICONERROR);
        return -1;
    }
/*    catch (Exception &exception)
    {
        Application->ShowException(&exception);
        return -1;
    }*/

    return 0;
}
//---------------------------------------------------------------------------
