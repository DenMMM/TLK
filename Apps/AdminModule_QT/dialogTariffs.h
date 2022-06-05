#ifndef DIALOGTARIFFS_H
#define DIALOGTARIFFS_H

#include <QDialog>
#include <QListWidget>

#include <optional>
#include <vector>
#include "..\..\Core\UnitTariffs.h"
#include "..\..\Core\UnitComputers.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class dialogTariffs;
}
QT_END_NAMESPACE

class dialogTariffs : public QDialog
{
    Q_OBJECT
public:
    dialogTariffs(QWidget* parent = 0);
    ~dialogTariffs();

    std::optional<MTariffs> exec(
            const MTariffs& src_,
            const MComputers& computers_,
            int left_, int top_);

private slots:
    void on_listWidgetNames_itemSelectionChanged();

    void on_lineEditName_editingFinished();

    void on_checkBoxRoute_clicked(bool checked);

    void on_checkBoxReboot_clicked(bool checked);

    void on_checkBoxDesktop_clicked(bool checked);

    void on_buttonTimes_clicked();

    void on_listWidgetApps_itemChanged(QListWidgetItem *item);

    void on_listWidgetComputers_itemChanged(QListWidgetItem *item);

    void on_buttonAdd_clicked();

    void on_buttonDel_clicked();

private:
    Ui::dialogTariffs *ui;

    MTariffs tmp_tariffs;
    std::vector <std::int8_t> tmp_comps;

    bool updating_listWidgetApps;
    bool updating_listWidgetComputers;

    void set_listWidgetNames_line(int row_index_, MTariffsItem* tariff_);
    void set_edit(bool edit_);
};

#endif // DIALOGTARIFFS_H
