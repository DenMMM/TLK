//---------------------------------------------------------------------------
#include "dialogOpenConfig.h"
#include "ui_dialogOpenConfig.h"

#include "..\..\Core\UnitOptions.h"
//---------------------------------------------------------------------------
dialogOpenConfig::dialogOpenConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogOpenConfig),
    configPass(nullptr)
{
    ui->setupUi(this);

    ui->lineEditPassword->setMaxLength(MAX_OptPassLen);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}
//---------------------------------------------------------------------------
dialogOpenConfig::~dialogOpenConfig()
{
    delete ui;
}
//---------------------------------------------------------------------------
bool dialogOpenConfig::exec(
    const MPassword& pass,
    int left, int top, bool leftTop)
{
    configPass = &pass;

    ui->lineEditPassword->setFocus();
    setCoord(left, top, leftTop);

    BasicRand.event();

    return QDialog::exec() == QDialog::Accepted;
}
//---------------------------------------------------------------------------
void dialogOpenConfig::accept()
{
    if (!configPass->Check(ui->lineEditPassword->text().toStdWString()))
        ui->lineEditPassword->setFocus();
    else
        QDialog::accept();
}
//---------------------------------------------------------------------------
void dialogOpenConfig::setCoord(int left, int top, bool leftTop)
{
    if (leftTop)
        this->move(left, top);
    else
        this->move(left - this->width(), top - this->height());
}
//---------------------------------------------------------------------------
