#include "logon/logon.h"
#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <Windows.h>
#include <winbase.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ::SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);  //防止系统休眠

    /*设置编码，一般在Window开发环境里，是GBK编码，在Linux开发环境里，是utf-8编码*/
    QTextCodec *codec = QTextCodec::codecForName("System");

    /*设置和对本地文件系统读写时候的默认编码格式*/
    QTextCodec::setCodecForLocale(codec);

    /*加载Qt中的资源文件，使Qt显示中文（包括QMessageBox、文本框右键菜单等）*/
    QTranslator translator ;
    translator.load(QString(":/qm/qt_zh_CN"));
    a.installTranslator(&translator);

    logon dlg;

#if 0
    if (dlg.exec() == QDialog::Accepted)
    {
        ManagementTool *w = new ManagementTool;
        w->show();
        return a.exec();
    }
    else
        return 0;
#endif

    dlg.show();
    return a.exec();
}
