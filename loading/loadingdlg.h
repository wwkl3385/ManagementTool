#ifndef LOADINGDLG_H
#define LOADINGDLG_H

#include "management/managementtool.h"
#include <QDialog>
#include <QMovie>
#include <QTimer>

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

public slots:
    void delayDisplay();

private:
    Ui::loadingDlg *ui;
    ManagementTool *pManageDlg;
    QMovie *movie;
    QTimer *pTimer;
};

#endif // LOADINGDLG_H
