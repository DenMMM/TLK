#ifndef DIALOGFINES_H
#define DIALOGFINES_H
//---------------------------------------------------------------------------
#include <QDialog>
#include <QTableWidget>

#include <optional>
#include "..\..\Core\UnitFines.h"
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogFines;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogFines : public QDialog
{
    Q_OBJECT
public:
    dialogFines(QWidget* parent = 0);
    ~dialogFines();

    std::optional <MFines> exec(const MFines& src, int left, int top);

private slots:
    void on_tableWidgetFines_itemSelectionChanged();
    void on_lineEditDescription_editingFinished();
    void on_comboBoxTime_activated(int index);
    void on_buttonAdd_clicked();
    void on_buttonDel_clicked();

private:
    Ui::dialogFines *ui;

    MFines tmpFines;

    void setTableWidgetFinesLine(int rowIndex, MFinesItem* fine);
    void setEditable(bool mode);
};
//---------------------------------------------------------------------------
#endif // DIALOGFINES_H
