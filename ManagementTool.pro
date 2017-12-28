#-------------------------------------------------
#
# Project created by QtCreator 2017-12-21T13:27:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ManagementTool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
     main.cpp \
    ./management/managementtool.cpp \
    ./logon/logon.cpp \
    ./deviceManage/devicedlg.cpp \
    ./accountManage/accountdlg.cpp \
    ./register/registerdlg.cpp \
    ./recordExport/recorddlg.cpp \
    ./password/passworddlg.cpp \
    loading/loadingdlg.cpp

HEADERS += \
    ./management/managementtool.h \
    ./logon/logon.h \
    ./deviceManage/devicedlg.h \
    ./accountManage/accountdlg.h \
    ./register/registerdlg.h \
    ./recordExport/recorddlg.h \
    ./password/passworddlg.h \
    loading/loadingdlg.h

FORMS += \
    ./management/managementtool.ui \
    ./logon/logon.ui \
    ./deviceManage/devicedlg.ui \
    ./accountManage/accountdlg.ui \
    ./register/registerdlg.ui \
    ./recordExport/recorddlg.ui \
    ./password/passworddlg.ui \
    loading/loadingdlg.ui
RC_FILE  = ./icon/icon.rc

RESOURCES += \
    ./icon/pictures.qrc
