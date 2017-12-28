#include "loadingdlg.h"
#include "ui_loadingdlg.h"
#include "logon/logon.h"
#include <QDebug>
#include <QMessageBox>
#include <QBitmap>

loadingDlg::loadingDlg(QWidget *parent) :
    QDialog(parent),
    sec(20),
    ui(new Ui::loadingDlg),
    pManageDlg(new ManagementTool())
{
    ui->setupUi(this);
    this->resize(475, 90);//提示框尺寸。

    pTimer = new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(delayDisplay()));
    pTimer->start(1000);//1s

    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(bmp.rect(), 20, 20);
    setMask(bmp);

    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    ui->loadingLabel->hide(); //加载图片的label
    ui->progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter); //对其方式
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);

#if 0
    /*动态图片加载*/
    movie = new QMovie(":/new/background/loading.gif");
    ui->gridLayout->setMargin(0);
    ui->loadingLabel->setMovie(movie);
    movie->start();
#endif
}

loadingDlg::~loadingDlg()
{
    delete ui;
}

void loadingDlg::delayDisplay()
{

    QString strTime;
    strTime.sprintf("正在连接服务器，倒计时:%d秒", sec--);
    qDebug() <<strTime;

    ui->timeLabel->setText(strTime);

    if (sec < 7) //登录校验
    {
        this->close();
        pManageDlg->show();
    }
    else if(sec == 5)
    {
       QMessageBox::warning(NULL, "连接超时", "连接超时，请重新连接");
       this->close();
       logon lg;
       lg.exec();
    }
}
