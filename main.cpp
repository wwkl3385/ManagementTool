#include "logon/logon.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    logon dlg;
    if (dlg.exec() == QDialog::Accepted)
    {
        ManagementTool w;
        w.show();
        return a.exec();
    }
    else {
        return 0;
    }
}
