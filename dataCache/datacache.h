#ifndef DATACACHE_H
#define DATACACHE_H

#include <QObject>

/*json内容:用户数据--解析数据结构*/
typedef struct ST_JSONDATA
{
    QString userType;     //登录认证：查询成功才有 管理员为1。

    QString user;        //查询当前所有的用户信息
    QString password;
    QString userMail;
    QString userPhone;
    QString userEnable;
    QString userStartDate;
    QString userEndDate;
}stJsonData;

typedef QList<stJsonData> dataList;

#if 0
/*json内容:设备信息--解析数据结构*/
typedef struct ST_JSONDATA_DEV
{
    QString userId;
    QString userIdMac;
    QString userIdAddr1;
    QString userIdAddr2;
    QString userIdAddr3;

    QString logTrustedIp;
    QString logRemoteIp;
    QString logStartTime;
}stJsonDataDevice;
typedef QList<stJsonDataDevice> deviceDataList;

#endif

class dataCache : public QObject
{
    Q_OBJECT
public:
    explicit dataCache(QObject *parent = nullptr);

};

#endif // DATACACHE_H
