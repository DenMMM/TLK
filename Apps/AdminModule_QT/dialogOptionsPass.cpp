//---------------------------------------------------------------------------
#include "dialogOptionsPass.h"
#include "ui_dialogOptionsPass.h"

#include "dialogNewPass.h"
//---------------------------------------------------------------------------
dialogOptionsPass::dialogOptionsPass(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogOptionsPass),
    optPass(nullptr)
{
    ui->setupUi(this);

    ui->lineEditPassword->setMaxLength(MAX_OptPassLen);
    ui->lineEditNew->setMaxLength(MAX_OptPassLen);
    ui->lineEditConfirm->setMaxLength(MAX_OptPassLen);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}
//---------------------------------------------------------------------------
dialogOptionsPass::~dialogOptionsPass()
{
    delete ui;
}
//---------------------------------------------------------------------------
bool dialogOptionsPass::exec(MOptions& options, int left, int top)
{
    optPass = &options.Pass;

    ui->lineEditPassword->setFocus();
    this->move(left, top);

    BasicRand.event();

    return QDialog::exec() == QDialog::Accepted;
}
//---------------------------------------------------------------------------
void dialogOptionsPass::accept()
{
    // Проверяем текущий пароль
    if (!optPass->Check(ui->lineEditPassword->text().toStdWString()))
    {
        ui->lineEditPassword->setFocus();
        return;
    }
    // Проверяем новый пароль
    if (ui->lineEditNew->text() != ui->lineEditConfirm->text())
    {
        ui->lineEditNew->setFocus();
        return;
    }
    //
    optPass->Set(ui->lineEditNew->text().toStdWString());

    QDialog::accept();
}
//---------------------------------------------------------------------------
void dialogOptionsPass::on_lineEditPassword_textEdited(const QString &arg1)
{
    BasicRand.event();
}
//---------------------------------------------------------------------------
void dialogOptionsPass::on_pushButtonGenerate_clicked()
{
    dialogNewPass dialog;

    auto result = dialog.exec(
        5, MAX_OptPassLen,
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
