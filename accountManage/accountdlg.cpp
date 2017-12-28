#include "accountdlg.h"
#include "ui_accountdlg.h"
#include "register/registerdlg.h"
#include <QScrollBar>

accountDlg::accountDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::accountDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

    ui->accountTableWidget->setColumnCount(7);                             //设置列数
    ui->accountTableWidget->horizontalHeader()->setDefaultSectionSize(150);
    ui->accountTableWidget->horizontalHeader()->setVisible(true);          //表头可见

    /*设置表头内容*/
    QStringList headerAccount;
    headerAccount<< "编号" << "用户名" << "邮箱" << "电话号码" << "开始时间"<< "结束时间"<< "状态";
    ui->accountTableWidget->setHorizontalHeaderLabels(headerAccount);

    /*设置表的行排列显示*/
    ui->accountTableWidget->horizontalHeader()->resizeSection(0, 60);       //设置表头第一列(编号)的宽度为70；
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);

    /*设置表头字体加粗*/
    QFont font = this->font();
    font.setBold(true);
    ui->accountTableWidget->horizontalHeader()->setFont(font);

    /*设置表的属性*/
    ui->accountTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:forestgreen;}"); //设置表头背景色
    ui->accountTableWidget->horizontalHeader()->setHighlightSections(false);        //点击表时，不对表头行高亮
    ui->accountTableWidget->horizontalHeader()->setFixedHeight(30);                 //设置表头的高度
    ui->accountTableWidget->horizontalHeader()->setStretchLastSection(true);        //设置充满表宽度
    ui->accountTableWidget->verticalHeader()->setDefaultSectionSize(35);            //设置行高
    ui->accountTableWidget->setFrameShape(QFrame::NoFrame);                         //设置无边框
    ui->accountTableWidget->setShowGrid(false);                                     //设置不显示格子线
    ui->accountTableWidget->verticalHeader()->setVisible(false);                    //设置垂直头不可见
    ui->accountTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);    //整行选中的方式
    ui->accountTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);     //设置不可编辑

    /* 设置水平、垂直滚动条样式 */
    ui->accountTableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:20px;}"
                                                              "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                                              "QScrollBar::handle:hover{background:gray;}"
                                                              "QScrollBar::sub-line{background:transparent;}"
                                                              "QScrollBar::add-line{background:transparent;}");
    ui->accountTableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 20px;}"
                                                            "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                                            "QScrollBar::handle:hover{background:gray;}"
                                                            "QScrollBar::sub-line{background:transparent;}"
                                                            "QScrollBar::add-line{background:transparent;}");

    /* 动态插入*/
    for (int i=1; i<100; i++)
    {
        int row_count = ui->accountTableWidget->rowCount(); //获取表单行数
        ui->accountTableWidget->insertRow(row_count);       //插入新行

        ui->accountTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i, 10)));
        ui->accountTableWidget->setItem(row_count, 1, new QTableWidgetItem("3702000260200"));
        ui->accountTableWidget->setItem(row_count, 2, new QTableWidgetItem("ff-ff-ff-ff-ff-ff"));
        ui->accountTableWidget->setItem(row_count, 3, new QTableWidgetItem("1234.ojnn"));
        ui->accountTableWidget->setItem(row_count, 4, new QTableWidgetItem("999999999999"));
    }
}


accountDlg::~accountDlg()
{
    delete ui;
}

void accountDlg::on_addAccountPushButton_clicked()
{
    registerDlg dlg;
    dlg.exec();
}
