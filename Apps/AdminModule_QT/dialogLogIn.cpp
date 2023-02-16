//---------------------------------------------------------------------------
#include "dialogLogIn.h"
#include "ui_dialogLogIn.h"
//---------------------------------------------------------------------------
dialogLogIn::dialogLogIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogLogIn),
    userId(0)
{
    ui->setupUi(this);

    ui->lineEditPassword->setMaxLength(MAX_UserPassLen);
    ui->labelPassword->setEnabled(false);
    ui->lineEditPassword->setEnabled(false);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}
//---------------------------------------------------------------------------
dialogLogIn::~dialogLogIn()
{
    delete ui;
}
//---------------------------------------------------------------------------
std::optional <std::uint32_t> dialogLogIn::exec(const MUsers& users)
{
    for (const auto& user: users)
    {
        if (user.Active)
        {
            ui->comboBoxLogin->addItem(
                QString::fromStdWString(user.Login),
                QVariant::fromValue(reinterpret_cast<void*>(
                    const_cast<MUsersItem*>(&user))));
        }
    }

    ui->comboBoxLogin->setFocus();

    BasicRand.event();

    std::optional <std::uint32_t> result;

    if (QDialog::exec() == QDialog::Accepted)
        result = userId;

    return result;
}
//---------------------------------------------------------------------------
void dialogLogIn::accept()
{
    const int index = ui->comboBoxLogin->currentIndex();

    if (index < 0)
    {
        ui->comboBoxLogin->setFocus();
        return;
    }

    auto data = ui->comboBoxLogin->currentData(Qt::UserRole);
    auto* user = reinterpret_cast<const MUsersItem*>(data.value<void*>());

    if (!user->Pass.Check(ui->lineEditPassword->text().toStdWString()))
    {
        ui->lineEditPassword->setFocus();
        return;
    }

    userId = user->gUUID();
    QDialog::accept();
}
//---------------------------------------------------------------------------
void dialogLogIn::on_comboBoxLogin_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    ui->labelPassword->setEnabled(true);
    ui->lineEditPassword->setEnabled(true);
}
//---------------------------------------------------------------------------
