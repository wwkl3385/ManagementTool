#ifndef LOGON_H
#define LOGON_H

#include <QPainter>
#include <QDialog>
#include <QMouseEvent>
#include "./management/managementtool.h"
#include<QTime>
#include<QTimer>

namespace Ui {
class logon;
}

class logon : public QDialog
{
    Q_OBJECT

public:
    explicit logon(QWidget *parent = 0);
    ~logon();


    /*窗口移动*/
    QPoint m_DragPosition;
    bool m_Drag;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_logonPushButton_clicked();

private:
    Ui::logon *ui;
};

#endif // LOGON_H
