//---------------------------------------------------------------------------
#include "dialogTariffs.h"
#include "ui_dialogTariffs.h"

#include "dialogTariffTimes.h"
//---------------------------------------------------------------------------
dialogTariffs::dialogTariffs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogTariffs),
    updatingListWidgetApps(false),
    updatingListWidgetComputers(false)
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
//---------------------------------------------------------------------------
dialogTariffs::~dialogTariffs()
{
    delete ui;
}
//---------------------------------------------------------------------------
std::optional<MTariffs> dialogTariffs::exec(
        const MTariffs& tariffs,
        const MComputers& computers,
        int left, int top)
{
    tmpTariffs = tariffs;

    for (auto& tariff: tmpTariffs)
    {
        const auto rowIndex = ui->listWidgetNames->count();
        ui->listWidgetNames->addItem("");
        setListWidgetNamesLine(rowIndex, &tariff);
    }

    for (const auto& comp: computers)
    {
        auto* newItem = new QListWidgetItem;
        newItem->setData(Qt::UserRole, comp.Number);
        newItem->setText(QString::number(comp.Number));
        newItem->setCheckState(Qt::Unchecked);

        QIcon iconColor;
        switch(comp.Color)
        {
            case mgcRed:    iconColor = QIcon(":/graphics/MarkerRed.png"); break;
            case mgcGreen:  iconColor = QIcon(":/graphics/MarkerGreen.png"); break;
            case mgcAqua:   iconColor = QIcon(":/graphics/MarkerAqua.png"); break;
            case mgcYellow: iconColor = QIcon(":/graphics/MarkerYellow.png"); break;
            case mgcNone:   break;
            default:        break;
        }
        newItem->setIcon(iconColor);

        ui->listWidgetComputers->addItem(newItem);
    }

    setEditable(false);
    ui->listWidgetNames->setFocus();
    this->move(left, top);

    BasicRand.event();

    std::optional <MTariffs> result;

    if (QDialog::exec() == QDialog::Accepted)
        result = std::move(tmpTariffs);

    return result;
}
//---------------------------------------------------------------------------
void dialogTariffs::on_listWidgetNames_itemSelectionChanged()
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1)
    {
        setEditable(false);
    }
    else
    {
        setEditable(true);

        auto data = items.first()->data(Qt::UserRole);
        auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

        ui->lineEditName->setText(QString::fromStdWString(tariff->Name));
        ui->checkBoxReboot->setChecked(tariff->Reboot);
        ui->checkBoxRoute->setChecked(tariff->Programs & mgpRoute);
        ui->checkBoxDesktop->setChecked(tariff->Programs & mgpDesktop);

        updatingListWidgetApps = true;
        ui->listWidgetApps->item(0)->setCheckState(tariff->Programs & mgp1? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(1)->setCheckState(tariff->Programs & mgp2? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(2)->setCheckState(tariff->Programs & mgp3? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(3)->setCheckState(tariff->Programs & mgp4? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(4)->setCheckState(tariff->Programs & mgp5? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(5)->setCheckState(tariff->Programs & mgp6? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(6)->setCheckState(tariff->Programs & mgp7? Qt::Checked: Qt::Unchecked);
        ui->listWidgetApps->item(7)->setCheckState(tariff->Programs & mgp8? Qt::Checked: Qt::Unchecked);
        updatingListWidgetApps = false;

        updatingListWidgetComputers = true;
        for (int index = ui->listWidgetComputers->count(); (index--) > 0;)
        {
            auto* item = ui->listWidgetComputers->item(index);
            auto number = item->data(Qt::UserRole).value<decltype(MComputersItem::Number)>();

            item->setCheckState(
                tariff->CheckForComp(number)?
                Qt::Checked: Qt::Unchecked);
        }
        updatingListWidgetComputers = false;
    }
}
//---------------------------------------------------------------------------
void dialogTariffs::on_buttonAdd_clicked()
{
    if (tmpTariffs.gCount() >= MAX_Tariffs)
    {
        ResMessageBox((HWND)this->windowHandle(), 0, 20, MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MTariffsItem& newTariff = tmpTariffs.Add();
    newTariff.Name = L"Новый тариф";

    const auto row_index = ui->listWidgetNames->count();
    ui->listWidgetNames->addItem("");
    setListWidgetNamesLine(row_index, &newTariff);

    ui->listWidgetNames->clearSelection();
    ui->listWidgetNames->item(row_index)->setSelected(true);
    ui->lineEditName->setFocus();
}
//---------------------------------------------------------------------------
void dialogTariffs::on_buttonDel_clicked()
{
    auto items = ui->listWidgetNames->selectedItems();

    for (auto iItem = items.rbegin(); iItem != items.rend(); ++iItem)
    {
        auto data = (*iItem)->data(Qt::UserRole);
        auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

        ui->listWidgetNames->removeItemWidget(*iItem);
        delete *iItem;
        tmpTariffs.Del(MTariffs::const_iterator(tariff));
    }
}
//---------------------------------------------------------------------------
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
    setListWidgetNamesLine(ui->listWidgetNames->row(item), tariff);
}
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
void dialogTariffs::on_listWidgetApps_itemChanged(QListWidgetItem *item)
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1 || updatingListWidgetApps)
        return;

    auto* tariffItem = items.first();
    auto data = tariffItem->data(Qt::UserRole);
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
//---------------------------------------------------------------------------
void dialogTariffs::on_listWidgetComputers_itemChanged(QListWidgetItem *item)
{
    auto items = ui->listWidgetNames->selectedItems();

    if (items.count() != 1 || updatingListWidgetComputers)
        return;

    auto* tariff_item = items.first();
    auto data = tariff_item->data(Qt::UserRole);
    auto* tariff = reinterpret_cast<MTariffsItem*>(data.value<void*>());

    tmpComps.clear();

    for (int index = ui->listWidgetComputers->count(); (index--) > 0;)
    {
        auto* item = ui->listWidgetComputers->item(index);
        auto number = item->data(Qt::UserRole).value<decltype(MComputersItem::Number)>();

        if (item->checkState() == Qt::Checked)
            tmpComps.push_back(number);
    }

    tariff->Comps = tmpComps;
}
//---------------------------------------------------------------------------
void dialogTariffs::setListWidgetNamesLine(
    int rowIndex, MTariffsItem* tariff)
{
    auto ptrToTariff = QVariant::fromValue(reinterpret_cast<void*>(tariff));

    auto* item = ui->listWidgetNames->item(rowIndex);
    item->setData(Qt::UserRole, ptrToTariff);
    item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item->setText(QString::fromStdWString(tariff->Name));
}
//---------------------------------------------------------------------------
void dialogTariffs::setEditable(bool mode)
{
    ui->labelTariffName->setEnabled(mode);
    ui->lineEditName->setEnabled(mode);
    ui->checkBoxReboot->setEnabled(mode);
    ui->checkBoxRoute->setEnabled(mode);
    ui->checkBoxDesktop->setEnabled(mode);
    ui->buttonTimes->setEnabled(mode);
    ui->labelTariffApps->setEnabled(mode);
    ui->listWidgetApps->setEnabled(mode);
    ui->labelTariffComputers->setEnabled(mode);
    ui->listWidgetComputers->setEnabled(mode);
//    ButtonSetSelComp->Enabled=Edit_;
//    ButtonSetAllComp->Enabled=Edit_;
//    ButtonResSelComp->Enabled=Edit_;
//    ButtonResAllComp->Enabled=Edit_;
}
//---------------------------------------------------------------------------
