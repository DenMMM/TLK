#ifndef DIALOGFINE_H
#define DIALOGFINE_H
//---------------------------------------------------------------------------
#include <QDialog>
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogFine;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogFine : public QDialog
{
    Q_OBJECT
public:
    dialogFine(QWidget* parent = 0);
    ~dialogFine();

private slots:
    void on_buttonFine_clicked();

private:
    Ui::dialogFine *ui;
};
//---------------------------------------------------------------------------
#endif // DIALOGFINE_H
