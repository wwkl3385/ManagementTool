//#include "logon/logon.h"
#include "controlWork/controlwork.h"
#include <QApplication>
#include <QSharedMemory>
#include <QTranslator>
#include <QMessageBox>
#include <QTextCodec>
#include <Windows.h>
#include <winbase.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HANDLE m_hMutex = ::CreateMutexW(NULL,FALSE, (wchar_t*)("ManagementTool")); //创建互斥量
    if(GetLastError() == ERROR_ALREADY_EXISTS) //判断互斥量是否存在
    {
        HWND hwndLogin = FindWindow(NULL, L"login");
        qDebug() << hwndLogin;
        HWND hwndTool = FindWindow(NULL, L"集中控制器智能远程管理系统");
        qDebug() << hwndTool;

        SwitchToThisWindow(hwndLogin, TRUE);   //如果最小化，则恢复，并显示到最前面
        SwitchToThisWindow(hwndTool, TRUE);    //如果最小化，则恢复，并显示到最前面
//        SetForegroundWindow(hwndLogin);       //显示该窗体到最前面
        BringWindowToTop(hwndLogin);
        BringWindowToTop(hwndTool);
        qDebug() << hwndLogin;
        return 0;
    }
    else
    {
        ::SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);  //防止系统休眠
        /*设置编码，一般在Window开发环境里，是GBK编码，在Linux开发环境里，是utf-8编码*/
        QTextCodec *codec = QTextCodec::codecForName("System");

        /*设置和对本地文件系统读写时候的默认编码格式*/
        QTextCodec::setCodecForLocale(codec);

        /*加载Qt中的资源文件，使Qt显示中文（包括QMessageBox、文本框右键菜单等）*/
        QTranslator translator ;
        translator.load(QString(":/qm/qt_zh_CN"));
        a.installTranslator(&translator);

<<<<<<< HEAD
//        logon dlg;
=======
        logon dlg;
>>>>>>> ee2bb8874067b0c54e479eb14aa328b73c7109c6
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
<<<<<<< HEAD
//        dlg.show();
        controlWork ctl;
=======
        dlg.show();
>>>>>>> ee2bb8874067b0c54e479eb14aa328b73c7109c6
        return a.exec();

        if(m_hMutex != NULL)
        {
            CloseHandle(m_hMutex);	//关闭句柄
        }
    }
    return 0;
}

#if 0
    QSharedMemory *shareMem = new QSharedMemory(QString("SingleInstanceIdentify"));

    /* if the sharedmemory has not been created, it returns false, otherwise true.
     * But if the application exit unexpectedly, the sharedmemory will not detach.
     * So, we try twice.
    */
    volatile short i = 2;
    while (i--)
    {
        if (shareMem->attach(QSharedMemory::ReadOnly)) /* no need to lock, bcs it's read only */
        {
            shareMem->detach();
        }
    }

    if (shareMem->create(1))
    {
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

        if (shareMem->isAttached())
            shareMem->detach();
        delete shareMem;
    }

    return 0;
#endif


