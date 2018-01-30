/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： devicedlg.cpp
* 描    述： 设备管理-- 管理员查询所有设备，查询单个设备
* 修改记录：
* 			V0.9，2018-01-08，new，刘卫明
*
***********************************************************************/
#include "devicedlg.h"
#include "logon/logon.h"
#include "ui_devicedlg.h"
#include "loading/loadingdlg.h"
#include <QScrollBar>
#include <QMessageBox>
#include <QNetworkReply>

#define  QUERY_ALL_INFO_URL  "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/query_all_connect_info"     //所有在线终端 信息
#define  QUERY_ALL_NUM_URL   "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/query_total_connect"        //所有在线终端 数量
#define  QUERY_INFO_URL      "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/query_connect_info"         //集控登录连接信息
#define  NUMPAGE             30    //每页显示条数

const QString signalInitensity = "--dB"; //无信号强度，默认为--dB
int pageNum = 0;     // 页数
int remainsRow = 0;  // 剩余行数
int indexPage = 1;   // 页的索引
QString deviceDlg::deviceIp; //被选择的设备ip地址

deviceDlg::deviceDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::deviceDlg),
    dataJson(new jsonManage),
    dataHttp(new httpManage),
    pLoadDlg(new loadingDlg(this))
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

    signalFlag = 0;  //发送数量信号

    ui->pageFrame->hide();
    pTimer = new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(onTimeRelay()));
    pTimer->start(1000);     //1s

    /*tableWidget 初始化设置*/
    ui->listTableWidget->setColumnCount(8);                   //设置列数
    ui->listTableWidget->horizontalHeader()->setDefaultSectionSize(150);
    ui->listTableWidget->horizontalHeader()->setVisible(true);//表头可见

    /*搜索按钮 样式设置*/
    ui->searchLineEdit->setStyleSheet("QLineEdit{background-color:white; color: steelblue; border-radius:5px; "
                                      "font:12pt;  border: 2px groove #d3dadd;}"\
                                      "QLineEdit:hover{background-color:#cce5f4; color: black;}"\
                                      "QLineEdit:focus{background-color:white; border-style: inset;}");

    /*设置表头内容*/
    QStringList header;
    header << "编号"  << "   mac地址   "<< "  站地址1  "<< "  站地址2  "<< "  站地址3  " <<  " 设备vpn地址 " << "设备上线时间" << "信号强度";
    ui->listTableWidget->setHorizontalHeaderLabels(header);

    /*设置表的行排列显示*/
    ui->listTableWidget->horizontalHeader()->resizeSection(0, 50); //设置表头第一列(编号)的宽度为50；
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);

    /*设置表头字体加粗*/
    QFont font = this->font();
    font.setBold(true);
    ui->listTableWidget->horizontalHeader()->setFont(font);

    /*设置表的属性*/
    ui->listTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:#58baf2; "
                                                           "color:white;font:12pt;font:bold;}"); //设置表头背景色
    ui->listTableWidget->horizontalHeader()->setHighlightSections(false);     //点击表时，不对表头行高亮
    ui->listTableWidget->horizontalHeader()->setStretchLastSection(true);     //设置充满表宽度
    ui->listTableWidget->verticalHeader()->setDefaultSectionSize(30);         //设置行高
    ui->listTableWidget->setFrameShape(QFrame::NoFrame);                      //设置无边框
    ui->listTableWidget->setShowGrid(false);                                  //设置不显示格子线
    ui->listTableWidget->verticalHeader()->setVisible(false);                 //设置垂直头不可见
    ui->listTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->listTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置不可编辑
    ui->listTableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);   //设置双击可编辑
    ui->listTableWidget->horizontalHeader()->setFixedHeight(28);              //设置表头的高度
    ui->listTableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //设置只能选择一行，不能选择多行
    ui->listTableWidget->setFocusPolicy(Qt::NoFocus);  //去除虚线

    /*设置table的滚动条*/
    //    ui->listTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->listTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    /*设置水平、垂直滚动条样式*/
    ui->listTableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:20px;}"
                                                              "QScrollBar::handle{background:lightgray; border:2px solid transparent;"
                                                              " border-radius:5px;}"
                                                              "QScrollBar::handle:hover{background:gray;}"
                                                              "QScrollBar::sub-line{background:transparent;}"
                                                              "QScrollBar::add-line{background:transparent;}");
    ui->listTableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
                                                            "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                                            "QScrollBar::handle:hover{background:gray;}"
                                                            "QScrollBar::sub-line{background:transparent;}"
                                                            "QScrollBar::add-line{background:transparent;}");

    /*管理员和普通用户*/
    if(logon::userType == 1)
    {
        /* 管理员---查询所有的在线终端--数量*/
        dataJson->connectNumberObj= dataJson->jsonPackQueryConnectNumber();
        dataHttp->httpPost(QUERY_ALL_NUM_URL, dataJson->connectNumberObj); //http请求终端数量
        connect(this->dataHttp->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));

        pLoadDlg->show();

        /* 管理员---查询所有的在线终端--数据*/
        dataJson->connectAllInfoObj= dataJson->jsonPackQueryConnectAllInfo();
        dataHttp->httpPost(QUERY_ALL_INFO_URL, dataJson->connectAllInfoObj); //http请求

        /*http请求 数量*/
        connect(this, SIGNAL(transmitNumSignal(QByteArray)), this, SLOT(onConnectTotalNumDataParse(QByteArray)));

        /*http请求 集控所有信息*/
        connect(this, SIGNAL(transmitAllInfoSignal(QByteArray)), this, SLOT(onConnectAllInfoDataParse(QByteArray)));

        /*http请求 单个集控连接信息*/
        connect(this, SIGNAL(transmitInfoSignal(QByteArray)), this, SLOT(onConnectInfoDataParse(QByteArray)));
    }
    else
    {
        ui->deviceLabel->hide();
        ui->numberLabel->hide();
        ui->unitLabel->hide();
        ui->pageFrame->hide();
        ui->dotHorizontalSpacer->minimumSize();

        /*http请求 单个集控连接信息*/
        connect(this->dataHttp->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
        connect(this, SIGNAL(transmitInfoSignal(QByteArray)), this, SLOT(onConnectInfoDataParse(QByteArray)));
    }
}

deviceDlg::~deviceDlg()
{
    delete dataJson;
    delete dataHttp;
    delete pLoadDlg;
    deviceIp.clear();

    delete ui;
    qDebug()<< "销毁deviceDlg";
}

void deviceDlg::keyPressEvent(QKeyEvent *event)
{
    /* 响应Esc键以退出程序 */
    if (event->key() == Qt::Key_Escape)
    {
        showMaximized();  //窗口最大化
    }

    /*绑定enter*/
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        on_searchPushButton_clicked();
    }
}

void deviceDlg::onReplyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();
        QMessageBox::warning(this, "提示", "连接服务器超时，请重试！");
        return;
    }

    QByteArray tempBuf = reply->readAll();

    switch (signalFlag)
    {
    case 0:
    {
        emit transmitNumSignal(tempBuf);
        break;
    }
    case 1:
    {
        emit transmitAllInfoSignal(tempBuf);
        break;
    }
    case 2:
    {
        emit transmitInfoSignal(tempBuf);
        break;
    }
    default:
        break;
    }
}

void deviceDlg::onConnectTotalNumDataParse(QByteArray tmpData)
{
    if (!dataJson->jsonParseData(tmpData, "total_num").isEmpty())
    {
        ui->numberLabel->setText(dataJson->jsonParseData(tmpData, "total_num").first());
    }

    signalFlag = 1;     //集控所有信息
}

#if 0
/*查询全部集控信息*/
void deviceDlg::on_searchAllPushButton_clicked()
{

    signalFlag = 1;     //集控所有信息

    /* 管理员---查询所有的在线终端--数据*/
    dataJson->connectAllInfoObj= dataJson->jsonPackQueryConnectAllInfo();
    dataHttp->httpPost(QUERY_ALL_INFO_URL, dataJson->connectAllInfoObj); //http请求
}
#endif

/*所有集控数据解析*/
void deviceDlg::onConnectAllInfoDataParse(QByteArray tmpData)
{
    /* 删除所有设备信息*/
    ui->listTableWidget->clearContents();

    startTimeList.clear();
    userIdList.clear();
    remoteIpList.clear();

    startTimeList = dataJson->jsonParseData(tmpData,"log_start_time");//解析
    userIdList    = dataJson->jsonParseData(tmpData,"user_id");       //解析
    remoteIpList  = dataJson->jsonParseData(tmpData,"log_remote_ip"); //解析
    int num = remoteIpList.size();
    if (num < 1)
        return;

    pageNum = num / NUMPAGE;  //每页显示NUMPAGE行
    remainsRow = num % NUMPAGE;
    indexPage  = 1;

    QString strNum = QString::number(NUMPAGE, 10);
    ui->numLabel->setText(strNum);  //每页显示条数

    ui->currentPageLineEdit->setText("1");
    if (remainsRow != 0)
        ui->allPageLabel->setText(QString::number(pageNum+1, 10));
    else
        ui->allPageLabel->setText(QString::number(pageNum, 10));

    /* 删除所有账户信息*/
    ui->listTableWidget->clearContents();
    ui->listTableWidget->setRowCount(0); //删除行
    for (int i = 0; i < NUMPAGE; i++)
    {
        QApplication::processEvents(); //处理大数据，防gui假死

        QString userId = userIdList.at(i);
        QList<QString> idList = userId.split('-');
        if (idList.size() < 4)  //站地址不足3个，补空
        {
            for (int i=0; i < (4 - idList.size()); i++)
                idList.append("");
        }

        int row_count = ui->listTableWidget->rowCount(); //获取表单行数
        ui->listTableWidget->insertRow(row_count);       //插入新行

        ui->listTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
        ui->listTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 1, new QTableWidgetItem(idList.value(0)));
        ui->listTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 2, new QTableWidgetItem(idList.value(1)));
        ui->listTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 3, new QTableWidgetItem(idList.value(2)));
        ui->listTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 4, new QTableWidgetItem(idList.value(3)));
        ui->listTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 5, new QTableWidgetItem(remoteIpList.value(i)));
        ui->listTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 6, new QTableWidgetItem(startTimeList.value(i)));
        ui->listTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 7, new QTableWidgetItem(signalInitensity));
        ui->listTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中

        if (!pLoadDlg->isVisible())
            return;
    }

    ui->pageFrame->show();
    pLoadDlg->hide();
}

/*search*/
void deviceDlg::on_searchPushButton_clicked()
{
    connect(this, SIGNAL(transmitInfoSignal(QByteArray)), this, SLOT(onConnectInfoDataParse(QByteArray)));
    signalFlag = 2;

    if (ui->searchLineEdit->text().isEmpty())
    {
        QMessageBox::information(NULL, "提示", "输入内容为空！！" );
        return;
    }

    /* 查询集控连接信息*/
    dataJson->connectInfoObj= dataJson->jsonPackQueryConnectInfo(ui->searchLineEdit->text());
    dataHttp->httpPost(QUERY_INFO_URL, dataJson->connectInfoObj); //http请求终端信息

    pLoadDlg->show();

    /* 删除所有设备信息*/
    ui->listTableWidget->clearContents();
    startTimeList.clear();
    userIdList.clear();
    remoteIpList.clear();
    ui->pageFrame->hide();
}

void deviceDlg::onConnectInfoDataParse(QByteArray tmpData)
{
    disconnect(this, SIGNAL(transmitInfoSignal(QByteArray)), this, SLOT(onConnectInfoDataParse(QByteArray)));
    indexPage  = 1;
    deviceDlg::deviceIp = "";

    /* 删除所有设备信息*/
    ui->listTableWidget->clearContents();
    ui->listTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    startTimeList = dataJson->jsonParseData(tmpData,"log_start_time");//解析
    userIdList    = dataJson->jsonParseData(tmpData,"user_id");       //解析
    remoteIpList  = dataJson->jsonParseData(tmpData,"log_remote_ip"); //解析

    int num = remoteIpList.size();

    if (num < 1)
    {
        pLoadDlg->hide();
        QMessageBox::information(NULL, "提示", "没有找到相匹配的数据！！" );
        return;
    }
    if (num > 5) //限定显示条数
        num = 5;

    /* 删除所有账户信息*/
    ui->listTableWidget->clearContents();
    ui->listTableWidget->setRowCount(0); //删除行
    for (int i = 0; i < num; i++)
    {
        QApplication::processEvents(); //处理大数据，防gui假死

        QString userId = userIdList.at(i);
        QList<QString> idList = userId.split('-');

        if (idList.size() < 4)
        {
            for (int i=0; i < (4 - idList.size()); i++)
                idList.append("");
        }


        int row_count = ui->listTableWidget->rowCount(); //获取表单行数
        ui->listTableWidget->insertRow(row_count);       //插入新行
        ui->listTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
        ui->listTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 1, new QTableWidgetItem(idList.value(0)));
        ui->listTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 2, new QTableWidgetItem(idList.value(1)));
        ui->listTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 3, new QTableWidgetItem(idList.value(2)));
        ui->listTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 4, new QTableWidgetItem(idList.value(3)));
        ui->listTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 5, new QTableWidgetItem(remoteIpList.value(i)));
        ui->listTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 6, new QTableWidgetItem(startTimeList.value(i)));
        ui->listTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->listTableWidget->setItem(row_count, 7, new QTableWidgetItem(signalInitensity));
        ui->listTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中

        if (!pLoadDlg->isVisible())
            return;
    }

    pLoadDlg->hide();

    if (num == 5) //限定显示条数
        QMessageBox::information(NULL, "提示", "匹配到的数据太多，只显示前 5 条，请重新搜索！！" );
}

/*列表*/
void deviceDlg::on_listTableWidget_cellClicked(int row, int column)
{
    qDebug() << "行-列:"<< row << "-" << column;
    int selectedDevNum = row +NUMPAGE * (indexPage - 1);

    if (!remoteIpList.isEmpty())
        deviceDlg::deviceIp =  remoteIpList.at(selectedDevNum);
    else
        deviceDlg::deviceIp = "";
}

void deviceDlg::onTimeRelay()
{
    static int sec = 0;
    static int min = 0;
    QString str_time;

    str_time.sprintf("%d分：%d秒", min, sec);

    if (sec < 59)
        sec++;
    else
    {
        ++ min;
        sec = 0;
    }
}

void deviceDlg::on_nextPushButton_clicked()
{
    if (pLoadDlg->isVisible())
        return;
    if (remainsRow != 0)   //存在未填满的页
    {
        if (indexPage > pageNum + 1 || indexPage < 1)
            indexPage = pageNum;

        if (indexPage < (pageNum + 1))
            indexPage ++ ;   // 页的索引
        else
        {
            QMessageBox::information(NULL, "提示", "已经到最后一页！！" );
            return;
        }
    }
    else
    {
        if (indexPage > pageNum  || indexPage < 1)
            indexPage = pageNum - 1;

        if (indexPage < pageNum)
            indexPage ++ ;   // 页的索引
        else
        {
            QMessageBox::information(NULL, "提示", "已经到最后一页！！" );
            return;
        }
    }

    /* 删除所有设备信息*/
    ui->listTableWidget->clearContents();
    ui->currentPageLineEdit->setText(QString::number(indexPage, 10));
    deviceDlg::deviceIp = "";

    pLoadDlg->show();

    if (indexPage <= pageNum)
    {
        /* 删除所有账户信息*/
        ui->listTableWidget->clearContents();
        ui->listTableWidget->setRowCount(0); //删除行
        for (int i = NUMPAGE * (indexPage - 1) ; i < NUMPAGE * indexPage; i++)
        {
            QApplication::processEvents(); //处理大数据，防gui假死

            QString userId = userIdList.at(i);
            QList<QString> idList = userId.split('-');
            if (idList.size() < 4)
            {
                for (int i=0; i < (4 - idList.size()); i++)
                    idList.append("");
            }

            int row_count = ui->listTableWidget->rowCount(); //获取表单行数
            ui->listTableWidget->insertRow(row_count);       //插入新行
            ui->listTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
            ui->listTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 1, new QTableWidgetItem(idList.value(0)));
            ui->listTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 2, new QTableWidgetItem(idList.value(1)));
            ui->listTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 3, new QTableWidgetItem(idList.value(2)));
            ui->listTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 4, new QTableWidgetItem(idList.value(3)));
            ui->listTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 5, new QTableWidgetItem(remoteIpList.value(i)));
            ui->listTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 6, new QTableWidgetItem(startTimeList.value(i)));
            ui->listTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 7, new QTableWidgetItem(signalInitensity));
            ui->listTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中

            if (!pLoadDlg->isVisible())
                return;
        }
        pLoadDlg->hide();
    }
    else
    {
        /* 删除所有账户信息*/
        ui->listTableWidget->clearContents();
        ui->listTableWidget->setRowCount(0); //删除行
        for (int i = NUMPAGE * (indexPage - 1) ; i < NUMPAGE * (indexPage-1) + remainsRow; i++)
        {
            QApplication::processEvents(); //处理大数据，防gui假死

            QString userId = userIdList.at(i);
            QList<QString> idList = userId.split('-');

            if (idList.size() < 4)
            {
                for (int i=0; i < (4 - idList.size()); i++)
                    idList.append("");
            }

            int row_count = ui->listTableWidget->rowCount(); //获取表单行数
            ui->listTableWidget->insertRow(row_count);       //插入新行
            ui->listTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
            ui->listTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 1, new QTableWidgetItem(idList.value(0)));
            ui->listTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 2, new QTableWidgetItem(idList.value(1)));
            ui->listTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 3, new QTableWidgetItem(idList.value(2)));
            ui->listTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 4, new QTableWidgetItem(idList.value(3)));
            ui->listTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 5, new QTableWidgetItem(remoteIpList.value(i)));
            ui->listTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 6, new QTableWidgetItem(startTimeList.value(i)));
            ui->listTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 7, new QTableWidgetItem(signalInitensity));
            ui->listTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中

            if (!pLoadDlg->isVisible())
                return;
        }
        pLoadDlg->hide();
    }
}

void deviceDlg::on_previousPushButton_clicked()
{
    if (pLoadDlg->isVisible())
        return;

    if (remainsRow != 0)
    {
        if (indexPage > pageNum + 1 || indexPage < 1)
            indexPage = 2; //针对设置页码时越界，设置为2

        if ((indexPage <= pageNum + 1) && (indexPage > 1))
            indexPage -- ;   // 页的索引
        else
        {
            QMessageBox::information(NULL, "提示", "已经到第一页！！" );
            return;
        }
    }
    else
    {
        if (indexPage > pageNum  || indexPage < 1)
            indexPage = 2;

        if ((indexPage <= pageNum) && (indexPage > 1))
            indexPage -- ;   // 页的索引
        else
        {
            QMessageBox::information(NULL, "提示", "已经到第一页！！" );
            return;
        }
    }

    /* 删除所有设备信息*/
    ui->listTableWidget->clearContents();
    deviceDlg::deviceIp = "";

    pLoadDlg->show();
    ui->currentPageLineEdit->setText(QString::number(indexPage, 10));

    if (indexPage <= pageNum)
    {
        /* 删除所有账户信息*/
        ui->listTableWidget->clearContents();
        ui->listTableWidget->setRowCount(0); //删除行
        for (int i = NUMPAGE * (indexPage - 1) ; i < NUMPAGE * indexPage; i++)
        {
            QApplication::processEvents(); //处理大数据，防gui假死

            QString userId = userIdList.at(i);
            QList<QString> idList = userId.split('-');

            if (idList.size() < 4)
            {
                for (int i=0; i < (4 - idList.size()); i++)
                    idList.append("");
            }

            int row_count = ui->listTableWidget->rowCount(); //获取表单行数
            ui->listTableWidget->insertRow(row_count);       //插入新行
            ui->listTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
            ui->listTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 1, new QTableWidgetItem(idList.value(0)));
            ui->listTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 2, new QTableWidgetItem(idList.value(1)));
            ui->listTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 3, new QTableWidgetItem(idList.value(2)));
            ui->listTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 4, new QTableWidgetItem(idList.value(3)));
            ui->listTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 5, new QTableWidgetItem(remoteIpList.value(i)));
            ui->listTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 6, new QTableWidgetItem(startTimeList.value(i)));
            ui->listTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 7, new QTableWidgetItem(signalInitensity));
            ui->listTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中

            if (!pLoadDlg->isVisible())
                return;
        }
        pLoadDlg->hide();
    }
    else
    {
        /* 删除所有账户信息*/
        ui->listTableWidget->clearContents();
        ui->listTableWidget->setRowCount(0); //删除行
        for (int i = NUMPAGE * (indexPage - 1) ; i < NUMPAGE * (indexPage-1) + remainsRow; i++)
        {
            QApplication::processEvents(); //处理大数据，防gui假死
            QString userId = userIdList.at(i);
            QList<QString> idList = userId.split('-');

            if (idList.size() < 4)
            {
                for (int i=0; i < (4 - idList.size()); i++)
                    idList.append("");
            }

            int row_count = ui->listTableWidget->rowCount(); //获取表单行数
            ui->listTableWidget->insertRow(row_count);       //插入新行
            ui->listTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
            ui->listTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 1, new QTableWidgetItem(idList.value(0)));
            ui->listTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 2, new QTableWidgetItem(idList.value(1)));
            ui->listTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 3, new QTableWidgetItem(idList.value(2)));
            ui->listTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 4, new QTableWidgetItem(idList.value(3)));
            ui->listTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 5, new QTableWidgetItem(remoteIpList.value(i)));
            ui->listTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 6, new QTableWidgetItem(startTimeList.value(i)));
            ui->listTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 7, new QTableWidgetItem(signalInitensity));
            ui->listTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中

            if (!pLoadDlg->isVisible())
                return;
        }
        pLoadDlg->hide();
    }
}

void deviceDlg::on_enterPushButton_clicked()
{
    indexPage = ui->currentPageLineEdit->text().toInt();
    if (remainsRow != 0)
    {
        if ((indexPage > pageNum + 1) || (indexPage < 1))
        {
            QMessageBox::information(NULL, "提示", "输入页码超过范围！！" );
            return;
        }
    }
    else
    {
        if ((indexPage > pageNum) || (indexPage < 1))
        {
            QMessageBox::information(NULL, "提示", "输入页码超过范围！！" );
            return;
        }
    }

    /* 删除所有设备信息*/
    ui->listTableWidget->clearContents();
    deviceDlg::deviceIp = "";

    pLoadDlg->show();

    if (indexPage <= pageNum)
    {
        /* 删除所有账户信息*/
        ui->listTableWidget->clearContents();
        ui->listTableWidget->setRowCount(0); //删除行
        for (int i = NUMPAGE * (indexPage - 1) ; i < NUMPAGE * indexPage; i++)
        {
            QApplication::processEvents(); //处理大数据，防gui假死

            QString userId = userIdList.at(i);
            QList<QString> idList = userId.split('-');
            if (idList.size() < 4)
            {
                for (int i=0; i < (4 - idList.size()); i++)
                    idList.append("");
            }

            int row_count = ui->listTableWidget->rowCount(); //获取表单行数
            ui->listTableWidget->insertRow(row_count);       //插入新行
            ui->listTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
            ui->listTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 1, new QTableWidgetItem(idList.value(0)));
            ui->listTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 2, new QTableWidgetItem(idList.value(1)));
            ui->listTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 3, new QTableWidgetItem(idList.value(2)));
            ui->listTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 4, new QTableWidgetItem(idList.value(3)));
            ui->listTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 5, new QTableWidgetItem(remoteIpList.value(i)));
            ui->listTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 6, new QTableWidgetItem(startTimeList.value(i)));
            ui->listTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 7, new QTableWidgetItem(signalInitensity));
            ui->listTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中

            if (!pLoadDlg->isVisible())
                return;
        }
        pLoadDlg->hide();
    }
    else
    {
        /* 删除所有账户信息*/
        ui->listTableWidget->clearContents();
        ui->listTableWidget->setRowCount(0); //删除行
        for (int i = NUMPAGE * (indexPage - 1) ; i < NUMPAGE * (indexPage-1) + remainsRow; i++)
        {
            QApplication::processEvents(); //处理大数据，防gui假死

            QString userId = userIdList.at(i);
            QList<QString> idList = userId.split('-');

            if (idList.size() < 4)
            {
                for (int i=0; i < (4 - idList.size()); i++)
                    idList.append("");
            }

            int row_count = ui->listTableWidget->rowCount(); //获取表单行数
            ui->listTableWidget->insertRow(row_count);       //插入新行
            ui->listTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
            ui->listTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 1, new QTableWidgetItem(idList.value(0)));
            ui->listTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 2, new QTableWidgetItem(idList.value(1)));
            ui->listTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 3, new QTableWidgetItem(idList.value(2)));
            ui->listTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 4, new QTableWidgetItem(idList.value(3)));
            ui->listTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 5, new QTableWidgetItem(remoteIpList.value(i)));
            ui->listTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 6, new QTableWidgetItem(startTimeList.value(i)));
            ui->listTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->listTableWidget->setItem(row_count, 7, new QTableWidgetItem(signalInitensity));
            ui->listTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中

            if (!pLoadDlg->isVisible())
                return;
        }
        pLoadDlg->hide();
    }
}
