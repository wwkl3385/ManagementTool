#ifndef PASSWORDDLG_H
#define PASSWORDDLG_H

#include <QDialog>

namespace Ui {
class passwordDlg;
}

class passwordDlg : public QDialog
{
    Q_OBJECT

public:
    explicit passwordDlg(QWidget *parent = 0);
    ~passwordDlg();

private:
    Ui::passwordDlg *ui;
};

#endif // PASSWORDDLG_H
