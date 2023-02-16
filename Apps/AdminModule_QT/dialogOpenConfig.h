#ifndef DIALOGOPENCONFIG_H
#define DIALOGOPENCONFIG_H
//---------------------------------------------------------------------------
#include <QDialog>

#include "..\..\Core\UnitPassword.h"
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogOpenConfig;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogOpenConfig : public QDialog
{
    Q_OBJECT
public:
    dialogOpenConfig(QWidget* parent = 0);
    ~dialogOpenConfig();

    bool exec(
        const MPassword& pass,
        int left, int top, bool leftTop);

private slots:
    void accept() override;

private:
    Ui::dialogOpenConfig *ui;

    const MPassword* configPass;

    void setCoord(int left, int top, bool leftTop);
};
//---------------------------------------------------------------------------
#endif // DIALOGOPENCONFIG_H
