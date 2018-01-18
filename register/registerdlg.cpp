/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称：  registerDlg.cpp
* 描    述：  添加账户，修改账户
* 修改记录：
* 			V0.9，2017-12-28，new，刘卫明
*
***********************************************************************/
#include "register/registerdlg.h"
#include "accountManage/accountdlg.h"
#include "ui_registerdlg.h"
#include <QNetworkReply>
#include <QMessageBox>
#include <QDebug>
#include <QDate>

#define  ADD_URL      "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/add_user"        //添加url
#define  MODIFY_URL   "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/modify_user"     //修改url

stJsonData stData; //注册的用户信息
bool registerDlg::addSuccessFlag = false; //添加成功 true：成功

registerDlg::registerDlg(QWidget *parent) :
    QDialog(parent),
    dataJson(new jsonManage),
    dataHttp(new httpManage),
    ui(new Ui::registerDlg)
{
//    this->setAttribute(Qt::WA_DeleteOnClose, true) ; //自动销毁
    ui->setupUi(this);
    setWindowTitle("注册账户");

    qDebug() << "创建registerDlg!";
    qDebug() << "修改标志static："<<accountDlg::modifyFlag;
    qDebug() << "用户信息static："<<accountDlg::modifyInfoData.password;
    qDebug() << "修改标志userList："<< accountDlg::userList.first().user;

    QDate currentDate = QDate::currentDate();
    QString currentDateStr = currentDate.toString("yyyy-MM-dd");
    qDebug() << "当前时间" << currentDateStr;
    ui->startDateLineEdit->setText(currentDateStr);

    rx.setPatternSyntax(QRegExp::RegExp);
    rx.setCaseSensitivity(Qt::CaseSensitive); //对大小写字母敏感，即区分大小写
    rx.setPattern(QString("^[A-Za-z0-9]+$")); //匹配格式为所有大小写字母和数字组成的字符串，位数不限

    if (accountDlg::modifyFlag== true)  //修改用户
    {
        setWindowTitle("修改账户");
        ui->cancelPushButton->setText("取消修改");
        ui->registerPushButton->setText("确认修改");
        ui->userLineEdit->setText(accountDlg::modifyInfoData.user);
        ui->mailLineEdit->setText(accountDlg::modifyInfoData.userMail);
        ui->phoneLineEdit->setText(accountDlg::modifyInfoData.userPhone);
        ui->startDateLineEdit->setText(accountDlg::modifyInfoData.userStartDate);
        ui->endDateLineEdit->setText(accountDlg::modifyInfoData.userEndDate);
        ui->userLineEdit->setDisabled(true);
    }

    /*绑定enter键*/
    connect(ui->userLineEdit, SIGNAL(returnPressed()), ui->registerPushButton, SIGNAL(clicked(bool)), Qt::UniqueConnection);

}

registerDlg::~registerDlg()
{
    delete ui;
    qDebug() << "销毁registerDlg!";
}

void registerDlg::on_registerPushButton_clicked()
{
    QString user = ui->userLineEdit->text();
    stData.user = user;
    QString pwd = ui->passwordLineEdit->text();
    stData.password= pwd;

    QString mail= ui->mailLineEdit->text();
    stData.userMail= mail;
    QString phone= ui->phoneLineEdit->text();
    stData.userPhone= phone;
    QString enable= ui->enableRadioButton->isChecked() == false ? "0" : "1";
    stData.userEnable= enable == "1" ? "有效" : "无效";

    QString startDate = ui->startDateLineEdit->text();
    stData.userStartDate = startDate;

    QString endDate = ui->endDateLineEdit->text();
    stData.userEndDate = endDate;
    QString admin = "2";  //普通用户
    stData.userType= admin;

    if(pwd.isEmpty() || user.isEmpty() || mail.isEmpty() ||
            phone.isEmpty() || startDate.isEmpty() || endDate.isEmpty())  //检测输入框是不是为空
    {
        QMessageBox::warning(NULL, "提示", "注册输入内容为空！");
        return;
    }
    else  if(!rx.exactMatch(pwd) || !rx.exactMatch(user) || !rx.exactMatch(phone))
    {
        QMessageBox::warning(NULL, "提示", "输入信息能为特殊符号或汉字，请重新输入");
        return;
    }

    if ((stData.user.isEmpty()) || (stData.password.size() < 6) || (stData.userPhone.size() != 11) ||
            (stData.userStartDate.size() != 10) || (stData.userStartDate.at(4) != "-") || (stData.userStartDate.at(7) != "-") ||
            (stData.userEndDate.at(4) != "-") || (stData.userEndDate.at(7) != "-"))
    {
        qDebug() << "user：" << stData.user;
        qDebug() << "password：" << stData.password;
        qDebug() << "userPhone：" << stData.userPhone;
        qDebug() << "startDate：" << stData.userStartDate;
        qDebug() << "endDAte：" << stData.userEndDate;
        QMessageBox::warning(NULL, "提示", "输入格式不合法，请重新输入！");
        return;
    }

    if (accountDlg::modifyFlag != true)
    {
        /*连接服务器, 添加用户*/
        dataJson->addUserObj = dataJson->jsonPackAddUser(user, pwd, mail, phone, enable, startDate, endDate, admin);
        qDebug() << "注册 组帧：" << dataJson->addUserObj;
        dataHttp->httpPost(ADD_URL, dataJson->addUserObj);    //http添加用户请求

        /*添加用户 -- http请求*/
        connect(this->dataHttp->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyRegisterFinished(QNetworkReply*)));
        connect(this, SIGNAL(transmitRegisterSignal(QByteArray)), this, SLOT(onRegisterDataParse(QByteArray)));

    }
    else
    {
         /*连接服务器, 修改用户*/
        dataJson->modifyUserObj = dataJson->jsonPackModifyUser(user, pwd, mail, phone, enable, startDate, endDate);
        qDebug() << "修改 组帧：" << dataJson->modifyUserObj;
        dataHttp->httpPost(MODIFY_URL, dataJson->modifyUserObj);    //http修改用户请求

        /*修改用户 -- http请求*/
        connect(this->dataHttp->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyModifyFinished(QNetworkReply*)));
        connect(this, SIGNAL(transmitModifySignal(QByteArray)), this, SLOT(onModifyDataParse(QByteArray)));
    }
}

void registerDlg::onReplyRegisterFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();
        QMessageBox::warning(this, "提示", "连接服务器超时，请重试！");
        return;
    }
    QByteArray tempBuf = reply->readAll();

    emit transmitRegisterSignal(tempBuf);
}

void registerDlg::onRegisterDataParse(QByteArray tmpData)
{
    disconnect(this, SIGNAL(transmitRegisterSignal(QByteArray)), this, SLOT(onRegisterDataParse(QByteArray)));
//    qDebug() <<"注册信息返回的值："<< tmpData;
//    qDebug() <<"传递返回的值status："<< dataJson->jsonParseData(tmpData,"status").isEmpty();
//    qDebug() <<"解析后得名字："<< dataJson->jsonParseParam(tmpData,"user").first();

    if (dataJson->jsonParseData(tmpData,"status").isEmpty())
    {
        QMessageBox::warning(NULL, "提示", "添加账户失败，请重试");
        return;
    }
    else
    {
        switch (dataJson->jsonParseData(tmpData,"status").first().toInt())
        {
        case 0:
            QMessageBox::warning(NULL, "提示", "添加失败，请重试！");
            break;
        case 1:
            QMessageBox::warning(NULL, "提示", "添加成功！");
            registerDlg::addSuccessFlag = true;
            accountDlg::userList.append(stData);
            qDebug() << "添加的用户：" << stData.user;
            this->close();
            break;
        case 2:
            QMessageBox::warning(NULL, "提示", "该用户名已存在！");
            break;
        default:
            break;
        }
    }
}

void registerDlg::onModifyDataParse(QByteArray tmpData)
{
//    qDebug() << "修改返回值：" << tmpData;
//    qDebug() <<"传递返回的值status："<< dataJson->jsonParseData(tmpData,"status").isEmpty();
    disconnect(this, SIGNAL(transmitModifySignal(QByteArray)), this, SLOT(onModifyDataParse(QByteArray)));
    if (dataJson->jsonParseData(tmpData,"status").isEmpty())
    {
        QMessageBox::warning(NULL, "提示", "修改用户失败，请重试");
    }
    else
    {

        switch (dataJson->jsonParseData(tmpData,"status").first().toInt())
        {
        case 0:
        {
            QMessageBox::warning(NULL, "提示", "修改失败，请重试！");
            break;
        }
        case 1:
        {
            QMessageBox::warning(NULL, "提示", "修改成功！");
            registerDlg::addSuccessFlag = true;
            accountDlg::userList.removeAt(accountDlg::deleteRow);
            qDebug () << "删除选中的行：" <<  accountDlg::deleteRow;
            accountDlg::userList.append(stData);

            qDebug() << "修改的用户：" << stData.user;

            this->close();
            break;
        }
        default:
            break;
        }
    }
}

void registerDlg::onReplyModifyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();
        QMessageBox::warning(this, "提示", "连接服务器超时，请重试！");
        return;
    }
    QByteArray tempBuf = reply->readAll();

    emit transmitModifySignal(tempBuf);
}
