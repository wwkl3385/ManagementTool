/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： downloadmanager.cpp
* 描    述： http下载--下载更新文件
* 修改记录：
* 			V0.9，2018-01-16，new，刘卫明
*
***********************************************************************/
#include "downloadmanager.h"
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>

#define DOWNLOAD_FILE_SUFFIX   "_tmp"

downloadManager::downloadManager(QObject *parent)
    : QObject(parent)
    , m_url(QUrl(""))
    , m_fileName("")
    , m_bytesReceived(0)
    , m_bytesTotal(0)
    , m_bytesCurrentReceived(0)
    , m_networkManager(NULL)
{
    m_networkManager = new QNetworkAccessManager(this);
}

downloadManager::~downloadManager()
{

}

// 获取当前下载链接;
QString downloadManager::getDownloadUrl(QUrl m_url)
{
    return m_url.toString();
}

// 开始下载文件，传入下载链接和文件的路径;
void downloadManager::downloadFile(QString url , QString fileName)
{
        m_url = QUrl(url);
        m_fileName = fileName + DOWNLOAD_FILE_SUFFIX; // 将当前文件名设置为临时文件名，下载完成时修改回来;

        // 如果当前下载的字节数为0那么说明未下载过或者重新下载
        // 则需要检测本地是否存在之前下载的临时文件，如果有则删除
        if (m_bytesCurrentReceived <= 0)
        {
            removeFile(m_fileName);
        }

        QNetworkRequest request;
        request.setUrl(m_url);

        m_reply = m_networkManager->get(request); // 请求下载;
        connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));
        connect(m_reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(m_reply, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
}

/*下载进度信息*/
void downloadManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_bytesReceived = bytesReceived;
    m_bytesTotal = bytesTotal;
    qDebug() << "文件总大小：" <<bytesTotal;

    /*更新下载进度;(加上 m_bytesCurrentReceived 是为了断点续传时之前下载的字节)*/
    emit signalDownloadProcess(m_bytesReceived + m_bytesCurrentReceived, m_bytesTotal + m_bytesCurrentReceived);
}

/*获取下载内容，保存到文件中*/
void downloadManager::onReadyRead()
{
    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        file.write(m_reply->readAll());
    }
    file.close();
}

/*下载完成*/
void downloadManager::onFinished()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute); // http请求状态码;

    qDebug() << m_reply->error();
    if (m_reply->error() == QNetworkReply::NoError)
    {
        QFileInfo fileInfo(m_fileName); // 重命名临时文件;
        if (fileInfo.exists())
        {
            int index = m_fileName.lastIndexOf(DOWNLOAD_FILE_SUFFIX);
            QString realName = m_fileName.left(index);

            this->removeFile(realName); //如果旧文件存在，删除旧文件

            QFile::rename(m_fileName, realName);
        }
    }
    else
    {
        QString strError = m_reply->errorString(); // 有错误输出错误;
        qDebug() << "__________" + strError;
    }

    emit signalReplyFinished(statusCode.toInt());
}

/*下载过程中出现错误，关闭下载，并上报错误，这里未上报错误类型，可自己定义进行上报*/
void downloadManager::onError(QNetworkReply::NetworkError code)
{
    QString strError = m_reply->errorString();
    qDebug() << "__________" + code + strError;

    closeDownload();
    emit signalDownloadError();
}

/*停止下载工作*/
void downloadManager::stopWork()
{
    if (m_reply != NULL)
    {
        disconnect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));
        disconnect(m_reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        disconnect(m_reply, SIGNAL(finished()), this, SLOT(onFinished()));
        disconnect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = NULL;
    }
}

/*重置参数*/
void downloadManager::reset()
{
    m_bytesCurrentReceived = 0;
    m_bytesReceived = 0;
    m_bytesTotal = 0;
}

/*删除文件*/
void downloadManager::removeFile(QString fileName)
{
    QFileInfo fileInfo(fileName);
    if (fileInfo.exists())   // 删除已下载的临时文件;
    {
        QFile::remove(fileName);
    }
    else
        return;
}

/*停止下载按钮被按下，关闭下载，重置参数，并删除下载的临时文件*/
void downloadManager::closeDownload()
{
    stopWork();
    reset();
    removeFile(m_fileName);
}
