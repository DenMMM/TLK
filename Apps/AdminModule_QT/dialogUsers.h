#ifndef DIALOGUSERS_H
#define DIALOGUSERS_H
//---------------------------------------------------------------------------
#include <QDialog>
#include <QCloseEvent>
#include <QTableWidget>

#include <optional>
#include "..\..\Core\UnitUsers.h"
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogUsers;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogUsers : public QDialog
{
    Q_OBJECT
public:
    dialogUsers(QWidget* parent = 0);
    ~dialogUsers();

    std::optional <MUsers> exec(const MUsers& src, int left, int top);

private slots:
    void on_tableWidgetUsers_itemSelectionChanged();
    void on_lineEditLogin_editingFinished();
    void on_lineEditName_editingFinished();
    void on_buttonActive_clicked();
    void on_buttonInactive_clicked();
    void on_buttonPassword_clicked();
    void on_buttonAdd_clicked();
    void on_buttonDel_clicked();

private:
    Ui::dialogUsers *ui;

    MUsers tmpUsers;

    void updateActive(bool active);
    void setTableWidgetUsersLine(int rowIndex, MUsersItem* user);
    void setEditable(bool mode);
};
//---------------------------------------------------------------------------
#endif // DIALOGUSERS_H
