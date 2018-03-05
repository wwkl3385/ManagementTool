/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： recordDlg.cpp
* 描    述： 登录记录
* 修改记录：
* 			V0.9，2018-01-12，new，刘卫明
*
***********************************************************************/
#include "recorddlg.h"
#include "ui_recorddlg.h"
#include<QScrollBar>

recordDlg::recordDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::recordDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

    /*导出记录*/
    ui->recordTableWidget->setColumnCount(7);                       //设置列数
    ui->recordTableWidget->horizontalHeader()->setDefaultSectionSize(150);
    ui->recordTableWidget->horizontalHeader()->setVisible(true);    //表头可见

    /*设置表头内容*/
    QStringList headerAccount;
    headerAccount<< "编号" << "用户名" << "VPN分配的IP" << "VPN服务端的端口号" << "远程设备的IP地址"<< "远程设备的端口号"<<"状态";
    ui->recordTableWidget->setHorizontalHeaderLabels(headerAccount);
    ui->recordTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->recordTableWidget->horizontalHeader()->resizeSection(0, 50);                          //设置表头第一列的宽度为50；
    ui->recordTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); //等比例显示
    ui->recordTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); //等比例显示
    ui->recordTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); //等比例显示
    ui->recordTableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch); //等比例显示
    ui->recordTableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch); //等比例显示
    ui->recordTableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch); //等比例显示

    /*设置表头字体加粗*/
    QFont font = this->font();
    font.setBold(true);
    ui->recordTableWidget->horizontalHeader()->setFont(font);
    ui->recordTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:forestgreen;}"); //设置表头背景色
    ui->recordTableWidget->horizontalHeader()->setHighlightSections(false);      //点击表时，不对表头行高亮
    ui->recordTableWidget->horizontalHeader()->setStretchLastSection(true);      //设置充满表宽度
    ui->recordTableWidget->verticalHeader()->setDefaultSectionSize(35);          //设置行高
    ui->recordTableWidget->setFrameShape(QFrame::NoFrame);                       //设置无边框
    ui->recordTableWidget->setShowGrid(false);                                   //设置不显示格子线
    ui->recordTableWidget->verticalHeader()->setVisible(false);                  //设置垂直头不可见
    ui->recordTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->recordTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置不可编辑
    ui->recordTableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);    //设置双击可编辑
    ui->recordTableWidget->horizontalHeader()->setFixedHeight(30);               //设置表头的高度

    /*设置水平、垂直滚动条样式*/
    ui->recordTableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:20px;}"
                                                                "QScrollBar::handle{background:lightgray;border:2px solid transparent;"
                                                                "border-radius:5px;}"
                                                                "QScrollBar::handle:hover{background:gray;}"
                                                                "QScrollBar::sub-line{background:transparent;}"
                                                                "QScrollBar::add-line{background:transparent;}");
    ui->recordTableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 20px;}"
                                                              "QScrollBar::handle{background:lightgray; border:2px solid transparent;"
                                                              " border-radius:5px;}"
                                                              "QScrollBar::handle:hover{background:gray;}"
                                                              "QScrollBar::sub-line{background:transparent;}"
                                                              "QScrollBar::add-line{background:transparent;}");
}

recordDlg::~recordDlg()
{
    delete ui;
}
