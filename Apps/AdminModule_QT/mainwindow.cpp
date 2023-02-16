//---------------------------------------------------------------------------
#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "unitGlobal.h"
#include "dialogLogIn.h"
#include "dialogComputers.h"
#include "dialogTariffs.h"
#include "dialogFines.h"
#include "dialogUsers.h"
#include "dialogOptions.h"
#include "dialogAuth.h"
#include "dialogOpenConfig.h"
#include "dialogOptionsPass.h"
#include "dialogUserPass.h"
#include "dialogFine.h"
//---------------------------------------------------------------------------
mainwindow::mainwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainwindow)
{
    ui->setupUi(this);

    QMenu* menuFile = ui->menuBar->addMenu(u8"Файл");
    actionLogIn = menuFile->addAction(u8"Начать новую смену...", this, SLOT(on_menuLogIn_clicked()));
    actionLogOut = menuFile->addAction(u8"Закончить смену", this, SLOT(on_menuLogOut_clicked()));
    menuFile->addSeparator();
    menuFile->addAction(u8"Закрыть");

    QMenu* menuOptions = ui->menuBar->addMenu(u8"Настройки");

    menuAdmin = menuOptions->addMenu(u8"Модуль управления");
    menuAdmin->addAction(u8"Компьютеры...", this, SLOT(on_menuComputers_clicked()));
    menuAdmin->addAction(u8"Тарифы...", this, SLOT(on_menuTariffs_clicked()));
    menuAdmin->addAction(u8"Штрафы...", this, SLOT(on_menuFines_clicked()));
    menuAdmin->addSeparator();
    menuAdmin->addAction(u8"Пользователи...", this, SLOT(on_menuUsers_clicked()));
    menuAdmin->addSeparator();
    menuAdmin->addAction(u8"Общие...", this, SLOT(on_menuCommon_clicked()));

    menuOptions->addSeparator();
    actionClient = menuOptions->addAction(u8"Настройка компьютеров...");
    actionAuth = menuOptions->addAction(u8"Сеетвой ключ...", this, SLOT(on_menuAuth_clicked()));
    menuOptions->addSeparator();
    actionLogReset = menuOptions->addAction(u8"Начать новый файл лога");
    menuOptions->addSeparator();
    actionConfigOpen = menuOptions->addAction(u8"Открыть...", this, SLOT(on_menuOpenConfig_clicked()));

    QMenu* menuPasswords = ui->menuBar->addMenu(u8"Пароли");
    menuPasswords->addAction(u8"На настройки...", this, SLOT(on_menuOptionsPassword_clicked()));
    menuPasswords->addSeparator();
    menuPasswords->addAction(u8"Пользователей...", this, SLOT(on_menuUsersPasswords_clicked()));

    QMenu* menuHelp = ui->menuBar->addMenu(u8"Справка");
    menuHelp->addAction(u8"Описание");
    menuHelp->addSeparator();
    menuHelp->addAction(u8"О программе");

    ui->progressBarNetProcess->setMaximum(Sync->gPCountMax());

    SetShell();
}
//---------------------------------------------------------------------------
mainwindow::~mainwindow()
{
    delete ui;
}
//---------------------------------------------------------------------------
void mainwindow::on_menuLogIn_clicked()
{
    // Открываем диалог авторизации
    dialogLogIn dialog;
    auto user = dialog.exec(*Users.get());

    if (!user.has_value())
        return;

    // Записываем в лог событие
    if (!Log->AddLogIn(user.value()))
    {
        ShellState->State |= mssErrorLog; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 17, MB_APPLMODAL|MB_OK|MB_ICONERROR, Log->gLastErr());
        return;
    }

    ShellState->User = user.value();
    SetShell();
}
//---------------------------------------------------------------------------
void mainwindow::on_menuLogOut_clicked()
{
    // Запрашиваем подтверждение на закрытие смены
    if (ResMessageBox(HWND(this->winId()), 25, 26,
        MB_APPLMODAL|MB_YESNO|MB_ICONQUESTION) != IDYES )
    {
        return;
    }

    // Записываем в лог событие
    if (!Log->AddLogOut())
    {
        ShellState->State |= mssErrorLog; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 18, MB_APPLMODAL|MB_OK|MB_ICONERROR, Log->gLastErr());
        return;
    }

    ShellState->User = 0; SetShell();

    // Проверяем не пора ли начать новый файл
    if (!Log->CheckPeriod(Options->LogPeriod))
        return;

    // Закрываем текущий лог и начинаем новый
    if (!(Log->End() &&
        Log->Begin(
            *ShellState,
            *States,
            *Tariffs,
            *Fines,
            *Users)))
    {
        ShellState->State |= mssErrorLog; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 6, MB_APPLMODAL|MB_OK|MB_ICONERROR, Log->gLastErr());
    }
}
//---------------------------------------------------------------------------
void mainwindow::on_menuComputers_clicked()
{
    if (!(ShellState->State & mssConfig))
        return;

    dialogComputers dialog;

    auto result = dialog.exec(*Computers.get(), this->x() + 30, this->y() +50);

    if (!result.has_value())
        return;

    // Замещаем список компьютеров записями из буфера
    *Computers = std::move(*result);
    // Сохраняем в файле
    if (!Computers->Save())
    {
        ShellState->State |= mssErrorConfig; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 3, MB_APPLMODAL|MB_OK|MB_ICONERROR, Computers->gLastErr());
        return;
    }
    // Останавливаем сетевые операции
    Sync->Stop();
    if (States->Update(*Computers))
    {
        // Запись в логах
        if (!Log->AddComputers(*States))
        {
            // Настройки сохранили, но без отображения их в логе работать не дадим
            ShellState->State |= mssErrorLog|mssErrorConfig; SetShell();
            ResMessageBox(HWND(this->winId()), 1, 5, MB_APPLMODAL|MB_OK|MB_ICONERROR, Log->gLastErr());
        }
        // Сохраняем новые состояния в файле
        if (!States->Save())
        {
            ShellState->State |= mssErrorState; SetShell();
            ResMessageBox(HWND(this->winId()), 1, 8, MB_APPLMODAL|MB_OK|MB_ICONERROR, States->gLastErr());
        }
    }
    //
    Sync->Associate(States.get(), Computers.get());
    // Обновляем индикатор сетевой активности и список компьютеров в главном окне
    ui->progressBarNetProcess->setMaximum(Sync->gPCountMax());
//    UpdateListViewComputers(true);
    // Запускаем сетевые операции
    Sync->Start();
}
//---------------------------------------------------------------------------
void mainwindow::on_menuTariffs_clicked()
{
    if (!(ShellState->State & mssConfig))
        return;

    dialogTariffs dialog;

    auto result = dialog.exec(*Tariffs.get(), *Computers.get(), this->x() + 30, this->y() +50);

    if (!result.has_value())
        return;

    // Замещаем тарифы записями из буфера
    *Tariffs = std::move(*result);
    // Задаем ID-номера для новых тарифов
    Tariffs->SetUUIDs();
    // Сохраняем в файле
    if (!Tariffs->Save())
    {
        ShellState->State |= mssErrorConfig; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 3, MB_APPLMODAL|MB_OK|MB_ICONERROR, Tariffs->gLastErr());
        return;
    }
    // Запись в логах
    if (!Log->AddTariffs(*Tariffs))
    {
        // Настройки сохранили, но без отображения их в логе работать не дадим
        ShellState->State |= mssErrorLog|mssErrorConfig; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 5, MB_APPLMODAL|MB_OK|MB_ICONERROR, Log->gLastErr());
    }
    // Обновляем список компьютеров в главном окне
//    UpdateListViewComputers(true);
}
//---------------------------------------------------------------------------
void mainwindow::on_menuFines_clicked()
{
    if (!(ShellState->State & mssConfig))
        return;

    dialogFines dialog;

    auto result = dialog.exec(*Fines.get(), this->x() + 30, this->y() +50);

    if (!result.has_value())
        return;

    // Замещаем штрафами из буфера текущие
    *Fines = std::move(*result);
    // Задаем ID-номера для новых
    Fines->SetUUIDs();
    // Сохраняем в файле
    if (!Fines->Save())
    {
        ShellState->State |= mssErrorConfig; SetShell();
        ResMessageBox(HWND(this->winId()), 1 , 3, MB_APPLMODAL|MB_OK|MB_ICONERROR, Fines->gLastErr());
        return;
    }
    // Запись в логах
    if (!Log->AddFines(*Fines))
    {
        // Настройки сохранили, но без отображения их в логе работать не дадим
        ShellState->State |= mssErrorLog|mssErrorConfig; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 5, MB_APPLMODAL|MB_OK|MB_ICONERROR, Log->gLastErr());
    }
}
//---------------------------------------------------------------------------
void mainwindow::on_menuUsers_clicked()
{
    if (!(ShellState->State & mssConfig))
        return;

    dialogUsers dialog;
    auto result = dialog.exec(*Users.get(), this->x() + 30, this->y() +50);

    if (!result.has_value())
        return;

    // Замещаем актуальных пользователями из буфера
    *Users = std::move(*result);
    // Задаем ID-номера для новых
    Users->SetUUIDs();                      /// проверить чья смена открыта
    // Сохраняем в файле
    if (!Users->Save())
    {
        ShellState->State |= mssErrorConfig; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 3, MB_APPLMODAL|MB_OK|MB_ICONERROR, Users->gLastErr());
        return;
    }
    // Запись в логах
    if (!Log->AddUsers(*Users))
    {
        // Настройки сохранили, но без отображения их в логе работать не дадим
        ShellState->State |= mssErrorLog|mssErrorConfig; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 5, MB_APPLMODAL|MB_OK|MB_ICONERROR, Log->gLastErr());
    }
}
//---------------------------------------------------------------------------
void mainwindow::on_menuCommon_clicked()
{
    if (!(ShellState->State & mssConfig))
        return;

    dialogOptions dialog;
    auto result = dialog.exec(*Options.get(), this->x() + 30, this->y() +50);

    if (!result.has_value())
        return;

    // Замещаем актуальные настройки новыми
    *Options = std::move(*result);

    // Сохраняем в файле
    if (!Options->Save())
    {
        ShellState->State |= mssErrorConfig; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 31, MB_APPLMODAL|MB_OK|MB_ICONERROR, Options->gLastErr());
        return;
    }

    // Запись в логах
    if (!Log->AddOptions())
    {
        // Настройки сохранили, но без отметки об их изменении (надо ли ?) работать не дадим
        ShellState->State |= mssErrorLog|mssErrorConfig; SetShell();
        ResMessageBox(HWND(this->winId()), 1, 5, MB_APPLMODAL|MB_OK|MB_ICONERROR, Log->gLastErr());
    }
}
//---------------------------------------------------------------------------
void mainwindow::on_menuAuth_clicked()
{
    if (!(ShellState->State & mssConfig))
        return;

    dialogAuth dialog;
    dialog.exec();
}
//---------------------------------------------------------------------------
void mainwindow::on_menuOpenConfig_clicked()
{
    if (!(ShellState->State & mssConfig))
    {
        dialogOpenConfig dialog;

        if (!dialog.exec(Options->Pass, this->x() + 20, this->y() + 50, true))
            return;
    }

    ShellState->State ^= mssConfig; SetShell();
    Log->AddConfig(ShellState->State & mssConfig);
}
//---------------------------------------------------------------------------
void mainwindow::on_menuOptionsPassword_clicked()
{
    dialogOptionsPass dialog;

    if (!dialog.exec(*Options, this->x() + 100, this->y() + 50))
        return;

    if (!Options->Save())
    {
//        ShellState->State |= mssErrorConfig; SetShell();  /// не имеет смысла и опасно
        ResMessageBox(HWND(this->winId()), 1, 31, MB_APPLMODAL|MB_OK|MB_ICONERROR, Options->gLastErr());
    }
}
//---------------------------------------------------------------------------
void mainwindow::on_menuUsersPasswords_clicked()
{
    dialogUserPass dialog;

    if (!dialog.exec(*Users, this->x() + 100, this->y() + 50, true))
        return;

    if (!Users->Save())
    {
//        ShellState->State |= mssErrorConfig; SetShell();  /// не имеет смысла и опасно
        ResMessageBox(HWND(this->winId()), 1 , 3, MB_APPLMODAL|MB_OK|MB_ICONERROR, Users->gLastErr());
    }
}
//---------------------------------------------------------------------------
void mainwindow::on_buttonRun_clicked()
{

}
//---------------------------------------------------------------------------
void mainwindow::on_buttonAdd_clicked()
{

}
//---------------------------------------------------------------------------
void mainwindow::on_buttonFine_clicked()
{
    dialogFine dialog;
    dialog.exec();
}
//---------------------------------------------------------------------------
void mainwindow::on_buttonClose_clicked()
{
    this->close();
}
//---------------------------------------------------------------------------
void mainwindow::SetShell()
{
    // Общие операции
    bool enable =
        (ShellState->State & mssConfig) ||
        (ShellState->User != 0);
    enable =
        ShellState->State &
        (mssErrorState | mssErrorConfig)?
        false: enable;

    ui->buttonRun->setEnabled(enable);      // NCmpRun->Enabled=enable;
    ui->buttonAdd->setEnabled(enable);      // NCmpAdd->Enabled=enable;
    ui->buttonFine->setEnabled(enable);     // NCmpFine->Enabled=enable;
    ui->buttonExchange->setEnabled(enable); // NCmpExchange->Enabled=enable;
    ui->buttonLock->setEnabled(enable);     // NCmpLock->Enabled=enable;
    ui->buttonUnLock->setEnabled(enable);   // NCmpUnLock->Enabled=enable;
    ui->buttonAdditionals->setEnabled(enable);

    // Приостановка компьютеров
//    NPause->Enabled=((ShellState->User!=0)&&(Options->UsersRights&murPause))||
//		(ShellState->State&mssConfig);
//	NRun->Enabled=NPause->Enabled;

    // Настройки, доступные только сисадмину
    actionConfigOpen->setText(
        ShellState->State & mssConfig?
        u8"Закрыть":
        u8"Открыть...");

    enable =
        (ShellState->State & mssConfig) &&
        !(ShellState->State & mssErrorLog);

    menuAdmin->setEnabled(enable);
//    NComputers->Enabled=enable;
//    NTariffs->Enabled=enable;
//    NFines->Enabled=enable;
//    NUsers->Enabled=enable;
//    NCommon->Enabled=enable;

    actionClient->setEnabled(enable);
    actionAuth->setEnabled(enable);

    ui->buttonSystem->setEnabled(enable);

    // Общесистемные функции
    actionLogIn->setEnabled(ShellState->User == 0);
    actionLogOut->setEnabled(ShellState->User != 0);
//    NOptionsPassword->Enabled=!error;
//    NUsersPasswords->Enabled=!error;

    // Аварийный сброс лога
    actionLogReset->setEnabled(
        (ShellState->State & mssConfig) &&
        (ShellState->State & mssErrorLog));

    // Заголовок окна программы
    QString line = u8"TLK - ";
    auto iUser = Users->SrchUUID(ShellState->User);

    if (iUser != Users->end())
        line += QString::fromStdWString(iUser->Login);
    else
        line += u8"смена не начата";

    if (ShellState->State & mssConfig)
        line += u8" (открыты настройки)";

    this->setWindowTitle(line);

    // Строка состояния
    line.clear();

    if (ShellState->State & mssErrorConfig)
        line += u8"Ошибка работы с конфигурацией, ";
    if (ShellState->State & mssErrorState)
        line += u8"Не удалось сохранить последнее состояние, ";
    if (ShellState->State & mssErrorLog)
        line += u8"Ошибка работы с логом, ";

    this->setStatusTip(line);   /// проверить
}
