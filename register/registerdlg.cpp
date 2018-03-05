/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称：  registerDlg.cpp
* 描    述：  添加账户，修改账户
* 修改记录：
* 			V0.9，2017-12-28，new，刘卫明
*
***********************************************************************/
#include "management/managementtool.h"
#include "accountManage/accountdlg.h"
#include "register/registerdlg.h"
#include "ui_registerdlg.h"
#include <logon/logon.h>
#include <QNetworkReply>
#include <QMessageBox>
#include <QDebug>
#include <QDate>

#define  ADD_URL      "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/add_user"        //添加url
#define  MODIFY_URL   "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/modify_user"     //修改url

stJsonData stData; //注册的用户信息
bool registerDlg::addSuccessFlag = false; //添加成功 true：成功
QString registerDlg::newPassword = "";    //修改后的密码.初始化

registerDlg::registerDlg(QWidget *parent) :
    QDialog(parent),
    dataJson(new jsonManage),
    dataHttp(new httpManage),
    ui(new Ui::registerDlg)
{
    //this->setAttribute(Qt::WA_DeleteOnClose, true) ; //自动销毁
    ui->setupUi(this);
    setWindowTitle("注册账户");

    qDebug() << "创建registerDlg!";

    qDebug() << "注册Dlg:" <<logon::userNameInfo;
    qDebug() << "注册Dlg:" <<logon::userPassInfo; //密码
    qDebug() << "注册Dlg:" <<logon::userType; // 用户类型

    //qDebug() << "修改标志static："<<accountDlg::modifyFlag;
    //qDebug() << "用户信息static："<<accountDlg::modifyInfoData.password;
    //qDebug() << "修改标志userList："<< accountDlg::userList.first().user;

    QDate currentDate = QDate::currentDate();
    QString currentDateStr = currentDate.toString("yyyy-MM-dd");
    qDebug() << "当前时间" << currentDateStr;
    ui->startDateLineEdit->setText(currentDateStr);
    ui->passwordLineEdit->setText("123456");

    rx.setPatternSyntax(QRegExp::RegExp);
    rx.setCaseSensitivity(Qt::CaseSensitive); //对大小写字母敏感，即区分大小写
    rx.setPattern(QString("^[A-Za-z0-9]+$")); //匹配格式为所有大小写字母和数字组成的字符串，位数不限

    rxMail.setPatternSyntax(QRegExp::RegExp);
    rxMail.setCaseSensitivity(Qt::CaseSensitive); //对大小写字母敏感，即区分大小写
    rxMail.setPattern(QString("^([a-zA-Z0-9]+[_|]?)*[a-zA-Z0-9]+@([a-zA-Z0-9]+[_|]?)*[a-zA-Z0-9]+.[a-zA-Z]{2,3}$")); //邮箱校验

    rxPhone.setPatternSyntax(QRegExp::RegExp);
    rxPhone.setCaseSensitivity(Qt::CaseSensitive); //对大小写字母敏感，即区分大小写
    rxPhone.setPattern(QString("^1[0-9]{10}$")); //电话校验

    rxDate.setPatternSyntax(QRegExp::RegExp);
    rxDate.setCaseSensitivity(Qt::CaseSensitive); //对大小写字母敏感，即区分大小写
    rxDate.setPattern(QString("(([0-9]{3}[1-9]|[0-9]{2}[1-9][0-9]{1}|[0-9]{1}[1-9][0-9]{2}|[1-9][0-9]{3})-(((0[13578]|1[02])-(0[1-9]|[12][0-9]|3[01]))|((0[469]|11)-(0[1-9]|[12][0-9]|30))|(02-(0[1-9]|[1][0-9]|2[0-8]))))|((([0-9]{2})(0[48]|[2468][048]|[13579][26])|((0[48]|[2468][048]|[3579][26])00))-02-29)")); //日期匹配


    if (accountDlg::modifyFlag== true)  //修改用户
    {
        if (logon::userType == 1 && accountDlg::deleteRow != -1)        //初始值
        {
            setWindowTitle("修改账户");
            ui->registerPushButton->setText("修改");
            ui->userLineEdit->setText(accountDlg::modifyInfoData.user);
            ui->mailLineEdit->setText(accountDlg::modifyInfoData.userMail);
            ui->phoneLineEdit->setText(accountDlg::modifyInfoData.userPhone);
            ui->passwordLineEdit->setText("");

            if (accountDlg::modifyInfoData.user == logon::userNameInfo)
            {
                ui->disenableRadioButton->hide();
                ui->enableRadioButton->hide();
                ui->operationLabel->hide();
                ui->startLabel->hide();
                ui->stopLabel->hide();
                ui->startDateLineEdit->hide();
                ui->endDateLineEdit->hide();
            }

            ui->startDateLineEdit->setText(accountDlg::modifyInfoData.userStartDate);
            ui->endDateLineEdit->setText(accountDlg::modifyInfoData.userEndDate);
            ui->userLineEdit->setDisabled(true);
        }
        else
        {
            setWindowTitle("修改账户");
            ui->registerPushButton->setText("修改");
            ui->userLineEdit->setText(logon::userNameInfo);
            ui->userLineEdit->setDisabled(true);

            ui->mailLineEdit->setText(ManagementTool::stUserInfo.userMail);
            ui->phoneLineEdit->setText(ManagementTool::stUserInfo.userPhone);
            ui->startDateLineEdit->setText(ManagementTool::stUserInfo.userStartDate);
            ui->endDateLineEdit->setText(ManagementTool::stUserInfo.userEndDate);
            ui->passwordLineEdit->setText("");

            ui->disenableRadioButton->hide();
            ui->enableRadioButton->hide();
            ui->operationLabel->hide();
            ui->startLabel->hide();
            ui->stopLabel->hide();
            ui->startDateLineEdit->hide();
            ui->endDateLineEdit->hide();
        }
    }

    /*绑定enter键*/
    connect(ui->userLineEdit, SIGNAL(returnPressed()), ui->registerPushButton, SIGNAL(clicked(bool)), Qt::UniqueConnection);
}

registerDlg::~registerDlg()
{
    delete ui;
    delete dataJson;
    delete dataHttp;

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
    QString admin;
    if (logon::userNameInfo == "admin") //超级用户
        admin = "1";  //管理员用户
    else
        admin = "2";  //普通用户
    stData.userType= admin;

    if (pwd.isEmpty() || user.isEmpty() || mail.isEmpty() ||
            phone.isEmpty() || startDate.isEmpty() || endDate.isEmpty())  //检测输入框是不是为空
    {
        QMessageBox box(QMessageBox::Information,"提示","输入内容为空，请重新输入！");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }

    if (!rx.exactMatch(pwd) || !rx.exactMatch(user))
    {
        QMessageBox box(QMessageBox::Information,"提示","用户名和密码不能为特殊符号或汉字，请重新输入！");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }

    if (accountDlg::modifyFlag == true)  //注册时默认密码为123456 ,修改时不能为123456
    {
        if (pwd.size() < 6 || pwd == "123456" )
        {
            QMessageBox box(QMessageBox::Information,"提示","该密码太简单，请重新设置！");
            box.setStandardButtons (QMessageBox::Ok);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.exec ();
            return;
        }
    }

    if (!rxMail.exactMatch(mail))
    {
        QMessageBox box(QMessageBox::Information,"提示","邮箱格式错误，请重新输入！");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }

    if (!rxPhone.exactMatch(phone))
    {
        QMessageBox box(QMessageBox::Information,"提示","电话号码格式错误，请重新输入！");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }

    if (!rxDate.exactMatch(startDate) || !rxDate.exactMatch(endDate))
    {
        QMessageBox box(QMessageBox::Information,"提示","日期格式错误，请重新输入！");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }

    QDate startDateTmp = QDate::fromString(startDate, "yyyy-MM-dd");
    QDate endDateTmp = QDate::fromString(endDate, "yyyy-MM-dd");

    if (startDateTmp >= endDateTmp || endDateTmp < QDate::currentDate())
    {
        QMessageBox box(QMessageBox::Information,"提示","日期错误，请检查开始时间与结束时间！");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }

    if (accountDlg::modifyFlag != true)
    {
        /*连接服务器, 添加用户*/
        dataJson->addUserObj = dataJson->jsonPackAddUser(logon::userNameInfo, user, pwd, mail, phone, enable, startDate, endDate, admin);
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
        QMessageBox box(QMessageBox::Information,"提示","连接服务器超时，请重试！");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }
    QByteArray tempBuf = reply->readAll();

    emit transmitRegisterSignal(tempBuf);
}

void registerDlg::onRegisterDataParse(QByteArray tmpData)
{
    disconnect(this, SIGNAL(transmitRegisterSignal(QByteArray)), this, SLOT(onRegisterDataParse(QByteArray)));
        qDebug() <<"注册信息返回的值："<< tmpData;
    //    qDebug() <<"传递返回的值status："<< dataJson->jsonParseData(tmpData,"status").isEmpty();
    //    qDebug() <<"解析后得名字："<< dataJson->jsonParseParam(tmpData,"user").first();

    if (dataJson->jsonParseData(tmpData,"status").isEmpty())
    {
        QMessageBox box(QMessageBox::Information,"提示","添加失败，请重试！");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }
    else
    {
        switch (dataJson->jsonParseData(tmpData,"status").first().toInt())
        {
        case 0:
        {
            QMessageBox box(QMessageBox::Information,"提示","用户名已存在，请重试！");
            box.setStandardButtons (QMessageBox::Ok);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.exec ();
            break;
        }
        case 1:
        {
            QMessageBox box(QMessageBox::Information,"提示","添加成功！");
            box.setStandardButtons (QMessageBox::Ok);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.exec ();
            registerDlg::addSuccessFlag = true;
            accountDlg::userList.append(stData);
            accountDlg::deleteRow = -1;       //添加成功后设置为默认值
            qDebug() << "添加的用户：" << stData.user;
            this->close();
            break;
        }
        case 2:
        {
            QMessageBox box(QMessageBox::Information,"提示","该用户名已存在！");
            box.setStandardButtons (QMessageBox::Ok);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.exec ();
            break;
        }
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
        QMessageBox box(QMessageBox::Information,"提示","修改失败，请重试！");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
    }
    else
    {

        switch (dataJson->jsonParseData(tmpData,"status").first().toInt())
        {
        case 0:
        {
            QMessageBox box(QMessageBox::Information,"提示","修改失败，请重试！");
            box.setStandardButtons (QMessageBox::Ok);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.exec ();
            break;
        }
        case 1:
        {
            QMessageBox box(QMessageBox::Information,"提示","修改成功！");
            box.setStandardButtons (QMessageBox::Ok);
            box.setButtonText (QMessageBox::Ok,QString("确 定"));
            box.exec ();
            registerDlg::addSuccessFlag = true;
            registerDlg::newPassword = ui->passwordLineEdit->text();  //修改后的密码
            accountDlg::userList.removeAt(accountDlg::deleteRow);
            qDebug () << "删除选中的行：" <<  accountDlg::deleteRow;
            accountDlg::userList.append(stData);

            accountDlg::modifyAdmin = false;  //修改管理员账户密码
            accountDlg::deleteRow = -1;       //修改成功后设置为默认值
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
        QMessageBox box(QMessageBox::Warning,"提示","连接服务器超时，请重试！");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }
    QByteArray tempBuf = reply->readAll();

    emit transmitModifySignal(tempBuf);
}
