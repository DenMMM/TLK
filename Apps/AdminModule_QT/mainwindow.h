#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//---------------------------------------------------------------------------
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class mainwindow;
}
QT_END_NAMESPACE

class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainwindow(QWidget *parent = nullptr);
    ~mainwindow();

    void SetShell();

private slots:
    void on_menuLogIn_clicked();

    void on_menuComputers_clicked();

    void on_menuTariffs_clicked();

    void on_menuFines_clicked();

    void on_menuUsers_clicked();

    void on_menuAuth_clicked();

    void on_menuOpenConfig_clicked();

    void on_buttonRun_clicked();

    void on_buttonAdd_clicked();

    void on_buttonFine_clicked();

    void on_buttonClose_clicked();

private:
    Ui::mainwindow *ui;
};

#endif // MAINWINDOW_H
