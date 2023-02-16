#ifndef DIALOGLOGIN_H
#define DIALOGLOGIN_H
//---------------------------------------------------------------------------
#include <QDialog>

#include <optional>
#include "..\..\Core\UnitUsers.h"
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogLogIn;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogLogIn : public QDialog
{
    Q_OBJECT
public:
    dialogLogIn(QWidget* parent = 0);
    ~dialogLogIn();

    std::optional <std::uint32_t> exec(const MUsers& users);

private slots:
    void accept() override;

    void on_comboBoxLogin_currentIndexChanged(int index);

private:
    Ui::dialogLogIn *ui;

    std::uint32_t userId;
};
//---------------------------------------------------------------------------
#endif // DIALOGLOGIN_H
