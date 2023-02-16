//---------------------------------------------------------------------------
#include "dialogTariffTimes.h"
#include "ui_dialogTariffTimes.h"

#include <QTime>
#include <QLocale>
//---------------------------------------------------------------------------
dialogTariffTimes::dialogTariffTimes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogTariffTimes)
{
    ui->setupUi(this);

    ui->tableWidgetTimes->setColumnCount(4);
    ui->tableWidgetTimes->setColumnWidth(0, 90);
    ui->tableWidgetTimes->setColumnWidth(1, 90);
    ui->tableWidgetTimes->setColumnWidth(2, 140);
    ui->tableWidgetTimes->setColumnWidth(3, 140);
    ui->tableWidgetTimes->setHorizontalHeaderItem(0, new QTableWidgetItem(u8"С"));
    ui->tableWidgetTimes->setHorizontalHeaderItem(1, new QTableWidgetItem(u8"До"));
    ui->tableWidgetTimes->setHorizontalHeaderItem(2, new QTableWidgetItem(u8"Время"));
    ui->tableWidgetTimes->setHorizontalHeaderItem(3, new QTableWidgetItem(u8"Стоимость"));

    ui->comboBoxType->addItem(u8"За каждый час");
    ui->comboBoxType->addItem(u8"\"Плавающий\" пакет");
    ui->comboBoxType->addItem(u8"Пакет в интервале времени");

    for (int i = 0; i < 24; ++i)
    {
        const auto line = QString::number(i);
        ui->comboBoxBeginH->addItem(line);
        ui->comboBoxEndH->addItem(line);
        ui->comboBoxSizeH->addItem(line);
    }

    for (int i = 0; i < 60; i += 5)
    {
        const auto line = QString::number(i / 10) + QString::number(i % 10);
        ui->comboBoxBeginM->addItem(line);
        ui->comboBoxEndM->addItem(line);
        ui->comboBoxSizeM->addItem(line);
    }

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}
//---------------------------------------------------------------------------
dialogTariffTimes::~dialogTariffTimes()
{
    delete ui;
}
//---------------------------------------------------------------------------
std::optional<MTariffTimes> dialogTariffTimes::exec(
    const MTariffTimes& src, int left, int top)
{
    tmpTimes = src;

    for (auto& time: tmpTimes)
    {
        const auto rowIndex = ui->tableWidgetTimes->rowCount();
        ui->tableWidgetTimes->setRowCount(rowIndex + 1);
        ui->tableWidgetTimes->setItem(rowIndex, 0, new QTableWidgetItem);
        ui->tableWidgetTimes->setItem(rowIndex, 1, new QTableWidgetItem);
        ui->tableWidgetTimes->setItem(rowIndex, 2, new QTableWidgetItem);
        ui->tableWidgetTimes->setItem(rowIndex, 3, new QTableWidgetItem);
        setTableWidgetTimesLine(rowIndex, &time);
    }

    setEditable(false);
    ui->tableWidgetTimes->setFocus();
    this->move(left, top);

    std::optional <MTariffTimes> result;

    if (QDialog::exec() == QDialog::Accepted)
        result = std::move(tmpTimes);

    return result;
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_tableWidgetTimes_itemSelectionChanged()
{
    auto items = ui->tableWidgetTimes->selectedItems();

    if (items.count() != ui->tableWidgetTimes->columnCount())
    {
        setEditable(false);
    }
    else
    {
        setEditable(true);

        auto data = items.first()->data(Qt::UserRole);
        auto* time = reinterpret_cast<MTariffTimesItem*>(data.value<void*>());

        ui->comboBoxBeginH->setCurrentIndex(time->BeginTime / 60);
        ui->comboBoxBeginM->setCurrentIndex((time->BeginTime % 60) / 5);

        const int endTime=
            time->EndTime >= (24 * 60)?
            time->EndTime - (24 * 60):
            time->EndTime;

        ui->comboBoxEndH->setCurrentIndex(endTime / 60);
        ui->comboBoxEndM->setCurrentIndex((endTime % 60) / 5);

        switch (time->Type)
        {
        case mttHours:
            ui->comboBoxType->setCurrentIndex(0);
            ui->labelSizeText->setEnabled(false);
            ui->comboBoxSizeH->setEnabled(false);
            ui->comboBoxSizeH->setCurrentIndex(1);
            ui->comboBoxSizeM->setEnabled(false);
            ui->comboBoxSizeM->setCurrentIndex(0);
            break;

        case mttFlyPacket:
            ui->comboBoxType->setCurrentIndex(1);
            ui->labelSizeText->setEnabled(true);
            ui->comboBoxSizeH->setEnabled(true);
            ui->comboBoxSizeH->setCurrentIndex(time->SizeTime / 60);
            ui->comboBoxSizeM->setEnabled(true);
            ui->comboBoxSizeM->setCurrentIndex((time->SizeTime % 60) / 5);
            break;

        case mttPacket:
            ui->comboBoxType->setCurrentIndex(2);
            ui->labelSizeText->setEnabled(false);
            ui->comboBoxSizeH->setEnabled(false);
            ui->comboBoxSizeH->setCurrentIndex(-1);
            ui->comboBoxSizeM->setEnabled(false);
            ui->comboBoxSizeM->setCurrentIndex(-1);
            break;

        default:
            break;
        }

        ui->lineEditCost->setText(QString::number(time->Cost, 'f', 2));
    }
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_comboBoxType_activated(int index)
{
    auto items = ui->tableWidgetTimes->selectedItems();

    if (items.count() != ui->tableWidgetTimes->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* time = reinterpret_cast<MTariffTimesItem*>(data.value<void*>());

    switch (index)
    {
        case 0: time->Type = mttHours; break;
        case 1: time->Type = mttFlyPacket; break;
        case 2: time->Type = mttPacket; break;
        default: break;
    }

    setTableWidgetTimesLine(item->row(), time);
    on_tableWidgetTimes_itemSelectionChanged();
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_comboBoxBeginH_activated(int index)
{
    auto items = ui->tableWidgetTimes->selectedItems();

    if (items.count() != ui->tableWidgetTimes->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* time = reinterpret_cast<MTariffTimesItem*>(data.value<void*>());

    time->BeginTime =
        ui->comboBoxBeginH->currentIndex() * 60 +
        ui->comboBoxBeginM->currentIndex() * 5;

    setTableWidgetTimesLine(item->row(), time);
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_comboBoxBeginM_activated(int index)
{
    on_comboBoxBeginH_activated(0);
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_comboBoxEndH_activated(int index)
{
    auto items = ui->tableWidgetTimes->selectedItems();

    if (items.count() != ui->tableWidgetTimes->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* time = reinterpret_cast<MTariffTimesItem*>(data.value<void*>());

    time->EndTime =
        ui->comboBoxEndH->currentIndex() * 60 +
        ui->comboBoxEndM->currentIndex() * 5;

    if (time->BeginTime >= time->EndTime )
        time->EndTime += 24 * 60;

    setTableWidgetTimesLine(item->row(), time);
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_comboBoxEndM_activated(int index)
{
    on_comboBoxEndH_activated(0);
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_comboBoxSizeH_activated(int index)
{
    auto items = ui->tableWidgetTimes->selectedItems();

    if (items.count() != ui->tableWidgetTimes->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* time = reinterpret_cast<MTariffTimesItem*>(data.value<void*>());

    time->SizeTime =
        ui->comboBoxSizeH->currentIndex() * 60 +
        ui->comboBoxSizeM->currentIndex() * 5;

    setTableWidgetTimesLine(item->row(), time);
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_comboBoxSizeM_activated(int index)
{
    on_comboBoxSizeH_activated(0);
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_lineEditCost_editingFinished()
{
    auto items = ui->tableWidgetTimes->selectedItems();

    if (items.count() != ui->tableWidgetTimes->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* time = reinterpret_cast<MTariffTimesItem*>(data.value<void*>());

    bool isOk = true;
    double cost = ui->lineEditCost->text().toDouble(&isOk);

    if (isOk && cost >= 0 && cost <= MAX_TariffTimeCost)
        time->Cost = cost;

    ui->lineEditCost->setText(QString::number(time->Cost, 'f', 2));
    setTableWidgetTimesLine(item->row(), time);
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_buttonAdd_clicked()
{
    if (tmpTimes.gCount() >= MAX_TariffTimes)
    {
        ResMessageBox((HWND)this->windowHandle(), 0, 21, MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MTariffTimesItem& newTime = tmpTimes.Add();

    const auto rowIndex = ui->tableWidgetTimes->rowCount();
    ui->tableWidgetTimes->setRowCount(rowIndex + 1);
    ui->tableWidgetTimes->setItem(rowIndex, 0, new QTableWidgetItem);
    ui->tableWidgetTimes->setItem(rowIndex, 1, new QTableWidgetItem);
    ui->tableWidgetTimes->setItem(rowIndex, 2, new QTableWidgetItem);
    ui->tableWidgetTimes->setItem(rowIndex, 3, new QTableWidgetItem);
    setTableWidgetTimesLine(rowIndex, &newTime);

    ui->tableWidgetTimes->selectRow(rowIndex);
    ui->comboBoxType->setFocus();
}
//---------------------------------------------------------------------------
void dialogTariffTimes::on_buttonDel_clicked()
{
    auto items = ui->tableWidgetTimes->selectedItems();

    for (auto iItem = items.rbegin(); iItem != items.rend(); ++iItem)
    {
        if ((*iItem)->column() != 0)
            continue;

        auto data = (*iItem)->data(Qt::UserRole);
        auto* time = reinterpret_cast<MTariffTimesItem*>(data.value<void*>());
        ui->tableWidgetTimes->removeRow((*iItem)->row());
        tmpTimes.Del(MTariffTimes::const_iterator(time));
    }
}
//---------------------------------------------------------------------------
void dialogTariffTimes::setTableWidgetTimesLine(
    int rowIndex, MTariffTimesItem* timesItem)
{
    auto ptrToTime = QVariant::fromValue(reinterpret_cast<void*>(timesItem));

    auto* item1 = ui->tableWidgetTimes->item(rowIndex, 0);
    item1->setData(Qt::UserRole, ptrToTime);
    item1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item1->setText(QTime(timesItem->BeginTime / 60, timesItem->BeginTime % 60).toString(u8"HH:mm"));

    const int endTime =
        timesItem->EndTime >= (24 * 60)?
        timesItem->EndTime - (24 * 60):
        timesItem->EndTime;

    auto* item2 = ui->tableWidgetTimes->item(rowIndex, 1);
    item2->setData(Qt::UserRole, ptrToTime);
    item2->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item2->setText(QTime(endTime / 60, endTime % 60).toString(u8"HH:mm"));

    auto* item3 = ui->tableWidgetTimes->item(rowIndex, 2);
    item3->setData(Qt::UserRole, ptrToTime);
    item3->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    switch (timesItem->Type)
    {
    case mttHours:
        item3->setText(u8"За каждый час");
        break;

    case mttFlyPacket:
        {
            QTime sizeTime(timesItem->SizeTime / 60, timesItem->SizeTime % 60);
            QString line;

            if (timesItem->SizeTime >= 60)
                line = sizeTime.toString(u8"H час. mm мин.");
            else
                line = sizeTime.toString(u8"mm мин.");

            item3->setText(line);
        }
        break;

    case mttPacket:
        item3->setText(u8"");
        break;

    default:
        break;
    }

    auto* item4 = ui->tableWidgetTimes->item(rowIndex, 3);
    item4->setData(Qt::UserRole, ptrToTime);
    item4->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item4->setText(QLocale().toCurrencyString(timesItem->Cost));
}
//---------------------------------------------------------------------------
void dialogTariffTimes::setEditable(bool mode)
{
    ui->labelTimeTypeText->setEnabled(mode);
    ui->comboBoxType->setEnabled(mode);
    ui->labelBeginText->setEnabled(mode);
    ui->comboBoxBeginH->setEnabled(mode);
    ui->comboBoxBeginM->setEnabled(mode);
    ui->labelEndText->setEnabled(mode);
    ui->comboBoxEndH->setEnabled(mode);
    ui->comboBoxEndM->setEnabled(mode);
    ui->labelSizeText->setEnabled(mode);
    ui->comboBoxSizeH->setEnabled(mode);
    ui->comboBoxSizeM->setEnabled(mode);
    ui->labelCostText->setEnabled(mode);
    ui->lineEditCost->setEnabled(mode);
}
//---------------------------------------------------------------------------
