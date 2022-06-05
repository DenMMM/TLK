#include "dialogComputers.h"
#include "ui_dialogComputers.h"

#include "..\..\Core\UnitComputers.h"


dialogComputers::dialogComputers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogComputers)
{
    ui->setupUi(this);

    ui->tableWidgetComputers->setColumnCount(2);
    ui->tableWidgetComputers->setColumnWidth(0, 50);
    ui->tableWidgetComputers->setColumnWidth(1, 150);
    ui->tableWidgetComputers->setHorizontalHeaderItem(0, new QTableWidgetItem(u8"№"));
    ui->tableWidgetComputers->setHorizontalHeaderItem(1, new QTableWidgetItem(u8"IP-адрес"));

    ui->lineEditNumber->setMaxLength(3);
    ui->lineEditAddress->setMaxLength(MAX_CompAddrLen);
}

dialogComputers::~dialogComputers()
{
    delete ui;
}

std::optional<MComputers> dialogComputers::exec(
        const MComputers& src_, int left_, int top_)
{
    tmp_computers = src_;

    for (auto& comp: tmp_computers)
    {
        const auto row_index = ui->tableWidgetComputers->rowCount();
        ui->tableWidgetComputers->setRowCount(row_index + 1);
        ui->tableWidgetComputers->setItem(row_index, 0, new QTableWidgetItem);
        ui->tableWidgetComputers->setItem(row_index, 1, new QTableWidgetItem);
        set_tableWidgetComputers_line(row_index, &comp);
    }

    this->move(left_, top_);
    set_edit(false);

    BasicRand.event();

    std::optional <MComputers> result;

    if (static_cast<QDialog*>(this)->exec() == QDialog::Accepted)
        result = std::move(tmp_computers);

    return result;
}

void dialogComputers::on_tableWidgetComputers_itemSelectionChanged()
{
    auto items = ui->tableWidgetComputers->selectedItems();

    if (items.count() != ui->tableWidgetComputers->columnCount())
    {
        set_edit(false);
    }
    else
    {
        set_edit(true);

        auto data = items.first()->data(Qt::UserRole);
        auto* comp = reinterpret_cast<MComputersItem*>(data.value<void*>());

        ui->lineEditNumber->setText(QString::number(comp->Number));
        ui->lineEditAddress->setText(QString::fromStdWString(comp->Address));
    }
}

void dialogComputers::on_lineEditNumber_editingFinished()
{
    auto items = ui->tableWidgetComputers->selectedItems();

    if (items.count() != ui->tableWidgetComputers->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* comp = reinterpret_cast<MComputersItem*>(data.value<void*>());

    bool is_ok = true;
    int number = ui->lineEditNumber->text().toUInt(&is_ok);

    if (is_ok && number > 0 && number <= MAX_Comps)
        comp->Number = number;

    ui->lineEditNumber->setText(QString::number(comp->Number));
    set_tableWidgetComputers_line(item->row(), comp);
}

void dialogComputers::on_lineEditAddress_editingFinished()
{
    auto items = ui->tableWidgetComputers->selectedItems();

    if (items.count() != ui->tableWidgetComputers->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* comp = reinterpret_cast<MComputersItem*>(data.value<void*>());

    ui->lineEditAddress->setText(ui->lineEditAddress->text().trimmed());
    comp->Address = ui->lineEditAddress->text().toStdWString();
    set_tableWidgetComputers_line(item->row(), comp);
}

void dialogComputers::on_buttonNone_clicked()
{
    update_color(mgcNone);
}

void dialogComputers::on_buttonRed_clicked()
{
    update_color(mgcRed);
}

void dialogComputers::on_buttonLime_clicked()
{
    update_color(mgcGreen);
}

void dialogComputers::on_buttonAqua_clicked()
{
    update_color(mgcAqua);
}

void dialogComputers::on_buttonYellow_clicked()
{
    update_color(mgcYellow);
}

void dialogComputers::on_buttonUsed_clicked()
{
    update_in_use(true);
}

void dialogComputers::on_buttonNotUsed_clicked()
{
    update_in_use(false);
}

void dialogComputers::on_buttonAdd_clicked()
{
    if (tmp_computers.gCount() >= MAX_Comps)
    {
        ResMessageBox((HWND)this->windowHandle(), 0, 19, MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MComputersItem& new_comp = tmp_computers.Add();

    auto items = ui->tableWidgetComputers->selectedItems();

    if (items.count() != ui->tableWidgetComputers->columnCount())
    {
        new_comp.Number = 1;
    }
    else
    {
        auto data = items.first()->data(Qt::UserRole);
        auto* sel_comp = reinterpret_cast<MComputersItem*>(data.value<void*>());
        new_comp.Number = sel_comp->Number + 1;
    }

    const auto row_index = ui->tableWidgetComputers->rowCount();
    ui->tableWidgetComputers->setRowCount(row_index + 1);
    ui->tableWidgetComputers->setItem(row_index, 0, new QTableWidgetItem);
    ui->tableWidgetComputers->setItem(row_index, 1, new QTableWidgetItem);
    set_tableWidgetComputers_line(row_index, &new_comp);
}

void dialogComputers::on_buttonDel_clicked()
{
    auto items = ui->tableWidgetComputers->selectedItems();

    for (auto i_item = items.rbegin(); i_item != items.rend(); ++i_item)
    {
        if ((*i_item)->column() != 0)
            continue;

        auto data = (*i_item)->data(Qt::UserRole);
        auto* comp = reinterpret_cast<MComputersItem*>(data.value<void*>());
        ui->tableWidgetComputers->removeRow((*i_item)->row());
        tmp_computers.Del(MComputers::const_iterator(comp));
    }
}

void dialogComputers::update_color(std::uint8_t color_)
{
    auto items = ui->tableWidgetComputers->selectedItems();

    if (items.empty())
        return;

    for (auto& item: items)
    {
        if (item->column() != 0)
            continue;

        auto data = item->data(Qt::UserRole);
        auto* comp = reinterpret_cast<MComputersItem*>(data.value<void*>());
        comp->Color = color_;
        set_tableWidgetComputers_line(item->row(), comp);
    }
}

void dialogComputers::update_in_use(bool in_use_)
{
    auto items = ui->tableWidgetComputers->selectedItems();

    if (items.empty())
        return;

    for (auto& item: items)
    {
        if (item->column() != 0)
            continue;

        auto data = item->data(Qt::UserRole);
        auto* comp = reinterpret_cast<MComputersItem*>(data.value<void*>());
        comp->NotUsed = !in_use_;
        set_tableWidgetComputers_line(item->row(), comp);
    }
}

void dialogComputers::set_tableWidgetComputers_line(
    int row_index_, MComputersItem* comp_)
{
    auto ptr_to_comp = QVariant::fromValue(reinterpret_cast<void*>(comp_));

    auto* item1 = ui->tableWidgetComputers->item(row_index_, 0);
    item1->setData(Qt::UserRole, ptr_to_comp);
    item1->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    item1->setText(QString::number(comp_->Number));

    QIcon icon_color;
    switch(comp_->Color)
    {
        case mgcRed:    icon_color = QIcon(":/graphics/MarkerRed.png"); break;
        case mgcGreen:  icon_color = QIcon(":/graphics/MarkerGreen.png"); break;
        case mgcAqua:   icon_color = QIcon(":/graphics/MarkerAqua.png"); break;
        case mgcYellow: icon_color = QIcon(":/graphics/MarkerYellow.png"); break;
        case mgcNone:   break;
        default:        break;
    }
    item1->setIcon(icon_color);

    auto* item2 = ui->tableWidgetComputers->item(row_index_, 1);
    item2->setData(Qt::UserRole, ptr_to_comp);
    item2->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item2->setText(QString::fromStdWString(comp_->Address));

    if (comp_->NotUsed)
        item2->setIcon(QIcon(":/graphics/IconNotUsed.png"));
    else
        item2->setIcon(QIcon(":/graphics/IconFree.png"));
}

void dialogComputers::set_edit(bool edit_)
{
    ui->labelNumber->setEnabled(edit_);
    ui->lineEditNumber->setEnabled(edit_);
    ui->labelAddress->setEnabled(edit_);
    ui->lineEditAddress->setEnabled(edit_);
}
