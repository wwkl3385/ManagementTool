/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称：  registerDlg.h
* 描    述：  添加账户，修改账户
* 修改记录：
* 			V0.9，2017-12-28，new，刘卫明
*
***********************************************************************/
#ifndef REGISTERDLG_H
#define REGISTERDLG_H

#include <QDialog>
#include "jsonManage/jsonmanage.h"
#include "httpManage/httpmanage.h"

namespace Ui {
class registerDlg;
}

class registerDlg : public QDialog
{
    Q_OBJECT

public:
    explicit registerDlg(QWidget *parent = 0);
    ~registerDlg();

    jsonManage  *dataJson;
    httpManage  *dataHttp;
    static bool addSuccessFlag;
    static QString newPassword;  //修改后的密码

signals:
    transmitRegisterSignal(QByteArray tmpData);    //数据传送
    transmitModifySignal(QByteArray tmpData);      //修改数据信号

private slots:
    void on_registerPushButton_clicked();
    void onReplyRegisterFinished(QNetworkReply *reply); //http接收数据
    void onRegisterDataParse(QByteArray tmpData);       //注册数据解析
    void onReplyModifyFinished(QNetworkReply *reply);   //修改
    void onModifyDataParse(QByteArray tmpData);         //修改数据解析

private:
    Ui::registerDlg *ui;
    QRegExp rx;
    QRegExp rxMail;
    QRegExp rxDate;
    QRegExp rxPhone;
};

#endif // REGISTERDLG_H
