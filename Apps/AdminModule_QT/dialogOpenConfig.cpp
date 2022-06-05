#include "dialogOpenConfig.h"
#include "ui_dialogOpenConfig.h"


dialogOpenConfig::dialogOpenConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogOpenConfig)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}


dialogOpenConfig::~dialogOpenConfig()
{
    delete ui;
}
