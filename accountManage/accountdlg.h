/***********************************************************************
* Copyright (c) 2017, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： accountdlg.h
* 描    述： 账户管理  -- 查询所有账户，查询单个账户，删除账户
* 修改记录：
* 			V0.9，2017-12-28，new，刘卫明
*
***********************************************************************/
#ifndef ACCOUNTDLG_H
#define ACCOUNTDLG_H

#include <QDialog>
#include <QByteArray>
#include "management/managementtool.h"
#include "jsonManage/jsonmanage.h"
#include "httpManage/httpmanage.h"

/*json内容:用户数据--解析数据结构*/
typedef struct ST_JSONDATA
{
    QString userType;     //登录认证：查询成功才有 管理员为1。

    QString user;        //查询当前所有的用户信息
    QString password;
    QString userMail;
    QString userPhone;
    QString userEnable;
    QString userStartDate;
    QString userEndDate;
}stJsonData;

typedef QList<stJsonData> dataList;

namespace Ui {
class accountDlg;
}

class accountDlg : public QDialog
{
    Q_OBJECT

public:
    explicit accountDlg(QWidget *parent = 0);
    ~accountDlg();

    void keyPressEvent(QKeyEvent  *event);

    static stJsonData modifyInfoData;
    static dataList   userList;     //用户数据列表
    static int        deleteRow;    //删除&修改 选中的行
    static bool       modifyFlag;
    static bool       modifyAdmin;  //管理员修改密码

    jsonManage *dataJson;
    httpManage *dataHttp;
    stJsonData stData;              //单个用户数据
    bool       signalFlag;          //请求&删除 信号标志

signals:
    transmitDeleteUserDataSignal(QByteArray tempBuf);
    transmitUserDataSignal(QByteArray tempBuf);

private slots:
    void onDeleteUserDataParse(QByteArray tmpData);
    void onReplyFinished(QNetworkReply *reply);
    void onUserDataParse(QByteArray tmpData);

    void on_accountTableWidget_cellClicked(int row, int column);
    void on_addAccountPushButton_clicked();
    void on_deletePushButton_clicked();
    void on_searchPushButton_clicked();
    void on_allUserPushButton_clicked();
    void on_modifyPushButton_clicked();

private:
    Ui::accountDlg *ui;
};

#endif // ACCOUNTDLG_H
