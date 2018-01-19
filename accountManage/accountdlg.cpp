/***********************************************************************
* Copyright (c) 2017, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： accountdlg.cpp
* 描    述： 账户管理  -- 查询所有账户，查询单个账户，删除账户
* 修改记录：
* 			V0.9，2017-12-28，new，刘卫明
*
***********************************************************************/
#include "accountdlg.h"
#include "logon/logon.h"
#include "ui_accountdlg.h"
#include "register/registerdlg.h"
#include "management/managementtool.h"
#include <QScrollBar>
#include <QMessageBox>
#include <QNetworkReply>

#define  QUERY_USER_URL    "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/query_user"      //查询用户信息url
#define  DELETE_USER_URL   "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/delete_user"     //删除用户信息url

bool accountDlg::modifyFlag = false;    //修改标志位
bool accountDlg::modifyAdmin= false;    //管理员修改密码
int  accountDlg::deleteRow = -1;        //初始值
stJsonData accountDlg:: modifyInfoData; //修改用户信息
dataList accountDlg::userList;          //用户数据列表

accountDlg::accountDlg(QWidget *parent) :
    QDialog(parent),
    dataJson(new jsonManage),
    dataHttp(new httpManage),
    ui(new Ui::accountDlg)
{
    qDebug() << "创建accountDlg";
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    ui->allUserPushButton->hide();

    signalFlag = false;  //请求 删除 信号

    ui->accountTableWidget->setColumnCount(8);                             //设置列数
    ui->accountTableWidget->horizontalHeader()->setDefaultSectionSize(150);
    ui->accountTableWidget->horizontalHeader()->setVisible(true);          //表头可见

    /*删除按钮*/
    ui->deletePushButton->setDisabled(true);
    ui->deletePushButton->setStyleSheet("QPushButton{background-color: gray; color: white; border-radius:10px; border: 2px groove gray; border-style: outset;}"\
                                               "QPushButton:hover{background-color:white; color: black;}"\
                                              "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");

    /*修改按钮*/
    ui->modifyPushButton->setDisabled(true);

    ui->modifyPushButton->setStyleSheet("QPushButton{background-color: gray; color: white; border-radius:10px; font:12pt;font:bold;border: 2px groove gray; border-style: outset;}"\
                                               "QPushButton:hover{background-color:white; color: black;}"\
                                              "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");

    ui->searchLineEdit->setStyleSheet("QLineEdit{background-color: oldlace; color: steelblue; border-radius:10px; font:12pt;font:bold;  border: 2px groove gray; border-style: outset;}"\
                                               "QLineEdit:hover{background-color:skyblue; color: black;}"\
                                              "QLineEdit:focus{background-color: lightblue; border-style: inset; }");

    /*设置表头内容*/
    QStringList headerAccount;
    headerAccount<< "编号" << "用户名" << "密码" << "邮箱" << "电话号码" << "开始时间"<< "结束时间"<< "状态";
    ui->accountTableWidget->setHorizontalHeaderLabels(headerAccount);

    /*设置表的行排列显示*/
    ui->accountTableWidget->horizontalHeader()->setStretchLastSection(true);        //设置充满表宽度
    ui->accountTableWidget->horizontalHeader()->resizeSection(0, 50);               //设置表头第一列(编号)的宽度为50；
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);

    /*设置表头字体加粗*/
    QFont font = this->font();
    font.setBold(true);
    ui->accountTableWidget->horizontalHeader()->setFont(font);

    /*设置表的属性*/
    ui->accountTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:deepskyblue;font:12pt;font:bold;}"); //设置表头背景色
    ui->accountTableWidget->horizontalHeader()->setHighlightSections(false);        //点击表时，不对表头行高亮
    ui->accountTableWidget->horizontalHeader()->setFixedHeight(30);                 //设置表头的高度
    ui->accountTableWidget->verticalHeader()->setDefaultSectionSize(35);            //设置行高
    ui->accountTableWidget->setFrameShape(QFrame::NoFrame);                         //设置无边框
    ui->accountTableWidget->setShowGrid(false);                                     //设置不显示格子线
    ui->accountTableWidget->verticalHeader()->setVisible(false);                    //设置垂直头不可见
    ui->accountTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);    //整行选中的方式
    ui->accountTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);     //设置不可编辑
    ui->accountTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);   //设置只能选择一行，不能选择多行

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

    /*查询当前所有的用户信息*/
    dataJson->queryUserObj= dataJson->jsonPackQueryUser();
    dataHttp->httpPost(QUERY_USER_URL, dataJson->queryUserObj); //http请求

    /*http请求*/
    connect(this->dataHttp->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    connect(this, SIGNAL(transmitUserDataSignal(QByteArray)), this, SLOT(onUserDataParse(QByteArray)));
}

accountDlg::~accountDlg()
{
    delete ui;
    delete dataJson;
    delete dataHttp;

    qDebug() << "销毁 accountDlg----------";
}

void accountDlg::on_addAccountPushButton_clicked()
{
    /*取消信号与槽的关联*/
    disconnect(this, SIGNAL(transmitUserDataSignal(QByteArray)), this, SLOT(onUserDataParse(QByteArray)));
    disconnect(this, SIGNAL(transmitDeleteUserDataSignal(QByteArray)), this, SLOT(onDeleteUserDataParse(QByteArray)));

    accountDlg::modifyFlag = false;

    registerDlg dlg;
    dlg.exec();

    qDebug() << "accountDlg  add 添加的用户：" << accountDlg::userList.first().user;
    qDebug() << "accountDlg  add 添加的标志：" << registerDlg::addSuccessFlag;

    if (registerDlg::addSuccessFlag == true) //添加成功后，更新列表
    {
        /* 删除所有账户信息*/
        ui->accountTableWidget->clearContents();
        ui->accountTableWidget->setRowCount(0); //删除行

        /* 动态插入*/
        qDebug() << "accountDlg  userlist数组大小：" << accountDlg::userList.size();
        for (int i = 0; i < accountDlg::userList.size(); i++)
        {
            int row_count = ui->accountTableWidget->rowCount(); //获取表单行数
            ui->accountTableWidget->insertRow(row_count);       //插入新行

            ui->accountTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
            ui->accountTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 1, new QTableWidgetItem(accountDlg::userList.value(i).user));
            ui->accountTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 2, new QTableWidgetItem(accountDlg::userList.value(i).password));
            ui->accountTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 3, new QTableWidgetItem(accountDlg::userList.value(i).userMail));
            ui->accountTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 4, new QTableWidgetItem(accountDlg::userList.value(i).userPhone));
            ui->accountTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 5, new QTableWidgetItem(accountDlg::userList.value(i).userStartDate));
            ui->accountTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 6, new QTableWidgetItem(accountDlg::userList.value(i).userEndDate));
            ui->accountTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 7, new QTableWidgetItem(accountDlg::userList.value(i).userEnable));
            ui->accountTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        }
    }
}

void accountDlg::onReplyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();
        QMessageBox::warning(this, "提示", "连接服务器超时，请重试！");
        return;
    }
    QByteArray tempBuf = reply->readAll();

    if (signalFlag == false)
        emit transmitUserDataSignal(tempBuf);
    else
        emit transmitDeleteUserDataSignal(tempBuf);
}

/*用户数据解析，动态插入表格*/
void accountDlg::onUserDataParse(QByteArray tmpData)
{
    qDebug() << "所有用户数据信息:" << tmpData;

    /* 删除所有账户信息*/
    ui->accountTableWidget->clearContents();
    ui->accountTableWidget->setRowCount(0); //删除行
    accountDlg::userList.clear(); //删除列表中所有数据

    /*list插入*/
    for (int i = 0; i < dataJson->jsonDataSize(tmpData); i++)
    {
        if (dataJson->jsonParseData(tmpData, "user").isEmpty())
            return;

        stData.user = dataJson->jsonParseData(tmpData,"user").value(i);
        stData.password = dataJson->jsonParseData(tmpData,"password").value(i);
        stData.userMail = dataJson->jsonParseData(tmpData,"user_mail").value(i);
        stData.userPhone = dataJson->jsonParseData(tmpData,"user_phone").value(i);
        stData.userStartDate = dataJson->jsonParseData(tmpData,"user_start_date").value(i);
        stData.userEndDate = dataJson->jsonParseData(tmpData,"user_end_date").value(i);
        stData.userType = dataJson->jsonParseData(tmpData,"user_type").value(i);

        if ( dataJson->jsonParseData(tmpData,"user_enable").value(i).toInt()  == 1)
            stData.userEnable= QString("有效");
        else
            stData.userEnable= QString("无效");

        if (stData.userType == "2" || stData.user == logon::userNameInfo) //普通用户 he 管理员自己
            accountDlg::userList.append(stData);
    }

    /* 动态插入*/
    for (int i = 0; i < accountDlg::userList.size(); i++)
    {
        int row_count = ui->accountTableWidget->rowCount(); //获取表单行数
        ui->accountTableWidget->insertRow(row_count);       //插入新行

        ui->accountTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
        ui->accountTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->accountTableWidget->setItem(row_count, 1, new QTableWidgetItem(accountDlg::userList.value(i).user));
        ui->accountTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->accountTableWidget->setItem(row_count, 2, new QTableWidgetItem(accountDlg::userList.value(i).password));
        ui->accountTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->accountTableWidget->setItem(row_count, 3, new QTableWidgetItem(accountDlg::userList.value(i).userMail));
        ui->accountTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->accountTableWidget->setItem(row_count, 4, new QTableWidgetItem(accountDlg::userList.value(i).userPhone));
        ui->accountTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->accountTableWidget->setItem(row_count, 5, new QTableWidgetItem(accountDlg::userList.value(i).userStartDate));
        ui->accountTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->accountTableWidget->setItem(row_count, 6, new QTableWidgetItem(accountDlg::userList.value(i).userEndDate));
        ui->accountTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        ui->accountTableWidget->setItem(row_count, 7, new QTableWidgetItem(accountDlg::userList.value(i).userEnable));
        ui->accountTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
    }
}

/*删除用户信息*/
void accountDlg::onDeleteUserDataParse(QByteArray tmpData)
{
    disconnect(this, SIGNAL(transmitDeleteUserDataSignal(QByteArray)), this, SLOT(onDeleteUserDataParse(QByteArray)));

    qDebug() << "删除数据返回信息：" << tmpData;
    if (dataJson->jsonParseData(tmpData, "status").isEmpty())
    {
        QMessageBox::warning(NULL, "提示", "服务器无响应，请重试");
        return;
    }
    else if (dataJson->jsonParseData(tmpData,"status").first().toInt() == 1)
    {
        QMessageBox::warning(NULL, "提示", "成功删除账户");
        accountDlg::userList.removeAt(accountDlg::deleteRow); //删除list

        /* 删除所有账户信息 */
        ui->accountTableWidget->clearContents();
        ui->accountTableWidget->setRowCount(0);     //删除行

        /* 动态插入 所有账户信息 */
        for (int i = 0; i < accountDlg::userList.size(); i++)
        {
            int row_count = ui->accountTableWidget->rowCount(); //获取表单行数
            ui->accountTableWidget->insertRow(row_count);       //插入新行

            ui->accountTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
            ui->accountTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 1, new QTableWidgetItem(accountDlg::userList.value(i).user));
            ui->accountTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 2, new QTableWidgetItem(accountDlg::userList.value(i).password));
            ui->accountTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 3, new QTableWidgetItem(accountDlg::userList.value(i).userMail));
            ui->accountTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 4, new QTableWidgetItem(accountDlg::userList.value(i).userPhone));
            ui->accountTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 5, new QTableWidgetItem(accountDlg::userList.value(i).userStartDate));
            ui->accountTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 6, new QTableWidgetItem(accountDlg::userList.value(i).userEndDate));
            ui->accountTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 7, new QTableWidgetItem(accountDlg::userList.value(i).userEnable));
            ui->accountTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        }
    }
    else
    {
        QMessageBox::warning(NULL, "提示", "删除账户失败，请重试");
        return;
    }

    bool focus = ui->accountTableWidget->isItemSelected(ui->accountTableWidget->currentItem()); // 判断是否选中一行
    if (focus != true)
    {
        ui->deletePushButton->setDisabled(true);
        ui->deletePushButton->setStyleSheet("QPushButton{background-color: gray; color: white; border-radius:10px; border: 2px groove gray; border-style: outset;}"\
                                            "QPushButton:hover{background-color:white; color: black;}"\
                                            "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");

        ui->modifyPushButton->setDisabled(true);
        ui->modifyPushButton->setStyleSheet("QPushButton{background-color: gray; color: white; border-radius:10px; border: 2px groove gray; border-style: outset;}"\
                                            "QPushButton:hover{background-color:white; color: black;}"\
                                            "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");
    }
}

/*查询账户信息*/
void accountDlg::on_searchPushButton_clicked()
{
    /* 查找用户名或邮箱, 并插入列表 */
    if (!ui->searchLineEdit->text().isEmpty())  //输入不为空
    {
        /* 删除所有账户信息*/
        ui->accountTableWidget->clearContents();
        ui->accountTableWidget->setRowCount(0); //删除行

        bool focus = ui->accountTableWidget->isItemSelected(ui->accountTableWidget->currentItem()); // 判断是否选中一行
        if (focus != true)
        {
            ui->deletePushButton->setDisabled(true);
            ui->deletePushButton->setStyleSheet("QPushButton{background-color: gray; color: white; border-radius:10px; border: 2px groove gray; border-style: outset;}"\
                                                "QPushButton:hover{background-color:white; color: black;}"\
                                                "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");

            ui->modifyPushButton->setDisabled(true);
            ui->modifyPushButton->setStyleSheet("QPushButton{background-color: gray; color: white; border-radius:10px; border: 2px groove gray; border-style: outset;}"\
                                                "QPushButton:hover{background-color:white; color: black;}"\
                                                "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");
        }

        for (int i = 0; i < accountDlg::userList.size(); i++)
        {
            if ((ui->searchLineEdit->text() == accountDlg::userList.value(i).user) || (ui->searchLineEdit->text() == accountDlg::userList.value(i).userMail))
            {
                int row_count = ui->accountTableWidget->rowCount(); //获取表单行数
                ui->accountTableWidget->insertRow(row_count);       //插入新行

                ui->accountTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
                ui->accountTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
                ui->accountTableWidget->setItem(row_count, 1, new QTableWidgetItem(accountDlg::userList.value(i).user));
                ui->accountTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
                ui->accountTableWidget->setItem(row_count, 2, new QTableWidgetItem(accountDlg::userList.value(i).password));
                ui->accountTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
                ui->accountTableWidget->setItem(row_count, 3, new QTableWidgetItem(accountDlg::userList.value(i).userMail));
                ui->accountTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
                ui->accountTableWidget->setItem(row_count, 4, new QTableWidgetItem(accountDlg::userList.value(i).userPhone));
                ui->accountTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
                ui->accountTableWidget->setItem(row_count, 5, new QTableWidgetItem(accountDlg::userList.value(i).userStartDate));
                ui->accountTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
                ui->accountTableWidget->setItem(row_count, 6, new QTableWidgetItem(accountDlg::userList.value(i).userEndDate));
                ui->accountTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
                ui->accountTableWidget->setItem(row_count, 7, new QTableWidgetItem(accountDlg::userList.value(i).userEnable));
                ui->accountTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            }
        }
        if (ui->accountTableWidget->rowCount() < 1)
        {
            QMessageBox::warning(NULL, "提示", "该用户不存在，请重新查找");
        }
    }
    else
    {
        QMessageBox::warning(NULL, "提示", "输入为空，请重新输入");
    }
}

/*所有账户信息*/
void accountDlg::on_allUserPushButton_clicked()
{
    signalFlag = false;

    /* 删除所有账户信息*/
    ui->accountTableWidget->clearContents();
    ui->accountTableWidget->setRowCount(0); //删除行

    /*查询当前所有的用户信息*/
    dataJson->queryUserObj= dataJson->jsonPackQueryUser();
    dataHttp->httpPost(QUERY_USER_URL, dataJson->queryUserObj); //http请求

    /*http请求*/
    connect(this, SIGNAL(transmitUserDataSignal(QByteArray)), this, SLOT(onUserDataParse(QByteArray)));

    bool focus = ui->accountTableWidget->isItemSelected(ui->accountTableWidget->currentItem()); // 判断是否选中一行
    if (focus != true)
    {
        ui->deletePushButton->setDisabled(true);
        ui->deletePushButton->setStyleSheet("QPushButton{background-color: gray; color: white; border-radius:10px;font:12pt;font:bold; border: 2px groove gray; border-style: outset;}"\
                                            "QPushButton:hover{background-color:white; color: black;}"\
                                            "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");

        ui->modifyPushButton->setDisabled(true);
        ui->modifyPushButton->setStyleSheet("QPushButton{background-color: gray; color: white; border-radius:10px;font:12pt;font:bold; border: 2px groove gray; border-style: outset;}"\
                                            "QPushButton:hover{background-color:white; color: black;}"\
                                            "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");
    }
}

/*删除账户*/
void accountDlg::on_deletePushButton_clicked()
{
    signalFlag = true;

    stJsonData userTmp =  accountDlg::userList[accountDlg::deleteRow];
    if (userTmp.user == logon::userNameInfo)
    {
        QMessageBox::warning(NULL, "提示", "无法完成删除该账号！");
        return;
    }

    QMessageBox Msg(QMessageBox::Question, QString("提示"), QString("请确认，是否要删除该用户？"));
    QAbstractButton *pYesBtn = (QAbstractButton*)Msg.addButton(QString("确定"), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = (QAbstractButton*)Msg.addButton(QString("取消"), QMessageBox::NoRole);
    Msg.exec();

    if (Msg.clickedButton() != pNoBtn)
    {
        /*删除用户*/
        dataJson->deleteUserObj = dataJson->jsonPackDelete(userTmp.user, userTmp.password);
        dataHttp->httpPost(DELETE_USER_URL , dataJson->deleteUserObj); //http请求

        /*请求*/
        connect(this, SIGNAL(transmitDeleteUserDataSignal(QByteArray)), this, SLOT(onDeleteUserDataParse(QByteArray)));

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

void accountDlg::on_accountTableWidget_cellClicked(int row, int column)
{
    qDebug() << "当前选中的行、列数：" << row << column;

    int at = ui->accountTableWidget->item(ui->accountTableWidget->currentRow(), 0)->text().toInt();
    accountDlg::deleteRow = at - 1;

    qDebug() << "当前选中的deleteRow："<< accountDlg::deleteRow;

    ui->deletePushButton->setEnabled(true);
    ui->deletePushButton->setStyleSheet("QPushButton{background-color: goldenrod; color: white; border-radius:10px; font:12pt;font:bold; border: 2px groove gray; border-style: outset;}"\
                                        "QPushButton:hover{background-color: deepskyblue; color: black;}"\
                                        "QPushButton:pressed{background-color:seagreen; border-style: inset; }");

    ui->modifyPushButton->setEnabled(true);
    ui->modifyPushButton->setStyleSheet("QPushButton{background-color: green; color: white; border-radius:10px; font:12pt;font:bold; border: 2px groove gray; border-style: outset;}"\
                                        "QPushButton:hover{background-color:deepskyblue; color: black;}"\
                                        "QPushButton:pressed{background-color:yellowgreen; border-style: inset; }");
}

/*修改账号信息*/
void accountDlg::on_modifyPushButton_clicked()
{
    /*取消信号与槽的关联*/
    disconnect(this, SIGNAL(transmitUserDataSignal(QByteArray)), this, SLOT(onUserDataParse(QByteArray)));
    disconnect(this, SIGNAL(transmitDeleteUserDataSignal(QByteArray)), this, SLOT(onDeleteUserDataParse(QByteArray)));

    accountDlg::modifyFlag =true;  //修改用户
    accountDlg::modifyInfoData  = accountDlg::userList[accountDlg::deleteRow];;

    registerDlg dlg;
    dlg.exec();

    bool focus = ui->accountTableWidget->isItemSelected(ui->accountTableWidget->currentItem()); // 判断是否选中一行
    if (focus != true)
    {
        ui->deletePushButton->setDisabled(true);
        ui->deletePushButton->setStyleSheet("QPushButton{background-color: gray; color: white; border-radius:10px;font:12pt;font:bold; border: 2px groove gray; border-style: outset;}"\
                                            "QPushButton:hover{background-color:white; color: black;}"\
                                            "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");

        ui->modifyPushButton->setDisabled(true);
        ui->modifyPushButton->setStyleSheet("QPushButton{background-color: gray; color: white; border-radius:10px; font:12pt;font:bold;border: 2px groove gray; border-style: outset;}"\
                                            "QPushButton:hover{background-color:white; color: black;}"\
                                            "QPushButton:pressed{background-color:rgb(14 , 150 , 254); border-style: inset; }");
    }

    qDebug() << "accountDlg modify 修改的标志：" << registerDlg::addSuccessFlag;
    qDebug() << "accountDlg modify 修改的用户：" << accountDlg::userList.last().user;
    if (accountDlg::userList.last().user == logon::userNameInfo)
    {
        qDebug()<< "OK";
        accountDlg::modifyAdmin = true;
    }

    if (registerDlg::addSuccessFlag == true) //添加成功后，更新列表
    {
        /* 删除所有账户信息*/
        ui->accountTableWidget->clearContents();
        ui->accountTableWidget->setRowCount(0); //删除行

        /* 动态插入*/
        for (int i = 0; i < accountDlg::userList.size(); i++)
        {
            int row_count = ui->accountTableWidget->rowCount(); //获取表单行数
            ui->accountTableWidget->insertRow(row_count);       //插入新行

            ui->accountTableWidget->setItem(row_count, 0, new QTableWidgetItem(QString::number(i+1, 10)));
            ui->accountTableWidget->item(row_count, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 1, new QTableWidgetItem(accountDlg::userList.value(i).user));
            ui->accountTableWidget->item(row_count, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 2, new QTableWidgetItem(accountDlg::userList.value(i).password));
            ui->accountTableWidget->item(row_count, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 3, new QTableWidgetItem(accountDlg::userList.value(i).userMail));
            ui->accountTableWidget->item(row_count, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 4, new QTableWidgetItem(accountDlg::userList.value(i).userPhone));
            ui->accountTableWidget->item(row_count, 4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 5, new QTableWidgetItem(accountDlg::userList.value(i).userStartDate));
            ui->accountTableWidget->item(row_count, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 6, new QTableWidgetItem(accountDlg::userList.value(i).userEndDate));
            ui->accountTableWidget->item(row_count, 6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
            ui->accountTableWidget->setItem(row_count, 7, new QTableWidgetItem(accountDlg::userList.value(i).userEnable));
            ui->accountTableWidget->item(row_count, 7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中
        }
    }
}
