/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： managementtool.cpp
* 描    述： 智能运维主界面，连接vpn,打开putty,打开winscp
* 修改记录：
* 			V0.9，2018-01-12，new，刘卫明
*
***********************************************************************/
#include "accountManage/accountdlg.h"
#include "ui_managementtool.h"
#include "managementtool.h"
#include "logon/logon.h"
#include "deviceManage/devicedlg.h"
#include "recordExport/recorddlg.h"
#include <QScrollBar>
#include <QMovie>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QHostInfo>
#include <QHeaderView>
#include <QApplication>
#include <QTime>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QDebug>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <string.h>

#define  QUERY_ONE_USER_URL "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/query_one_user"   //查询单个用户信息url

static int n = 0;       //记录创建的次数
static int secTmp = 0;  //连接vpn 超时时间
static int sec = 0;     //vpn 连接时间 s
static int min = 0;     //vpn 连接时间 min
bool connectVpnSuccessFlag = false;  //vpn连接成功标志  false:未连接
bool openPutty  = true;
bool openWinscp = true;
stJsonUserData  ManagementTool::stUserInfo;

ManagementTool::ManagementTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ManagementTool)
{
    ui->setupUi(this);
    qDebug() <<"managementDlg创建的次数："<< ++n;
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);  //防止系统休眠

    setWindowTitle("集中控制器智能远程管理系统");
    setWindowIcon(QIcon("teld.ico")); //设置软件图标
    this->resize(1500, 800);          //设置初始尺寸
    ui->flashLabel->setStyleSheet("QLabel{background-color:lightblue;border-radius:6px;}");

    QMovie *movie = new QMovie(":/new/background/loading2.gif");
    ui->loadingLabel->setMovie(movie);
    movie->start();

    dataJson = new jsonManage;
    dataHttp = new httpManage;
    funcWidget = NULL;
    regDlg = NULL;
    vpnClickedConnect = false;
    vpnClicked = false;

    ui->statusStackedWidget->setCurrentIndex(1);
    ui->loadingLabel->hide();
    ui->connectVpnLabel->hide();

    pTimer = new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(onTimeDelay()));
    pTimer->start(500);     //500ms

    layout = new QHBoxLayout;
    layout->setMargin(3);
    ui->spaceFrame->setLayout(layout);

    vpnProcess = new QProcess(this);
    puttyProcess = new QProcess(this);
    winscpProcess = new QProcess(this);

    qDebug() << "managementDlg:" <<logon::userNameInfo;
    qDebug() << "managementDlg:" <<logon::userPassInfo; //密码

    ui->nameLineEdit->setText(logon::userNameInfo);

    ui->puttyPushButton->hide();
    ui->winscpPushButton->hide();
    ui->connectVPNPushButton->setText("连接VPN");

    ui->connectVPNPushButton->setStyleSheet("QPushButton { font-family:Microsoft Yahei; color:white; "
                                            "background-color:#0f9ef7; border-radius:10px; font: 12pt; } "
                                            "QPushButton:hover { background-color:deepskyblue; }  "
                                            "QPushButton:pressed { background-color:deepskyblue; padding-left:4px; "
                                            "padding-top:4px; } "
                                            "QPushButton:unpressed { background-color:#0f9ef7; "
                                            "padding-left:4px; padding-top:4px;}");

    ui->recordPushButton->hide();
    ui->manageTabWidget->setCurrentIndex(0);

    /*连接服务器,请求单个用户*/
    dataJson->queryOneUserObj= dataJson->jsonPackQueryOneUser(logon::userNameInfo);
    dataHttp->httpPost(QUERY_ONE_USER_URL, dataJson->queryOneUserObj); //http请求当前用户
    qDebug() << "当前用户信息请求:" << dataJson->queryOneUserObj;

    /*http请求*/
    connect(this->dataHttp->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    connect(this, SIGNAL(transmitSignal(QByteArray)), this, SLOT(onOneUserDataParse(QByteArray)));
}

ManagementTool::~ManagementTool()
{
    /*强制 结束任务*/

    if (findApp("openvpn.exe") == true)
        terminateApp("taskkill /im openvpn.exe /f");
    if (findApp("putty.exe") ==  true)
        terminateApp("taskkill /im putty.exe /f");
    if (findApp("WinSCP.exe") == true)
        terminateApp("taskkill /im WinSCP.exe /f");

    if (funcWidget)
    {
        layout->removeWidget(funcWidget);
        delete funcWidget;
        funcWidget = NULL;
    }

    if (regDlg)
    {
        delete regDlg;
        regDlg = NULL;
    }

    qDebug() <<"managementDlg创建的次数："<< --n;
    qDebug()<< "销毁managementToolDlg";

    vpnProcess->close();
    puttyProcess->close();
    winscpProcess->close();
    delete vpnProcess;
    delete puttyProcess;
    delete winscpProcess;

    delete ui;
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
    /*管理员和普通用户*/
    if(logon::userType == 1) //管理员
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
    else
    {
        /*http请求*/
        connect(this, SIGNAL(transmitSignal(QByteArray)), this, SLOT(onOneUserDataParse(QByteArray)));
        /*连接服务器,请求单个用户*/
        dataHttp->httpPost(QUERY_ONE_USER_URL, dataJson->queryOneUserObj); //http请求当前用户

        accountDlg::modifyFlag = true;  //修改用户
        if (regDlg)
        {
            delete regDlg;
            regDlg = NULL;
        }

         regDlg = new registerDlg;
         regDlg->exec();
         return;
    }
}


void ManagementTool::closeEvent(QCloseEvent *event)
{
    QMessageBox Msg(QMessageBox::Question, QString("最后提示"), QString("是否要退出？"));
    QAbstractButton *pYesBtn = (QAbstractButton*)Msg.addButton(QString("确定"), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = (QAbstractButton*)Msg.addButton(QString("取消"), QMessageBox::NoRole);
    Msg.exec();

    if (Msg.clickedButton() != pNoBtn)
    {
        delete pYesBtn;
        delete pNoBtn;

        event->accept();
    }
    else
    {
        delete pYesBtn;
        delete pNoBtn;
        event->ignore();
    }
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

void ManagementTool::onTimeDelay()
{
    QApplication::processEvents(); //处理大数据，防gui假死

    QStringList newIp= getIP(QHostInfo::localHostName());
    QString strTime;

    if (!ui->loadingLabel->isHidden())
        ui->connectVPNPushButton->setDisabled(true);
    else
        ui->connectVPNPushButton->setEnabled(true);

    if (vpnClickedConnect == true && connectVpnSuccessFlag != true)
    {
        ++secTmp;
        if (secTmp > 20) //10s
        {
            secTmp = 0;
            vpnClickedConnect = false;  //断开
            ui->statusStackedWidget->setCurrentIndex(1);
            ui->loadingLabel->hide();
            ui->connectVpnLabel->setText("连接失败，请重新连接...");
        }
    }

    /*是否连接vpn,连接状态*/
    if(vpnClicked != true)
    {
        oldIp  = getIP(QHostInfo::localHostName());
    }

    if (vpnClickedConnect == true &&  newIp.size() > oldIp.size())
    {
        static int halfSec = 0;
        halfSec ++;
        if (halfSec > 6000)
            halfSec = 0;

        if (halfSec % 2 == 0)
        {
            if (sec < 59)
                sec++;
            else
            {
                ++ min;
                sec = 0;
            }
        }

        QString dirPath = QCoreApplication::applicationDirPath();
        QString fileName = dirPath + "/thirdApp/config/2314nh4j23h40980.txt";
         QFile file(fileName);
        if (file.exists())
        {
            file.resize(0);
        }

        ui->connectVpnLabel->setText("正在连接vpn,请稍后...");
        ui->connectVpnLabel->hide();

        connectVpnSuccessFlag = true;
        ui->connectVPNPushButton->setText("断开VPN");
        ui->connectVPNPushButton->setStyleSheet("QPushButton{font-family:Microsoft Yahei; color:white; background-color:#729ab4;"
                                                "border-radius:10px; font:12pt; } QPushButton:hover { background-color:#aac8db;} "
                                                "QPushButton:pressed { background-color:#9fbfd4; padding-left:4px; padding-top:4px;}"
                                                "QPushButton:unpressed { background-color:rgb(0 , 100 , 0); padding-left:4px; "
                                                "padding-top:4px; }");
        ui->connectVPNPushButton->setEnabled(true);
        ui->statusStackedWidget->setCurrentIndex(0);
        ui->connectStatusLabel->setText("已连接");
        ui->connectStatusLabel->setStyleSheet("QLabel{color:seagreen;font:10pt;font:bold;}");

        if (halfSec % 2 == 1)
            ui->flashLabel->setStyleSheet("QLabel{background-color:#A2CD5A;border-radius:6px;}");
        else
            ui->flashLabel->setStyleSheet("QLabel{background-color:#EE8262; border-radius:6px;}");

        strTime.sprintf("%d分%d秒", min, sec);
        ui->timeLable->setText(strTime);
        ui->ipAddrLabel->setText(newIp.last());
    }
    else
    {
        ui->connectVPNPushButton->setText("连接VPN");
        ui->connectVPNPushButton->setStyleSheet("QPushButton {font-family:Microsoft Yahei; color:white; background-color:#0f9ef7;"
                                   "border-radius:10px; font: 12pt; } QPushButton:hover { background-color:deepskyblue;}"
                                   "QPushButton:pressed { background-color:deepskyblue; padding-left:4px; padding-top:4px;} "
                                   "QPushButton:unpressed { background-color:rgb(0 , 100 , 0); padding-left:4px; padding-top:4px;}");
        ui->flashLabel->setStyleSheet("QLabel{background-color:lightblue;border-radius:6px;}");
        ui->statusStackedWidget->setCurrentIndex(1);
        ui->loadingLabel->setText("");
        ui->timeLable->setText("");
        connectVpnSuccessFlag = false;
    }

    if (!deviceDlg::deviceIp.isEmpty() && connectVpnSuccessFlag== true)
    {
        ui->puttyPushButton->show();
        ui->winscpPushButton->show();
    }
    else
    {
        ui->puttyPushButton->hide();
        ui->winscpPushButton->hide();
    }

    if (findApp("putty.exe") ==  true)
    {
        openPutty = false;
        ui->puttyPushButton->setText("关闭putty");
        ui->puttyPushButton->setStyleSheet("QPushButton{font-family:Microsoft Yahei; color:white; background-color:#88acc3; "
                                           "border-radius:10px; font: 12pt; } QPushButton:hover { background-color:#aac8db; }"
                                           "QPushButton:pressed { background-color:skyblue; padding-left:4px; padding-top:4px;}"
                                           "QPushButton:unpressed { background-color:rgb(0 , 100 , 0); padding-left:4px; "
                                           "padding-top:4px; }");
    }
    else
    {
        openPutty = true;
        ui->puttyPushButton->setText("打开putty");
        ui->puttyPushButton->setStyleSheet("QPushButton { font-family:Microsoft Yahei; color:white; background-color:#47b4f6; "
                                           "border-radius:10px; font: 12pt; } QPushButton:hover { background-color:deepskyblue;}"
                                           "QPushButton:pressed { background-color:deepskyblue; padding-left:4px; "
                                           "padding-top:4px;} QPushButton:unpressed { background-color:rgb(0 , 100 , 0); "
                                           "padding-left:4px; padding-top:4px;}");
    }

    if (findApp("WinSCP.exe") == true)
    {
        openWinscp= false;
        ui->winscpPushButton->setText("关闭winscp");
        ui->winscpPushButton->setStyleSheet("QPushButton{font-family:Microsoft Yahei; color:white; background-color:#88acc3;"
                                            "border-radius:10px; font: 12pt; } QPushButton:hover { background-color:#aac8db; } "
                                            "QPushButton:pressed { background-color:skyblue; padding-left:4px; padding-top:4px; }"
                                            "QPushButton:unpressed { background-color:rgb(0 , 100 , 0); padding-left:4px; "
                                            "padding-top:4px; }");
    }
    else
    {
        openWinscp = true;
        ui->winscpPushButton->setText("打开winscp");
        ui->winscpPushButton->setStyleSheet("QPushButton { font-family:Microsoft Yahei; color:white; background-color:#47b4f6;"
                                            "border-radius:10px; font: 12pt; } QPushButton:hover { background-color:deepskyblue;}"
                                            "QPushButton:pressed { background-color:deepskyblue; padding-left:4px; "
                                            "padding-top:4px; } QPushButton:unpressed { background-color:rgb(0 , 100 , 0); "
                                            "padding-left:4px; padding-top:4px;}");
    }

    /*管理员密码被修改后,退出主界面*/
    if (accountDlg::modifyAdmin == true)
    {
        QMessageBox::information(NULL, "提示", "您的密码已被修改，请重新登录！" );
        accountDlg::modifyAdmin = false;

        delete this;
    }
}

/*连接 vpn*/
void ManagementTool::on_connectVPNPushButton_clicked()
{
    secTmp = 0;  //连接vpn 超时时间
    sec = 0;     //vpn 连接时间 s
    min = 0;     //vpn 连接时间 min

    /*结束 openvpn 任务*/
    if (findApp("openvpn.exe") == true)
        terminateApp("taskkill /im openvpn.exe /f");

    QString program = QCoreApplication::applicationDirPath() + "/thirdApp/bin/openvpn.exe";
    QString workFileDir = QCoreApplication::applicationDirPath() + "/thirdApp/config";
    vpnProcess->setWorkingDirectory(workFileDir);    //指定进程的工作目录

    QStringList args;
    args.append("--config");
    args.append("client.ovpn");
    args.append("--auth-user-pass");
    args.append("2314nh4j23h40980.txt");
    args.append("script-security");
    args.append("3");

    qDebug() <<"vpn连接状态："<< connectVpnSuccessFlag ;

    if (connectVpnSuccessFlag == false && vpnClickedConnect != true)
    {
        vpnClicked = true;
        vpnClickedConnect = true; //连接
        connectVpnSuccessFlag = false;

        ui->statusStackedWidget->show();
        ui->statusStackedWidget->setCurrentIndex(1);
        ui->loadingLabel->show();
        ui->connectVpnLabel->show();
        ui->connectVpnLabel->setText("正在连接VPN,请稍后...");

        QString dirPath = QCoreApplication::applicationDirPath();
        QDir *userPass= new QDir;
        bool exist = userPass->exists(dirPath +"/thirdApp/config");
        if(!exist)
        {
            bool ok = userPass->mkdir(dirPath +"/thirdApp/config");
            qDebug() <<ok;
        }

        QString fileName = dirPath + "/thirdApp/config/2314nh4j23h40980.txt";

        qDebug() << fileName;
        QFile file(fileName);
        if (file.exists())
        {
            file.resize(0);
        }

        if(!file.open(QIODevice::WriteOnly  | QIODevice::Text|QIODevice::Append))
        {
            QMessageBox::warning(this,"提示","can't open",QMessageBox::Yes);
        }
        QTextStream in(&file);

        qDebug() << "managementDlg:" <<logon::userNameInfo;
        qDebug() << "managementDlg:" <<logon::userPassInfo; //密码

        in << logon::userNameInfo+ "\r\n" + logon::userPassInfo;
        file.close();

        oldIp  = getIP(QHostInfo::localHostName());
        vpnProcess->start(program, args);

//        vpnProcess->waitForStarted();
//        vpnProcess->waitForFinished();

        QString strTemp=QString::fromLocal8Bit(vpnProcess->readAllStandardOutput());
        qDebug() << strTemp;
    }
    else
    {
        vpnProcess->close();
        qDebug() <<"断开vpn："<< connectVpnSuccessFlag ;
        connectVpnSuccessFlag  = false;
        vpnClickedConnect = false;  //断开
        vpnClicked = false;

        ui->ipAddrLabel->setText("");
        ui->loadingLabel->hide();
    }
}

void ManagementTool::on_puttyPushButton_clicked()
{
//     QProcess puttyProcess(this);

    if (openPutty == true)
    {
        openPutty = false;
        ui->puttyPushButton->setText("关闭putty");
        ui->puttyPushButton->setStyleSheet("QPushButton{font-family:Microsoft Yahei; color:white; background-color:#88acc3; "
                                           "border-radius:10px; font: 12pt; } QPushButton:hover { background-color:#aac8db; } "
                                           "QPushButton:pressed { background-color:skyblue; padding-left:4px; padding-top:4px; }"
                                           "QPushButton:unpressed { background-color:rgb(0 , 100 , 0); padding-left:4px; "
                                           "padding-top:4px; }");

        QString program = QCoreApplication::applicationDirPath() + "./thirdApp/bin/putty.exe";

        QStringList arguments;
        QString ip = deviceDlg::deviceIp;

        arguments<< "-telnet" << ip << "23";

        qDebug() << arguments;

        puttyProcess->setProcessChannelMode(QProcess::SeparateChannels);
        puttyProcess->setReadChannel(QProcess::StandardOutput);
        puttyProcess->start(program, arguments, QIODevice::ReadWrite);
    }
    else
    {
        terminateApp("taskkill /im putty.exe /f");
        openPutty = true;
        ui->puttyPushButton->setText("打开putty");
        ui->puttyPushButton->setStyleSheet("QPushButton { font-family:Microsoft Yahei; color:white; background-color:#47b4f6; "
                                           "border-radius:10px; font: 12pt; } QPushButton:hover { background-color:deepskyblue;}"
                                           "QPushButton:pressed { background-color:deepskyblue; padding-left:4px; "
                                           "padding-top:4px; } QPushButton:unpressed { background-color:rgb(0 , 100 , 0); "
                                           "padding-left:4px; padding-top:4px;}");
    }
}

void ManagementTool::on_winscpPushButton_clicked()
{
    if (openWinscp== true)
    {
        openWinscp= false;
        ui->winscpPushButton->setText("关闭winscp");
        ui->winscpPushButton->setStyleSheet("QPushButton{font-family:Microsoft Yahei; color:white; background-color:#88acc3;"
                                            "border-radius:10px; font: 12pt; } QPushButton:hover {background-color:#aac8db;}"
                                            "QPushButton:pressed {background-color:skyblue; padding-left:4px; padding-top:4px;}"
                                            "QPushButton:unpressed {background-color:rgb(0 , 100 , 0); padding-left:4px; "
                                            "padding-top:4px; }");

        QString program = QCoreApplication::applicationDirPath() + "./thirdApp/bin/WinScp.exe";
        QStringList arguments;
        QString ip = deviceDlg::deviceIp;

        arguments << QString("%1://%2:%3@%4:%5").arg("ftp").arg("EM9280").arg("root").arg(ip).arg(21);

        winscpProcess->setProcessChannelMode(QProcess::SeparateChannels);
        winscpProcess->setReadChannel(QProcess::StandardOutput);
        winscpProcess->start(program, arguments, QIODevice::ReadWrite);
    }
    else
    {
        terminateApp("taskkill /im WinSCP.exe /f");
        openWinscp = true;
        ui->winscpPushButton->setText("打开winscp");
        ui->winscpPushButton->setStyleSheet("QPushButton { font-family:Microsoft Yahei; color:white; background-color:#47b4f6; "
                                            "border-radius:10px; font: 12pt; } QPushButton:hover { background-color:deepskyblue; }  "
                                            "QPushButton:pressed { background-color:deepskyblue; padding-left:4px; "
                                            "padding-top:4px; } QPushButton:unpressed { background-color:rgb(0 , 100 , 0); "
                                            "padding-left:4px; padding-top:4px;}");
    }
}

#if 0
void ManagementTool::onProcessError(QProcess::ProcessError error)
{
    switch(error)
    {
    case QProcess::FailedToStart:
        QMessageBox::information(0, "Tip", "FailedToStart");
        break;
    case QProcess::Crashed:
        QMessageBox::information(0, "Tip", "Crashed");
        break;
    case QProcess::Timedout:
        QMessageBox::information(0, "Tip", "Timedout");
        break;
    case QProcess::WriteError:
        QMessageBox::information(0, "Tip", "WriteError");
        break;
    case QProcess::ReadError:
        QMessageBox::information(0, "Tip", "ReadError");
        break;
    case QProcess::UnknownError:
        QMessageBox::information(0, "Tip", "UnknownError");
        break;
    default:
        QMessageBox::information(0, "Tip", "UnknownError");
        break;
    }
}
#endif

QStringList ManagementTool::getIP(QString localHost)
{
    QStringList ipAddr;
    QHostInfo info = QHostInfo::fromName(localHost);
    info.addresses();   //QHostInfo的address函数获取本机ip地址

    /*如果存在多条ip地址ipv4和ipv6*/
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol) //只取ipv4协议的地址
        {
            ipAddr.append(address.toString());
        }
    }
    return ipAddr;
}


/*结束 外部APP 任务*/
void terminateApp(QString app)
{
    qDebug()<< app;
    QProcess process;
    process.execute(app);
    process.close();
}

/**********************************************************************
* 功    能：提示信息
* 输    入：
* 输    出：
* 作    者：刘卫明
* 编写日期：2017.12.26
***********************************************************************/
void ManagementTool::on_aboutPushButton_clicked()
{
    QMessageBox::about(NULL, "版权所有", "Copyright (c) 2018, 青岛特来电新能源有限公司, All rights reserved." \
                                     "\n\n集中控制器智能远程管理系统 V1.0.0\n\n智能充电研发中心-集控开发部");
}


void ManagementTool::on_aboutAction_triggered()
{
    QMessageBox::about(NULL, "版权所有", "Copyright (c) 2018, 青岛特来电新能源有限公司, All rights reserved." \
                                     "\n\n集中控制器远程管理系统 V1.0.0\n\n智能充电研发中心-集控开发部");
}

/*使用说明书*/
void ManagementTool::on_helpAction_triggered()
{
    QString dirPath = QCoreApplication::applicationDirPath();
    QString fileName = dirPath + "/thirdApp/config/使用说明书.html";
    QDesktopServices::openUrl ( QUrl::fromLocalFile(fileName) );
}

void ManagementTool::on_exitAction_triggered()
{
    this->close();
}


void ManagementTool::onOneUserDataParse(QByteArray tmpData)
{
    disconnect(this, SIGNAL(transmitSignal(QByteArray)), this, SLOT(onOneUserDataParse(QByteArray)));
    qDebug() << "获取当个数据用户：" <<tmpData;

    if (!dataJson->jsonParseData(tmpData, "password").isEmpty())
    {
        ManagementTool::stUserInfo.user =  dataJson->jsonParseData(tmpData,"password").first();
        ManagementTool::stUserInfo.password =  dataJson->jsonParseData(tmpData,"password").first();
        ManagementTool::stUserInfo.userMail =  dataJson->jsonParseData(tmpData,"user_mail").first();
        ManagementTool::stUserInfo.userPhone =  dataJson->jsonParseData(tmpData,"user_phone").first();
        ManagementTool::stUserInfo.userEnable =  dataJson->jsonParseData(tmpData,"password").first();
        ManagementTool::stUserInfo.userStartDate =  dataJson->jsonParseData(tmpData,"user_start_date").first();
        ManagementTool::stUserInfo.userEndDate =  dataJson->jsonParseData(tmpData,"user_end_date").first();
    }

    if (ManagementTool::stUserInfo.password == "123456")
    {
        accountDlg::modifyFlag = true;
        if (regDlg)
        {
            QMessageBox::warning(this->regDlg, "提示", "当前密码为初始密码，请修改密码！");
            /*默认显示设备管理界面*/
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
        else
        {
            QMessageBox::warning(this, "提示", "当前密码为初始密码，请修改密码！");

            regDlg = new registerDlg;
            regDlg->exec();

            /*默认显示设备管理界面*/
            if (funcWidget)
            {
                layout->removeWidget(funcWidget);
                delete funcWidget;
                funcWidget = NULL;
            }

            funcWidget = new deviceDlg(ui->spaceFrame);
            layout->addWidget(funcWidget);
            funcWidget->show();

            return;
        }
    }
    else
    {
        /*默认显示设备管理界面*/
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
}

void ManagementTool::onReplyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();
        QMessageBox::warning(this, "提示", "连接服务器超时，请重试！");
        return;
    }
    QByteArray tempBuf = reply->readAll();
    emit transmitSignal(tempBuf);
}

/*根据进程名称找到PID */
bool findApp(const QString& exe)
{
    //根据进程名称找到PID
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        return false;
    }

//    DWORD  dwPid = -1;
    while (Process32Next(hProcessSnap, &pe32))
    {
        //将WCHAR转成const char*
        int iLn = WideCharToMultiByte (CP_UTF8, 0, const_cast<LPWSTR> (pe32.szExeFile),
                                       static_cast<int>(sizeof(pe32.szExeFile)), NULL, 0, NULL, NULL);
        std::string result (iLn, 0);
        WideCharToMultiByte (CP_UTF8, 0, pe32.szExeFile, static_cast<int>(sizeof(pe32.szExeFile)),
                             const_cast<LPSTR> (result.c_str()), iLn, NULL, NULL);
        if (0 == strcmp(exe.toStdString().c_str(), result.c_str ()))
        {
//            dwPid = pe32.th32ProcessID;
            return true;
        }
    }

    CloseHandle(hProcessSnap);
//    qDebug()<<"进程id:"<< dwPid;
    return false;
}

bool ManagementTool::nativeEvent(const QByteArray &eventType, void *message, long *result)
{

}
