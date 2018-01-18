/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： loadon.cpp
* 描    述： 登录对话框，检测软件更新
* 修改记录：
* 			V0.9，2018-01-09，new，刘卫明
*
***********************************************************************/
#include "ui_logon.h"
#include "logon/logon.h"
#include "loading/loadingdlg.h"
#include "management/managementtool.h"
#include "downloadManage/downloadmanager.h"
#include <QBitmap>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QNetworkReply>
#include <QProgressDialog>

#define UNIT_KB 1024            //KB
#define UNIT_MB 1024*1024       //MB
#define UNIT_GB 1024*1024*1024  //GB
#define TIME_INTERVAL 300       //0.3s

#define  LOGIN_URL          "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/login"                   //登录url
#define  UPDATE_URL         "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/query_update"            //更新url
#define  UPDATE_URL_INDEX   "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/download?filename="      //下载url

QString version = "1.0.0";    //版本号
bool signalFlag = true;       //登录(false)，升级(true)，请求标志
unsigned logon::userType = 0; //登录类型--1：管理员，--2：普通用户
QString logon::userNameInfo;  //登录名
QString logon::userPassInfo;  //密码
static int n = 0;             //记录创建的次数

logon::logon(QWidget *parent) :
    QDialog(parent),
    dataJson(new jsonManage),
    dataHttp(new httpManage),
    pLoadDlg(new loadingDlg(this)),
    ui(new Ui::logon)
{
    ui->setupUi(this);

    qDebug() <<"logonDlg创建的次数："<< ++n;

    m_downloadManager = NULL;
    m_url = "";
    m_timeInterval = 0;
    m_currentDownload = 0;
    m_intervalDownload = 0;

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );
    pLoadDlg->hide();

    progressDlg = new QProgressDialog(this);

    /*进度条设置样式*/
    progressDlg->setCancelButtonText(tr("取消"));
    progressDlg->setModal(true);
    progressDlg->setStyleSheet("QProgressBar{border:1px solid #FFFFFF;"
                           "height:30;"
                           "font:15pt;font:bold;"
                           "background:wheat;"
                           "text-align:center;"
                           "color:white;"
                           "border-radius:10px;}"
                           "QProgressBar::chunk{""border-radius:3px;"    // 斑马线圆角
                           "border:0.5px " "solid gold;" // 黑边，默认无边
                           "background-color:tomato;} QLabel{font:16px;font:bold}");

    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setMinimumDuration(5);
    progressDlg->setWindowTitle(tr("更新"));
    progressDlg->setLabelText(tr("正在下载应用程序，请稍后..."));
    progressDlg->setCancelButtonText(tr("取消"));
    progressDlg->setMinimum(0);
    progressDlg->setMaximum(0);
    progressDlg->close();
    progressDlg->reset();

    /*圆角窗口*/
    QPixmap pixmap(":/new/background/tgood.png");
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    setPalette(palette);
    resize(pixmap.size());
    setMask(pixmap.mask());
    qDebug() << "x坐标位置：" << this->pos();

    /*绑定enter键*/
    connect(ui->passwordLineEdit, SIGNAL(returnPressed()), ui->logonPushButton, SIGNAL(clicked(bool)), Qt::UniqueConnection);

    /*连接服务器,检测是否更新*/
    dataJson->updateObj= dataJson->jsonPackUpdate(version);
    dataHttp->httpPost(UPDATE_URL, dataJson->updateObj); //http更新请求
    qDebug() << "更新请求:" << dataJson->updateObj;

    /*http请求*/
    connect(this->dataHttp->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(this, SIGNAL(transmitUpdateSignal(QByteArray)), this, SLOT(updateDataParse(QByteArray)));
    connect(this, SIGNAL(transmitSignal(QByteArray)), this, SLOT(loginDataParse(QByteArray)));
}

logon::~logon()
{
    delete ui;
    delete dataJson;
    delete dataHttp;
    delete progressDlg;

    qDebug()<< "销毁 logonDlg";
    qDebug() <<"logonDlg还存在的个数："<< --n;
}

/*窗口移动操作*/
void logon::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_Drag = true;
        m_DragPosition = event->globalPos() - this->pos();
        event->accept();
    }
}

void logon::mouseMoveEvent(QMouseEvent *event)
{
    if (m_Drag && (event->buttons() && Qt::LeftButton))
    {
        move(event->globalPos() - m_DragPosition);
        event->accept();
    }
}

void logon::mouseReleaseEvent(QMouseEvent *)
{
    m_Drag = false;
    qDebug() << "坐标位置：" << this->pos();
    qDebug() << "x位置：" << this->pos().x();
}

void logon::on_logonPushButton_clicked()
{

    if (ui->userLineEdit->text().isEmpty() || ui->passwordLineEdit->text().isEmpty())
    {
       QMessageBox::critical(this, "错误", "请输入正确的用户名和密码!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
       return;
    }

    signalFlag = false;  //登录(false)，升级(true)，请求标志

    /*连接服务器,校验用户名和密码*/
    dataJson->loginObj= dataJson->jsonPackLogin(ui->userLineEdit->text(), ui->passwordLineEdit->text()); //登录
    dataHttp->httpPost(LOGIN_URL, dataJson->loginObj); //http登录请求
    qDebug() << "登录信息帧：" << dataJson->loginObj;

    pLoadDlg->exec(); //动态登录
}

void logon::replyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();

        pLoadDlg->close(); //关闭动态登录显示窗口
        QMessageBox::warning(this, "提示", "连接服务器超时，请重试！");

        if(pLoadDlg->isActiveWindow())
            delete pLoadDlg;

        return;
    }
    QByteArray tempBuf = reply->readAll();

    if (signalFlag == true)
        emit transmitUpdateSignal(tempBuf);  //更新
    else
        emit transmitSignal(tempBuf);
}

void logon::loginDataParse(QByteArray tmpData)
{
    qDebug() <<"传递返回的值："<< tmpData;
    int status = 0;
    if (!dataJson->jsonParseData(tmpData, "status").isEmpty())
        status = dataJson->jsonParseData(tmpData,"status").first().toInt();

    qDebug() <<"status返回解析值："<< status ;

    if (status == 1) //登录成功
    {
        pLoadDlg->close(); //关闭动态登录显示窗口
        delete pLoadDlg;

        /*登录校验*/
        int userType = dataJson->jsonParseData(tmpData,"user_type").first().toInt(); //管理员用户

        logon::userNameInfo = ui->userLineEdit->text();
        logon::userPassInfo = ui->passwordLineEdit->text();  //密码
        logon::userType = userType; //管理员&普通用户

        qDebug() <<"userNameInfo返回解析值："<< logon::userNameInfo;
        qDebug() <<"userType返回解析值："<< userType << "管理员";

        this->accept();
    }
    else
    {
        pLoadDlg->close(); //关闭动态登录显示窗口
        QMessageBox::critical(this, "错误", "请输入正确的用户名和密码!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }
}

void logon::updateDataParse(QByteArray tmpData)
{
    disconnect(this, SIGNAL(transmitUpdateSignal(QByteArray)), this, SLOT(updateDataParse(QByteArray)));

    qDebug() <<"更新返回的数据："<< tmpData;
    int status = dataJson->jsonParseData(tmpData,"status").first().toInt();
    qDebug() <<"status返回解析值："<< status ;

    if (status == 1)
    {
        QMessageBox Msg(QMessageBox::Question, QString("更新提示"), QString("有新的应用，请更新到最新应用!"));
        QAbstractButton *pYesBtn = (QAbstractButton*)Msg.addButton(QString("确定"), QMessageBox::YesRole);
        QAbstractButton *pNoBtn = (QAbstractButton*)Msg.addButton(QString("取消"), QMessageBox::NoRole);
        Msg.exec();

        if (Msg.clickedButton() != pNoBtn)
        {
            /* 开始下载  */
            QString strFileName = dataJson->jsonParseData(tmpData, "filename").first();
            QString downloadUrl = UPDATE_URL_INDEX + strFileName;
            qDebug() << "下载链接：" << downloadUrl;

            m_url = downloadUrl; // 获取下载链接;
            QString fileName= QFileDialog::getSaveFileName(this, tr("保存升级文件"),
                                                           "智能集中器远程管理工具", tr("*.tar.gz;; *.zip;;")); //选择路径
            qDebug() << "下载文件名字：" <<fileName;

            if (m_downloadManager == NULL)
            {
                m_downloadManager = new downloadManager(this);
                connect(m_downloadManager , SIGNAL(signalDownloadProcess(qint64, qint64)), this, SLOT(onDownloadProcess(qint64, qint64)));
                connect(m_downloadManager, SIGNAL(signalReplyFinished(int)), this, SLOT(onReplyFinished(int)));
            }

            /*这里先获取到m_downloadManager中的url与当前的m_url 对比，如果url变了需要重置参数,防止文件下载不全*/
            QString url = m_downloadManager->getDownloadUrl(m_url);
            if (url != m_url)
            {
                m_downloadManager->reset();
            }

            m_downloadManager->downloadFile(m_url, fileName);
            m_timeRecord.start();
            m_timeInterval = 0;
            qDebug() << "正在下载";

            delete pYesBtn;
            delete pNoBtn;
        }
        else
        {
            delete pYesBtn;
            delete pNoBtn;
            return;
        }
    }
}

/*更新下载进度*/
void logon::onDownloadProcess(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug() << "文件总大小：" <<bytesTotal;
    qDebug() << "文件已传输：" <<bytesReceived;

    progressDlg->setMaximum(bytesTotal);
    progressDlg->setValue(bytesReceived);

    if (progressDlg->wasCanceled())
    {
        progressDlg->reset();
        progressDlg->hide();
        m_downloadManager->closeDownload();
        return;
    }

    m_intervalDownload += bytesReceived - m_currentDownload; // m_intervalDownload 为下次计算速度之前的下载字节数;
    m_currentDownload = bytesReceived;

    uint timeNow = m_timeRecord.elapsed();

    /*超过0.3s更新计算一次速度*/
    if (timeNow - m_timeInterval > TIME_INTERVAL)
    {
        qint64 ispeed = m_intervalDownload * 1000 / (timeNow - m_timeInterval);
        QString strSpeed = transformUnit(ispeed, true);
        QString speedInfo = "下载速度："+ strSpeed ;

        /*剩余时间*/
        qint64 timeRemain = (bytesTotal - bytesReceived) / ispeed;
        QString strRemainTime = QString::number(timeRemain, 10);

        QString timeInfo= "剩余时间："+ strRemainTime + "秒";
        QString allInfo = speedInfo + "\r\n" + timeInfo;

        progressDlg->setLabelText(allInfo);

        m_intervalDownload = 0;
        m_timeInterval = timeNow;
    }
}

/*下载完成*/
void logon::onReplyFinished(int statusCode)
{
    /*根据状态码判断当前下载是否出错*/
    qDebug() <<  "状态码为："<<statusCode;
    if (statusCode > 200 && statusCode < 400)
    {
        qDebug() << "Download Failed";
        QMessageBox::information(this, "提示", "下载失败!");
    }
    else if( statusCode == 200)
    {
        qDebug() << "Download Success";
        QMessageBox::information(this, "提示", "下载完成!");
    }
}

/*转换单位*/
QString logon::transformUnit(qint64 bytes , bool isSpeed)
{
    QString strUnit = " B";
    if (bytes <= 0)
    {
        bytes = 0;
    }
    else if (bytes < UNIT_KB)
    {
    }
    else if (bytes < UNIT_MB)
    {
        bytes /= UNIT_KB;
        strUnit = " KB";
    }
    else if (bytes < UNIT_GB)
    {
        bytes /= UNIT_MB;
        strUnit = " MB";
    }
    else if (bytes > UNIT_GB)
    {
        bytes /= UNIT_GB;
        strUnit = " GB";
    }

    if (isSpeed)
    {
        strUnit += "/S";
    }
    return QString("%1%2").arg(bytes).arg(strUnit);
}

/*转换时间*/
QString logon::transformTime(qint64 seconds)
{
    QString strValue;
    QString strSpacing(" ");
    if (seconds <= 0)
    {
        strValue = QString("%1s").arg(0);
    }
    else if (seconds < 60)
    {
        strValue = QString("%1s").arg(seconds);
    }
    else if (seconds < 60 * 60)
    {
        int nMinute = seconds / 60;
        int nSecond = seconds - nMinute * 60;

        strValue = QString("%1m").arg(nMinute);

        if (nSecond > 0)
            strValue += strSpacing + QString("%1s").arg(nSecond);
    }
    else if (seconds < 60 * 60 * 24)
    {
        int nHour = seconds / (60 * 60);
        int nMinute = (seconds - nHour * 60 * 60) / 60;
        int nSecond = seconds - nHour * 60 * 60 - nMinute * 60;

        strValue = QString("%1h").arg(nHour);

        if (nMinute > 0)
            strValue += strSpacing + QString("%1m").arg(nMinute);

        if (nSecond > 0)
            strValue += strSpacing + QString("%1s").arg(nSecond);
    }
    else
    {
        int nDay = seconds / (60 * 60 * 24);
        int nHour = (seconds - nDay * 60 * 60 * 24) / (60 * 60);
        int nMinute = (seconds - nDay * 60 * 60 * 24 - nHour * 60 * 60) / 60;
        int nSecond = seconds - nDay * 60 * 60 * 24 - nHour * 60 * 60 - nMinute * 60;

        strValue = QString("%1d").arg(nDay);

        if (nHour > 0)
            strValue += strSpacing + QString("%1h").arg(nHour);

        if (nMinute > 0)
            strValue += strSpacing + QString("%1m").arg(nMinute);

        if (nSecond > 0)
            strValue += strSpacing + QString("%1s").arg(nSecond);
    }
    return strValue;
}
