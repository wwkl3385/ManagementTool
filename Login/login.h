#ifndef LOGIN_H
#define LOGIN_H

#include "management/managementtool.h"
#include "jsonManage/jsonmanage.h"
#include "httpManage/httpmanage.h"
#include "loading/loadingdlg.h"
#include <QWidget>
#include <QPainter>
#include <QDialog>
#include <QMouseEvent>
#include <QTimer>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

    jsonManage *dataJson;
    httpManage *dataHttp;

    static QString userNameInfo; //登录名
    static QString userPassInfo;  //密码
    static unsigned userType;    //登录类型--1：管理员，--2：普通用户

    /*窗口移动*/
    QPoint m_DragPosition;
    bool m_Drag;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

signals:
    transmitSignal(QByteArray tmpData);   //数据传送
    transmitUpdateSignal(QByteArray tmpData);   //数据传送

private slots:
    void on_loginPushButton_clicked();
    void replyFinished(QNetworkReply *reply); //http接收数据
    void loginDataParse(QByteArray tmpData);       //登录数据解析
    void updateDataParse(QByteArray tmpData);       //登录数据解析
    void onConnectError();

private:
    loadingDlg     *pLoadDlg;        //登录动态效果
    QTimer         *pTimer;
    Ui::Login      *ui;
};

#endif // LOGIN_H
