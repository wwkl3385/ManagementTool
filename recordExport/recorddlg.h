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
