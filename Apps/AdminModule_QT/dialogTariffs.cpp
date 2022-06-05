#include "dialogTariffs.h"
#include "ui_dialogTariffs.h"

#include "dialogTariffTimes.h"


dialogTariffs::dialogTariffs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogTariffs),
    updating_listWidgetApps(false),
    updating_listWidgetComputers(false)
{
    ui->setupUi(this);

    for (int i = 1; i <= 8; ++i)
    {
        auto* new_item = new QListWidgetItem;
        new_item->setCheckState(Qt::Unchecked);
        new_item->setText(QString::number(i));
        ui->listWidgetApps->addItem(new_item);
    }

    ui->lineEditName->setMaxLength(MAX_TariffNameLen);
}

dialogTariffs::~dialogTariffs()
{
    delete ui;
}

std::optional<MTariffs> dialogTariffs::exec(
        const MTariffs& src_,
        const MComputers& computers_,
        int left_, int top_)
{
    tmp_tariffs = src_;

    for (auto& tariff: tmp_tariffs)
    {
        const auto row_index = ui->listWidgetNames->count();
        ui->listWidgetNames->addItem("");
        set_listWidgetNames_line(row_index, &tariff);
    }

    for (const auto& comp: computers_)
    {
        auto* new_item = new QListWidgetItem;
        new_item->setData(Qt::UserRole, comp.Number);
        new_item->setText(QString::number(comp.Number));
        new_item->setCheckState(Qt::Unchecked);

        QIcon icon_color;
        switch(comp.Color)
        {
            case mgcRed:    icon_color = QIcon(":/graphics/MarkerRed.png"); break;
            case mgcGreen:  icon_color = QIcon(":/graphics/MarkerGreen.png"); break;
            case mgcAqua:   icon_color = QIcon(":/graphics/MarkerAqua.png"); break;
            case mgcYellow: icon_color = QIcon(":/graphics/MarkerYellow.png"); break;
            case mgcNone:   break;
            default:        break;
        }
        new_item->setIcon(icon_color);

        ui->listWidgetComputers->addItem(new_item);
    }

    this->move(left_, top_);
    set_edit(false);

    BasicRand.event();

    std::optional <MTariffs> result;

    if (static_cast<QDialog*>(this)->exec() == QDialog::Accepted)
        result = std::move(tmp_tariffs);

    return result;
}

void dialogTariffs::on_listWidgetNames_itemSelectionChanged()
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1)
    {
        set_edit(false);
    }
    else
    {
        set_edit(true);

        auto data = items.first()->data(Qt::UserRole);
        auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

        ui->lineEditName->setText(QString::fromStdWString(tariff->Name));
        ui->checkBoxReboot->setChecked(tariff->Reboot);
        ui->checkBoxRoute->setChecked(tariff->Programs & mgpRoute);
        ui->checkBoxDesktop->setChecked(tariff->Programs & mgpDesktop);

        updating_listWidgetApps = true;
        ui->listWidgetApps->item(0)->setCheckState(tariff->Programs & mgp1? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(1)->setCheckState(tariff->Programs & mgp2? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(2)->setCheckState(tariff->Programs & mgp3? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(3)->setCheckState(tariff->Programs & mgp4? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(4)->setCheckState(tariff->Programs & mgp5? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(5)->setCheckState(tariff->Programs & mgp6? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(6)->setCheckState(tariff->Programs & mgp7? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(7)->setCheckState(tariff->Programs & mgp8? Qt::Checked: Qt::Unchecked);
        updating_listWidgetApps = false;

        updating_listWidgetComputers = true;
        for (int index = ui->listWidgetComputers->count(); (index--) > 0;)
        {
            auto* item = ui->listWidgetComputers->item(index);
            auto number = item->data(Qt::UserRole).value<decltype(MComputersItem::Number)>();

            item->setCheckState(
                tariff->CheckForComp(number)?
                Qt::Checked: Qt::Unchecked);
        }
        updating_listWidgetComputers = false;
    }
}

void dialogTariffs::on_buttonAdd_clicked()
{
    if (tmp_tariffs.gCount() >= MAX_Tariffs)
    {
        ResMessageBox((HWND)this->windowHandle(), 0, 20, MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MTariffsItem& new_tariff = tmp_tariffs.Add();
    new_tariff.Name = L"Новый тариф";

    const auto row_index = ui->listWidgetNames->count();
    ui->listWidgetNames->addItem("");
    set_listWidgetNames_line(row_index, &new_tariff);
}

void dialogTariffs::on_buttonDel_clicked()
{
    auto items = ui->listWidgetNames->selectedItems();

    for (auto i_item = items.rbegin(); i_item != items.rend(); ++i_item)
    {
        auto data = (*i_item)->data(Qt::UserRole);
        auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

        ui->listWidgetNames->removeItemWidget(*i_item);
        delete *i_item;
        tmp_tariffs.Del(MTariffs::const_iterator(tariff));
    }
}

void dialogTariffs::on_lineEditName_editingFinished()
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1)
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

    ui->lineEditName->setText(ui->lineEditName->text().trimmed());
    tariff->Name = ui->lineEditName->text().toStdWString();
    set_listWidgetNames_line(ui->listWidgetNames->row(item), tariff);
}

void dialogTariffs::on_checkBoxReboot_clicked(bool checked)
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1)
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());
    tariff->Reboot = checked;
}

void dialogTariffs::on_checkBoxRoute_clicked(bool checked)
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1)
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

    if (checked)
        tariff->Programs |= mgpRoute;
    else
        tariff->Programs &= ~mgpRoute;
}

void dialogTariffs::on_checkBoxDesktop_clicked(bool checked)
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1)
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

    if (checked)
        tariff->Programs |= mgpDesktop;
    else
        tariff->Programs &= ~mgpDesktop;
}

void dialogTariffs::on_buttonTimes_clicked()
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1)
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

    dialogTariffTimes dialog;
    auto result = dialog.exec(tariff->Times, this->x() + 30, this->y() + 30);

    if (result.has_value())
        tariff->Times = std::move(*result);
}

void dialogTariffs::on_listWidgetApps_itemChanged(QListWidgetItem *item)
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1 || updating_listWidgetApps)
        return;

    auto* tariff_item = items.first();
    auto data = tariff_item->data(Qt::UserRole);
    auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

    std::uint32_t progs = tariff->Programs & (~mgpAll);
    if (ui->listWidgetApps->item(0)->checkState() == Qt::Checked) progs |= mgp1;
    if (ui->listWidgetApps->item(1)->checkState() == Qt::Checked) progs |= mgp2;
    if (ui->listWidgetApps->item(2)->checkState() == Qt::Checked) progs |= mgp3;
    if (ui->listWidgetApps->item(3)->checkState() == Qt::Checked) progs |= mgp4;
    if (ui->listWidgetApps->item(4)->checkState() == Qt::Checked) progs |= mgp5;
    if (ui->listWidgetApps->item(5)->checkState() == Qt::Checked) progs |= mgp6;
    if (ui->listWidgetApps->item(6)->checkState() == Qt::Checked) progs |= mgp7;
    if (ui->listWidgetApps->item(7)->checkState() == Qt::Checked) progs |= mgp8;

    tariff->Programs = progs;
}

void dialogTariffs::on_listWidgetComputers_itemChanged(QListWidgetItem *item)
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1 || updating_listWidgetComputers)
        return;

    auto* tariff_item = items.first();
    auto data = tariff_item->data(Qt::UserRole);
    auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

    tmp_comps.clear();

    for (int index = ui->listWidgetComputers->count(); (index--) > 0;)
    {
        auto* item = ui->listWidgetComputers->item(index);
        auto number = item->data(Qt::UserRole).value<decltype(MComputersItem::Number)>();

        if (item->checkState() == Qt::Checked)
            tmp_comps.push_back(number);
    }

    tariff->Comps = tmp_comps;
}

void dialogTariffs::set_listWidgetNames_line(
    int row_index_, MTariffsItem* tariff_)
{
    auto ptr_to_tariff = QVariant::fromValue(reinterpret_cast<void*>(tariff_));

    auto* item = ui->listWidgetNames->item(row_index_);
    item->setData(Qt::UserRole, ptr_to_tariff);
    item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item->setText(QString::fromStdWString(tariff_->Name));
}

void dialogTariffs::set_edit(bool edit_)
{
    ui->labelTariffName->setEnabled(edit_);
    ui->lineEditName->setEnabled(edit_);
    ui->checkBoxReboot->setEnabled(edit_);
    ui->checkBoxRoute->setEnabled(edit_);
    ui->checkBoxDesktop->setEnabled(edit_);
    ui->buttonTimes->setEnabled(edit_);
    ui->labelTariffApps->setEnabled(edit_);
    ui->listWidgetApps->setEnabled(edit_);
    ui->labelTariffComputers->setEnabled(edit_);
    ui->listWidgetComputers->setEnabled(edit_);
//    ButtonSetSelComp->Enabled=Edit_;
//    ButtonSetAllComp->Enabled=Edit_;
//    ButtonResSelComp->Enabled=Edit_;
//    ButtonResAllComp->Enabled=Edit_;
}
