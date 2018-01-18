/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： httpmanage.h
* 描    述： http post请求 Json数据
* 修改记录：
* 			V0.9，2018-01-01，new，刘卫明
*
***********************************************************************/
#ifndef HTTPMANAGE_H
#define HTTPMANAGE_H

#include<QNetworkAccessManager>


class httpManage
{
public:
    httpManage();
    void httpPost(QString url, QJsonObject obj);

    QNetworkAccessManager *manager;
};

#endif // HTTPMANAGE_H
