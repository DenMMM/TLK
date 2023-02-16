#ifndef DIALOGUSERPASS_H
#define DIALOGUSERPASS_H
//---------------------------------------------------------------------------
#include <QDialog>

#include "..\..\Core\UnitUsers.h"
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogUserPass;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogUserPass : public QDialog
{
    Q_OBJECT
public:
    dialogUserPass(QWidget* parent = 0);
    ~dialogUserPass();

    bool exec(MUsersItem& user, int left, int top, bool leftTop);
    bool exec(MUsers& users, int left, int top, bool leftTop);

private slots:
    void accept() override;

    void on_comboBoxLogin_currentIndexChanged(int index);
    void on_lineEditPassword_textEdited(const QString &arg1);
    void on_pushButtonGenerate_clicked();

private:
    Ui::dialogUserPass *ui;

    bool modeUsers;

    void setCoord(int left, int top, bool leftTop);
    void setEditable(bool modeForCurrent, bool modeForNew);
};
//---------------------------------------------------------------------------
#endif // DIALOGUSERPASS_H
