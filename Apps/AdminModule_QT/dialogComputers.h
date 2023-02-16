#ifndef DIALOGCOMPUTERS_H
#define DIALOGCOMPUTERS_H
//---------------------------------------------------------------------------
#include <QDialog>
#include <QTableWidget>

#include <optional>
#include "..\..\Core\UnitComputers.h"
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogComputers;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogComputers : public QDialog
{
    Q_OBJECT
public:
    dialogComputers(QWidget* parent = 0);
    ~dialogComputers();

    std::optional <MComputers> exec(const MComputers& src, int left, int top);

private slots:
    void on_tableWidgetComputers_itemSelectionChanged();
    void on_lineEditNumber_editingFinished();
    void on_lineEditAddress_editingFinished();
    void on_buttonNone_clicked();
    void on_buttonRed_clicked();
    void on_buttonLime_clicked();
    void on_buttonAqua_clicked();
    void on_buttonYellow_clicked();
    void on_buttonUsed_clicked();
    void on_buttonNotUsed_clicked();
    void on_buttonAdd_clicked();
    void on_buttonDel_clicked();

private:
    Ui::dialogComputers *ui;

    MComputers tmpComputers;

    void updateColor(std::uint8_t color);
    void updateInUse(bool inUse);
    void setTableWidgetComputersLine(int rowIndex, MComputersItem* comp);
    void setEditable(bool mode);
};
//---------------------------------------------------------------------------
#endif // DIALOGCOMPUTERS_H
