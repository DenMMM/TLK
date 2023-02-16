#ifndef DIALOGOPTIONSPASS_H
#define DIALOGOPTIONSPASS_H
//---------------------------------------------------------------------------
#include <QDialog>

#include "..\..\Core\UnitOptions.h"
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogOptionsPass;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogOptionsPass : public QDialog
{
    Q_OBJECT
public:
    dialogOptionsPass(QWidget* parent = 0);
    ~dialogOptionsPass();

    bool exec(MOptions& options, int left, int top);

private slots:
    void accept() override;

    void on_lineEditPassword_textEdited(const QString &arg1);
    void on_pushButtonGenerate_clicked();

private:
    Ui::dialogOptionsPass *ui;

    MPassword* optPass;
};
//---------------------------------------------------------------------------
#endif // DIALOGOPTIONSPASS_H
