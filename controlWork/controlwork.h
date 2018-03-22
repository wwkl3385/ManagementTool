#ifndef CONTROLWORK_H
#define CONTROLWORK_H

#include <QObject>
#include "logon/logon.h"
#include "management/managementtool.h"

class controlWork : public QObject
{
    Q_OBJECT
public:
    explicit controlWork(QObject *parent = nullptr);
    ~controlWork();

private slots:
    void onControl(int);

private:
    logon          *logonDlg;
    ManagementTool *manageWindow;
};

#endif // CONTROLWORK_H
