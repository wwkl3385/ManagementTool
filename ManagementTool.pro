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


QT += network
LIBS += -lIphlpapi

SOURCES += \
    main.cpp \
    management/managementtool.cpp \
    deviceManage/devicedlg.cpp \
    accountManage/accountdlg.cpp \
    register/registerdlg.cpp \
    recordExport/recorddlg.cpp \
    loading/loadingdlg.cpp \
    jsonManage/jsonmanage.cpp \
    httpManage/httpmanage.cpp \
    logon/logon.cpp \
    downloadManage/downloadmanager.cpp \
    deviceUpgrade/deviceupgrade.cpp

HEADERS += \
    management/managementtool.h \
    deviceManage/devicedlg.h \
    accountManage/accountdlg.h \
    register/registerdlg.h \
    recordExport/recorddlg.h \
    loading/loadingdlg.h \
    jsonManage/jsonmanage.h \
    httpManage/httpmanage.h \
    main.rc \
    logon/logon.h \
    downloadManage/downloadmanager.h \
    deviceUpgrade/deviceupgrade.h

FORMS += \
    management/managementtool.ui \
    deviceManage/devicedlg.ui \
    accountManage/accountdlg.ui \
    register/registerdlg.ui \
    recordExport/recorddlg.ui \
    loading/loadingdlg.ui \
    logon/logon.ui \
    deviceUpgrade/deviceupgrade.ui

RC_FILE  = ./icon/icon.rc

RC_FILE += main.rc

RESOURCES += \
    ./icon/pictures.qrc \
    language/language.qrc

DISTFILES += \
    uac.manifest
