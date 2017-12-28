#include "managementtool.h"
#include "ui_managementtool.h"
#include "logon/logon.h"
#include "password/passworddlg.h"
#include "accountManage/accountdlg.h"
#include "deviceManage/devicedlg.h"
#include "recordExport/recorddlg.h"
#include<QScrollBar>
#include<QDebug>
#include<QMessageBox>
#include<QHeaderView>
#include<QApplication>
#include<QTime>
#include<QHBoxLayout>
#include<QGridLayout>

ManagementTool::ManagementTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ManagementTool)
{
    ui->setupUi(this);
    setWindowTitle("智能集中控制器远程管理系统");
    setWindowIcon(QIcon("teld.ico")); //设置软件图标
    ui->spaceFrame->setStyleSheet("QFrame#myframe{border-image:url(:/new/background/tgood.png)}");

    funcWidget = NULL;

    layout = new QHBoxLayout;
    layout->setMargin(3);
    ui->spaceFrame->setLayout(layout);

    ui->adminToolBox->hide();
    ui->puttyPushButton->hide();
    ui->winscpPushButton->hide();

    ui->nameLabel->setText("普通用户");
    ui->nameLineEdit->setText("Qstring:name");

    /*管理员和普通用户*/
    if(FLAG)
    {
        ui->nameLabel->setText("管理员");
        ui->nameLineEdit->setText("Qstring:admin");
        ui->adminToolBox->show();
    }
}

ManagementTool::~ManagementTool()
{
    delete ui;
    qDebug()<< "销毁 tool";
}

/*设备管理*/
void ManagementTool::on_devicePushButton_clicked()
{
    if (funcWidget)
    {
        layout->removeWidget(funcWidget);
        delete funcWidget;
        funcWidget = NULL;
    }

    funcWidget = new deviceDlg(ui->spaceFrame);
    layout->addWidget(funcWidget);
    funcWidget->show();
}

/*账户设置*/
void ManagementTool::on_accountPushButton_clicked()
{
    if (funcWidget)
    {
        layout->removeWidget(funcWidget);
        delete funcWidget;
        funcWidget = NULL;
    }

    funcWidget = new accountDlg(ui->spaceFrame);

    layout->addWidget(funcWidget);
    funcWidget->show();
}

void ManagementTool::on_passwordChangePushButton_clicked()
{
    if (funcWidget)
    {
        layout->removeWidget(funcWidget);
        delete funcWidget;
        funcWidget = NULL;
    }

    funcWidget = new passwordDlg(ui->spaceFrame);

    layout->addWidget(funcWidget);
    funcWidget->show();
}

void ManagementTool::on_actionexit_triggered()
{
    this->close();
}

void ManagementTool::on_addAccountPushButton_clicked()
{
    if (funcWidget)
    {
        layout->removeWidget(funcWidget);
        delete funcWidget;
        funcWidget = NULL;
    }

    funcWidget = new accountDlg(ui->spaceFrame);

    layout->addWidget(funcWidget);
    funcWidget->show();
}

void ManagementTool::on_registerPushButton_clicked()
{
    QMessageBox::information(this, "提示", "注册成功！");
//    ui->stackedWidget->setCurrentIndex(1);
}

/**********************************************************************
* 功    能：菜单-帮助，提示信息
* 输    入：
* 输    出：
* 作    者：刘卫明
* 编写日期：2017.12.26
***********************************************************************/
void ManagementTool::on_actionabout_A_triggered()
{
    QMessageBox::about(NULL, "版权所有", "Copyright (c) 2017, 青岛特来电新能源有限公司, All rights reserved." \
                                   "\n\n集中控制器远程管理系统 V0.9\n\n智能充电中心");
}

void ManagementTool::on_logoutPushButton_clicked()
{
    this->close();
    delete this;
    logon logonDlg;
    logonDlg.exec();
}

/*登录记录*/
void ManagementTool::on_recordPushButton_clicked()
{
      if (funcWidget)
    {
        layout->removeWidget(funcWidget);
        delete funcWidget;
        funcWidget = NULL;
    }

    funcWidget = new recordDlg(ui->spaceFrame);
    layout->addWidget(funcWidget);
    funcWidget->show();
}
