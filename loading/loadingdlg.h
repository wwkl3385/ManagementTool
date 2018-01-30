/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： loadingdlg.h
* 描    述： 登录缓冲进度条
* 修改记录：
* 			V0.9，2017-12-19，new，刘卫明
*
***********************************************************************/
#ifndef LOADINGDLG_H
#define LOADINGDLG_H

#include <QDialog>
#include <QMovie>

namespace Ui {
class loadingDlg;
}

class loadingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit loadingDlg(QWidget *parent = 0);
    ~loadingDlg();

    int sec; //计时

private slots:
    void on_cancelPushButton_clicked();

private:
    Ui::loadingDlg *ui;
    QMovie *movie;
    QTimer *pTimer;
};

#endif // LOADINGDLG_H
