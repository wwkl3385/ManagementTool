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

#include "jsonManage/jsonmanage.h"
#include "httpManage/httpmanage.h"
#include "register/registerdlg.h"
#include <QProgressDialog>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QProcess>
#include <QWidget>
#include <QTimer>
#include <QRect>

/*json内容:用户数据--解析数据结构*/

bool findApp(const QString& exe);  //根据进程名字找到该进程
void terminateApp(QString app);

typedef struct ST_JSONUSERDATA
{
    QString user;        //查询当前所有的用户信息
    QString password;
    QString userMail;
    QString userPhone;
    QString userEnable;
    QString userStartDate;
    QString userEndDate;
}stJsonUserData;


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

    jsonManage  *dataJson;
    httpManage  *dataHttp;

    static stJsonUserData  stUserInfo;

    QWidget     *funcWidget;
    registerDlg *regDlg;
    bool        vpnClickedConnect;   //连接vpn---1 ,   断开vpn---0
    bool        vpnClicked;
    QStringList oldIp;

signals:
    transmitSignal(QByteArray tmpData);          //数据传送

private slots:
    void onTimeDelay();
    void on_puttyPushButton_clicked();
    void on_winscpPushButton_clicked();
    void on_recordPushButton_clicked();
    void on_devicePushButton_clicked();
    void on_accountPushButton_clicked();
    void on_connectVPNPushButton_clicked();
    void on_aboutPushButton_clicked();
    void on_aboutAction_triggered();
    void on_helpAction_triggered();
    void on_exitAction_triggered();

    void onOneUserDataParse(QByteArray tmpData);    //登录数据解析
    void onReplyFinished(QNetworkReply *reply);    //http接收数据

private:
    QProcess           *vpnProcess;
    QProcess           *puttyProcess;
    QProcess           *winscpProcess;
    Ui::ManagementTool *ui;
    QHBoxLayout        *layout;
    QTimer             *pTimer ;
};

#endif // MANAGEMENTTOOL_H
