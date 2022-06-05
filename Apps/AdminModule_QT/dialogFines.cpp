#include "dialogFines.h"
#include "ui_dialogFines.h"


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

dialogFines::~dialogFines()
{
    delete ui;
}

std::optional<MFines> dialogFines::exec(
    const MFines& src_, int left_, int top_)
{
    tmp_fines = src_;

    for (auto& fine: tmp_fines)
    {
        const auto row_index = ui->tableWidgetFines->rowCount();
        ui->tableWidgetFines->setRowCount(row_index + 1);
        ui->tableWidgetFines->setItem(row_index, 0, new QTableWidgetItem);
        ui->tableWidgetFines->setItem(row_index, 1, new QTableWidgetItem);
        set_tableWidgetFines_line(row_index, &fine);
    }

    ui->comboBoxTime->addItem(u8"Все время");
    for ( int i=1; i <= 60; ++i )
    {
        ui->comboBoxTime->addItem(QString::number(i));
    }

    this->move(left_, top_);
    set_edit(false);

    BasicRand.event();

    std::optional <MFines> result;

    if (static_cast<QDialog*>(this)->exec() == QDialog::Accepted)
        result = std::move(tmp_fines);

    return result;
}

void dialogFines::on_tableWidgetFines_itemSelectionChanged()
{
    auto items = ui->tableWidgetFines->selectedItems();

    if (items.count() != ui->tableWidgetFines->columnCount())
    {
        set_edit(false);
    }
    else
    {
        set_edit(true);

        auto data = items.first()->data(Qt::UserRole);
        auto* fine = reinterpret_cast<MFinesItem*>(data.value<void*>());

        ui->lineEditDescription->setText(QString::fromStdWString(fine->Descr));
        ui->comboBoxTime->setCurrentIndex(fine->Time == (24 * 60)? 0: fine->Time);
    }
}

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
    set_tableWidgetFines_line(item->row(), fine);
}

void dialogFines::on_comboBoxTime_activated(int index)
{
    auto items = ui->tableWidgetFines->selectedItems();

    if (items.count() != ui->tableWidgetFines->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* fine = reinterpret_cast<MFinesItem*>(data.value<void*>());

    fine->Time = index == 0? (24 * 60): index;
    set_tableWidgetFines_line(item->row(), fine);
}

void dialogFines::on_buttonAdd_clicked()
{
    if (tmp_fines.gCount() >= MAX_Fines)
    {
        ResMessageBox((HWND)this->windowHandle(), 0, 22, MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MFinesItem& new_fine = tmp_fines.Add();
    new_fine.Descr = L"Новый штраф";
    new_fine.Time = 1;

    const auto row_index = ui->tableWidgetFines->rowCount();
    ui->tableWidgetFines->setRowCount(row_index + 1);
    ui->tableWidgetFines->setItem(row_index, 0, new QTableWidgetItem);
    ui->tableWidgetFines->setItem(row_index, 1, new QTableWidgetItem);
    set_tableWidgetFines_line(row_index, &new_fine);
}

void dialogFines::on_buttonDel_clicked()
{
    auto items = ui->tableWidgetFines->selectedItems();

    for (auto i_item = items.rbegin(); i_item != items.rend(); ++i_item)
    {
        if ((*i_item)->column() != 0)
            continue;

        auto data = (*i_item)->data(Qt::UserRole);
        auto* fine = reinterpret_cast<MFinesItem*>(data.value<void*>());
        ui->tableWidgetFines->removeRow((*i_item)->row());
        tmp_fines.Del(MFines::const_iterator(fine));
    }
}

void dialogFines::set_tableWidgetFines_line(
    int row_index_, MFinesItem* fine_)
{
    auto ptr_to_fine = QVariant::fromValue(reinterpret_cast<void*>(fine_));

    auto* item1 = ui->tableWidgetFines->item(row_index_, 0);
    item1->setData(Qt::UserRole, ptr_to_fine);
    item1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item1->setText(QString::fromStdWString(fine_->Descr));

    auto* item2 = ui->tableWidgetFines->item(row_index_, 1);
    item2->setData(Qt::UserRole, ptr_to_fine);
    item2->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

    if (fine_->Time == (24 * 60))
        item2->setText(u8"Все время");
    else
        item2->setText(QString::number(fine_->Time) + u8" мин.");
}

void dialogFines::set_edit(bool edit_)
{
    ui->labelDescription->setEnabled(edit_);
    ui->lineEditDescription->setEnabled(edit_);
    ui->labelTime->setEnabled(edit_);
    ui->comboBoxTime->setEnabled(edit_);
}
