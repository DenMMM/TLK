#ifndef DIALOGOPTIONS_H
#define DIALOGOPTIONS_H
//---------------------------------------------------------------------------
#include <QDialog>

#include <optional>
#include "..\..\Core\UnitOptions.h"
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogOptions;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogOptions : public QDialog
{
    Q_OBJECT

public:
    explicit dialogOptions(QWidget *parent = nullptr);
    ~dialogOptions();

    std::optional <MOptions> exec(const MOptions& src, int left, int top);

private slots:
    void on_comboBoxLogPeriod_activated(int index);
    void on_lineEditCostDialogTime_editingFinished();
    void on_lineEditCostPrecision_editingFinished();
    void on_lineEditFilterFreeTime_editingFinished();
    void on_checkBoxRightPause_clicked(bool checked);

private:
    Ui::dialogOptions *ui;

    MOptions tmpOptions;
};
//---------------------------------------------------------------------------
#endif // DIALOGOPTIONS_H
