#include "./management/managementtool.h"
#include "./logon/logon.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    logon lg;
    lg.show();

    return a.exec();
}
