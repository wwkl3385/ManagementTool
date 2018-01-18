/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： managementtool.cpp
* 描    述： 智能运维主界面，连接vpn,打开putty,打开winscp
* 修改记录：
* 			V0.9，2018-01-12，new，刘卫明
*
***********************************************************************/
#include "managementtool.h"
#include "ui_managementtool.h"
#include "logon/logon.h"
#include "accountManage/accountdlg.h"
#include "deviceManage/devicedlg.h"
#include "recordExport/recorddlg.h"
#include <QScrollBar>
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
#include <QDebug>
#include <WinSock2.h>
#include <iphlpapi.h>

static int n = 0;       //记录创建的次数
static int secTmp = 0;  //连接vpn 超时时间
static int sec = 0;     //vpn 连接时间 s
static int min = 0;     //vpn 连接时间 min
bool connectVpnSuccessFlag = false;  //vpn连接成功标志  false:未连接

ManagementTool::ManagementTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ManagementTool)
{
    ui->setupUi(this);
    qDebug() <<"managementDlg创建的次数："<< ++n;

    setWindowTitle("智能集中控制器远程管理系统");
    setWindowIcon(QIcon("teld.ico")); //设置软件图标
    this->resize(1500, 800);          //设置初始尺寸
    funcWidget = NULL;
    vpnClickedConnect = false;
    vpnClicked = false;

    ui->connectVpnProgressBar->hide();
    ui->connectVpnProgressBar->setMaximum(0);
    ui->connectVpnProgressBar->setMinimum(0);
    ui->connectVpnLabel->hide();
    ui->connectVpnLabel->setStyleSheet("QLabel{color:black;font:11pt;font:bold}");

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
    ui->puttyPushButtonClose->hide();
    ui->winscpPushButton->hide();
    ui->winscpPushButtonClose->hide();
    ui->connectVPNPushButton->setText("连接");

    ui->recordPushButton->hide();

    /*管理员和普通用户*/
    if(logon::userType == 1)
    {
        ui->nameLabel->setText("管理员");
        ui->manageToolBox->setCurrentIndex(0);
    }
    else
    {
        ui->nameLabel->setText("普通用户");
        ui->AdminPage->setVisible(false);
        ui->manageToolBox->removeItem(1);
    }
}

ManagementTool::~ManagementTool()
{
    if (funcWidget)
    {
        layout->removeWidget(funcWidget);
        delete funcWidget;
        funcWidget = NULL;
    }

    qDebug() <<"managementDlg创建的次数："<< --n;
    qDebug()<< "销毁managementToolDlg";

    vpnProcess->close();
    puttyProcess->close();
    winscpProcess->close();
    delete vpnProcess;
    delete puttyProcess;
    delete winscpProcess;

    /*强制 结束任务*/
    terminateApp("taskkill /im openvpn.exe /f");
    terminateApp("taskkill /im putty.exe /f");
    terminateApp("taskkill /im winscp.exe /f");
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

    if (!ui->connectVpnProgressBar->isHidden())
        ui->connectVPNPushButton->setDisabled(true);
    else
        ui->connectVPNPushButton->setEnabled(true);

    if (vpnClickedConnect == true && connectVpnSuccessFlag != true)
    {
        ++secTmp;
        if (secTmp > 30)
        {
            secTmp = 0;
            vpnClickedConnect = false;  //断开
            ui->connectVpnLabel->setText("连接失败，请重新连接...");
            ui->connectVpnLabel->setStyleSheet("QLabel{color:red;font:12pt;font:bold}");
            ui->connectVpnProgressBar->hide();
            ui->connectVpnProgressBar->setEnabled(true);
        }
    }

    /*是否连接vpn,连接状态*/
    if(vpnClicked != true)
    {
        oldIp  = getIP(QHostInfo::localHostName());
        qDebug()<<"odip.size" <<oldIp.size();
    }

    if (vpnClickedConnect == true &&  newIp.size() > oldIp.size())
    {
        qDebug()<<"newip.size" <<newIp.size();
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

        ui->connectVpnProgressBar->hide();
        ui->connectVpnLabel->setText("正在连接vpn,请稍后...");
        ui->connectVpnLabel->hide();
        ui->connectVpnLabel->setStyleSheet("QLabel{color:black;font:12pt;font:bold}");

        connectVpnSuccessFlag = true;
        ui->connectVPNPushButton->setText("断开");
        ui->connectVPNPushButton->setEnabled(true);
        ui->connectStatusLabel->setText("连接成功");
        ui->connectStatusLabel->setStyleSheet("QLabel{color:yellow;font:15px;font:bold;}");

        if (halfSec % 2 == 1)
            ui->flashLabel->setStyleSheet("QLabel{background-color:yellow;border-radius:6px;}");
        else
            ui->flashLabel->setStyleSheet("QLabel{background-color:green; border-radius:6px;}");

        strTime.sprintf("%d分：%d秒", min, sec);
        ui->timeLable->setText(strTime);
        ui->ipAddrLabel->setText(newIp.last());
    }
    else
    {
        ui->connectVPNPushButton->setText("连接");
        ui->connectStatusLabel->setText("未连接");
        ui->timeLable->setText("");
        connectVpnSuccessFlag = false;
    }

    if (!deviceDlg::deviceIp.isEmpty())
    {
        ui->puttyPushButton->show();
        ui->puttyPushButtonClose->show();
        ui->winscpPushButton->show();
        ui->winscpPushButtonClose->show();
    }
    else
    {
        ui->puttyPushButton->hide();
        ui->puttyPushButtonClose->hide();
        ui->winscpPushButton->hide();
        ui->winscpPushButtonClose->hide();
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
    QString app = "taskkill /im openvpn.exe /f";
    terminateApp(app);

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

    if (connectVpnSuccessFlag == false && vpnClickedConnect != true)
    {
        vpnClicked = true;
        vpnClickedConnect = true; //连接
        connectVpnSuccessFlag = false;
        qDebug() <<"vpn连接状态："<< connectVpnSuccessFlag ;

        ui->connectVpnProgressBar->show();
        ui->connectVpnLabel->show();
        ui->connectVpnLabel->setText("正在连接vpn,请稍后...");
        ui->connectVpnLabel->setStyleSheet("QLabel{color:black;font:12pt;font:bold}");

        ui->connectStatusLabel->setText("连接中...");
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
        connectVpnSuccessFlag  = false;
        vpnClicked = false;

        qDebug() <<"断开 clicked  vpn连接状态："<< connectVpnSuccessFlag ;
        vpnClickedConnect = false;  //断开
        ui->ipAddrLabel->setText("");
    }
}

void ManagementTool::on_puttyPushButton_clicked()
{
    QString program = QCoreApplication::applicationDirPath() + "./thirdApp/bin/putty.exe";

    QStringList arguments;
    QString ip = deviceDlg::deviceIp;

    arguments<< "-telnet" << ip << "23";

    qDebug() << arguments;

    puttyProcess->setProcessChannelMode(QProcess::SeparateChannels);
    puttyProcess->setReadChannel(QProcess::StandardOutput);
    puttyProcess->start(program, arguments, QIODevice::ReadWrite);

    //    connect(puttyProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onProcessError(QProcess::ProcessError)));
}

void ManagementTool::on_winscpPushButton_clicked()
{
    QString program = QCoreApplication::applicationDirPath() + "./thirdApp/bin/WinScp.exe";
    QStringList arguments;
    QString ip = deviceDlg::deviceIp;

    arguments << QString("%1://%2:%3@%4:%5").arg("ftp").arg("EM9280").arg("root").arg(ip).arg(21);

    winscpProcess->setProcessChannelMode(QProcess::SeparateChannels);
    winscpProcess->setReadChannel(QProcess::StandardOutput);
    winscpProcess->start(program, arguments, QIODevice::ReadWrite);
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

void ManagementTool::on_puttyPushButtonClose_clicked()
{
    /*结束 putty 任务*/
    QString app = "taskkill /im putty.exe /f";
    terminateApp(app);
}

void ManagementTool::on_winscpPushButtonClose_clicked()
{
    QString app = "taskkill /im winscp.exe /f";
    terminateApp(app);
}

/*结束 外部APP 任务*/
void ManagementTool::terminateApp(QString app)
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
    QMessageBox::about(NULL, "版权所有", "Copyright (c) 2017, 青岛特来电新能源有限公司, All rights reserved." \
                                     "\n\n集中控制器远程管理系统 V1.0.0\n\n智能充电中心");
}
