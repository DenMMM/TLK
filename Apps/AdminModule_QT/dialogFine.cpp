#include "dialogFine.h"
#include "ui_dialogFine.h"


dialogFine::dialogFine(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogFine)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}


dialogFine::~dialogFine()
{
    delete ui;
}


void dialogFine::on_buttonFine_clicked()
{
//
}
