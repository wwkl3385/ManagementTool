#ifndef ACCOUNTDLG_H
#define ACCOUNTDLG_H

#include <QDialog>

namespace Ui {
class accountDlg;
}

class accountDlg : public QDialog
{
    Q_OBJECT

public:
    explicit accountDlg(QWidget *parent = 0);
    ~accountDlg();

private slots:
    void on_addAccountPushButton_clicked();

private:
    Ui::accountDlg *ui;
};

#endif // ACCOUNTDLG_H
