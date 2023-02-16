//---------------------------------------------------------------------------
#include "dialogNewPass.h"
#include "ui_dialogNewPass.h"

#include "..\..\Core\UnitPassword.h"
//---------------------------------------------------------------------------
dialogNewPass::dialogNewPass(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogNewPass),
    minLength(0)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}
//---------------------------------------------------------------------------
dialogNewPass::~dialogNewPass()
{
    delete ui;
}
//---------------------------------------------------------------------------
std::optional<std::wstring>
dialogNewPass::exec(
    int minLength, int maxLength,
    int left, int top, bool leftTop)
{
    this->minLength = minLength;

    for (int i = minLength; i <= maxLength; ++i)
    {
        const auto line = QString::number(i);
        ui->comboBoxLength->addItem(line);
    }

    ui->checkBoxCapital->setChecked(false);
    ui->checkBoxSmall->setChecked(false);
    ui->checkBoxNum->setChecked(true);

    ui->buttonNew->setFocus();
    setCoord(left, top, leftTop);

    std::optional <std::wstring> result;

    if (QDialog::exec() == QDialog::Accepted)
        result = std::move(tmpPass);

    return result;
}
//---------------------------------------------------------------------------
void dialogNewPass::on_buttonNew_clicked()
{
    tmpPass =
        MPassword::New(
            ui->comboBoxLength->currentIndex() + minLength,
            ui->checkBoxCapital->isChecked(),
            ui->checkBoxSmall->isChecked(),
            ui->checkBoxNum->isChecked());

    ui->labelPassword->setText(QString::fromStdWString(tmpPass));
}
//---------------------------------------------------------------------------
void dialogNewPass::setCoord(int left, int top, bool leftTop)
{
    if (leftTop)
        this->move(left, top);
    else
        this->move(left - this->width(), top - this->height());
}
//---------------------------------------------------------------------------
