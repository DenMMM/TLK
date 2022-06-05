#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "unitGlobal.h"
#include "dialogLogIn.h"
#include "dialogComputers.h"
#include "dialogTariffs.h"
#include "dialogFines.h"
#include "dialogUsers.h"
#include "dialogAuth.h"
#include "dialogOpenConfig.h"
#include "dialogFine.h"

mainwindow::mainwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainwindow)
{
    ui->setupUi(this);

    QMenu *menuFile = ui->menuBar->addMenu(u8"Файл");
    menuFile->addAction(u8"Начать новую смену...", this, SLOT(on_menuLogIn_clicked()));
    menuFile->addAction(u8"Закончить смену");
    menuFile->addSeparator();
    menuFile->addAction(u8"Закрыть");

    QMenu *menuOptions = ui->menuBar->addMenu(u8"Настройки");

    QMenu *menuAdmin = menuOptions->addMenu(u8"Модуль управления");
    menuAdmin->addAction(u8"Компьютеры...", this, SLOT(on_menuComputers_clicked()));
    menuAdmin->addAction(u8"Тарифы...", this, SLOT(on_menuTariffs_clicked()));
    menuAdmin->addAction(u8"Штрафы...", this, SLOT(on_menuFines_clicked()));
    menuAdmin->addSeparator();
    menuAdmin->addAction(u8"Пользователи...", this, SLOT(on_menuUsers_clicked()));
    menuAdmin->addSeparator();
    menuAdmin->addAction(u8"Общие...");

    menuOptions->addSeparator();
    menuOptions->addAction(u8"Настройка компьютеров...");
    menuOptions->addAction(u8"Сеетвой ключ...", this, SLOT(on_menuAuth_clicked()));
    menuOptions->addSeparator();
    menuOptions->addAction(u8"Начать новый файл лога");
    menuOptions->addSeparator();
    menuOptions->addAction(u8"Открыть...", this, SLOT(on_menuOpenConfig_clicked()));

    QMenu *menuPasswords = ui->menuBar->addMenu(u8"Пароли");
    menuPasswords->addAction(u8"На настройки...");
    menuPasswords->addSeparator();
    menuPasswords->addAction(u8"Пользователей...");

    QMenu *menuHelp = ui->menuBar->addMenu(u8"Справка");
    menuHelp->addAction(u8"Описание");
    menuHelp->addSeparator();
    menuHelp->addAction(u8"О программе");
}

mainwindow::~mainwindow()
{
    delete ui;
}

void mainwindow::SetShell()
{
    ui->progressBarNetProcess->setMaximum(Sync->gPCountMax());

}

void mainwindow::on_menuLogIn_clicked()
{
    dialogLogIn dialog;
    dialog.exec(*Users.get());
}

void mainwindow::on_menuComputers_clicked()
{
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

void mainwindow::on_menuTariffs_clicked()
{
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

void mainwindow::on_menuFines_clicked()
{
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

void mainwindow::on_menuUsers_clicked()
{
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

void mainwindow::on_menuAuth_clicked()
{
    dialogAuth dialog;
    dialog.exec();
}

void mainwindow::on_menuOpenConfig_clicked()
{
    dialogOpenConfig dialog;
    dialog.exec();
}

void mainwindow::on_buttonRun_clicked()
{

}

void mainwindow::on_buttonAdd_clicked()
{

}

void mainwindow::on_buttonFine_clicked()
{
    dialogFine dialog;
    dialog.exec();
}

void mainwindow::on_buttonClose_clicked()
{
    this->close();
}
