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

#include <QProxyStyle>
#include <QPainter>
#include <QStyleOption>

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
    void delaySec(unsigned int msec);

    void        closeEvent(QCloseEvent *event);
    QStringList getIP(QString localHost);
    QString     getAdapterInfoWithWindows();
//    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    jsonManage  *dataJson;
    httpManage  *dataHttp;
    QString     aboutStr ;//关于版权

    static stJsonUserData  stUserInfo;

    QWidget     *funcWidget;
    registerDlg *regDlg;
    bool        vpnClickedConnect;   //连接vpn---1 ,   断开vpn---0
    bool        vpnClicked;
    QStringList oldIp;

signals:
    transmitSignal(QByteArray tmpData);          //数据传送
    closeManagementToolSignal(int);             //关闭主窗口信号

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
    void on_logoutPushButton_clicked();
    void on_upgradePushButton_clicked();

private:
    QProcess           *vpnProcess;
    QProcess           *puttyProcess;
    QProcess           *winscpProcess;
    Ui::ManagementTool *ui;
    QHBoxLayout        *layout;
    QTimer             *pTimer ;
};

#if 0
/*tab横向显示*/
class CustomTabStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab)
        {
            s.transpose();
            s.rwidth() = 50; // 设置每个tabBar中item的大小
            s.rheight() = 35;
        }
        return s;
    }

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel)
        {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
            {
                QRect allRect = tab->rect;

                if (tab->state & QStyle::State_Selected)
                {
                    painter->save();
                    painter->setPen(0x89cfff);
                    painter->setBrush(QBrush(0x89cfff));
                    painter->drawRect(allRect.adjusted(6, 6, -6, -6));
                    painter->restore();
                }
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                if (tab->state & QStyle::State_Selected)
                {
                    painter->setPen(0xf8fcff);
                }
                else
                {
                    painter->setPen(0x5d5d5d);
                }

                painter->drawText(allRect, tab->text, option);
                return;
            }
        }

        if (element == CE_TabBarTab)
        {
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }
};
#endif

#endif // MANAGEMENTTOOL_H
