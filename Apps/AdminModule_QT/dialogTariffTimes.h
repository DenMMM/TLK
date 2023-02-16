#ifndef DIALOGTARIFFTIMES_H
#define DIALOGTARIFFTIMES_H
//---------------------------------------------------------------------------
#include <QDialog>

#include <optional>
#include "..\..\Core\UnitTariffs.h"
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogTariffTimes;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogTariffTimes : public QDialog
{
    Q_OBJECT
public:
    dialogTariffTimes(QWidget* parent = 0);
    ~dialogTariffTimes();

    std::optional <MTariffTimes> exec(const MTariffTimes& src, int left, int top);

private slots:
    void on_tableWidgetTimes_itemSelectionChanged();
    void on_comboBoxType_activated(int index);
    void on_comboBoxBeginH_activated(int index);
    void on_comboBoxBeginM_activated(int index);
    void on_comboBoxEndH_activated(int index);
    void on_comboBoxEndM_activated(int index);
    void on_comboBoxSizeH_activated(int index);
    void on_comboBoxSizeM_activated(int index);
    void on_lineEditCost_editingFinished();
    void on_buttonAdd_clicked();
    void on_buttonDel_clicked();

private:
    Ui::dialogTariffTimes *ui;

    MTariffTimes tmpTimes;

    void setTableWidgetTimesLine(int rowIndex, MTariffTimesItem* timesItem);
    void setEditable(bool mode);
};
//---------------------------------------------------------------------------
#endif // DIALOGTARIFFTIMES_H
