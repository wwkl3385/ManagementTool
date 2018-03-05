/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： jsonmanage.h
* 描    述： json数据--组帧--解析
* 修改记录：
* 			V0.9，2017-12-19，new，刘卫明
*
***********************************************************************/
#ifndef JSONMANAGE_H
#define JSONMANAGE_H

#include <QJsonObject>
#include <QList>
#include <QByteArray>

class jsonManage
{
public:
    jsonManage();

    QJsonObject loginObj;          //登录认证
    QJsonObject addUserObj;        //添加用户
    QJsonObject deleteUserObj;     //删除用户
    QJsonObject modifyUserObj;     //修改用户
    QJsonObject queryUserObj;      //查询当前所有用户信息
    QJsonObject queryOneUserObj;   //查询当前用户信息
    QJsonObject connectInfoObj;    //查询集控登录连接的信息
    QJsonObject connectNumberObj;  //查询在线终端数量
    QJsonObject connectAllInfoObj; //查询所有的在线终端数据
    QJsonObject updateObj;         //请求最新客户端程序

    QJsonObject jsonPackUpdate(QString version);
    QJsonObject jsonPackLogin( QString user, QString password);

    QJsonObject jsonPackAddUser(QString user, QString password, QString mail,
                                QString phone, QString enable, QString startDate, QString endDate, QString admin);

    QJsonObject jsonPackAddUser(QString adminParam, QString user, QString password, QString mail, QString phone,
                                QString enable, QString startDate, QString endDate, QString admin);
    void jsonPackAddUserAppend(QJsonObject &obj, QString user, QString password,
                               QString mail, QString phone, QString enable, QString startDate, QString endDate, QString admin);

    QJsonObject jsonPackDelete( QString user, QString password);
    void jsonPackDeleteAppend(QJsonObject &obj, QString user, QString password);

    QJsonObject jsonPackModifyUser(const QString user, QString password, QString mail,
                                   QString phone, QString enable, QString startDate, QString endDate);
    void jsonPackModifyUserAppend(QJsonObject &obj, QString user, QString password,
                                  QString mail, QString phone, QString enable, QString startDate, QString endDate);

    QJsonObject jsonPackQueryUser();
    QJsonObject jsonPackQueryUser(QString admin);

    QJsonObject jsonPackQueryOneUser(QString user);

    QJsonObject jsonPackQueryConnectInfo(QString userId);
    QJsonObject jsonPackQueryConnectNumber();
    QJsonObject jsonPackQueryConnectAllInfo();

    QString jsonParseCmd(QByteArray reply);
    QList<QString>jsonParseParam(QByteArray reply, QString name);
    QList<QString> jsonParseData(QByteArray reply, QString name);
    QString regionCodeParse(QString provinceCode,QString cityCode, QString districtCode); //行政区域码 解析

    int jsonParamSize(QByteArray reply);
    int jsonDataSize(QByteArray reply);
};

#endif // JSONMANAGE_H

