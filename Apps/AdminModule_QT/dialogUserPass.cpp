//---------------------------------------------------------------------------
#include "dialogUserPass.h"
#include "ui_dialogUserPass.h"

#include "dialogNewPass.h"
//---------------------------------------------------------------------------
dialogUserPass::dialogUserPass(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogUserPass),
    modeUsers(false)
{
    ui->setupUi(this);

    ui->lineEditPassword->setMaxLength(MAX_UserPassLen);
    ui->lineEditNew->setMaxLength(MAX_UserPassLen);
    ui->lineEditConfirm->setMaxLength(MAX_UserPassLen);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}
//---------------------------------------------------------------------------
dialogUserPass::~dialogUserPass()
{
    delete ui;
}
//---------------------------------------------------------------------------
bool dialogUserPass::exec(MUsersItem& user, int left, int top, bool leftTop)
{
    modeUsers = false;

    ui->comboBoxLogin->addItem(
        QString::fromStdWString(user.Login),
        QVariant::fromValue(reinterpret_cast<void*>(
            const_cast<MUsersItem*>(&user))));

    ui->comboBoxLogin->setEnabled(false);
    ui->comboBoxLogin->setCurrentIndex(0);

    setEditable(false, true);
    ui->lineEditNew->setFocus();
    setCoord(left, top, leftTop);

    BasicRand.event();

    return QDialog::exec() == QDialog::Accepted;
}
//---------------------------------------------------------------------------
bool dialogUserPass::exec(MUsers& users, int left, int top, bool leftTop)
{
    modeUsers = true;

    for (auto& user: users)
    {
        if (user.Active)
        {
            ui->comboBoxLogin->addItem(
                QString::fromStdWString(user.Login),
                QVariant::fromValue(reinterpret_cast<void*>(
                    const_cast<MUsersItem*>(&user))));
        }
    }

    ui->comboBoxLogin->setEnabled(true);
    ui->comboBoxLogin->setCurrentIndex(-1);

    setEditable(false, false);
    ui->comboBoxLogin->setFocus();
    setCoord(left, top, leftTop);

    BasicRand.event();

    return QDialog::exec() == QDialog::Accepted;
}
//---------------------------------------------------------------------------
void dialogUserPass::accept()
{
    const int index = ui->comboBoxLogin->currentIndex();

    if (index < 0)
    {
        ui->comboBoxLogin->setFocus();
        return;
    }

    auto data = ui->comboBoxLogin->itemData(index, Qt::UserRole);
    auto* user = reinterpret_cast<MUsersItem*>(data.value<void*>());

    if (modeUsers && !user->Pass.Check(ui->lineEditPassword->text().toStdWString()))
    {
        ui->lineEditPassword->setFocus();
        return;
    }

    if (ui->lineEditNew->text() != ui->lineEditConfirm->text())
    {
        ui->lineEditNew->setFocus();
        return;
    }

    //
    user->Pass.Set(ui->lineEditNew->text().toStdWString());

    QDialog::accept();
}
//---------------------------------------------------------------------------
void dialogUserPass::on_comboBoxLogin_currentIndexChanged(int index)
{
    setEditable(true, true);
}
//---------------------------------------------------------------------------
void dialogUserPass::on_lineEditPassword_textEdited(const QString &arg1)
{
    BasicRand.event();
}
//---------------------------------------------------------------------------
void dialogUserPass::on_pushButtonGenerate_clicked()
{
    dialogNewPass dialog;

    auto result = dialog.exec(
        5, MAX_UserPassLen,
        this->x() + 20, this->y() + 30,
        true);

    if (result.has_value())
    {
        ui->lineEditNew->setText(
            QString::fromStdWString(result.value()));
        ui->lineEditConfirm->clear();
        ui->lineEditConfirm->setFocus();
    }
}
//---------------------------------------------------------------------------
void dialogUserPass::setCoord(int left, int top, bool leftTop)
{
    if (leftTop)
        this->move(left, top);
    else
        this->move(left - this->width(), top - this->height() - 30);      /// Уточнить высоту заголовка окна.
}
//---------------------------------------------------------------------------
void dialogUserPass::setEditable(bool modeForCurrent, bool modeForNew)
{
    ui->labelPassword->setEnabled(modeForCurrent);
    ui->lineEditPassword->setEnabled(modeForCurrent);

    ui->labelNew->setEnabled(modeForNew);
    ui->lineEditNew->setEnabled(modeForNew);
    ui->pushButtonGenerate->setEnabled(modeForNew);
    ui->labelConfirm->setEnabled(modeForNew);
    ui->lineEditConfirm->setEnabled(modeForNew);
}
//---------------------------------------------------------------------------
