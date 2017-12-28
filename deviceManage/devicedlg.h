#ifndef DEVICEDLG_H
#define DEVICEDLG_H

#include <QDialog>

namespace Ui {
class deviceDlg;
}

class deviceDlg : public QDialog
{
    Q_OBJECT

public:
    explicit deviceDlg(QWidget *parent = 0);
    ~deviceDlg();

private:
    Ui::deviceDlg *ui;
};

#endif // DEVICEDLG_H
