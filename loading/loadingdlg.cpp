/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： loadingdlg.cpp
* 描    述： 登录缓冲进度条
* 修改记录：
* 			V0.9，2017-12-19，new，刘卫明
*
***********************************************************************/
#include "loadingdlg.h"
#include "ui_loadingdlg.h"
#include "logon/logon.h"
#include <QDebug>
#include <QMessageBox>
#include <QBitmap>

loadingDlg::loadingDlg(QWidget *parent) :
    QDialog(parent),
    sec(30),
    ui(new Ui::loadingDlg)
{
    ui->setupUi(this);
    this->resize(400, 90);   //提示框尺寸。

    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(bmp.rect(), 20, 20);
    setMask(bmp);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint );
    ui->loadingLabel->hide(); //加载图片的label
    ui->progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter); //对其方式
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);
}

loadingDlg::~loadingDlg()
{
    delete ui;
    qDebug() << "销毁 loadingDlg";
}
