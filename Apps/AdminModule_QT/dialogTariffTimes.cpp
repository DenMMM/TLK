#include "dialogTariffTimes.h"
#include "ui_dialogTariffTimes.h"

#include <QTime>
#include <QLocale>


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

dialogTariffTimes::~dialogTariffTimes()
{
    delete ui;
}

std::optional<MTariffTimes> dialogTariffTimes::exec(
        const MTariffTimes& src_, int left_, int top_)
{
    tmp_times = src_;

    for (auto& time: tmp_times)
    {
        const auto row_index = ui->tableWidgetTimes->rowCount();
        ui->tableWidgetTimes->setRowCount(row_index + 1);
        ui->tableWidgetTimes->setItem(row_index, 0, new QTableWidgetItem);
        ui->tableWidgetTimes->setItem(row_index, 1, new QTableWidgetItem);
        ui->tableWidgetTimes->setItem(row_index, 2, new QTableWidgetItem);
        ui->tableWidgetTimes->setItem(row_index, 3, new QTableWidgetItem);
        set_tableWidgetTimes_line(row_index, &time);
    }

    this->move(left_, top_);
    set_edit(false);

    std::optional <MTariffTimes> result;

    if (static_cast<QDialog*>(this)->exec() == QDialog::Accepted)
        result = std::move(tmp_times);

    return result;
}

void dialogTariffTimes::on_tableWidgetTimes_itemSelectionChanged()
{
    auto items = ui->tableWidgetTimes->selectedItems();

    if (items.count() != ui->tableWidgetTimes->columnCount())
    {
        set_edit(false);
    }
    else
    {
        set_edit(true);

        auto data = items.first()->data(Qt::UserRole);
        auto* time = reinterpret_cast<MTariffTimesItem*>(data.value<void*>());

        ui->comboBoxBeginH->setCurrentIndex(time->BeginTime / 60);
        ui->comboBoxBeginM->setCurrentIndex((time->BeginTime % 60) / 5);

        const int end_time=
            time->EndTime >= (24 * 60)?
            time->EndTime - (24 * 60):
            time->EndTime;

        ui->comboBoxEndH->setCurrentIndex(end_time / 60);
        ui->comboBoxEndM->setCurrentIndex((end_time % 60) / 5);

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

    set_tableWidgetTimes_line(item->row(), time);
    on_tableWidgetTimes_itemSelectionChanged();
}

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

    set_tableWidgetTimes_line(item->row(), time);
}

void dialogTariffTimes::on_comboBoxBeginM_activated(int index)
{
    on_comboBoxBeginH_activated(0);
}

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

    set_tableWidgetTimes_line(item->row(), time);
}

void dialogTariffTimes::on_comboBoxEndM_activated(int index)
{
    on_comboBoxEndH_activated(0);
}

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

    set_tableWidgetTimes_line(item->row(), time);
}

void dialogTariffTimes::on_comboBoxSizeM_activated(int index)
{
    on_comboBoxSizeH_activated(0);
}

void dialogTariffTimes::on_buttonAdd_clicked()
{
    if (tmp_times.gCount() >= MAX_TariffTimes)
    {
        ResMessageBox((HWND)this->windowHandle(), 0, 21, MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MTariffTimesItem& new_time = tmp_times.Add();

    const auto row_index = ui->tableWidgetTimes->rowCount();
    ui->tableWidgetTimes->setRowCount(row_index + 1);
    ui->tableWidgetTimes->setItem(row_index, 0, new QTableWidgetItem);
    ui->tableWidgetTimes->setItem(row_index, 1, new QTableWidgetItem);
    ui->tableWidgetTimes->setItem(row_index, 2, new QTableWidgetItem);
    ui->tableWidgetTimes->setItem(row_index, 3, new QTableWidgetItem);
    set_tableWidgetTimes_line(row_index, &new_time);
}

void dialogTariffTimes::on_buttonDel_clicked()
{
    auto items = ui->tableWidgetTimes->selectedItems();

    for (auto i_item = items.rbegin(); i_item != items.rend(); ++i_item)
    {
        if ((*i_item)->column() != 0)
            continue;

        auto data = (*i_item)->data(Qt::UserRole);
        auto* time = reinterpret_cast<MTariffTimesItem*>(data.value<void*>());
        ui->tableWidgetTimes->removeRow((*i_item)->row());
        tmp_times.Del(MTariffTimes::const_iterator(time));
    }
}

void dialogTariffTimes::set_tableWidgetTimes_line(
    int row_index_, MTariffTimesItem* time_)
{
    auto ptr_to_time = QVariant::fromValue(reinterpret_cast<void*>(time_));

    auto* item1 = ui->tableWidgetTimes->item(row_index_, 0);
    item1->setData(Qt::UserRole, ptr_to_time);
    item1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item1->setText(QTime(time_->BeginTime / 60, time_->BeginTime % 60).toString(u8"HH:mm"));

    const int end_time =
        time_->EndTime >= (24 * 60)?
        time_->EndTime - (24 * 60):
        time_->EndTime;

    auto* item2 = ui->tableWidgetTimes->item(row_index_, 1);
    item2->setData(Qt::UserRole, ptr_to_time);
    item2->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item2->setText(QTime(end_time / 60, end_time % 60).toString(u8"HH:mm"));

    auto* item3 = ui->tableWidgetTimes->item(row_index_, 2);
    item3->setData(Qt::UserRole, ptr_to_time);
    item3->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    switch (time_->Type)
    {
    case mttHours:
        item3->setText(u8"За каждый час");
        break;

    case mttFlyPacket:
        {
            QTime size_time(time_->SizeTime / 60, time_->SizeTime % 60);
            QString line;

            if (time_->SizeTime >= 60)
                line = size_time.toString(u8"H час. mm мин.");
            else
                line = size_time.toString(u8"mm мин.");

            item3->setText(line);
        }
        break;

    case mttPacket:
        item3->setText(u8"");
        break;

    default:
        break;
    }

    auto* item4 = ui->tableWidgetTimes->item(row_index_, 3);
    item4->setData(Qt::UserRole, ptr_to_time);
    item4->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item4->setText(QLocale().toCurrencyString(time_->Cost));
}

void dialogTariffTimes::set_edit(bool edit_)
{
    ui->labelTimeTypeText->setEnabled(edit_);
    ui->comboBoxType->setEnabled(edit_);
    ui->labelBeginText->setEnabled(edit_);
    ui->comboBoxBeginH->setEnabled(edit_);
    ui->comboBoxBeginM->setEnabled(edit_);
    ui->labelEndText->setEnabled(edit_);
    ui->comboBoxEndH->setEnabled(edit_);
    ui->comboBoxEndM->setEnabled(edit_);
    ui->labelSizeText->setEnabled(edit_);
    ui->comboBoxSizeH->setEnabled(edit_);
    ui->comboBoxSizeM->setEnabled(edit_);
    ui->labelCostText->setEnabled(edit_);
    ui->lineEditCost->setEnabled(edit_);
}

void dialogTariffTimes::on_lineEditCost_editingFinished()
{
    auto items = ui->tableWidgetTimes->selectedItems();

    if (items.count() != ui->tableWidgetTimes->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* time = reinterpret_cast<MTariffTimesItem*>(data.value<void*>());

    bool is_ok = true;
    double cost = ui->lineEditCost->text().toDouble(&is_ok);

    if (is_ok && cost >= 0 && cost <= MAX_TariffTimeCost)
        time->Cost = cost;

    ui->lineEditCost->setText(QString::number(time->Cost, 'f', 2));
    set_tableWidgetTimes_line(item->row(), time);
}
