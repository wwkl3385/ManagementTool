/***********************************************************************
* Copyright (c) 2018, 青岛特来电新能源有限公司  All rights reserved.
*
* 文件名称： recordDlg.h
* 描    述： 登录记录
* 修改记录：
* 			V0.9，2018-01-12，new，刘卫明
*
***********************************************************************/
#ifndef RECORDDLG_H
#define RECORDDLG_H

#include <QDialog>

namespace Ui {
class recordDlg;
}

class recordDlg : public QDialog
{
    Q_OBJECT

public:
    explicit recordDlg(QWidget *parent = 0);
    ~recordDlg();

private:
    Ui::recordDlg *ui;
};

#endif // RECORDDLG_H
