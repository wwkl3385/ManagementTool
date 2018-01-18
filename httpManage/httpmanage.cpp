/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： httpmanage.cpp
* 描    述： http post请求 Json数据
* 修改记录：
* 			V0.9，2018-01-01，new，刘卫明
*
***********************************************************************/
#include "httpmanage.h"
#include<QJsonObject>
#include<QJsonDocument>

httpManage::httpManage()
{
     manager = new QNetworkAccessManager; //初始化
}

void httpManage::httpPost(QString url, QJsonObject obj)
{
    QNetworkRequest networkRequest;

    /*设置头信息*/
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    /*设置url*/
    networkRequest.setUrl(QUrl(url));

    /*发送请求*/
    this->manager->post(networkRequest, QJsonDocument(obj).toJson());
}
