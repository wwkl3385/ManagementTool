#include "registerdlg.h"
#include "ui_registerdlg.h"

registerDlg::registerDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::registerDlg)
{
    ui->setupUi(this);
    setWindowTitle("注册账户");
}

registerDlg::~registerDlg()
{
    delete ui;
}
