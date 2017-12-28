#include "devicedlg.h"
#include "ui_devicedlg.h"
#include <QScrollBar>

deviceDlg::deviceDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::deviceDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

    /*tableWidget 初始化设置*/
    ui->listTableWidget->setColumnCount(6);                   //设置列数
    ui->listTableWidget->horizontalHeader()->setDefaultSectionSize(150);
    ui->listTableWidget->horizontalHeader()->setVisible(true);//表头可见

    /*设置表头内容*/
    QStringList header;
    header << "编号" << "站地址" << "mac地址" << "VPN分配的IP地址" << "远程设备的IP地址" << "信号强度";
    ui->listTableWidget->setHorizontalHeaderLabels(header);

    /*设置表的行排列显示*/
    ui->listTableWidget->horizontalHeader()->resizeSection(0, 60); //设置表头第一列(编号)的宽度为70；
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); //等比例显示
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); //等比例显示
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->listTableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    /*设置表头字体加粗*/
    QFont font = this->font();
    font.setBold(true);
    ui->listTableWidget->horizontalHeader()->setFont(font);

    /*设置表的属性*/
    ui->listTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:forestgreen;}"); //设置表头背景色
    ui->listTableWidget->horizontalHeader()->setHighlightSections(false);     //点击表时，不对表头行高亮
    ui->listTableWidget->horizontalHeader()->setStretchLastSection(true);     //设置充满表宽度
    ui->listTableWidget->verticalHeader()->setDefaultSectionSize(35);         //设置行高
    ui->listTableWidget->setFrameShape(QFrame::NoFrame);                      //设置无边框
    ui->listTableWidget->setShowGrid(false);                                  //设置不显示格子线
    ui->listTableWidget->verticalHeader()->setVisible(false);                 //设置垂直头不可见
    ui->listTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->listTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置不可编辑
    ui->listTableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);   //设置双击可编辑
    ui->listTableWidget->horizontalHeader()->setFixedHeight(30);              //设置表头的高度

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

    {
        /* 动态插入*/
        for (int i=1; i<100; i++)
        {
            int row_count = ui->listTableWidget->rowCount(); //获取表单行数
            ui->listTableWidget->insertRow(row_count); //插入新行

            ui->listTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i, 10)));
            ui->listTableWidget->setItem(row_count, 1, new QTableWidgetItem("3702000260200jjjjjjjjjjjjjjjjjjjjjjj"));
            ui->listTableWidget->setItem(row_count, 2, new QTableWidgetItem("ff-ff-ff-ff-ff-ff"));
            ui->listTableWidget->setItem(row_count, 3, new QTableWidgetItem("1234.ojnn"));
            ui->listTableWidget->setItem(row_count, 4, new QTableWidgetItem("999999999999"));
        }
    }
}

    deviceDlg::~deviceDlg()
    {
        delete ui;
    }
