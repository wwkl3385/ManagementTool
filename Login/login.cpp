#include "Login/login.h"
#include "ui_login.h"
#include "loading/loadingdlg.h"
#include "management/managementtool.h"
#include <QLine>
#include <QBitmap>
#include <QMessageBox>
#include <QDebug>
#include <QNetworkReply>
#include <QProgressDialog>


#define  LOGIN_URL    "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/login"           //登录url
#define  UPDATE_URL   "http://D-BJ-3rdCOM.chinacloudapp.cn:1195/roam/query_update"    //更新url

static int n = 0;             //记录创建的次数
QString Login::userNameInfo;  //登录名
QString Login::userPassInfo;  //密码
unsigned Login::userType = 0; //登录类型--1：管理员，--2：普通用户

bool signalFlag = true;  //登录(false)，升级(true)，请求标志


Login::Login(QWidget *parent) :
    QWidget(parent),
    dataJson(new jsonManage),
    dataHttp(new httpManage),
    pLoadDlg(new loadingDlg(this)),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    qDebug() <<"LoginDlg创建的次数："<< ++n;

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );
    pLoadDlg->hide();

ui->loginPushButton->setShortcut(Qt::Key_Enter);//将字母区回车键与登录按钮绑定在一起

    /*圆角窗口*/
    QPixmap pixmap(":/new/background/tgood.png");
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    setPalette(palette);
    resize(pixmap.size());
    setMask(pixmap.mask());
    qDebug() << "x坐标位置：" << this->pos();

    /*绑定enter键*/
    connect(ui->passwordLineEdit, SIGNAL(returnPressed()), ui->loginPushButton, SIGNAL(clicked(bool)), Qt::UniqueConnection);

    pTimer = new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(onConnectError()));
    pTimer->start(1000);     //1s

    QString version = "1.2.3";

    /*连接服务器,检测是否更新*/
//    dataJson->updateObj= dataJson->jsonPackUpdate(version);
//    dataHttp->httpPost(UPDATE_URL, dataJson->updateObj); //http更新请求

    /*http请求*/
    connect(this->dataHttp->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    connect(this, SIGNAL(transmitUpdateSignal(QByteArray)), this, SLOT(updateDataParse(QByteArray)));
    connect(this, SIGNAL(transmitSignal(QByteArray)), this, SLOT(loginDataParse(QByteArray)));
}

Login::~Login()
{
    delete ui;
    delete dataJson;
    delete dataHttp;

    qDebug()<< "销毁 LoginDlg";
    qDebug() <<"LoginDlg还存在的个数："<< --n;
}

/*窗口移动操作*/
void Login::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_Drag = true;
        m_DragPosition = event->globalPos() - this->pos();
        event->accept();
    }
}

void Login::mouseMoveEvent(QMouseEvent *event)
{
    if (m_Drag && (event->buttons() && Qt::LeftButton))
    {
        move(event->globalPos() - m_DragPosition);
        event->accept();
    }
}

void Login::mouseReleaseEvent(QMouseEvent *)
{
    m_Drag = false;
    qDebug() << "坐标位置：" << this->pos();
    qDebug() << "x位置：" << this->pos().x();
}

void Login::on_loginPushButton_clicked()
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

    pLoadDlg->exec(); //动态登录
}

void Login::replyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();

        pLoadDlg->close(); //关闭动态登录显示窗口
        QMessageBox::warning(this, "提示", "连接服务器超时，请重试！");
        delete pLoadDlg;

        return;
    }
    QByteArray tempBuf = reply->readAll();

    if (signalFlag == true)
        emit transmitUpdateSignal(tempBuf);  //更新
    else
        emit transmitSignal(tempBuf);
}

void Login::loginDataParse(QByteArray tmpData)
{
    qDebug() <<"传递返回的值："<< tmpData;
    int status = dataJson->jsonParseData(tmpData,"status").first().toInt();

    qDebug() <<"status返回解析值："<< status ;

    if (status == 1) //登录成功
    {
        pLoadDlg->close(); //关闭动态登录显示窗口
        delete pLoadDlg;

        /*登录校验*/
        int userType = dataJson->jsonParseData(tmpData,"user_type").first().toInt(); //管理员用户

        Login::userNameInfo = ui->userLineEdit->text();
        Login::userPassInfo = ui->passwordLineEdit->text();  //密码
        Login::userType = userType; //管理员&普通用户

        qDebug() <<"userNameInfo返回解析值："<< Login::userNameInfo;
        qDebug() <<"userType返回解析值："<< userType << "管理员";


        ManagementTool *manageDlg = new ManagementTool;
        this->close();
        manageDlg->show();
    }
    else
    {
        pLoadDlg->close(); //关闭动态登录显示窗口
        QMessageBox::critical(this, "错误", "请输入正确的用户名和密码!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }
}

void Login::updateDataParse(QByteArray tmpData)
{
    qDebug() <<"更新传递返回的值："<< tmpData;
    int status = dataJson->jsonParseData(tmpData,"status").first().toInt();

    qDebug() <<"status返回解析值："<< status ;
}

void Login::onConnectError()
{
    static int sec = 0;
    static int min = 0;
    QString str_time;

    str_time.sprintf("%d分：%d秒", min, sec);
//    qDebug() << "分：秒"<<  str_time;

    if (sec < 59)
        sec++;
    else
    {
        ++ min;
        sec = 0;
    }

//    qDebug() <<"Login存在："<< sec ;
}

