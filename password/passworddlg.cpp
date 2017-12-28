#include "passworddlg.h"
#include "ui_passworddlg.h"
#include "management/managementtool.h"

passwordDlg::passwordDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::passwordDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
}

passwordDlg::~passwordDlg()
{
    delete ui;
}
