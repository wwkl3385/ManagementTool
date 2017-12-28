#include "./logon/logon.h"
#include "ui_logon.h"
#include "loading/loadingdlg.h"
#include <QBitmap>
#include <QMessageBox>
#include <QDebug>
#include "./management/managementtool.h"

logon::logon(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logon)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    /*圆角窗口*/
    QPixmap pixmap(":/new/background/tgood.png");
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    setPalette(palette);
    resize(pixmap.size());
    setMask(pixmap.mask());

    /*绑定enter键*/
    connect(ui->passwordLineEdit, SIGNAL(returnPressed()), ui->logonPushButton, SIGNAL(clicked(bool)), Qt::UniqueConnection);
}

logon::~logon()
{
    delete ui;
    qDebug()<< "xiaohui";
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
}

void logon::on_logonPushButton_clicked()
{
    /*添加等待超时。服务器连接。*/
//    if ((ui->userLineEdit->text() == "teld") && (ui->passwordLineEdit->text() == "teld"))
    {
        loadingDlg load;
        load.exec();
        this->close();
    }
//    else
    //        QMessageBox::warning(this, "提示", "用户名或密码错误，请重新输入！", QMessageBox::Yes | QMessageBox::No);
}
