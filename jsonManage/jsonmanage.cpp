/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： jsonmanage.cpp
* 描    述： json数据--组帧--解析
* 修改记录：
* 			V0.9，2017-12-19，new，刘卫明
*
***********************************************************************/
#include "jsonmanage.h"
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonParseError>
#include<QDebug>

jsonManage::jsonManage()
{

}

  /*软件更新*/
QJsonObject jsonManage::jsonPackUpdate(QString version)
{
    QJsonObject updateObj;
    QJsonObject updateObjParam, updateObjData;
    QJsonArray  updateParamArray, updateDataArray;

    updateObjParam.insert("version", version);

    updateDataArray.append(updateObjData);
    updateParamArray.append(updateObjParam);

    updateObj.insert("cmd", QString("query_update"));
    updateObj.insert("param", updateParamArray);
    updateObj.insert("data", updateDataArray);

    return updateObj;
}

    /*登录认证*/
QJsonObject jsonManage::jsonPackLogin( QString user, QString password)
{
    QJsonObject loginObj;
    QJsonObject loginObjParam, loginObjData;
    QJsonArray  paramArray, dataArray;

    loginObjParam.insert("user", user);
    loginObjParam.insert("password", password);

    paramArray.append(loginObjParam);
    dataArray.append(loginObjData);

    loginObj.insert("cmd", QString("login"));
    loginObj.insert("param", paramArray);
    loginObj.insert("data", dataArray);

    return loginObj;
}

    /*添加用户*/
QJsonObject jsonManage::jsonPackAddUser(QString user, QString password, QString mail, QString phone,
                                        QString enable, QString startDate, QString endDate, QString admin)
{
    QJsonObject addObj;
    QJsonObject addObjParam,   addObjData;
    QJsonArray  addDataArray;
    QJsonArray  addParamArray;

    qDebug() << "默认添加用户：" << user;
//    qDebug() << "转码utf8用户名：" << user.toUtf8().data();
    qDebug() << "转码utf8用户名：" << user.toUtf8();

    addObjData.insert("user", user.toUtf8().data());//用户名
//    addObjData.insert("user", user.toUtf8());//用户名
    addObjData.insert("password", password);        //密码
    addObjData.insert("user_mail", mail);
    addObjData.insert("user_phone", phone);
    addObjData.insert("user_enable", enable.toInt());
    addObjData.insert("user_start_date", startDate);
    addObjData.insert("user_end_date", endDate);
    addObjData.insert("user_type", admin.toInt());
    addDataArray.append(addObjData);
    addParamArray.append(addObjParam);

    addObj.insert("cmd", QString("add_user"));
    addObj.insert("param", addParamArray);
    addObj.insert("data", addDataArray);

    return addObj;
}

    /*追加多个用户*/
void jsonManage::jsonPackAddUserAppend(QJsonObject &obj, QString user, QString password, QString mail, QString phone, QString enable, QString startDate, QString endDate, QString admin)
{
    QJsonObject addObjParam,   addObjData;
    QJsonArray  addDataArray = obj.take("data").toArray();
    QJsonArray  addParamArray;

    addObjData.insert("user", user.toUtf8().data());  //用户名
    addObjData.insert("password", password);          //密码
    addObjData.insert("user_mail", mail);
    addObjData.insert("user_phone", phone);
    addObjData.insert("user_enable", enable.toInt());
    addObjData.insert("user_start_date", startDate);
    addObjData.insert("user_end_date", endDate);
    addObjData.insert("user_type", admin.toInt());
    addDataArray.append(addObjData);
    addParamArray.append(addObjParam);

    obj.insert("cmd", QString("add_user"));
    obj.insert("param", addParamArray);
    obj.insert("data", addDataArray);
}

    /*删除用户*/
QJsonObject jsonManage::jsonPackDelete(QString user, QString password)
{
    QJsonObject deleteObj;
    QJsonObject deleteObjParam, deleteObjData;
    QJsonArray  paramArray, dataArray;

    deleteObjParam.insert("user", user);
    deleteObjParam.insert("password", password);

    paramArray.append(deleteObjParam);
    dataArray.append(deleteObjData);

    deleteObj.insert("cmd", QString("delete_user"));
    deleteObj.insert("param", paramArray);
    deleteObj.insert("data", dataArray);

    return deleteObj;
}

    /*追加删除多个用户*/
void jsonManage::jsonPackDeleteAppend(QJsonObject &obj, QString user, QString password)
{
    QJsonObject deleteObjParam, deleteObjData;
    QJsonArray  dataArray;
    QJsonArray  paramArray = obj.take("param").toArray();

    deleteObjParam.insert("user", user);
    deleteObjParam.insert("password", password);

    paramArray.append(deleteObjParam);
    dataArray.append(deleteObjData);

    obj.insert("cmd", QString("delete_user"));
    obj.insert("param", paramArray);
    obj.insert("data", dataArray);
}


    //修改用户
QJsonObject jsonManage::jsonPackModifyUser(const QString user, QString password, QString mail,
                                           QString phone, QString enable, QString startDate, QString endDate)
{
    QJsonObject modifyObj;
    QJsonObject modifyObjParam,   modifyObjData;
    QJsonArray  modifyParamArray, modifyDataArray;

    modifyObjData.insert("password", password);        //密码
    modifyObjData.insert("user_mail", mail);
    modifyObjData.insert("user_phone", phone);
    modifyObjData.insert("user_enable", enable.toInt());
    modifyObjData.insert("user_start_date", startDate);
    modifyObjData.insert("user_end_date", endDate);
    modifyObjParam.insert("user", user);
    modifyDataArray.append(modifyObjData);
    modifyParamArray.append(modifyObjParam);

    modifyObj.insert("cmd", QString("modify_user"));
    modifyObj.insert("param", modifyParamArray);
    modifyObj.insert("data", modifyDataArray);

    return modifyObj;
}

    /*追加修改用户*/
void jsonManage::jsonPackModifyUserAppend(QJsonObject &obj, QString user, QString password, QString mail, QString phone, QString enable, QString startDate, QString endDate)
{
    QJsonObject modifyObjParam,   modifyObjData;
    QJsonArray  modifyParamArray = obj.take("param").toArray();
    QJsonArray  modifyDataArray = obj.take("data").toArray();

    modifyObjData.insert("password", password);        //密码
    modifyObjData.insert("user_mail", mail);
    modifyObjData.insert("user_phone", phone);
    modifyObjData.insert("user_enable", enable.toInt());
    modifyObjData.insert("user_start_date", startDate);
    modifyObjData.insert("user_end_date", endDate);
    modifyObjParam.insert("user", user);
    modifyDataArray.append(modifyObjData);
    modifyParamArray.append(modifyObjParam);

    obj.insert("cmd", QString("modify_user"));
    obj.insert("param", modifyParamArray);
    obj.insert("data", modifyDataArray);
}

    /*查询当前所有用户信息*/
QJsonObject jsonManage::jsonPackQueryUser()
{
    QJsonObject queryUserObj;
    QJsonObject queryUserObjParam, queryUserObjData;
    QJsonArray  queryUserParamArray, queryUserDataArray;

    queryUserDataArray.append(queryUserObjData);
    queryUserParamArray.append(queryUserObjParam);

    queryUserObj.insert("cmd", QString("query_user"));
    queryUserObj.insert("param", queryUserParamArray);
    queryUserObj.insert("data", queryUserDataArray);

    return queryUserObj;
}

  /*查询当前登录用户信息*/
QJsonObject jsonManage::jsonPackQueryOneUser(QString user)
{
    QJsonObject oneUserObj;
    QJsonObject oneUserObjParam, oneUserObjData;
    QJsonArray  oneUserParamArray, oneUserDataArray;

    oneUserObjParam.insert("user", user);

    oneUserDataArray.append(oneUserObjData);
    oneUserParamArray.append(oneUserObjParam);

    oneUserObj.insert("cmd", QString("query_one_user"));
    oneUserObj.insert("param", oneUserParamArray);
    oneUserObj.insert("data", oneUserDataArray);

    return oneUserObj;
}

    /*查询集控登录连接的信息*/
QJsonObject jsonManage::jsonPackQueryConnectInfo(QString userId)
{
    QJsonObject queryConnectObj;
    QJsonObject queryConnectObjParam, queryConnectObjData;
    QJsonArray  queryConnectParamArray, queryConnectDataArray;

    queryConnectObjParam.insert("keyword", userId);
    queryConnectDataArray.append(queryConnectObjData);
    queryConnectParamArray.append(queryConnectObjParam);

    queryConnectObj.insert("cmd", QString("query_connect_info"));
    queryConnectObj.insert("param", queryConnectParamArray);
    queryConnectObj.insert("data", queryConnectDataArray);

    return queryConnectObj;
}

    /*查询在线终端数量*/
QJsonObject jsonManage::jsonPackQueryConnectNumber()
{
    QJsonObject queryTotalObj;
    QJsonObject queryTotalObjParam, queryTotalObjData;
    QJsonArray  queryTotalParamArray, queryTotalDataArray;

    queryTotalDataArray.append(queryTotalObjData);
    queryTotalParamArray.append(queryTotalObjParam);

    queryTotalObj.insert("cmd", QString("query_total_connect"));
    queryTotalObj.insert("param", queryTotalParamArray);
    queryTotalObj.insert("data", queryTotalDataArray);

    return queryTotalObj;
}

    /*查询所有的在线终端数据*/
QJsonObject jsonManage::jsonPackQueryConnectAllInfo()
{
    QJsonObject queryAllConnectObj;
    QJsonObject queryAllConnectObjParam, queryAllConnectObjData;
    QJsonArray  queryAllConnectParamArray, queryAllConnectDataArray;

    queryAllConnectDataArray.append(queryAllConnectObjData);
    queryAllConnectParamArray.append(queryAllConnectObjParam);

    queryAllConnectObj.insert("cmd", QString("query_all_connect_info"));
    queryAllConnectObj.insert("param", queryAllConnectParamArray);
    queryAllConnectObj.insert("data", queryAllConnectDataArray);

    return queryAllConnectObj;
}

    /*解析cmd*/
QString jsonManage::jsonParseCmd(QByteArray reply)
{
    QJsonParseError jsonError;
    QString str = reply;
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    QString cmdVal = nullptr;
    if (jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << QStringLiteral("解析Json失败");
        return nullptr;
    }

    /*开始解析*/
    if (document.isObject())
    {
        QJsonObject obj = document.object();
        QJsonValue value;
        if (obj.contains("cmd"))
        {
            value = obj.take("cmd");
            if (value.isString())
            {
                cmdVal = value.toString();
//                qDebug() << "解析后的数据cmd:"<< cmdVal;
            }
        }
    }
    else
        return nullptr;
    return cmdVal;
}

    /*解析Param*/
QList<QString> jsonManage::jsonParseParam(QByteArray reply, QString name)
{
    QJsonParseError jsonError;
    QString str = reply;
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    QList<QString> list;
    list.empty();
    if (jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << QStringLiteral("解析Json失败");
        return list;
    }

    if (document.isObject())
    {
        QJsonObject obj = document.object();
        QJsonValue value;
        if (obj.contains("param"))
        {
            value = obj.take("param");
            if (value.isArray())
            {
                QJsonArray paramArray= value.toArray();
                //                qDebug() << "解析后的数据param:"<< paramArray;
                for (int i=0; i<paramArray.size(); i++)
                {
                    QJsonObject objParam = paramArray.at(i).toObject();
                    QJsonValue value;
                    if (objParam.contains(name))
                    {
                        value = objParam.take(name);
                        if (value.isString())
                        {
                            QString nameVal= value.toString().toUtf8().data();
                            //                            QString nameVal= QString::fromLocal8Bit(value.toString().toLocal8Bit().data());
                            //                            qDebug() << "解析后的数据param-contains:"<<nameVal;
                            //                            strcpy(nameVal, str.toStdString().data());
                            list.append(nameVal);
                            //                            qDebug() << "解析后的数据param-contains:"<<nameVal;
                        }
                        if (value.isDouble())
                        {
                            QString nameVal =  QString::number(value.toInt(), 10);
                            list.append(nameVal);
                            //                            qDebug() << "解析后的数据param-contains:"<<nameVal;
                        }
                    }
                }
            }
        }
    }
    return list;
}

    /*解析data*/
QList<QString> jsonManage::jsonParseData(QByteArray reply, QString name)
{
    QJsonParseError jsonError;
    QString str = reply;

//    qDebug()<< "解析之前的数据：" << reply;

    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8(), &jsonError); //解决乱码
    QList<QString> list;
    list.empty();

    if (jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << QStringLiteral("解析Json失败");
        return   list;
    }

    if (document.isObject())
    {
        QJsonObject obj = document.object();
        QJsonValue value;
        if (obj.contains("data"))
        {
            value = obj.take("data");
            if (value.isArray())
            {
                QJsonArray dataArray= value.toArray();
//                qDebug() << "解析后的数据data:"<< dataArray;
//                qDebug() << "dataSize:"<< dataArray.size();
                for (int i=0; i<dataArray.size(); i++)
                {
                    QJsonObject objdata= dataArray.at(i).toObject();
                    QJsonValue value;
                    if (objdata.contains(name))
                    {
                        value = objdata.take(name);
                        if (value.isString())
                        {
                            QString nameVal = value.toString().toUtf8().data();
//                            QString nameVal = QString::fromLocal8Bit(value.toString().toLocal8Bit().data());
//                            qDebug() << "解析后的数据data-contains:"<<nameVal;
//                            strcpy(nameVal, str.);
                            list.append(nameVal);
                        }

                        if (value.isDouble())
                        {
                            QString nameVal =  QString::number(value.toInt(), 10);
                            list.append(nameVal);
//                            qDebug() << "解析后的数据data-contains:"<<nameVal;
                        }
                    }
                }
            }
        }
    }
    return list;
}


    /*获取 data数组大小*/
int jsonManage::jsonDataSize(QByteArray reply)
{
    QJsonParseError jsonError;
    QString str = reply;
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
//    qDebug() << "传回数据：" << document;
//    qDebug() << "传回数据reply：" << reply.data();
    int dataSize(0);

    if (jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << QStringLiteral("解析Json失败");
        return dataSize;
    }

    if (document.isObject())
    {
        QJsonObject obj = document.object();
        QJsonValue value;
        if (obj.contains("data"))
        {
            value = obj.take("data");
            if (value.isArray())
            {
                QJsonArray dataArray= value.toArray();
                dataSize = dataArray.size();
//                qDebug() << "dataSize:"<< dataSize;
            }
        }
    }

//    qDebug() << "传回数据datasize大小：" << dataSize;
    return dataSize;
}

    /*获取 param数组大小*/
int jsonManage::jsonParamSize(QByteArray reply)
{
    QJsonParseError jsonError;
    QString str = reply;
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    int dataSize(0);

    if (jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << QStringLiteral("解析Json失败");
        return  dataSize;
    }

    if (document.isObject())
    {
        QJsonObject obj = document.object();
        QJsonValue value;
        if (obj.contains("param"))
        {
            value = obj.take("param");
            if (value.isArray())
            {
                QJsonArray dataArray= value.toArray();
                dataSize = dataArray.size();
//                qDebug() << "dataSize:"<< dataSize;
            }
        }
    }
    return dataSize;
}
