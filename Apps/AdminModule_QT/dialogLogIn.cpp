#include "dialogLogIn.h"
#include "ui_dialogLogIn.h"


dialogLogIn::dialogLogIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogLogIn)
{
    ui->setupUi(this);

    ui->lineEditPassword->setMaxLength(MAX_UserPassLen);
    ui->labelPassword->setEnabled(false);
    ui->lineEditPassword->setEnabled(false);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}

dialogLogIn::~dialogLogIn()
{
    delete ui;
}

std::optional<std::uint32_t> dialogLogIn::exec(const MUsers &users_)
{
    for(const auto& user: users_)
    {
        if (user.Active)
        {
            ui->comboBoxLogin->addItem(
                QString::fromStdWString(user.Login),
                QVariant::fromValue(reinterpret_cast<void*>(
                    const_cast<MUsersItem*>(&user))));
        }
    }

    while(true)
    {
        if (static_cast<QDialog*>(this)->exec() == QDialog::Accepted)
        {
            const int index = ui->comboBoxLogin->currentIndex();

            if (index >= 0)
            {
                auto data = ui->comboBoxLogin->currentData(Qt::UserRole);
                auto* user = reinterpret_cast<const MUsersItem*>(data.value<void*>());

                if (user->Pass.Check(ui->lineEditPassword->text().toStdWString()))
                    return user->gUUID();
                else
                    ui->lineEditPassword->setFocus();
            }
        }
        else
        {
            return {};
        }
    }
}

void dialogLogIn::on_comboBoxLogin_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    ui->labelPassword->setEnabled(true);
    ui->lineEditPassword->setEnabled(true);
}
