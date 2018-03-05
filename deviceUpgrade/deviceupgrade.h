#ifndef DEVICEUPGRADE_H
#define DEVICEUPGRADE_H

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QTcpSocket>
#include <QFile>
#include <string>

namespace Ui {
class deviceUpgrade;
}

class deviceUpgrade : public QWidget
{
    Q_OBJECT
public:
    explicit deviceUpgrade(QWidget *parent = 0);
    ~deviceUpgrade();

    private slots:
    void on_updateConfirmButton_clicked();
    void timerUpdate(void);

    void on_selectFileButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::deviceUpgrade *ui;
    QTcpSocket *tcpClient;
    QFile *localFile;
    QString fileName;     //文件名

    QByteArray outBlock;  //分次传
    qint64 loadSize;      //每次发送数据的大小
    qint64 byteToWrite;   //剩余数据的大小
    qint64 totalSize;     //文件总大小

    int sendTimes;  //用来标记是否为第一次发送，第一次以后连接信号触发，后面则手动调用
};
#endif // DEVICEUPGRADE_H
