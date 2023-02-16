//---------------------------------------------------------------------------
#include "dialogAuth.h"
#include "ui_dialogAuth.h"
//---------------------------------------------------------------------------
dialogAuth::dialogAuth(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogAuth)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}
//---------------------------------------------------------------------------
dialogAuth::~dialogAuth()
{
    delete ui;
}
//---------------------------------------------------------------------------
