/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： controlWork.cpp
* 描    述： 登录、主窗口管理
* 修改记录：
* 			V0.9，2018-03-20，new，刘卫明
*
***********************************************************************/
#include "controlwork.h"
#include <QDebug>

controlWork::controlWork(QObject *parent) : QObject(parent)
{
    logonDlg = new logon;
    logonDlg->show();
    connect(logonDlg, SIGNAL(closeLogonSignal(int)), this, SLOT(onControl(int)));
}

controlWork::~controlWork()
{
}

void controlWork::onControl(int n)
{
    switch (n)
    {
    case 1: //关闭登录窗口,登录到主窗口
    {
        qDebug() << "关闭窗口，切换到主窗口";
        delete logonDlg;
        manageWindow = new ManagementTool;
        connect(manageWindow, SIGNAL(closeManagementToolSignal(int)), this, SLOT(onControl(int)));
        manageWindow->show();
        break;
    }
    case 2: //关闭主窗口
    {
        qDebug() << "关闭窗口，切换到登录界面";
        delete manageWindow;
        logonDlg = new logon;
        connect(logonDlg, SIGNAL(closeLogonSignal(int)), this, SLOT(onControl(int)));
        logonDlg->show();
        break;
    }
    case 3: //关闭主窗口，退出
        qDebug() << "关闭窗口，退出程序";
        delete manageWindow;
        break;
    default:
        break;
    }
}
