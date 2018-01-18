/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： loadon.h
* 描    述： 登录对话框，检测软件更新
* 修改记录：
* 			V0.9，2018-01-09，new，刘卫明
*
***********************************************************************/
#ifndef LOGON_H
#define LOGON_H

#include "downloadManage/downloadmanager.h"
#include "management/managementtool.h"
#include "jsonManage/jsonmanage.h"
#include "httpManage/httpmanage.h"
#include "loading/loadingdlg.h"
#include <QProgressDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QDialog>
#include <QTimer>

namespace Ui {
class logon;
}

class logon : public QDialog
{
    Q_OBJECT
public:
    explicit logon(QWidget *parent = 0);
    ~logon();

    static QString userNameInfo;  //登录名
    static QString userPassInfo;  //密码
    static unsigned userType;     //登录类型--1：管理员，--2：普通用户

    QTime   m_timeRecord;
    QString m_url;
    uint    m_timeInterval;
    qint64  m_currentDownload;
    qint64  m_intervalDownload;

    jsonManage *dataJson;
    httpManage *dataHttp;

    QProgressDialog *progressDlg;
    QString transformUnit(qint64 bytes , bool isSpeed);
    QString transformTime(qint64 seconds);

    /*窗口移动*/
    QPoint m_DragPosition;
    bool   m_Drag;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

signals:
    transmitSignal(QByteArray tmpData);          //数据传送
    transmitUpdateSignal(QByteArray tmpData);   //数据传送

private slots:
    void on_logonPushButton_clicked();
    void replyFinished(QNetworkReply *reply);    //http接收数据
    void loginDataParse(QByteArray tmpData);     //登录数据解析
    void updateDataParse(QByteArray tmpData);    //登录数据解析

    void onReplyFinished(int statusCode);
    void onDownloadProcess(qint64 bytesReceived, qint64 bytesTotal);

private:
    loadingDlg       *pLoadDlg;                   //登录动态效果
    QTimer           *pTimer;
    Ui::logon        *ui;
    downloadManager  *m_downloadManager;
};

#endif // LOGON_H
