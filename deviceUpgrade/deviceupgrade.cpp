#include "deviceupgrade.h"
#include "ui_deviceupgrade.h"

#include <QHostAddress>
#include <QTextCodec>
#include <QFileDialog>

deviceUpgrade::deviceUpgrade(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::deviceUpgrade)
{
    ui->setupUi(this);
    ui->pathInfoLine->setDisabled(true);
    ui->stationLineEdit->setDisabled(true);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timer->start(1000);
}

deviceUpgrade::~deviceUpgrade()
{
    delete ui;
}

void deviceUpgrade::on_updateConfirmButton_clicked()
{

}

void deviceUpgrade::timerUpdate()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd\thh:mm:ss\t\tdddd");
    ui->messageBrower->setText(str);
}

void deviceUpgrade::on_selectFileButton_clicked()
{
    ui->messageBrower->setText(tr("正在打开文件..."));
    ui->messageBrower->append(tr("正在打开文件..."));

    //ui->progressBar->setValue(0);  //非第一次发送

    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();

    fileName = QFileDialog::getOpenFileName(this);
    localFile = new QFile(fileName);
    localFile->open(QFile::ReadOnly);

    ui->pathInfoLine->show();
    ui->pathInfoLine->setText(tr("%1").arg(fileName));
}

void deviceUpgrade::on_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
    {
        ui->stationLineEdit->setDisabled(true);
        ui->stationLineEdit->setPlaceholderText("请确认是否要全部升级？？？");
        break;
    }
    case 1:
    {
        ui->stationLineEdit->setEnabled(true);
        ui->stationLineEdit->setPlaceholderText("请输入站地址");
        break;
    }
    case 2:
    {
        ui->stationLineEdit->setEnabled(true);
        ui->stationLineEdit->setPlaceholderText("请输入版本号");
        break;
    }
    default:
        break;
    }

}
