#ifndef DIALOGOPENCONFIG_H
#define DIALOGOPENCONFIG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class dialogOpenConfig;
}
QT_END_NAMESPACE

class dialogOpenConfig : public QDialog
{
    Q_OBJECT
public:
    dialogOpenConfig(QWidget* parent = 0);
    ~dialogOpenConfig();

private:
    Ui::dialogOpenConfig *ui;
};

#endif // DIALOGOPENCONFIG_H
