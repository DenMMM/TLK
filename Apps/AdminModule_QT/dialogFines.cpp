//---------------------------------------------------------------------------
#include "dialogFines.h"
#include "ui_dialogFines.h"
//---------------------------------------------------------------------------
dialogFines::dialogFines(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogFines)
{
    ui->setupUi(this);

    ui->tableWidgetFines->setColumnCount(2);
    ui->tableWidgetFines->setColumnWidth(0, 300);
    ui->tableWidgetFines->setColumnWidth(1, 150);
    ui->tableWidgetFines->setHorizontalHeaderItem(0, new QTableWidgetItem(u8"Описание"));
    ui->tableWidgetFines->setHorizontalHeaderItem(1, new QTableWidgetItem(u8"Время"));

    ui->lineEditDescription->setMaxLength(MAX_FineDescrLen);
}
//---------------------------------------------------------------------------
dialogFines::~dialogFines()
{
    delete ui;
}
//---------------------------------------------------------------------------
std::optional<MFines> dialogFines::exec(
    const MFines& src, int left, int top)
{
    tmpFines = src;

    for (auto& fine: tmpFines)
    {
        const auto rowIndex = ui->tableWidgetFines->rowCount();
        ui->tableWidgetFines->setRowCount(rowIndex + 1);
        ui->tableWidgetFines->setItem(rowIndex, 0, new QTableWidgetItem);
        ui->tableWidgetFines->setItem(rowIndex, 1, new QTableWidgetItem);
        setTableWidgetFinesLine(rowIndex, &fine);
    }

    ui->comboBoxTime->addItem(u8"Все время");
    for (int i = 1; i <= 60; ++i)
    {
        ui->comboBoxTime->addItem(QString::number(i));
    }

    setEditable(false);
    ui->tableWidgetFines->setFocus();
    this->move(left, top);

    BasicRand.event();

    std::optional <MFines> result;

    if (QDialog::exec() == QDialog::Accepted)
        result = std::move(tmpFines);

    return result;
}
//---------------------------------------------------------------------------
void dialogFines::on_tableWidgetFines_itemSelectionChanged()
{
    auto items = ui->tableWidgetFines->selectedItems();

    if (items.count() != ui->tableWidgetFines->columnCount())
    {
        setEditable(false);
    }
    else
    {
        setEditable(true);

        auto data = items.first()->data(Qt::UserRole);
        auto* fine = reinterpret_cast<MFinesItem*>(data.value<void*>());

        ui->lineEditDescription->setText(QString::fromStdWString(fine->Descr));
        ui->comboBoxTime->setCurrentIndex(fine->Time == (24 * 60)? 0: fine->Time);
    }
}
//---------------------------------------------------------------------------
void dialogFines::on_lineEditDescription_editingFinished()
{
    auto items = ui->tableWidgetFines->selectedItems();

    if (items.count() != ui->tableWidgetFines->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* fine = reinterpret_cast<MFinesItem*>(data.value<void*>());

    ui->lineEditDescription->setText(ui->lineEditDescription->text().trimmed());
    fine->Descr = ui->lineEditDescription->text().toStdWString();
    setTableWidgetFinesLine(item->row(), fine);
}
//---------------------------------------------------------------------------
void dialogFines::on_comboBoxTime_activated(int index)
{
    auto items = ui->tableWidgetFines->selectedItems();

    if (items.count() != ui->tableWidgetFines->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* fine = reinterpret_cast<MFinesItem*>(data.value<void*>());

    fine->Time = index == 0? (24 * 60): index;
    setTableWidgetFinesLine(item->row(), fine);
}
//---------------------------------------------------------------------------
void dialogFines::on_buttonAdd_clicked()
{
    if (tmpFines.gCount() >= MAX_Fines)
    {
        ResMessageBox((HWND)this->windowHandle(), 0, 22, MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MFinesItem& newFine = tmpFines.Add();
    newFine.Descr = L"Новый штраф";
    newFine.Time = 1;

    const auto rowIndex = ui->tableWidgetFines->rowCount();
    ui->tableWidgetFines->setRowCount(rowIndex + 1);
    ui->tableWidgetFines->setItem(rowIndex, 0, new QTableWidgetItem);
    ui->tableWidgetFines->setItem(rowIndex, 1, new QTableWidgetItem);
    setTableWidgetFinesLine(rowIndex, &newFine);

    ui->tableWidgetFines->selectRow(rowIndex);
    ui->lineEditDescription->setFocus();
}
//---------------------------------------------------------------------------
void dialogFines::on_buttonDel_clicked()
{
    auto items = ui->tableWidgetFines->selectedItems();

    for (auto iItem = items.rbegin(); iItem != items.rend(); ++iItem)
    {
        if ((*iItem)->column() != 0)
            continue;

        auto data = (*iItem)->data(Qt::UserRole);
        auto* fine = reinterpret_cast<MFinesItem*>(data.value<void*>());
        ui->tableWidgetFines->removeRow((*iItem)->row());
        tmpFines.Del(MFines::const_iterator(fine));
    }
}
//---------------------------------------------------------------------------
void dialogFines::setTableWidgetFinesLine(
    int rowIndex, MFinesItem* fine)
{
    auto ptrToFine = QVariant::fromValue(reinterpret_cast<void*>(fine));

    auto* item1 = ui->tableWidgetFines->item(rowIndex, 0);
    item1->setData(Qt::UserRole, ptrToFine);
    item1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item1->setText(QString::fromStdWString(fine->Descr));

    auto* item2 = ui->tableWidgetFines->item(rowIndex, 1);
    item2->setData(Qt::UserRole, ptrToFine);
    item2->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

    if (fine->Time == (24 * 60))
        item2->setText(u8"Все время");
    else
        item2->setText(QString::number(fine->Time) + u8" мин.");
}
//---------------------------------------------------------------------------
void dialogFines::setEditable(bool mode)
{
    ui->labelDescription->setEnabled(mode);
    ui->lineEditDescription->setEnabled(mode);
    ui->labelTime->setEnabled(mode);
    ui->comboBoxTime->setEnabled(mode);
}
//---------------------------------------------------------------------------
