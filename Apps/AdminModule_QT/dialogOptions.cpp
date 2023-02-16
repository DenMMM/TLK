//---------------------------------------------------------------------------
#include "dialogOptions.h"
#include "ui_dialogOptions.h"
//---------------------------------------------------------------------------
dialogOptions::dialogOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogOptions)
{
    ui->setupUi(this);

    ui->comboBoxLogPeriod->addItem(u8"Сутки");
    ui->comboBoxLogPeriod->addItem(u8"Неделя");
    ui->comboBoxLogPeriod->addItem(u8"Месяц");

    ui->lineEditCostDialogTime->setMaxLength(5);
    ui->lineEditCostPrecision->setMaxLength(10);
    ui->lineEditFilterFreeTime->setMaxLength(5);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}
//---------------------------------------------------------------------------
dialogOptions::~dialogOptions()
{
    delete ui;
}
//---------------------------------------------------------------------------
std::optional<MOptions> dialogOptions::exec(
    const MOptions& src, int left, int top)
{
    // Скопируем настройки в буфер
    tmpOptions = src;

    // Заполним данными
    int periodIdx = -1;
    switch(tmpOptions.LogPeriod)
    {
        case mlpDay:    periodIdx = 0; break;
        case mlpWeek:   periodIdx = 1; break;
        case mlpMonth:  periodIdx = 2; break;
        default:        break;
    }
    ui->comboBoxLogPeriod->setCurrentIndex(periodIdx);

    ui->lineEditCostDialogTime->setText(QString::number(tmpOptions.CostDialogTime));
    ui->lineEditCostPrecision->setText(QString::number(tmpOptions.CostPrecision, 'f', 2));
    ui->lineEditFilterFreeTime->setText(QString::number(tmpOptions.FilterFreeTime));
    ui->checkBoxRightPause->setChecked(tmpOptions.UsersRights&murPause);

    ui->comboBoxLogPeriod->setFocus();
    this->move(left, top);

    // Добавим энтропии
    BasicRand.event();

    // Покажем диалог пользователю
    std::optional <MOptions> result;

    if (QDialog::exec() == QDialog::Accepted)
        result = std::move(tmpOptions);

    return result;
}
//---------------------------------------------------------------------------
void dialogOptions::on_comboBoxLogPeriod_activated(int index)
{
    switch(index)
    {
        case 0: tmpOptions.LogPeriod = mlpDay; break;
        case 1: tmpOptions.LogPeriod = mlpWeek; break;
        case 2: tmpOptions.LogPeriod = mlpMonth; break;
        default: break;
    }
}
//---------------------------------------------------------------------------
void dialogOptions::on_lineEditCostDialogTime_editingFinished()
{
    bool isOk = true;
    const auto time = ui->lineEditCostDialogTime->text().toUInt(&isOk);

    if (isOk && time >= 0 && time <= MAX_DialogTime)
        tmpOptions.CostDialogTime = time;

    ui->lineEditCostDialogTime->setText(
        QString::number(tmpOptions.CostDialogTime));
}
//---------------------------------------------------------------------------
void dialogOptions::on_lineEditCostPrecision_editingFinished()
{
    bool isOk = true;
    const auto precision = ui->lineEditCostPrecision->text().toFloat(&isOk);

    if (isOk && precision >= 0 && precision <= MAX_TariffTimeCost)
        tmpOptions.CostPrecision = static_cast<int>(precision * 100.) / 100.;

    ui->lineEditCostPrecision->setText(
        QString::number(tmpOptions.CostPrecision, 'f', 2));
}
//---------------------------------------------------------------------------
void dialogOptions::on_lineEditFilterFreeTime_editingFinished()
{
    bool isOk = true;
    const auto time = ui->lineEditFilterFreeTime->text().toUInt(&isOk);

    if (isOk && time >= 0 && time <= MAX_FilterFreeTime)
        tmpOptions.FilterFreeTime = time;

    ui->lineEditFilterFreeTime->setText(
        QString::number(tmpOptions.FilterFreeTime));
}
//---------------------------------------------------------------------------
void dialogOptions::on_checkBoxRightPause_clicked(bool checked)
{
    if (checked)
        tmpOptions.UsersRights |= murPause;
    else
        tmpOptions.UsersRights &= ~murPause;
}
//---------------------------------------------------------------------------
