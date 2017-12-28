#ifndef MANAGEMENTTOOL_H
#define MANAGEMENTTOOL_H

#include <QWidget>
#include <QRect>
#include <QMainWindow>
#include "./password/passworddlg.h"
#include<QHBoxLayout>

const int FLAG = 1; //1-管理员，0-普通用户

namespace Ui {
class ManagementTool;
}

class ManagementTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManagementTool(QWidget *parent = 0);
    QRect getFillSize();
    ~ManagementTool();

//    passwordDlg pwdDlg;
    QWidget *funcWidget;

private slots:
    void on_devicePushButton_clicked();
    void on_accountPushButton_clicked();
    void on_passwordChangePushButton_clicked();
    void on_actionexit_triggered();
    void on_addAccountPushButton_clicked();
    void on_registerPushButton_clicked();
    void on_actionabout_A_triggered();
    void on_logoutPushButton_clicked();
    void on_recordPushButton_clicked();

private:
    Ui::ManagementTool *ui;
     QHBoxLayout *layout;
};

#endif // MANAGEMENTTOOL_H
