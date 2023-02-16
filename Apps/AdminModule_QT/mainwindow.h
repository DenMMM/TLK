#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//---------------------------------------------------------------------------
#include <QMainWindow>
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class mainwindow;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainwindow(QWidget *parent = nullptr);
    ~mainwindow();

    void SetShell();

private slots:
    void on_menuLogIn_clicked();
    void on_menuLogOut_clicked();
    void on_menuComputers_clicked();
    void on_menuTariffs_clicked();
    void on_menuFines_clicked();
    void on_menuUsers_clicked();
    void on_menuCommon_clicked();
    void on_menuAuth_clicked();
    void on_menuOpenConfig_clicked();
    void on_menuOptionsPassword_clicked();
    void on_menuUsersPasswords_clicked();
    void on_buttonRun_clicked();
    void on_buttonAdd_clicked();
    void on_buttonFine_clicked();
    void on_buttonClose_clicked();

private:
    Ui::mainwindow *ui;

    QAction* actionLogIn;
    QAction* actionLogOut;
    QMenu* menuAdmin;
    QAction* actionClient;
    QAction* actionAuth;
    QAction* actionLogReset;
    QAction* actionConfigOpen;
};
//---------------------------------------------------------------------------
#endif // MAINWINDOW_H
