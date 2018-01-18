/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： downloadmanager.h
* 描    述： http下载--下载更新文件
* 修改记录：
* 			V0.9，2018-01-16，new，刘卫明
*
***********************************************************************/
#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class downloadManager : public QObject
{
    Q_OBJECT
public:
    explicit downloadManager(QObject *parent = nullptr);
    ~downloadManager();

    void downloadFile(QString url , QString fileName);
    void removeFile(QString fileName);
    QString getDownloadUrl(QUrl m_url);
    void closeDownload();
    void stopWork();
    void reset();

    QUrl    m_url;
    QString m_fileName;
    qint64  m_bytesReceived;
    qint64  m_bytesTotal;
    qint64  m_bytesCurrentReceived;

signals:
    signalDownloadProcess(qint64, qint64);
    signalReplyFinished(int);
    signalDownloadError();

public slots:
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onError(QNetworkReply::NetworkError code);
    void onReadyRead();
    void onFinished();

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_reply;
};

#endif // DOWNLOADMANAGER_H
