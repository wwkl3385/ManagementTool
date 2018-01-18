/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： managementtool.h
* 描    述： 智能运维主界面，连接vpn,打开putty,打开winscp
* 修改记录：
* 			V0.9，2018-01-12，new，刘卫明
*
***********************************************************************/
#ifndef MANAGEMENTTOOL_H
#define MANAGEMENTTOOL_H

#include <QProgressDialog>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QProcess>
#include <QWidget>
#include <QTimer>
#include <QRect>

namespace Ui {
class ManagementTool;
}

class ManagementTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManagementTool(QWidget *parent = 0);
    ~ManagementTool();

    void        closeEvent(QCloseEvent *event);
    QStringList getIP(QString localHost);
    QStringList getAdapterInfoWithWindows();
    void        terminateApp(QString app);

    QWidget     *funcWidget;
    bool        vpnClickedConnect;   //连接vpn---1 ,   断开vpn---0
    bool        vpnClicked;
    QStringList oldIp;

private slots:
    void onTimeDelay();
    void on_puttyPushButton_clicked();
    void on_winscpPushButton_clicked();
    void on_recordPushButton_clicked();
    void on_devicePushButton_clicked();
    void on_accountPushButton_clicked();
    void on_puttyPushButtonClose_clicked();
    void on_connectVPNPushButton_clicked();
    void on_winscpPushButtonClose_clicked();
    void on_aboutPushButton_clicked();

private:
    QProcess           *vpnProcess;
    QProcess           *puttyProcess;
    QProcess           *winscpProcess;
    Ui::ManagementTool *ui;
    QHBoxLayout        *layout;
    QTimer             *pTimer ;
};

#endif // MANAGEMENTTOOL_H
