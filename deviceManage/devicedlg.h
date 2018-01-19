/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： devicedlg.h
* 描    述： 设备管理-- 管理员查询所有设备，查询单个设备
* 修改记录：
* 			V0.9，2018-01-08，new，刘卫明
*
***********************************************************************/
#ifndef DEVICEDLG_H
#define DEVICEDLG_H

#include <QDialog>
#include <QProgressDialog>
#include "jsonManage/jsonmanage.h"
#include "httpManage/httpmanage.h"
#include <QTimer>
//#include <QMenu>
//#include <QAction>

namespace Ui {
class deviceDlg;
}

class deviceDlg : public QDialog
{
    Q_OBJECT

public:
    explicit deviceDlg(QWidget *parent = 0);
    ~deviceDlg();

    void keyPressEvent(QKeyEvent *event);
//    void createActions();
//    void contextMenuEvent(QContextMenuEvent *event);

    int            signalFlag;    //信号标志位
    static QString deviceIp;      //被选择的设备ip地址
    QList<QString> remoteIpList;  //存储解析的远程ip
    QList<QString> startTimeList; //存储解析最新上线时间
    QList<QString> userIdList;    //存储解析用户id地址

#if 0
    QMenu *pop_menu;
    QMenu *sort_style;
    QAction *action_name;
    QAction *action_size;
    QAction *action_type;
    QAction *action_date;
    QAction *action_open;
    QAction *action_download;
    QAction *action_flush;
    QAction *action_delete;
    QAction *action_rename ;
    QAction *action_create_folder;
#endif

signals:
    transmitNumSignal(QByteArray);
    transmitInfoSignal(QByteArray);
    transmitAllInfoSignal(QByteArray);

private slots:
    void onTimeRelay();
    void onReplyFinished(QNetworkReply *reply);
    void onConnectInfoDataParse(QByteArray tmpData);
    void onConnectAllInfoDataParse(QByteArray tmpData);
    void onConnectTotalNumDataParse(QByteArray tmpData);

    void on_nextPushButton_clicked();
    void on_searchPushButton_clicked();
    void on_previousPushButton_clicked();
    void on_searchAllPushButton_clicked();
    void on_currentPageLineEdit_returnPressed();
    void on_listTableWidget_cellClicked(int row, int column);

private:
    Ui::deviceDlg   *ui;
    jsonManage      *dataJson;
    httpManage      *dataHttp;
    QProgressDialog *progressDlg;
    QTimer          *pTimer;
};

#endif // DEVICEDLG_H
