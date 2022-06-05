#include "dialogNewPass.h"
#include "ui_dialogNewPass.h"


dialogNewPass::dialogNewPass(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogNewPass)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}


dialogNewPass::~dialogNewPass()
{
    delete ui;
}
