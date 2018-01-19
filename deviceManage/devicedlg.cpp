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
#define  NUMPAGE             25    //每页显示条数

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
    progressDlg(new QProgressDialog(this))
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

#if 0
    //创建菜单项
    pop_menu = new QMenu();
    sort_style = new QMenu(this);
    action_name = new QAction(this);
    action_size = new QAction(this);
    action_type = new QAction(this);
    action_date = new QAction(this);
    action_open = new QAction(this);
    action_download = new QAction(this);
    action_flush = new QAction(this);
    action_delete = new QAction(this);
    action_rename = new QAction(this);
    action_create_folder = new QAction(this);
#endif

    signalFlag = 0;  //发送数量信号

    ui->searchAllPushButton->hide();
    ui->pageFrame->hide();
    pTimer = new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(onTimeRelay()));
    pTimer->start(1000);     //1s

    /*tableWidget 初始化设置*/
    ui->listTableWidget->setColumnCount(8);                   //设置列数
    ui->listTableWidget->horizontalHeader()->setDefaultSectionSize(150);
    ui->listTableWidget->horizontalHeader()->setVisible(true);//表头可见

    /*搜索按钮 样式设置*/
     ui->currentPageLineEdit->setStyleSheet("QLineEdit{background-color: oldlace; color: blue; border-radius:8px; font:11pt;font:bold;  border: 2px groove gray; border-style: outset;}"\
                                               "QLineEdit:hover{background-color:khaki; color: black;}"\
                                              "QLineEdit:focus{background-color:lightblue; border-style: inset; }");
     ui->searchLineEdit->setStyleSheet("QLineEdit{background-color: oldlace; color: steelblue; border-radius:8px; font:12pt;   border: 2px groove gray; border-style: outset;}"\
                                               "QLineEdit:hover{background-color:skyblue; color: black;}"\
                                              "QLineEdit:focus{background-color:lightblue; border-style: inset; }");

     ui->searchPushButton->setStyleSheet("QPushButton{background-color: rgb(14 , 150 , 254); color: white; border-radius:8px; font:12pt;font:bold; border: 2px groove gray; border-style: outset;}"\
                                         "QPushButton:hover{background-color:skyblue; color: black;}"\
                                              "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");

     ui->searchAllPushButton->setStyleSheet("QPushButton{background-color:deepskyblue; color: white; border-radius:8px; font:12pt;font:bold; border: 2px groove gray; border-style: outset;}"\
                                         "QPushButton:hover{background-color:skyblue; color: black;}"\
                                              "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");

    /*进度条设置样式*/
    progressDlg->setStyleSheet("QProgressBar{border-width: 0 10 0 10; border-left: 1px, gray; border-right: 1px, gray; border-image:url(:/Resources/progressbar_back.png); } "
                               "QProgressBar::chunk {border-width: 0 10 0 10; }");

    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setMinimumDuration(5);
    progressDlg->setWindowTitle(tr("获取数据"));
    progressDlg->setLabelText(tr("正在从服务器获取数据，请稍后..."));
    progressDlg->setCancelButtonText(tr("取消"));
    progressDlg->setMinimum(0);
    progressDlg->setMaximum(0);
    progressDlg->close();
    progressDlg->reset();

    /*设置表头内容*/
    QStringList header;
    header << "编号"  << "   mac地址   "<< "  站地址1  "<< "  站地址2  "<< "  站地址3  " <<  " 设备vpn地址 " << "设备上线时间" << " 信号强度 ";
    ui->listTableWidget->setHorizontalHeaderLabels(header);

    /*设置表的行排列显示*/
    ui->listTableWidget->horizontalHeader()->resizeSection(0, 50); //设置表头第一列(编号)的宽度为50；
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);

    /*设置表头字体加粗*/
    QFont font = this->font();
    font.setBold(true);
    ui->listTableWidget->horizontalHeader()->setFont(font);

    /*设置表的属性*/
    ui->listTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:deepskyblue;font:12pt;font:bold;}"); //设置表头背景色
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

    /*设置水平、垂直滚动条样式*/
    ui->listTableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:20px;}"
                                                              "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                                              "QScrollBar::handle:hover{background:gray;}"
                                                              "QScrollBar::sub-line{background:transparent;}"
                                                              "QScrollBar::add-line{background:transparent;}");
    ui->listTableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 20px;}"
                                                            "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                                            "QScrollBar::handle:hover{background:gray;}"
                                                            "QScrollBar::sub-line{background:transparent;}"
                                                            "QScrollBar::add-line{background:transparent;}");

#if 0
    /* 删除所有设备信息*/
    ui->listTableWidget->clearContents();

    for (int i=0; i<NUMPAGE;i++)
        ui->listTableWidget->insertRow(i);       //插入NUMPAGE行
#endif


    /*管理员和普通用户*/
    if(logon::userType == 1)
    {
        /* 管理员---查询所有的在线终端--数量*/
        dataJson->connectNumberObj= dataJson->jsonPackQueryConnectNumber();
        dataHttp->httpPost(QUERY_ALL_NUM_URL, dataJson->connectNumberObj); //http请求终端数量
        connect(this->dataHttp->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));

//    signalFlag = 1;     //集控所有信息
    progressDlg->show();

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
        ui->searchAllPushButton->hide();
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
    delete progressDlg;
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

/*查询全部集控信息*/
void deviceDlg::on_searchAllPushButton_clicked()
{

    signalFlag = 1;     //集控所有信息
    progressDlg->show();

    /* 管理员---查询所有的在线终端--数据*/
    dataJson->connectAllInfoObj= dataJson->jsonPackQueryConnectAllInfo();
    dataHttp->httpPost(QUERY_ALL_INFO_URL, dataJson->connectAllInfoObj); //http请求
}

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


    //qDebug() << "页数：" <<pageNum;
    //qDebug() << "剩余行数：" << remainsRow;

    ui->currentPageLineEdit->setText("1");
    if (remainsRow != 0)
        ui->allPageLabel->setText(QString::number(pageNum+1, 10));
    else
        ui->allPageLabel->setText(QString::number(pageNum, 10));

    //    int j = 0; //插入行数
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

        if (progressDlg->wasCanceled())
        {
            progressDlg->reset();
            progressDlg->hide();
            return;
        }
    }

    ui->pageFrame->show();
    progressDlg->hide();
    progressDlg->reset();
}

/*search*/
void deviceDlg::on_searchPushButton_clicked()
{
    signalFlag = 2;

    if (ui->searchLineEdit->text().isEmpty())
    {
        QMessageBox::information(NULL, "提示", "输入内容为空！！" );
        return;
    }

    /* 查询集控连接信息*/
    dataJson->connectInfoObj= dataJson->jsonPackQueryConnectInfo(ui->searchLineEdit->text());
    dataHttp->httpPost(QUERY_INFO_URL, dataJson->connectInfoObj); //http请求终端信息

    progressDlg->show();

    /* 删除所有设备信息*/
    ui->listTableWidget->clearContents();
    startTimeList.clear();
    userIdList.clear();
    remoteIpList.clear();
    ui->pageFrame->hide();
}

void deviceDlg::onConnectInfoDataParse(QByteArray tmpData)
{
    //qDebug() <<"dange集控  xinxi  返回的值："<< tmpData;
    //qDebug() <<"dange集控  dange  的长度：" << tmpData.size();

    /* 删除所有设备信息*/
    ui->listTableWidget->clearContents();

    startTimeList = dataJson->jsonParseData(tmpData,"log_start_time");//解析
    userIdList    = dataJson->jsonParseData(tmpData,"user_id");       //解析
    remoteIpList  = dataJson->jsonParseData(tmpData,"log_remote_ip"); //解析

    int num = remoteIpList.size();

    if (num < 1)
    {
        progressDlg->hide();
        progressDlg->reset();
        QMessageBox::information(NULL, "提示", "没有找到相匹配的数据！！" );
        return;
    }
    if (num > 5) //限定显示条数
        num = 5;

    //    int row_count = 0;
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
        //        row_count++;

        //qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
        if (progressDlg->wasCanceled())
        {
            qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
            qDebug() << "取消------------";
            progressDlg->reset();
            progressDlg->hide();
            return;
        }
    }

    progressDlg->hide();
    progressDlg->reset();

    if (num == 5) //限定显示条数
        QMessageBox::information(NULL, "提示", "匹配到的数据太多，只显示前 5 条，请重新搜索！！" );
}

/*列表*/
void deviceDlg::on_listTableWidget_cellClicked(int row, int column)
{
    qDebug() << "行、列：" << row << column;
    int selectedDevNum = row +NUMPAGE * (indexPage - 1);

    qDebug() << "设备ip行：" << selectedDevNum;
    //    QList<QString> remoteIpList;  //存储解析的远程ip

    if (signalFlag == 1)
    {
        if (!remoteIpList.isEmpty() && remoteIpList.size() > selectedDevNum)
            deviceDlg::deviceIp =  remoteIpList.at(selectedDevNum);
        else
            deviceDlg::deviceIp = "";
    }
    else if (signalFlag == 2)
    {
        if (!remoteIpList.isEmpty() && remoteIpList.size() > row && row < 5)
            deviceDlg::deviceIp =  remoteIpList.at(row);
        else
            deviceDlg::deviceIp = "";
    }

    qDebug() << "设备ip 大小" << remoteIpList.size();
    qDebug() << "设备ip" << deviceIp;
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

    progressDlg->show();

    if (indexPage <= pageNum)
    {
        //        int row_count = 0;
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

            //            row_count ++;

            //            qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
            if (progressDlg->wasCanceled())
            {
                //                qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
                //                qDebug() << "取消------------";
                progressDlg->reset();
                progressDlg->hide();
                return;
            }

        }
        progressDlg->hide();
        progressDlg->reset();
    }
    else
    {
        //        int row_count = 0;
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
            //            row_count ++;

            //            qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
            if (progressDlg->wasCanceled())
            {
                //                qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
                //                qDebug() << "取消------------";
                progressDlg->reset();
                progressDlg->hide();
                return;
            }
        }
        progressDlg->hide();
        progressDlg->reset();
    }
}

void deviceDlg::on_currentPageLineEdit_returnPressed()
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

    progressDlg->show();

    if (indexPage <= pageNum)
    {
        //        int row_count = 0;
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

            //            row_count ++;
            //            qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
            if (progressDlg->wasCanceled())
            {
                //                qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
                //                qDebug() << "取消------------";
                progressDlg->reset();
                progressDlg->hide();
                return;
            }
        }
        progressDlg->hide();
        progressDlg->reset();
    }
    else
    {
        //        int row_count = 0;
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

            //            row_count ++;
            //            qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
            if (progressDlg->wasCanceled())
            {
                //                qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
                //                qDebug() << "取消------------";
                progressDlg->reset();
                progressDlg->hide();
                return;
            }

        }
        progressDlg->hide();
        progressDlg->reset();
    }
}


void deviceDlg::on_previousPushButton_clicked()
{
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

    progressDlg->show();
    ui->currentPageLineEdit->setText(QString::number(indexPage, 10));

    if (indexPage <= pageNum)
    {
        //        int row_count = 0;
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
            //            row_count ++;

            //            qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
            if (progressDlg->wasCanceled())
            {
                //                qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
                //                qDebug() << "取消------------";
                progressDlg->reset();
                progressDlg->hide();
                return;
            }

        }
        progressDlg->hide();
        progressDlg->reset();
    }
    else
    {
        //        int row_count  = 0;

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
            //            row_count ++;

            //qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
            if (progressDlg->wasCanceled())
            {
                //qDebug() <<"wasCanceled"<< progressDlg->wasCanceled();
                //qDebug() << "取消------------";
                progressDlg->reset();
                progressDlg->hide();
                return;
            }
        }
        progressDlg->hide();
        progressDlg->reset();
    }
}

#if 0
void deviceDlg::createActions()
{
    action_open->setText("打开");
    action_download->setText(QString("下载"));
    action_flush->setText(QString("刷新"));
    action_delete->setText(QString("删除"));
    action_rename->setText(QString("重命名"));
    action_create_folder->setText(QString("新建文件夹"));
    action_name->setText(QString("名称"));
    action_size->setText(QString("大小"));
    action_type->setText(QString("项目类型"));
    action_date->setText(QString("修改日期"));

    //设置快捷键
    action_flush->setShortcut(QKeySequence::Refresh);

    //设置文件夹图标
    //    action_create_folder->setIcon(icon);
    QObject::connect(action_create_folder, SIGNAL(triggered()), this, SLOT(createFolder()));
}

void deviceDlg::contextMenuEvent(QContextMenuEvent *event)
{
    pop_menu->clear(); //清除原有菜单
    QPoint point = event->pos(); //得到窗口坐标
    QTableWidgetItem *item = ui->listTableWidget->itemAt(point);
    if(item != NULL)
    {
        pop_menu->addAction(action_download);
        pop_menu->addAction(action_flush);
        pop_menu->addSeparator();
        pop_menu->addAction(action_delete);
        pop_menu->addAction(action_rename);
        pop_menu->addSeparator();
        pop_menu->addAction(action_create_folder);
        sort_style = pop_menu->addMenu("排序");
        sort_style->addAction(action_name);
        sort_style->addAction(action_size);
        sort_style->addAction(action_type);
        sort_style->addAction(action_date);

        //菜单出现的位置为当前鼠标的位置
        pop_menu->exec(QCursor::pos());
        event->accept();
    }
}

#endif
