//---------------------------------------------------------------------------
#include "dialogComputers.h"
#include "ui_dialogComputers.h"

#include "..\..\Core\UnitComputers.h"
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
dialogComputers::~dialogComputers()
{
    delete ui;
}
//---------------------------------------------------------------------------
std::optional<MComputers> dialogComputers::exec(
        const MComputers& src, int left, int top)
{
    tmpComputers = src;

    for (auto& comp: tmpComputers)
    {
        const auto rowIndex = ui->tableWidgetComputers->rowCount();
        ui->tableWidgetComputers->setRowCount(rowIndex + 1);
        ui->tableWidgetComputers->setItem(rowIndex, 0, new QTableWidgetItem);
        ui->tableWidgetComputers->setItem(rowIndex, 1, new QTableWidgetItem);
        setTableWidgetComputersLine(rowIndex, &comp);
    }

    setEditable(false);
    ui->tableWidgetComputers->setFocus();
    this->move(left, top);

    BasicRand.event();

    std::optional <MComputers> result;

    if (QDialog::exec() == QDialog::Accepted)
        result = std::move(tmpComputers);

    return result;
}
//---------------------------------------------------------------------------
void dialogComputers::on_tableWidgetComputers_itemSelectionChanged()
{
    auto items = ui->tableWidgetComputers->selectedItems();

    if (items.count() != ui->tableWidgetComputers->columnCount())
    {
        setEditable(false);
    }
    else
    {
        setEditable(true);

        auto data = items.first()->data(Qt::UserRole);
        auto* comp = reinterpret_cast<MComputersItem*>(data.value<void*>());

        ui->lineEditNumber->setText(QString::number(comp->Number));
        ui->lineEditAddress->setText(QString::fromStdWString(comp->Address));
    }
}
//---------------------------------------------------------------------------
void dialogComputers::on_lineEditNumber_editingFinished()
{
    auto items = ui->tableWidgetComputers->selectedItems();

    if (items.count() != ui->tableWidgetComputers->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* comp = reinterpret_cast<MComputersItem*>(data.value<void*>());

    bool isOk = true;
    int number = ui->lineEditNumber->text().toUInt(&isOk);

    if (isOk && number > 0 && number <= MAX_Comps)
        comp->Number = number;

    ui->lineEditNumber->setText(QString::number(comp->Number));
    setTableWidgetComputersLine(item->row(), comp);
}
//---------------------------------------------------------------------------
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
    setTableWidgetComputersLine(item->row(), comp);
}
//---------------------------------------------------------------------------
void dialogComputers::on_buttonNone_clicked()
{
    updateColor(mgcNone);
}
//---------------------------------------------------------------------------
void dialogComputers::on_buttonRed_clicked()
{
    updateColor(mgcRed);
}
//---------------------------------------------------------------------------
void dialogComputers::on_buttonLime_clicked()
{
    updateColor(mgcGreen);
}
//---------------------------------------------------------------------------
void dialogComputers::on_buttonAqua_clicked()
{
    updateColor(mgcAqua);
}
//---------------------------------------------------------------------------
void dialogComputers::on_buttonYellow_clicked()
{
    updateColor(mgcYellow);
}
//---------------------------------------------------------------------------
void dialogComputers::on_buttonUsed_clicked()
{
    updateInUse(true);
}
//---------------------------------------------------------------------------
void dialogComputers::on_buttonNotUsed_clicked()
{
    updateInUse(false);
}
//---------------------------------------------------------------------------
void dialogComputers::on_buttonAdd_clicked()
{
    if (tmpComputers.gCount() >= MAX_Comps)
    {
        ResMessageBox((HWND)this->windowHandle(), 0, 19, MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MComputersItem& newComp = tmpComputers.Add();

    auto items = ui->tableWidgetComputers->selectedItems();

    if (items.count() != ui->tableWidgetComputers->columnCount())
    {
        newComp.Number = 1;
    }
    else
    {
        auto data = items.first()->data(Qt::UserRole);
        auto* selComp = reinterpret_cast<MComputersItem*>(data.value<void*>());
        newComp.Number = selComp->Number + 1;
    }

    const auto rowIndex = ui->tableWidgetComputers->rowCount();
    ui->tableWidgetComputers->setRowCount(rowIndex + 1);
    ui->tableWidgetComputers->setItem(rowIndex, 0, new QTableWidgetItem);
    ui->tableWidgetComputers->setItem(rowIndex, 1, new QTableWidgetItem);
    setTableWidgetComputersLine(rowIndex, &newComp);

    ui->tableWidgetComputers->selectRow(rowIndex);
    ui->lineEditNumber->setFocus();
}
//---------------------------------------------------------------------------
void dialogComputers::on_buttonDel_clicked()
{
    auto items = ui->tableWidgetComputers->selectedItems();

    for (auto iItem = items.rbegin(); iItem != items.rend(); ++iItem)
    {
        if ((*iItem)->column() != 0)
            continue;

        auto data = (*iItem)->data(Qt::UserRole);
        auto* comp = reinterpret_cast<MComputersItem*>(data.value<void*>());
        ui->tableWidgetComputers->removeRow((*iItem)->row());
        tmpComputers.Del(MComputers::const_iterator(comp));
    }
}
//---------------------------------------------------------------------------
void dialogComputers::updateColor(std::uint8_t color)
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
        comp->Color = color;
        setTableWidgetComputersLine(item->row(), comp);
    }
}
//---------------------------------------------------------------------------
void dialogComputers::updateInUse(bool inUse)
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
        comp->NotUsed = !inUse;
        setTableWidgetComputersLine(item->row(), comp);
    }
}
//---------------------------------------------------------------------------
void dialogComputers::setTableWidgetComputersLine(
    int rowIndex, MComputersItem* comp)
{
    auto ptrToComp = QVariant::fromValue(reinterpret_cast<void*>(comp));

    auto* item1 = ui->tableWidgetComputers->item(rowIndex, 0);
    item1->setData(Qt::UserRole, ptrToComp);
    item1->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    item1->setText(QString::number(comp->Number));

    QIcon iconColor;
    switch(comp->Color)
    {
        case mgcRed:    iconColor = QIcon(":/graphics/MarkerRed.png"); break;
        case mgcGreen:  iconColor = QIcon(":/graphics/MarkerGreen.png"); break;
        case mgcAqua:   iconColor = QIcon(":/graphics/MarkerAqua.png"); break;
        case mgcYellow: iconColor = QIcon(":/graphics/MarkerYellow.png"); break;
        case mgcNone:   break;
        default:        break;
    }
    item1->setIcon(iconColor);

    auto* item2 = ui->tableWidgetComputers->item(rowIndex, 1);
    item2->setData(Qt::UserRole, ptrToComp);
    item2->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item2->setText(QString::fromStdWString(comp->Address));

    if (comp->NotUsed)
        item2->setIcon(QIcon(":/graphics/IconNotUsed.png"));
    else
        item2->setIcon(QIcon(":/graphics/IconFree.png"));
}
//---------------------------------------------------------------------------
void dialogComputers::setEditable(bool mode)
{
    ui->labelNumber->setEnabled(mode);
    ui->lineEditNumber->setEnabled(mode);
    ui->labelAddress->setEnabled(mode);
    ui->lineEditAddress->setEnabled(mode);
}
//---------------------------------------------------------------------------
