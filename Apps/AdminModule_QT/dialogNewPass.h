#ifndef DIALOGNEWPASS_H
#define DIALOGNEWPASS_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class dialogNewPass;
}
QT_END_NAMESPACE

class dialogNewPass : public QDialog
{
    Q_OBJECT
public:
    dialogNewPass(QWidget* parent = 0);
    ~dialogNewPass();

private:
    Ui::dialogNewPass *ui;
};

#endif // DIALOGNEWPASS_H
