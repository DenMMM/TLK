#ifndef DIALOGAUTH_H
#define DIALOGAUTH_H
//---------------------------------------------------------------------------
#include <QDialog>
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogAuth;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogAuth : public QDialog
{
    Q_OBJECT
public:
    dialogAuth(QWidget* parent = 0);
    ~dialogAuth();

private:
    Ui::dialogAuth *ui;
};
//---------------------------------------------------------------------------
#endif // DIALOGAUTH_H
